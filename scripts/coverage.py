"""Count every instrumented production C line, using GCC gcov JSON."""
import gzip
import json
from pathlib import Path
import sys

directory = Path(sys.argv[1])
covered = total = 0
seen = set()
for report in sorted(directory.glob("*.gcov.json.gz")):
    with gzip.open(report, "rt") as handle:
        files = json.load(handle)["files"]
    for item in files:
        path = Path(item["file"])
        if path.parent.name != "src" or path.suffix != ".c":
            continue
        if path.name in seen:
            raise SystemExit(f"Duplicate report for {path}")
        seen.add(path.name)
        lines = item["lines"]
        hits = sum(row["count"] > 0 for row in lines)
        covered += hits
        total += len(lines)
        missed = [str(row["line_number"]) for row in lines if not row["count"]]
        print(f"{path.name:<12} {hits}/{len(lines)} lines; missed {', '.join(missed) or 'none'}")
expected = {path.name for path in Path("src").glob("*.c")}
if expected != seen:
    raise SystemExit(f"Coverage sources mismatch: {expected ^ seen}")
print(f"Unit line coverage: {100 * covered / total:.2f}% ({covered}/{total}).")
if covered * 100 < total * 95:
    raise SystemExit("Coverage below 95%")
