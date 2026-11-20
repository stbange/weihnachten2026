"""Extract the in-game catalogue from the same Markdown a designer edits."""
import json
import re
import sys
from pathlib import Path

def quoted_rows(file, size):
    source = Path("src", file).read_text(encoding="utf-8")
    source = re.sub(r"/\*[\s\S]*?\*/", "", source)
    words = [json.loads(v) for v in re.findall(r'"(?:[^"\\]|\\.)*"', source)]
    if len(words) % size:
        raise ValueError(f"Incomplete record in {file}")
    return [words[i:i + size] for i in range(0, len(words), size)]

def add_floorplans(cards):
    sectors = quoted_rows("floorplans.inc", 7)
    fieldwork = quoted_rows("fieldwork.inc", 7)
    wards = quoted_rows("wardrooms.inc", 9)
    briefs = [line.strip("|").strip().split(" | ") for line in
              Path("design/expedition-briefs.md").read_text(encoding="utf-8").splitlines()
              if re.match(r"^\| \d{2} ", line)]
    gestures = dict(p="aim and pulse", t="pulse on the bright beat", h="lift through the crown",
                    l="cross at ground height", c="charge and release on the plate", u="press Use")
    encounters = quoted_rows("encounters.inc", 6)
    areas = quoted_rows("expeditions.inc", 10)
    districts = quoted_rows("districts.inc", 2)
    strides = [int(s) for s in re.findall(r"^\{(\d+),", Path("src/districts.inc").read_text(), re.M)]
    if len(sectors) != 120 or any(len(t) != 24 for t in (areas, encounters, districts, strides, fieldwork, briefs, wards)):
        raise ValueError("The atlas requires five sectors per door")
    for index, card in enumerate(cards):
        forks, moves, *names, prize, shelter = fieldwork[index]
        _, card["purpose"], card["twist"], card["decisions"] = briefs[index]
        card.update(wardRoom=wards[index][0], wardSightline=wards[index][1], prize=prize, shelter=shelter, marks=[dict(name=name, move=gestures[move])
                    for name, move in zip(names, moves)])
        card["feature"], card["featureHint"] = districts[index]
        card["stride"] = strides[index]
        card["encounters"] = [dict(name=name, instruction=instruction)
                              for name, instruction in zip(encounters[index][::2], encounters[index][1::2])]
        card["sectors"] = []
        for area in range(5):
            spine, _, _, link, order, staging, description = sectors[index*5+area]
            card["sectors"].append(dict(name=areas[index][area+1], brief=description,
                start=spine[0], exit=spine[-1], link=link, fork=forks[3*area:3*area+3],
                required=[names[i] for i in range(3) if (1, 2, 5, 2, 7)[area] & (1<<i)],
                tools=[areas[index][6+int(i)] for i in order],
                encounters=[dict(name=encounters[index][2*int(i)], at=loc,
                                 instruction=encounters[index][2*int(i)+1])
                            for i, loc in zip(staging[::2], staging[1::2])]))

def read_almanac(path):
    text = Path(path).read_text(encoding="utf-8")
    cards = []
    for number, title, body in re.findall(r"^## (\d\d) · ([^\n]+)\n([\s\S]*?)(?=^## |\Z)", text, re.M):
        fields = dict(re.findall(r"^\*\*(.+?)\.\*\* (.+)$", body, re.M))
        reference = re.search(r"^\*([^*]+)\*$", body, re.M).group(1)
        signatures = re.findall(r"^- (.+)$", body, re.M)
        if len(signatures) != 3 or int(number) != len(cards) + 1:
            raise ValueError(f"Invalid portal {number}")
        cards.append(dict(day=int(number), title=title, reference=reference,
                          mission=fields["Mission"], kit=signatures, place=fields["Art and opposition"],
                          gift=fields["Back in the hall"], route=fields["Twenty-minute route"], coop=fields["Solo and co-op"]))
    if len(cards) != 24:
        raise ValueError("The hall must have 24 doors")
    add_floorplans(cards)
    return cards

if __name__ == "__main__":
    Path(sys.argv[1]).write_text(json.dumps(read_almanac("design/portals.md"),
                                 ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
