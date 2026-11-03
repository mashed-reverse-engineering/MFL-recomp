#!/bin/bash
# sa_oracles.sh <data|render|scen|sweep> <link dir> <delta> <out dir>: whole-program oracles of `gate.sh standalone` (plan 2026-11-03 lane A). Exit 0 = ok.
#   data    <link dir>/ours.map: .data length must be 00052000H.  An initialised writable static in a new object grows .data and shifts the original bss
#           that original-byte code addresses absolutely (T-P15: 00052004H, every course crashed).  Change DATA_LEN only when the DELTA contract changes.
#   render  d3dtrace of every RENDER_TARGETS (default: the targets of tools/d3dtrace/render_limits.txt, course:0 course:7 course:12 title; races DRIVE=31,
#           40 race frames): ours.exe twice and the control, compared by d3dtrace/drawcmp.py (prim / mhash per aligned draw pair, unmatched draws).
#           ours vs control: frames, limits "prim mhash unmatched" and masked keys per target from render_limits.txt (measured noise there);
#           ours vs ours (determinism, M3): 0 0 0.  The control trace is cached in
#           output/scenario/_ctrlcache/render/<target>-<key of ctrl.exe, proxy dll, scripts>/.  Known bad: T-P18 swapped Base/MinVertexIndex (link 1556,
#           course 0: 6712 6993 736, course 7: 7144 7511 915), T-P19 timer_ticks lost the high dword (race camera off, link 2152, course 0: 24 305 813,
#           course 12: 197 278 376 and ours vs ours 68 69 60, title: unmatched 1488 > 1350).
#   scen    tools/wine-run/scenario.sh on the link: options_all, pause_all training, menus_main, arcade_win (control cached in output/scenario/_ctrlcache).
#           FAIL on a Wine Unhandled, rc != 0, title not reached, unhealthy control, a GameState dump difference or a final menu screenshot that differs
#           (race scenarios run with SCEN_IMGTHRESH=off).  T-P17 (link 1228, menu_page_enter without volatile): pause_all training crashes.
#   sweep   mini course sweep through the menus: arcade rows 0-14 (RIGHT 0) and cup rows 0-14 (RIGHT row%3), SCEN_DRIVE=31, freeze at race frame 300;
#           scenario.sh verdicts (no screenshot compare) + colorcheck of every race_start.png.  SWEEP_P parallel scenarios (default 4, gameslot.sh caps), SWEEP_ROWS (default 0..14).
# scen / sweep: a crash fails at once, any other failure is retried once; a PASS on the retry is appended to output/scenario/flaky.tsv (the flaky ledger, shared with
# wine-run/sasweep.py; FLAKY_TSV overrides).
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f $T/../..); K=$1; L=$(readlink -f "${2:?link dir}"); DL=${3:?delta}; O=${4:?out dir}; mkdir -p $O
S=$R/tools/wine-run/scenario.sh; A="SCEN_EXE=$L/ours.exe SCEN_DELTA=$DL SCEN_OUT=$O/scen"
verdict() {   # verdict <log>: the failure reasons (empty = ok)
  local v=$(grep -h "^RESULT:" $1); [ -n "$v" ] || v="RESULT: FAIL (no result: $(tail -1 $1))"
  echo "$v" | sed -e 's/^RESULT: FAIL (\(.*\))$/\1/' -e 's/^RESULT: PASS$//' | tr ';' '\n' | grep -v '^ *$'; }
one() {       # one <name> <scenario.sh args>: a crash fails at once; any other failure (GameState: pause_all training differed once in 2 runs of link 1659) is retried once
  local f=$1; shift; $S $A SCEN_TAG=$f "$@" > $O/$f.log 2>&1; local r=$(verdict $O/$f.log)
  [ -n "$r" ] && ! echo "$r" | grep -q crashed || return 0
  echo "scen $f: retry after: $r" > $O/$f.retry; mv $O/$f.log $O/$f.first.log; $S $A SCEN_TAG=$f "$@" > $O/$f.log 2>&1
  local F=${FLAKY_TSV:-$R/output/scenario/flaky.tsv}; [ -z "$(verdict $O/$f.log)" ] || return 0      # a FAIL on the retry too is a real failure, not flaky
  [ -s $F ] || printf "date\tlink\tjob\tfirst try\n" > $F      # same columns as wine-run/sasweep.py
  printf "%s\t%s\t%s\t%s\n" "$(date '+%F %H:%M')" $(basename $(dirname $L)) "sa_oracles $K $f $*" "$(echo $r)" >> $F; }      # unquoted echo: one line
report() {    # report <name>: verdict line, FAIL line -> rc 1
  local r=$(verdict $O/$1.log); cat $O/$1.retry 2>/dev/null
  echo "scen $1: $(grep -h "^RESULT:" $O/$1.log)$(grep -h "^GameState" $O/$1.log | sed 's/^/  /')"; [ -z "$r" ] || { echo "scen $1: FAIL: $r"; return 1; }; }
scenlinks() { mkdir -p $O/scen; for x in _build _ctrlcache; do [ -e $O/scen/$x ] || ln -s $R/output/scenario/$x $O/scen/$x; done; }
case $K in
  data) n=$(awk '$3==".data" && $4 ~ /^DATA/ {print $2; exit}' $L/ours.map); echo ".data length ${n:-missing} (want ${DATA_LEN:-00052000H})"; [ "$n" = "${DATA_LEN:-00052000H}" ];;
  render)
    D3=$R/tools/d3dtrace; CR=$R/output/scenario/_ctrlcache/render; LF=$D3/render_limits.txt; TG=${RENDER_TARGETS:-$(awk '!/^#/ && NF {print $1}' $LF)}
    lim() { awk -v t=$1 '$1==t {$1=""; print; f=1} END {if (!f) print "40 0 0 0"}' $LF; }      # lim <target>: frames prim mhash unmatched [masked key prefixes]
    rtrace() { local a=($(lim $1)); ( [ -n "$3" ] && export SA_EXE=$L/ours.exe SA_DELTA=$DL; DRIVE=31 $D3/run.sh $1 $2 ${a[0]} > $2.log 2>&1 ); }      # rtrace <target> <out> [ours]
    cdir() { echo $CR/${1/:/}-$( (echo "$1 $(lim $1 | cut -d' ' -f1-2) 31"; cat $R/output/scenario/_build/ctrl.exe $D3/d3d9.dll $D3/run.sh $D3/normalise.py $R/tools/wine-run/race_script.sh $R/tools/wine-run/scenarios/lib.sh) | sha1sum | cut -c1-16); }
    for t in $TG; do n=${t/:/}; [ -s $(cdir $t)/trace.d3d.gz ] || rtrace $t $O/ctrl_$n & rtrace $t $O/ours_$n 1 & rtrace $t $O/ours2_$n 1 & done; wait
    rc=0; for t in $TG; do n=${t/:/}; C=$(cdir $t); a=($(lim $t))
      [ -s $C/trace.d3d.gz ] || { grep -q " rc=0 " $O/ctrl_$n/run.txt 2>/dev/null && [ -s $O/ctrl_$n/trace.d3d.gz ] && mkdir -p $C && cp $O/ctrl_$n/trace.d3d.gz $O/ctrl_$n/run.txt $C/; }
      for s in ours ours2; do [ -s $O/${s}_$n/trace.d3d.gz ] || { echo "render $t: no trace for $s"; tail -3 $O/${s}_$n.log; rc=1; continue 2; }; done
      [ -s $C/trace.d3d.gz ] || { echo "render $t: no control trace"; tail -3 $O/ctrl_$n.log; rc=1; continue; }
      echo "render $t ours vs control:"; python3 $D3/drawcmp.py $O/ours_$n/trace.d3d.gz $C/trace.d3d.gz "${a[@]:1}" | sed 's/^/  /'; [ ${PIPESTATUS[0]} = 0 ] || rc=1
      echo "render $t ours vs ours:"; python3 $D3/drawcmp.py $O/ours_$n/trace.d3d.gz $O/ours2_$n/trace.d3d.gz 0 0 0 "${a[@]:4}" | sed 's/^/  /'; [ ${PIPESTATUS[0]} = 0 ] || rc=1
    done; exit $rc;;
  scen)
    scenlinks
    one options_all options_all & one pause_all_training SCEN_MODE=training pause_all & one menus_main menus_main &
    one arcade_win SCEN_DRIVE=41 SCEN_AFTER=win SCEN_SECS=25 SCEN_IMGTHRESH=off arcade & wait; rc=0
    for f in options_all pause_all_training menus_main arcade_win; do report $f || rc=1; done; exit $rc;;
  sweep)
    scenlinks; J=(); X="SCEN_DRIVE=31 SCEN_IMGTHRESH=off"; for r in ${SWEEP_ROWS:-$(seq 0 14)}; do J+=("sw_a$r SCEN_ROW=$r SCEN_RIGHT=0 $X arcade" "sw_c$r SCEN_ROW=$r SCEN_RIGHT=$((r % 3)) $X cup"); done
    for j in "${J[@]}"; do while [ $(jobs -rp | wc -l) -ge ${SWEEP_P:-4} ]; do wait -n; done; one $j & done; wait
    rc=0; np=0; for j in "${J[@]}"; do set -- $j; f=$1
      report $f > $O/$f.rep || { cat $O/$f.rep; rc=1; continue; }; cat $O/$f.retry 2>/dev/null
      ls $O/scen/*_sa_$f/out_ours/race_start.png > /dev/null 2>&1 || { echo "scen $f: FAIL: no race_start.png"; rc=1; continue; }; np=$((np+1)); done
    echo "sweep: $np of ${#J[@]} scenarios passed"
    python3 -I $T/colorcheck.py $O/scen 1000 '*_sa_sw_*/out_ours/race_start.png' || rc=1; exit $rc;;
  *) sed -n 2,18p $0; exit 2;;
esac
