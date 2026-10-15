#!/bin/bash
# clone.sh <game dir> <new dir>: a copy of a prepared game directory that costs no disk space and no time: hardlinks for everything, real copies of the files that
# a run writes or that differ per run (App_Executables/MFL.exe, videocfg.bin, mashed.log; the game writes nothing else into its directory).
# Never write into a clone with `cp` over an existing file of another clone without unlinking it first (hardlinks share the data).
rm -rf "$2"; cp -al "$1" "$2" || exit 1
for f in MFL.exe videocfg.bin mashed.log; do
  p="$2/App_Executables/$f"
  [ -f "$p" ] && { cp "$p" "$p.new" && chmod u+w "$p.new" && mv -f "$p.new" "$p"; }
done
