#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
# SDL_CFLAGS and SDL_LIBS can point at a locally installed SDL3.
if [[ -z ${SDL_CFLAGS:-} ]]; then SDL_CFLAGS=$(pkg-config --cflags sdl3); fi
if [[ -z ${SDL_LIBS:-} ]]; then SDL_LIBS=$(pkg-config --libs sdl3); fi
dir=build/coverage
mkdir -p "$dir"
rm -f "$dir"/*.gcda "$dir"/*.gcov.json.gz
flags=(-std=c11 -O0 -g --coverage -Isrc)
for name in hall scene draw app; do
  extra=()
  if [[ $name == app ]]; then extra=(-Dmain=lh_native_main); fi
  gcc "${flags[@]}" $SDL_CFLAGS "${extra[@]}" -c "src/$name.c" -o "$dir/$name.o"
done
gcc "${flags[@]}" tests/unit.c "$dir/hall.o" "$dir/scene.o" -lm -o "$dir/unit"
gcc "${flags[@]}" tests/platform.c "$dir/app.o" "$dir/draw.o" "$dir/hall.o" "$dir/scene.o" $SDL_LIBS -lm -o "$dir/platform"
"$dir/unit"
prefs=$(mktemp -d)
trap 'rm -rf "$prefs"' EXIT
XDG_DATA_HOME="$prefs" SDL_VIDEODRIVER=${SDL_VIDEODRIVER:-offscreen} "$dir/platform"
(cd "$dir" && gcov --json-format hall.gcno scene.gcno draw.gcno app.gcno >/dev/null)
python3 scripts/coverage.py "$dir"
# Keep the game replay independent of the reported unit percentage.
gcc -std=c11 -O2 -Isrc tests/replay.c src/hall.c -lm -o "$dir/replay"
"$dir/replay"
