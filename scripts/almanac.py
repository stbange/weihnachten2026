"""Extract the in-game catalogue from the same Markdown a designer edits."""
import json
import re
import sys
from pathlib import Path

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
    return cards

if __name__ == "__main__":
    Path(sys.argv[1]).write_text(json.dumps(read_almanac("design/portals.md"),
                                 ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
