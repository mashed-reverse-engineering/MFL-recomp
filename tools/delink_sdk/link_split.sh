#!/bin/bash
# Linked exe with libraries split out of the delinked image (M2 lib data step).  Usage: link_split.sh <fresh work dir>
# LIB_LUA / LIB_QHULL = source: Lua 4.0 / qhull 2002.1 rebuilt from upstream source replace the exe's bytes (plan_srclib.py); LIBS_SRC limits what is built.
# Per library of tools/delink_sdk/libs.cfg a variant switch: LIB_RW=sdk (split the planned RW members out, link the SDK objects with their own
# data) | original (keep the delinked original bytes); VARIANTS=1 also splits members whose game functions are another source revision of the
# SDK function (the SDK version replaces them); LIB_CRT=libcmt|original (default original: the libcmt split is not working yet).  RUN=1: race course 0.
# Steps: ownership.py (owner table, interface lists, lib names) -> rw_overlay_prep.py (member objects) -> plan_split.py (splittable members)
# -> link_ours.sh with EXTRA_OBJS + mkobj --drop-members.  ONLY=<file of member keys> restricts the plan (bisecting).
set -e
T=$(dirname "$(readlink -f "$0")"); G=$T/../..; W=$(readlink -f "$1")
[ -e $W ] && { echo "work dir $W exists (never reuse one: link.rsp lists o/*.obj)"; exit 1; }
mkdir -p $W/lib
python3 $T/ownership.py > $W/ownership.log
: > $W/lib/extra_objs.txt; : > $W/lib/members.txt
if [ "${LIB_RW:-sdk}" = sdk ]; then
  python3 $G/tools/rw_overlay_prep.py $W/lib/obj > /dev/null
  python3 $T/plan_split.py rw $W/lib/obj $W/lib/rw | tail -1
  cat $W/lib/rw/extra_objs.txt >> $W/lib/extra_objs.txt; cat $W/lib/rw/members.txt >> $W/lib/members.txt
fi
if [ "${LIB_CRT:-original}" = libcmt ]; then      # experimental: the CRT start-up stays original, its tables point at the split functions
  [ -d $W/lib/obj ] || python3 $G/tools/rw_overlay_prep.py $W/lib/obj > /dev/null
  python3 $T/plan_split.py crt $W/lib/obj $W/lib/crt | tail -1
  cat $W/lib/crt/extra_objs.txt >> $W/lib/extra_objs.txt; cat $W/lib/crt/members.txt >> $W/lib/members.txt; sort -u -o $W/lib/extra_objs.txt $W/lib/extra_objs.txt
fi
for L in lua qhull; do      # open source libs (libs.cfg variant = source, env LIB_LUA / LIB_QHULL = source|original): rebuilt from upstream source, the exe's code range of the lib is dropped
  V=$(eval echo \${LIB_$(echo $L | tr a-z A-Z):-$(python3 -c "import configparser; c = configparser.ConfigParser(); c.read('$T/libs.cfg'); print(c['$L'].get('variant', 'original'))")})
  [ "$V" = source ] || continue
  [ -d $W/lib/src ] || VC7_CONTAINER=libsrc $G/output/libmatch2/build_libs.sh $W/lib/src ${LIBS_SRC:-lua qhull} > $W/lib/src.log 2>&1
  python3 $T/plan_srclib.py $L $W/lib/src/$L/_o $W/lib/$L $ITEMS
  cat $W/lib/$L/extra_objs.txt >> $W/lib/extra_objs.txt; cat $W/lib/$L/members.txt >> $W/lib/members.txt; ITEMS=$W/lib/$L/items.csv
done
if [ -s $W/lib/members.txt ]; then      # stubs the split members need: IID_IUnknown (uuid.lib is not linked), errno (rtpng gzio, single thread libc)
  cp $T/stubs.c $W/lib/; VC7_CONTAINER=ac $G/tools/vc7.sh $W/lib cl.exe /nologo /c /O2 stubs.c > /dev/null
  echo "stub:stubs $W/lib/stubs.obj" >> $W/lib/extra_objs.txt
fi
while [ -n "$RUN" ] && [ "$(cut -d' ' -f1 /proc/loadavg | cut -d. -f1)" -ge 6 ]; do sleep 10; done      # load guard before the game start
EXTRA_OBJS=$W/lib/extra_objs.txt MKOBJ_ARGS="$MKOBJ_ARGS $([ -s $W/lib/members.txt ] && echo --drop-members $W/lib/members.txt) ${ITEMS:+--drop-items $ITEMS}" $G/tools/delink/link_ours.sh $W/link
if grep -qE "LNK4006|LNK2005" $W/link/link.log; then grep -E "LNK4006|LNK2005" $W/link/link.log | head -3; echo "WARNING: duplicate definitions (link_ours.sh links with /force:multiple)"; fi
