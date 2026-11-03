#!/bin/bash
# sa_oracles.sh <data|render|scen> <link dir> <delta> <out dir>: whole-program oracles of `gate.sh standalone` (plan 2026-11-03 lane A). Exit 0 = ok.
#   data    <link dir>/ours.map: .data length must be 00052000H.  An initialised writable static in a new object grows .data and shifts the original bss
#           that original-byte code addresses absolutely (T-P15: 00052004H, every course crashed).  Change DATA_LEN only when the DELTA contract changes.
#   render  d3dtrace course:0 DRIVE=31, 40 race frames, of ours.exe and of the control (both deterministic run to run), compared by d3dtrace/drawcmp.py:
#           prim / mhash per aligned draw pair, unmatched draws.  Limits RENDER_LIMITS="prim mhash unmatched" (link 1659 vs control: 24 305 762;
#           T-P18 swapped Base/MinVertexIndex, link 1556: 6712 6993 736; T-P19 timer_ticks lost the high dword, race camera off: 24 305 813).
#           Noise floor (T-P19, overlay with the fix): ctrl vs ctrl 0 0 0, ours vs ours 0 0 0, ours vs ctrl 0 1 0 (frame 0 draw 0 `pre.events`: our
#           intro loop runs with the null video backend, the control's is stubbed by prepare.py, so ours has two extra device resets).
#   scen    tools/wine-run/scenario.sh on the link: options_all, pause_all training, menus_main, arcade_win (control cached in output/scenario/_ctrlcache).
#           FAIL on a Wine Unhandled, rc != 0, title not reached, unhealthy control or a GameState dump difference; the final screenshot is NOT judged
#           (race shots differ run to run, render covers the picture); a non-crash failure is retried once.  T-P17 (link 1228, menu_page_enter without volatile): pause_all training crashes.
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f $T/../..); K=$1; L=$(readlink -f "${2:?link dir}"); DL=${3:?delta}; O=${4:?out dir}; mkdir -p $O
case $K in
  data) n=$(awk '$3==".data" && $4 ~ /^DATA/ {print $2; exit}' $L/ours.map); echo ".data length ${n:-missing} (want ${DATA_LEN:-00052000H})"; [ "$n" = "${DATA_LEN:-00052000H}" ];;
  render)
    for s in ours ctrl; do
      ( [ $s = ours ] && export SA_EXE=$L/ours.exe SA_DELTA=$DL; DRIVE=31 $R/tools/d3dtrace/run.sh course:0 $O/$s 40 > $O/$s.log 2>&1 ) & done; wait
    for s in ours ctrl; do [ -s $O/$s/trace.d3d.gz ] || { echo "render: no trace for $s"; tail -3 $O/$s.log; exit 1; }; done
    python3 $R/tools/d3dtrace/drawcmp.py $O/ours/trace.d3d.gz $O/ctrl/trace.d3d.gz ${RENDER_LIMITS:-0 1 0};;
  scen)
    mkdir -p $O/scen; for x in _build _ctrlcache; do [ -e $O/scen/$x ] || ln -s $R/output/scenario/$x $O/scen/$x; done
    S=$R/tools/wine-run/scenario.sh; A="SCEN_EXE=$L/ours.exe SCEN_DELTA=$DL SCEN_OUT=$O/scen"
    verdict() {   # verdict <log>: the failure reasons except the screenshot one (empty = ok)
      local v=$(grep -h "^RESULT:" $1); [ -n "$v" ] || v="RESULT: FAIL (no result: $(tail -1 $1))"
      echo "$v" | sed -e 's/^RESULT: FAIL (\(.*\))$/\1/' -e 's/^RESULT: PASS$//' | tr ';' '\n' | grep -v "final screenshot differs" | grep -v '^ *$'; }
    one() {       # one <name> <scenario.sh args>: a crash fails at once; any other failure (GameState: pause_all training differed once in 2 runs of link 1659) is retried once
      local f=$1; shift; $S $A "$@" > $O/$f.log 2>&1; local r=$(verdict $O/$f.log)
      [ -n "$r" ] && ! echo "$r" | grep -q crashed && { echo "scen $f: retry after: $r" > $O/$f.retry; $S $A "$@" SCEN_TAG=${f}_retry > $O/$f.log 2>&1; }
      return 0; }
    one options_all options_all & one pause_all_training SCEN_MODE=training SCEN_TAG=training pause_all & one menus_main menus_main &
    one arcade_win SCEN_TAG=win SCEN_DRIVE=41 SCEN_AFTER=win SCEN_SECS=25 arcade & wait; rc=0
    for f in options_all pause_all_training menus_main arcade_win; do
      r=$(verdict $O/$f.log); cat $O/$f.retry 2>/dev/null
      echo "scen $f: $(grep -h "^RESULT:" $O/$f.log)$(grep -h "^GameState" $O/$f.log | sed 's/^/  /')"; [ -z "$r" ] || { echo "scen $f: FAIL: $r"; rc=1; }
    done; exit $rc;;
  *) sed -n 2,11p $0; exit 2;;
esac
