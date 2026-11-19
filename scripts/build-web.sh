#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p dist/licenses
emcc -std=c11 -O2 src/app.c src/hall.c src/scene.c src/draw.c \
  -sUSE_SDL=3 -sMIN_WEBGL_VERSION=1 -sMAX_WEBGL_VERSION=1 \
  -sMODULARIZE=1 -sEXPORT_NAME=createLantern -sENVIRONMENT=web \
  -sALLOW_MEMORY_GROWTH=1 -sEXPORTED_RUNTIME_METHODS=HEAPF32,GL \
  --no-entry -o dist/lantern.js
cp -R web/. dist/
cp LICENSE ART-CREDITS.md dist/
cp licenses/*.txt dist/licenses/
python3 scripts/almanac.py dist/almanac.json
touch dist/.nojekyll
