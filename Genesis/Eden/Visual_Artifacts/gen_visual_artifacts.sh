#!/bin/sh
# gen_visual_artifacts.sh -- makes word-card SVGs across all tablets.
# Naming law: lvl<level>_<series>_<item>.svg  (level-sorted, new items obvious).
# One size, one type: 320x320 SVG. POSIX sh only. NO PYTHON, EVER.
# usage: sh gen_visual_artifacts.sh <level> <series> <item> [<item>...]
[ $# -ge 3 ] || { echo "usage: $0 <level> <series> <item>..."; exit 2; }
L=$1; S=$2; shift 2
for W in "$@"; do
  F="lvl${L}_${S}_${W}.svg"
  printf '<svg xmlns="http://www.w3.org/2000/svg" width="320" height="320"><rect width="320" height="320" fill="#111"/><text x="160" y="185" font-family="monospace" font-size="96" fill="#eee" text-anchor="middle">%s</text></svg>\n' "$W" > "$F"
  echo "made $F"
done
