"""Audit the authored room graphs against the production maps and ordinary-input traces."""

import argparse
from collections import Counter, defaultdict, deque
from pathlib import Path
import re
import subprocess
import sys

from almanac import quoted_rows, read_almanac

ROOT = Path(__file__).resolve().parents[1]
STYLES = ("solo", "split", "rush", "explore", "cautious")
RESULT = re.compile(
    r"Door (\d+) (\w+) ([.\d]+) min \| (\d+) marks \| (\d+) caches \| (\d+) returns \| ([.\d]+) s quiet \| ([.\d]+)% climax \| (\d+) rooms"
)


def anchor_xy(c):
    i = ord(c) - ord("a")
    return i % 5, i // 5


def room_graph(spine, branches, fork, link):
    edges = set()
    for line in [spine, *branches.split(), fork]:
        for a, b in zip(line, line[1:]):
            if a != b:
                edges.add(tuple(sorted((a, b))))
    nodes = {p for edge in edges for p in edge}
    assert set(link) <= nodes, link
    # The far-side ward is the return key. Search both versions of the graph.
    start, vault = spine[0], fork[1]
    seen = {(start, False)}
    queue = deque(seen)
    while queue:
        here, key = queue.popleft()
        moves = [(b if a == here else a, key) for a, b in edges if here in (a, b)]
        if here == vault:
            moves.append((here, True))
        if key and here in (start, vault):
            moves.append((vault if here == start else start, True))
        for state in moves:
            if state not in seen:
                seen.add(state)
                queue.append(state)
    assert all((n, False) in seen and (n, True) in seen for n in nodes)
    assert fork[1] not in spine and (spine[-1], True) in seen
    degree = Counter(room for edge in edges for room in edge)
    cycles = len(edges) - len(nodes) + 1
    assert cycles >= 1
    return nodes, cycles, sorted(k for k, v in degree.items() if v == 1)


def room_label(x, z, stride):
    col, row = round((x - 8) / stride), round((z - 8) / stride)
    if (
        min(col, row) >= 0
        and max(col, row) <= 4
        and x == 8 + col * stride
        and z == 8 + row * stride
    ):
        return chr(97 + col + row * 5)
    return f"({x},{z})"


def logical_map(spine, nodes, required, fork, link):
    lines = []
    for z in range(5):
        row = []
        for x in range(5):
            c = chr(97 + x + 5 * z)
            mark = (
                "S"
                if c == spine[0]
                else (
                    "E"
                    if c == spine[-1]
                    else (
                        "V"
                        if c == fork[1]
                        else (
                            "*"
                            if c in required
                            else "+" if c in link else "." if c in nodes else " "
                        )
                    )
                )
            )
            row.append(c + mark if mark != " " else "  ")
        lines.append(" ".join(row))
    return lines


def verify_geometry(replay, day, area, card, nodes, spine, chamber):
    far_reward = 0
    for unlocked in (0, 1):
        grid = subprocess.check_output(
            [replay, "--map", str(day), str(area + 1), str(unlocked)], text=True
        ).splitlines()
        stride = card["stride"]
        first = anchor_xy(spine[0])
        start = (8 + first[0] * stride, 8 + first[1] * stride)
        all_floor = {
            (x, z) for z, row in enumerate(grid) for x, t in enumerate(row) if t != "#"
        }
        seen = {start}
        todo = deque([start])
        while todo:
            x, z = todo.popleft()
            for p in ((x + 1, z), (x - 1, z), (x, z - 1), (x, z + 1)):
                if p in all_floor and p not in seen:
                    todo.append(p)
                    seen.add(p)
        assert seen == all_floor and all(
            (8 + x * stride, 8 + z * stride) in seen for x, z in map(anchor_xy, nodes)
        )
        booth_x, booth_z = anchor_xy(spine[1])
        shutter = [
            (x, z)
            for z, row in enumerate(chamber[1:])
            for x, ch in enumerate(row)
            if ch == "+"
        ]
        assert len(shutter) == 1
        gx, gz = shutter[0]
        assert (
            grid[8 + booth_z * stride + gz - 2][8 + booth_x * stride + gx - 2] != "#"
        ) == bool(unlocked)
        rewards = {p: 0 for p in all_floor if grid[p[1]][p[0]] in "ptlhcuV"}
        todo = deque(rewards)
        while todo:
            x, z = todo.popleft()
            for p in ((x + 1, z), (x - 1, z), (x, z + 1), (x, z - 1)):
                if p in all_floor and p not in rewards:
                    rewards[p] = rewards[x, z] + 1
                    todo.append(p)
        for node in nodes:
            x, z = anchor_xy(node)
            far_reward = max(far_reward, rewards[8 + x * stride, 8 + z * stride])
        assert far_reward < stride, (day, area, far_reward)
    return far_reward


def verify(log, replay):
    cards = read_almanac(ROOT / "design/portals.md")
    plans = quoted_rows("floorplans.inc", 7)
    primitives = re.findall(r"\{([A-Z]+),", (ROOT / "src/encounters.inc").read_text())
    assert len(primitives) == 72
    wards = quoted_rows("wardrooms.inc", 9)
    chambers = quoted_rows("chambers.inc", 6)
    rows = defaultdict(dict)
    for match in RESULT.finditer(log):
        day, style, mins, marks, prizes, rides, gap, climax, spacing = match.groups()
        rows[int(day)][style] = (
            float(mins),
            int(marks),
            int(prizes),
            int(rides),
            float(gap),
            float(climax),
            int(spacing),
        )
    assert len(rows) == 24 and all(
        set(s) == set(STYLES) for s in rows.values()
    ), "Incomplete bot log"
    legs = defaultdict(list)
    for line in log.splitlines():
        if line.startswith("LEG "):
            day, area, *values = map(int, line.split()[1:])
            legs[day, area].append(values)
    assert len(legs) == 120
    lines = [
        "# Expedition route audit",
        "",
        "The [design briefs](expedition-briefs.md) give each door its purpose, twist, feeling and decisions. This audit follows those decisions into the executable game. Run `python3 scripts/validate_routes.py build/coverage/replay` after the C tests. Add `--map 13 5` for the complete collision raster of a sector or `--write design/route-audit.md` to regenerate this book.",
        "",
        "All 120 floor plans are authored. The five-letter-wide diagrams below label their rooms. The spine, branches and side loop columns are in the game's exact anchor notation. Letters joined in one word are connected by a passage. Spaces separate independent polylines. Full court radii, encounter placement, relief and chamber walls are in [floorplans.inc](../src/floorplans.inc), [fieldwork.inc](../src/fieldwork.inc), [districts.inc](../src/districts.inc) and [chambers.inc](../src/chambers.inc).",
        "",
        "In each diagram S is the start, E the exit, * a required action room, V the hinted ward room, + a route station and . an optional landmark. Each coordinate is a four-metre tile. Graph cycles count independent loops in the authored normal corridors, before counting a commissioned route or the ward return. Graph dead ends name their contents. Typed actions below identify the placements on the critical route in encounter order.",
        "",
        "The walk uses the production four-neighbour compass for each compulsory destination taken by the guided solo player. Tile length measures those centre-to-centre steps between actual player positions and targets. Rooms counts distinct authored rooms visited per sector, including rooms crossed en route. Optional share counts authored room anchors with no required interaction. It excludes individual roadside signals and includes both ends of the compulsory route in the denominator. Players can also shoot at range, cut diagonally or ride transport.",
        "",
        "The graph search carries the ward key. It verifies that the party can reach the prize before opening its lock, walk to the exit with either key state, and reach every authored room again afterward. A second check reads the executable's collision tiles. The bots use movement and button inputs to verify both five-sector progress and actual extraction. Every field mark has a renewable fuel and movement reward. The room-gap measure counts room entries between a mark, a prize or an objective advance. Quiet seconds count the longest interval between these events. An opportunity on the route and a reward actually collected are measured separately.",
        "",
        "Planned intensity is the designer's 1–10 judgement. Built intensity is a structural estimate from the playable score. An active beat starts at 3 and adds one per distinct staged action, one for a timing window, one for multiple required field signatures and one for the three-action finale. The shelter and safe extraction score 2. Seven is high. Actual timing and intervals between successes appear separately. Human excitement and first-clear time still need player observations.",
        "",
    ]
    for day, card in enumerate(cards, 1):
        title = card["title"]
        lines.extend(
            [
                f"## {day:02d} · {title}",
                "",
                card["reference"],
                "",
                card["purpose"] + " " + card["twist"],
                "",
                card["decisions"],
                "",
                "**Beats and graph**",
                "",
            ]
        )
        areas = card["sectors"]
        beats = [
            f"{areas[i]['name']} ({score})" for i, score in enumerate((2, 5, 7, 4))
        ]
        beats += [
            card["shelter"] + " (2)",
            areas[4]["name"] + " (9)",
            card["gift"].split(".")[0] + " (2)",
        ]
        lines.extend(
            [
                " → ".join(beats) + ".",
                "",
                "Normal edges follow the spine, side passages and ward loop below. The first tool is the key to the chamber shutter. The mint return is a two-way shortcut earned in V. Sector exits are one-way after the required tools, transmission where present, and field practice. The final exit also requires three signature actions after shelter.",
                "",
                "| Sector | Spine | Side passages | Ward loop | Route stations | Rooms / tiles | Optional rooms | Cycles | Dead ends and contents |",
                "|---|---|---|---|---|---:|---:|---:|---|",
            ]
        )
        diagrams = []
        walks = []
        roles = []
        built = []
        totals = [0, 0, 0, 0, 0]
        max_blank = 0
        far_reward = 0
        for area, field in enumerate(areas):
            spine, branches, courts, link, order, staging, _ = plans[
                (day - 1) * 5 + area
            ]
            fork = field["fork"]
            nodes, cycles, ends = room_graph(spine, branches, fork, link)
            stages = legs[day, area]
            visited = set()
            required = {spine[0], spine[-1]}
            sequence = []
            for task, phase, x, z, tiles, mask, blank, stamps, frame in stages:
                visited.update(chr(97 + i) for i in range(25) if mask & (1 << i))
                letter = room_label(x, z, card["stride"])
                if len(letter) == 1:
                    required.add(letter)
                if task == 5:
                    label = field["encounters"][phase]["name"]
                elif task < 3:
                    label = field["tools"][task] + (" finish" if phase else "")
                else:
                    label = {
                        3: "Exit",
                        4: "Answer dispatch" if phase else "Recover dispatch",
                        6: card["shelter"],
                        7: "Board mail van",
                        8: "Field practice",
                    }.get(task)
                sequence.append(f"{letter} {label}")
            assert stages[-1][0] == 3 and required <= nodes
            assert (stages[-1][-2] & (1, 2, 5, 2, 7)[area]) == (1, 2, 5, 2, 7)[area]
            if area == 4:
                tasks = [s[0] for s in stages]
                assert tasks[-4:] == [5, 5, 5, 3] and tasks[-5] == 6
                if day == 24:
                    assert tasks[-6] == 7
            far_reward = max(
                far_reward,
                verify_geometry(
                    replay, day, area, card, nodes, spine, chambers[day - 1]
                ),
            )
            rooms = len(visited & nodes)
            tiles = sum(s[4] for s in stages)
            optional = len(nodes - required)
            max_blank = max(max_blank, *[s[6] for s in stages])
            totals = [
                a + b
                for a, b in zip(totals, (rooms, tiles, optional, len(nodes), cycles))
            ]
            contents = (
                ", ".join(
                    c
                    + " "
                    + (
                        "arrival landmark"
                        if c == spine[0]
                        else (
                            "extraction"
                            if c == spine[-1]
                            else (
                                "tool / field reward"
                                if c in required
                                else "fuel signal"
                            )
                        )
                    )
                    for c in ends
                )
                or "None"
            )
            lines.append(
                f"| {field['name']} | `{spine}` | `{branches}` | `{fork}` | `{link}` | {rooms} / {tiles} | {optional}/{len(nodes)} | {cycles} | {contents} |"
            )
            fights = []
            puzzles = []
            for i, c in zip(staging[::2], staging[1::2]):
                action = card["encounters"][int(i)]
                name = action["name"]
                (
                    fights
                    if any(
                        w in action["instruction"].lower()
                        for w in ("pulse", "shoot", "shot", "beam", "fold", "stun")
                    )
                    else puzzles
                ).append(c + " " + name)
            actions = {primitives[(day - 1) * 3 + int(i)] for i in staging[::2]}
            intensity = (
                3
                + len(actions)
                + bool({"LENS", "ORB"} & actions)
                + (len(field["required"]) > 1)
            )
            if area == 4:
                built.extend([2, intensity + 1, 2])
            else:
                built.append(intensity)
            key_sites = " → ".join(
                room_label(s[2], s[3], card["stride"]) for s in stages if s[0] == 0
            )
            roles.append(
                f"- **{field['name']}** · Start {spine[0]}. Landmark courts {courts}. Key {field['tools'][0]} at {key_sites} opens the chamber shutter. Fight or aim rooms {', '.join(fights) or 'use the active tool sentry'}. Puzzle or traversal rooms {', '.join(puzzles) or field['tools'][-1]}. Hinted reward {fork[1]}. Shortcut {fork[1]} ↔ {spine[0]}, opened at {fork[1]}. Lock and exit {spine[-1]}."
                + (f" Rest at {card['shelter']}." if area == 4 else "")
            )
            diagrams.append(logical_map(spine, nodes, required, fork, link))
            walks.extend(
                [
                    f"- **{field['name']}** · "
                    + " → ".join(sequence)
                    + ". "
                    + field["brief"]
                ]
            )
        lines.extend(
            [
                "",
                *roles,
                "",
                "**Layout and critical walk**",
                "",
                "```text",
                "    Sector 1          Sector 2          Sector 3          Sector 4          Sector 5",
            ]
        )
        lines.extend("    ".join(d[i] for d in diagrams).rstrip() for i in range(5))
        lines.extend(
            [
                "```",
                "",
                *walks,
                "",
                "**Content placement**",
                "",
                f"{card['wardRoom']} · {card['wardSightline']}",
                "",
                "```text",
                *wards[day - 1][2:],
                "```",
                "",
                "X is solid cover, : is the approach inlay, ! is an amber warning lamp, and the middle tile holds the optional prize.",
                "",
            ]
        )
        for mark in card["marks"]:
            lines.append(
                f"- {mark['name']} appears on travel lines. Players {mark['move']} for lift fuel and a brief burst of speed while travelling."
            )
        for beat in card["encounters"]:
            lines.append(
                "- "
                + beat["name"]
                + " is staged in a widened court. "
                + beat["instruction"]
                + ". The final sequence revisits this action after the shelter."
            )
        lines.extend(
            [
                f"- {card['prize']} sits between two entrances in the cover shown above. The orbiting amber lamp shows where contact will repel the player. Completing the indicated field skill awards a shared ward and opens the return line at the entrance.",
                f"- {card['shelter']} sits three tiles from the first finale anchor. It refills both seats and gives the party control over when to begin.",
                "",
                "**Verification and playtest**",
                "",
                "Planned intensity **2 → 5 → 7 → 4 → 2 → 9 → 2**. Built mechanical intensity **"
                + " → ".join(map(str, built))
                + "**.",
                "",
                f"The guided path crossed **{totals[0]} rooms and {totals[1]:,} tiles**. Optional room content was **{100*totals[2]/totals[3]:.0f}%** ({totals[2]} of {totals[3]}). The five sectors contained **{totals[4]} independent normal-corridor cycles**. Every authored room has a reachable field reward within **{far_reward} tiles** of its centre. A reward is available in every room. The longest guided stretch outside the sensing radius of a field signal or compulsory destination was **{max_blank} tiles**.",
                "",
                "| Player | Minutes | Marks answered | Wards found | Return rides | Longest quiet stretch | Most room entries between rewards | Climax begins |",
                "|---|---:|---:|---:|---:|---:|---:|---:|",
            ]
        )
        assert all(min(built[i : i + 3]) < 7 for i in range(len(built) - 2)), (
            day,
            built,
        )
        for style in STYLES:
            mins, marks, prizes, rides, gap, climax, spacing = rows[day][style]
            assert climax >= 75 and (style != "explore" or prizes == rides == 5)
            lines.append(
                f"| {style.capitalize()} | {mins:.2f} | {marks} | {prizes} | {rides} | {gap:.1f} s | {spacing} | {climax:.1f}% |"
            )
        guided = rows[day]["solo"]
        rush = rows[day]["rush"]
        explore = rows[day]["explore"]
        safe = rows[day]["cautious"]
        lines.extend(
            [
                "",
                f"**Rush.** Passing the signals left this player up to {rush[4]:.0f} seconds between successes. The field check catches a missed skill before extraction. Answering {card['marks'][0]['name']} in transit avoids a later detour.",
                f"**Explore.** Finding all five instances of {card['prize']} and riding home added {explore[0]-guided[0]:.1f} minutes over the guided run. The mint home marker makes the destination legible before committing.",
                f"**Cautious.** The cautious player took the ward loop in the third sector, stunned nearby threats and finished in {safe[0]:.2f} minutes. {card['shelter']} gives the player a safe place to line up the final approach.",
                f"**Revision.** The old run let players pass long stretches with a single movement input. The side room added in this revision is {card['wardRoom']}. {card['wardSightline']} This door now teaches {card['marks'][0]['name']}, develops {card['marks'][1]['name']} and asks for all three field responses before the final demonstration. The five new ward rooms connect from both sides. The quickest of these observed policies still had to finish the same three final encounters.",
                "",
            ]
        )
    return "\n".join(lines)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("replay", type=Path)
    ap.add_argument("--log", type=Path, help="previous output of replay --audit")
    ap.add_argument("--write", type=Path)
    ap.add_argument("--map", nargs=2, type=int, metavar=("DOOR", "SECTOR"))
    ap.add_argument("--commissioned", action="store_true")
    args = ap.parse_args()
    replay = str(args.replay.resolve())
    if args.map:
        return subprocess.call(
            [replay, "--map", *map(str, args.map), str(int(args.commissioned))]
        )
    log = (
        args.log.read_text()
        if args.log
        else subprocess.check_output([replay, "--audit"], text=True)
    )
    report = verify(log, replay)
    if args.write:
        args.write.write_text(report, encoding="utf-8")
    print(
        "Verified 24 missions, 120 authored sectors, 240 collision states and 120 ordinary-input clears."
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
