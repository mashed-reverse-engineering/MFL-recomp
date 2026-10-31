#!/bin/bash
# librw side of the asset-parity oracle: asset_dump over every leaf directory (= one piz) of the extracted game data, one group per directory
# (the group's own TXDs first, then every TXD of Common/ so that panel / HUD clumps find their textures like in the game).
# Usage: offline.sh <asset_dump exe> <out csv> [extracted root]     WINE=1: the exe is the mingw .exe, run it in docker tianon/wine
EXE=$(readlink -f "$1"); OUT=$(readlink -f -m "$2"); R=$(readlink -f "${3:-$(dirname "$0")/../../../MashedGame/FullyLoaded/extracted}")
common=$(find "$R" -ipath '*/common/*' -iname '*.txd' -not -ipath '*/PS2/*' -not -ipath '*/XBOX/*' | sort)
args=()
while IFS= read -r d; do
  case "$d" in */PS2|*/XBOX|*/PS2/*|*/XBOX/*) continue;; esac
  fs=$(find "$d" -maxdepth 1 -type f \( -iname '*.txd' -o -iname '*.dff' -o -iname '*.bsp' \) | sort)
  [ -n "$fs" ] || continue
  args+=(-- $fs $common)
done < <(find "$R" -type d | sort)
args=("${args[@]:1}")
if [ -n "$WINE" ]; then
  printf '%s\n' "${args[@]}" > "$OUT.args"      # @listfile: the Wine command line is too short for the ~1000 paths
  docker run --rm -v "$R":"$R":ro -v "$(dirname "$OUT")":/o -v "$(dirname "$EXE")":/t tianon/wine:latest wine /t/$(basename "$EXE") /o/$(basename "$OUT") @/o/$(basename "$OUT").args
else "$EXE" "$OUT" "${args[@]}"; fi
