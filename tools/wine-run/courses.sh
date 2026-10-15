#!/bin/bash
# courses.sh <outdir> [first=0] [last=14] [frames=90]: one regress per course (COURSE=n: all courses unlocked, course page moved down n times) with the
# coverage counters on; prints per course the ctrl1-ovl1 differences (cars vehicles aibots statsrace powerups) and at the end which replaced functions
# no course ever ran.  Frame 90 by default: on some courses the race ends at ~140 frames (standings screen) and the real time timers of the end make dumps noisy.  Control dumps are cached per course (CTRL_CACHE), a repeated pass takes ~15-20 s per course.
T=$(dirname "$(readlink -f "$0")"); O=$(readlink -f "$1"); A=${2:-0}; B=${3:-14}; FR=${4:-90}
mkdir -p "$O"
for c in $(seq $A $B); do
  rm -f "$O/c$c/out_ovl_1/counters.bin" "$O/c$c/out_ovl_1/race_start.png"      # exact files of an earlier run in this directory: a failed build must not feed old coverage into the result
  # a course that does not finish within COURSE_TIMEOUT s is reported and the sweep goes on (a hung sweep must never go unnoticed)
  # COURSE_TIMEOUT is scaled by the load at the start (x1 up to load 4, max x3); a course whose runs were INVALID (regress exit 3: did not freeze) is repeated exactly once with PAR=1
  CT=$(awk -v t=${COURSE_TIMEOUT:-420} '{s=$1/4; if(s<1)s=1; if(s>3)s=3; printf "%d", t*s}' /proc/loadavg)
  for attempt in 1 2; do
    SAMELAYOUT=${SAMELAYOUT-0} COURSE=$c OVL_ARGS="${COUNT---count} $EXTRA_OVL" PAR=$([ $attempt = 1 ] && echo ${PAR:-4} || echo 1) timeout -k 10 $CT $T/regress.sh $O/c$c $FR ${RUNS:-3} > $O/c$c.log 2>&1; rc=$?
    [ $rc = 3 ] && [ $attempt = 1 ] && { echo "course $c: runs did not freeze (retry), repeating once with PAR=1 (load $(cut -d' ' -f1 /proc/loadavg))"; continue; }
    break
  done
  [ $rc = 0 ] || echo "course $c: regress failed or timed out (exit $rc), see $O/c$c.log"
  r=$(grep -E "^(cars|vehicles|aibots|statsrace|powerups):" $O/c$c.log | sed -E 's/^(\w+): .* best ([0-9n/a]+).*/\1=\2/;t;s/^(\w+): .*ctrl1-ovl1 ([0-9n/a]+).*/\1=\2/' | tr '\n' ' ')
  CH=""; for k in ${CHAOTIC-2 5 6}; do [ $k = $c ] && CH=" (chaotic course: the game state depends on the exe layout, not comparable)"; done
  echo "course $c: $r$CH $(grep -c WARNING $O/c$c.log) warnings  race_start $(md5sum < $O/c$c/out_ovl_1/race_start.png 2>/dev/null | cut -c1-8)"
done
python3 - "$O" "$A" "$B" <<'PY'
import glob, os, struct, sys
O, A, B = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
ran, allf = {}, set()
for c in range(A, B + 1):
    mp, cb = "%s/c%d/ovl.exe.map" % (O, c), "%s/c%d/out_ovl_1/counters.bin" % (O, c)
    if not (os.path.exists(mp) and os.path.exists(cb)): continue
    base, names = None, []
    for l in open(mp):
        p = l.split()
        if p[0].startswith("count:"): names.append((p[0][6:], int(p[1], 16)))
        elif p[0] == "count_table": base = int(p[1], 16)
    d = open(cb, "rb").read()
    for n, va in names:
        allf.add(n)
        if struct.unpack_from("<I", d, va - base)[0] > 0: ran.setdefault(n, []).append(c)
never = sorted(allf - set(ran))
open(O + "/never_executed.txt", "w").write("\n".join(never) + "\n")
print("replaced %d, executed by at least one course %d, never executed %d (list: %s/never_executed.txt)" % (len(allf), len(ran), len(never), O))
PY
