#!/bin/bash
# zneg.sh <work dir> [file.c]: W9 T-ZNEG (plan W9 section 1 point 5), the negative test of the zero-original baseline.
# Links the gate's standalone harness link (gate.sh standalone_link env: MKRANGE GEN_DATA IMPLIBS HARNESS, LIB_RW default from gate.sh) with ONE installed
# source file left out (SKIP_FILES; default an_ok_002.c, app tier 2 rows), runs origbytes.py on it and compares the code layers with
# output/zero_original_baseline.txt like gate.sh origcheck.  The test PASSES (exit 0) only when the compare FAILS on a grown layer; exit 1 otherwise.
# Needs the gate lock free (a link).  DELTA: output/gate/standalone_delta.txt, a "DELTA mismatch" is retried (3 links at most).
# W10 A5: `zneg.sh <work dir> rsrc` = the rsrc case: nothing left out, the link carries the old output/res/mfl.res; PASS when the baseline's rsrc line
# (0 once Z-rsrc is the default) would FAIL on it.
set -u
T=$(dirname "$(readlink -f "$0")"); G=$(readlink -f $T/../..); W=${1:?work dir}; F=${2:-an_ok_002.c}
SK=$F; RS=${RES:-}; [ "$F" = rsrc ] && { SK=; RS=$G/output/res/mfl.res; }
lr=$(grep -o 'lr=${LIB_RW-librw:[^}]*' $T/../gate.sh | head -1); lr=${lr#*LIB_RW-}
[ -n "$lr" ] || { echo "zneg: LIB_RW default not found in tools/gate.sh"; exit 1; }
d=$(cat $G/output/gate/standalone_delta.txt 2>/dev/null || echo ba000)
mkdir -p $W
for k in link link2 link3; do
  env LIB_LUA=src LIB_QHULL=src LIB_ZLIB=src LIB_CRT=libcmt LIB_RW="$lr" DROP_DSHOW=1 DROP_RW=1 DROP_RWTRAP=1 DROP_DINPUT=1 MKRANGE=1 GEN_DATA=1 IMPLIBS=1 HARNESS=1 \
      DELTA=$d SKIP_FILES=$SK RES=$RS $T/link_ours.sh $W/$k 0x400000 --automatch --automatch-near --bridges --clusters > $W/$k.log 2>&1 && break
  n=$(grep -o "DELTA mismatch: rerun with DELTA=[0-9a-f]*" $W/$k.log | tail -1); n=${n##*=}
  [ -n "$n" ] || { tail -8 $W/$k.log; echo "zneg: link failed ($W/$k.log)"; exit 1; }
  d=$n; echo "zneg: DELTA mismatch, next link with DELTA=$d"
done
[ -f $W/$k/ours.exe ] || { echo "zneg: no link after 3 DELTA tries"; exit 1; }
echo "zneg: linked without $F: $W/$k (DELTA=$d)"
python3 $T/origbytes.py $W/$k --out $W > $W/origbytes.log 2>&1 || { tail -5 $W/origbytes.log; echo "zneg: origbytes failed"; exit 1; }
python3 -I - $W/zero_original_bytes.json $G/output/zero_original_baseline.txt "$F" <<'PY'
import json, sys
j = json.load(open(sys.argv[1]))
cur = {"rsrc": j["resources"]["rsrc_original_bytes"]} if sys.argv[3] == "rsrc" else {"code:" + L: v.get("original", 0) for L, v in j["code"].items()}
old = {}
for l in open(sys.argv[2]):
    if l.strip() and l[0] != "#" and not l.startswith("state "):
        k, n = l.rsplit(None, 1); old[k] = int(n)
grown = [(k, old.get(k), cur[k]) for k in sorted(cur) if cur[k] > old.get(k, 0)]
for k, o, c in grown: print("GROWN %s %s -> %d" % (k, o, c))
if grown: print("zneg: PASS, the baseline compare FAILS without %s (%d layer(s) grew)" % (sys.argv[3], len(grown))); sys.exit(0)
if sys.argv[3] == "rsrc": sys.exit("zneg: FAIL, rsrc %d is not above the baseline's %s: the rsrc line would not notice linked resources (expected while the baseline line is still 710568)" % (cur["rsrc"], old.get("rsrc")))
sys.exit("zneg: FAIL, no code layer grew without %s: the zero line would not notice a dropped row" % sys.argv[3])
PY
