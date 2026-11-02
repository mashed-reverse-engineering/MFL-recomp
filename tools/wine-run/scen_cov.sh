#!/bin/bash
# scen_cov.sh <scenario> <ours.exe> <delta hex> <names file "name hexaddr" (standalone_fns.py)> [chunk i/n]: coverage run of ONE scenario on a standalone exe (no control):
# scenario script like scenario.sh SCEN_EXE (addrmove.py, harness symbols from <exe>.map), plus gdbcov with the (chunk of the) names -> output/scenario/<scenario>_cov<tag>/out/gdbcov.txt ("HIT name"), timing.txt.
# Env: SCEN_* as the scenario, SCEN_TAG (dir suffix), SLOT (default 1), RUN_TIMEOUT, SCEN_RWDLL.
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f $T/../..); N=$1; EXE=$(readlink -f $2); D=$3; NAMES=$(readlink -f $4); CH=${5:-1/1}
export MASHED_ROOT=${MASHED_ROOT:-$(readlink -f $R/..)}; SC=${SCEN_OUT:-$R/output/scenario}; B=$SC/_build; XMAP=${EXE%.exe}.map
W=$SC/${N}_cov${SCEN_TAG:+_$SCEN_TAG}; rm -rf $W; mkdir -p $W; SLOT=${SLOT:-1}
. $T/scenarios/lib.sh; . $T/scenarios/$N.sh
MAP=$B/ctrl.exe.map; export MAP
mapva() { local a=$(awk -v n="_$1" '$2==n{print $3; exit}' $XMAP); [ -n "$a" ] || { echo "harness symbol $1 not in $XMAP" >&2; echo 0; return; }; printf "%x" $((0x$a - 0x$D)); }
declare -f scenario_footer | grep -q count_table && scenario_footer() { echo "wait ${SCEN_END_WAIT:-1}"; echo "shot final"; }
{ echo "gdbcov gdbcov_names.txt"; scenario_header; scenario; scenario_footer; } | python3 $T/addrmove.py $D > $W/script.txt
python3 $T/prepare.py $R/../MashedGame/FullyLoaded/installed $W/run $EXE $XMAP > /dev/null || exit 1
RW=${SCEN_RWDLL:-$(dirname $EXE)/game/App_Executables/mflrw.dll}; [ -f "$RW" ] && cp -f $RW $W/run/App_Executables/mflrw.dll
awk -v i=${CH%/*} -v n=${CH#*/} '(NR-1)%n==i-1' $NAMES > $W/run/gdbcov_names.txt; wc -l < $W/run/gdbcov_names.txt > $W/nbreak.txt
export LOADMAX=${LOADMAX:-40}; LD=$(cut -d' ' -f1 /proc/loadavg); RT=$(awk -v t=${RUN_TIMEOUT:-400} -v l=$LD 'BEGIN{s=l/4; if(s<1)s=1; if(s>3)s=3; print int(t*s)}')
$T/mkpfx.sh $SLOT
PFX=wine_pfx_$SLOT PERSIST=slot$SLOT $T/gameslot.sh timeout -k 5 $RT $T/scen_play.sh $W/run $W/script.txt $W/out > /dev/null 2>&1; echo $? > $W/rc
echo "rc $(cat $W/rc) breakpoints $(cat $W/nbreak.txt) hits $(grep -c '^HIT ' $W/out/gdbcov.txt 2>/dev/null) unhandled $(grep -a -c Unhandled $W/out/wine.log 2>/dev/null) final $([ -f $W/out/final.png ] && echo yes || echo no)"
