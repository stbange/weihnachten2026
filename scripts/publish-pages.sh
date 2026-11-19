#!/usr/bin/env bash
set -euo pipefail
cd "$(git rev-parse --show-toplevel)"
if [[ $(git branch --show-current) != main || -n $(git status --porcelain) ]]; then
  echo "Publish from a clean main checkout." >&2
  exit 1
fi
source_sha=$(git rev-parse HEAD)
remote_main=$(git ls-remote origin refs/heads/main | cut -f1)
if [[ $source_sha != "$remote_main" ]]; then
  echo "Push the source commit to main before publishing." >&2
  exit 1
fi
bash scripts/build-web.sh
site_dir=$(mktemp -d)
trap 'git worktree remove --force "$site_dir" >/dev/null 2>&1 || true' EXIT
git fetch origin refs/heads/gh-pages
git worktree add --detach "$site_dir" FETCH_HEAD
# Only the temporary publication worktree is replaced.
git -C "$site_dir" rm -qr --ignore-unmatch .
cp -R dist/. "$site_dir/"
git -C "$site_dir" add --all
if git -C "$site_dir" diff --cached --quiet; then exit 0; fi
git -C "$site_dir" commit -m "Publish Lantern Hall from ${source_sha:0:12}"
git -C "$site_dir" push origin HEAD:refs/heads/gh-pages
