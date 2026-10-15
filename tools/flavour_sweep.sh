#!/bin/bash
# flavour_sweep.sh <autocompile dir> <automatch.csv of the /O2 run> <out dir>: compile every chunk with other compiler flag sets (the idea of the GBA matching skill: library code was built with
# other flags than the game) and list the functions that become byte identical under a flavour although they do not under /O2.  Output: <out dir>/flavours.txt (flavour function).
T=$(dirname "$(readlink -f "$0")"); A=$(readlink -f "$1"); C=$(readlink -f "$2"); O=$(readlink -f "$3"); mkdir -p $O
for fl in "/O1" "/Os" "/Ox" "/O2 /Oy-" "/O2 /Ob0" "/O2 /GF" "/Od"; do
  tag=$(echo "$fl" | tr -d ' /'); AM_FLAGS="$fl" python3 $T/automatch.py $A $O/$tag.csv 6 > $O/$tag.log 2>&1
  python3 - "$C" "$O/$tag.csv" "$fl" >> $O/flavours.txt <<'PY'
import csv, sys
base = {r[1]: r[3] for r in csv.reader(open(sys.argv[1])) if len(r) >= 4}
for r in csv.reader(open(sys.argv[2])):
    if len(r) >= 4 and r[3] == "MATCH" and base.get(r[1], "MATCH") != "MATCH": print(sys.argv[3].replace(" ", ""), r[1])
PY
  echo "$fl: $(grep -c . $O/flavours.txt 2>/dev/null) total new matches so far"
done
