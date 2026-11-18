#!/bin/bash
# sa_oracles.sh <data|render|scen|sweep|training|scenset> <link dir> <delta> <out dir>: whole-program oracles of `gate.sh standalone` (plan 2026-11-03 lane A). Exit 0 = ok.
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
#   training training lessons played to the end (scenarios/training_tt.sh, SCEN_DRIVE 287), TT_P parallel (default 2), TT_SECS (W12 hx5a: safety cap only, default 600; a judged lesson ends on its done word; RUN_TIMEOUT = TT_SECS + 300), TT_CRASH_SECS (crash-only rows, fixed play time, default 200).  Judged rows (TT_ROWS,
#           default 0 2 3): scenario.sh RESULT + wine-run/tt_check.py (done mark, retry-free step / text-id progression, GameState; UNJUDGED fails).
#           Crash-only rows (TT_CRASHONLY, default 5: its outcome is noise in the control too): Wine crash / rc only.
#   scenset  (TS W11, not a gate step in W11) the judged scenario set: each entry of SS (menus, options pages, load / save, pause, arcade / cup win and lose,
#           time trial, multi standard / team, load icon, two races) runs twice with a fresh control (scenarios/scenset.sh wraps the base scenario and dumps
#           GameState at an end event); wine-run/scen_judge.py on the pair: controls agree, ours equals, no crash, no mfl_trap.log.  SS_P parallel
#           scenario.sh (default 2 = 4 game processes), SS_ONLY="<tags>" a subset, SS_MIN judged PASSes needed (default: all race-free entries of the run).  The race entries whose
#           controls differ without it run with SCEN_PIN=1 (wine-run/scen_pin.sh: d3d9 proxy + V2's pins; needs the b8snd proxy with vdssnap).
# scen / sweep / training: a crash fails at once, any other failure is retried once; a PASS on the retry is appended to output/scenario/flaky.tsv (the flaky ledger, shared with
# wine-run/sasweep.py; FLAKY_TSV overrides).  Wine kinds refuse to start while a gate holds gate.lock, unless the gate calls them (MFL_IN_GATE).  The control exe
# output/scenario/_build/ctrl.exe is checked once here (ctrlcheck: rebuilt when missing, REBUILD=1, or built from other harness sources than this tree's;
# then REBUILD is dropped: parallel scenario.sh runs would all rebuild the same file).
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
ctrlcheck() {   # W12 (harness review finding 1): one content check of the tree's control before any scenario starts; its lines (rebuilt / CTRL STALE / control: sources)
  # go to this script's output = the gate log.  scenario.sh --ctrl rebuilds when ctrl.exe.build.json differs from this tree's harness sources (or REBUILD=1)
  SCEN_OUT=$R/output/scenario $S --ctrl 2>&1 || { echo "sa_oracles.sh: control check failed"; exit 1; }; unset REBUILD; }
scenlinks() { mkdir -p $O/scen; for x in _build _ctrlcache; do [ -e $O/scen/$x ] || ln -s $R/output/scenario/$x $O/scen/$x; done; ctrlcheck; }
case $K in render|scen|sweep|training|scenset) [ -n "$MFL_IN_GATE" ] || flock -s -n ${GATE_LOCK:-/tmp/claude-$(id -u)/gate.lock} true || { echo "sa_oracles.sh: REFUSED: a gate is running (gate.lock held)"; exit 2; };; esac
case $K in
  data) n=$(awk '$3==".data" && $4 ~ /^DATA/ {print $2; exit}' $L/ours.map); echo ".data length ${n:-missing} (want ${DATA_LEN:-00052000H})"; [ "$n" = "${DATA_LEN:-00052000H}" ];;
  render)
    ctrlcheck; D3=$R/tools/d3dtrace; CR=$R/output/scenario/_ctrlcache/render; LF=$D3/render_limits.txt; TG=${RENDER_TARGETS:-$(awk '!/^#/ && NF {print $1}' $LF)}
    lim() { awk -v t=$1 '$1==t {$1=""; print; f=1} END {if (!f) print "40 0 0 0"}' $LF; }      # lim <target>: frames prim mhash unmatched [masked key prefixes]
    rtrace() { local a=($(lim $1)); ( [ -n "$3" ] && export SA_EXE=$L/ours.exe SA_DELTA=$DL; DRIVE=31 $D3/run.sh $1 $2 ${a[0]} > $2.log 2>&1 ); }      # rtrace <target> <out> [ours]
    # W12 hx2 (finding 6): the key also covers the player scripts, the game-dir preparer, the import patcher and the proxy env; "v2" = older stores ignored
    cdir() { echo $CR/${1/:/}-v2-$( (echo "$1 $(lim $1 | cut -d' ' -f1-2) 31 env ALOCK=$ALOCK SNDPIN=$SNDPIN STEP_US=$STEP_US GEOM=$GEOM"; cat $R/output/scenario/_build/ctrl.exe $D3/d3d9.dll $D3/run.sh $D3/normalise.py $R/tools/wine-run/race_script.sh $R/tools/wine-run/scenarios/lib.sh $R/tools/wine-run/play.sh $R/tools/wine-run/play_inner.sh $R/tools/wine-run/prepare.py $D3/patch_import.py) | sha1sum | cut -c1-16); }
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
  training)
    scenlinks; rc=0; JR=${TT_ROWS-0 2 3}; CR=${TT_CRASHONLY-5}
    for r in $JR $CR; do while [ $(jobs -rp | wc -l) -ge ${TT_P:-2} ]; do wait -n; done
      case " $CR " in *" $r "*) $S $A SCEN_TAG=tt$r SCEN_ROW=$r SCEN_SECS=${TT_CRASH_SECS:-200} SCEN_IMGTHRESH=off training_tt > $O/tt$r.log 2>&1 &;;
                      *) one tt$r SCEN_ROW=$r SCEN_SECS=${TT_SECS:-600} RUN_TIMEOUT=$(( ${TT_SECS:-600} + 300 )) SCEN_IMGTHRESH=off training_tt &;; esac; done; wait
    for r in $CR; do l=$(grep -h "^ours:" $O/tt$r.log); echo "training row $r (crash-only): ${l:-no ours: line ($(tail -1 $O/tt$r.log))}"
      echo "$l" | grep -q "^ours: clean  rc 0$" || rc=1; done
    for r in $JR; do report tt$r || rc=1      # tt_check judges the final try only (one: the retry overwrote the run dir)
      python3 -I $R/tools/wine-run/tt_check.py $O/scen/training_tt_sa_tt$r > $O/tt$r.check 2>&1; echo "training row $r: $(tail -1 $O/tt$r.check)"
      grep -q "^TT: PASS" $O/tt$r.check || { grep -E "^  [^ ]" $O/tt$r.check | head -3; rc=1; }; done
    exit $rc;;
  scenset)
    scenlinks; rc=0; np=0; nj=0
    SS=("ss_menus all SCEN_BASE=menus_main" "ss_options all SCEN_BASE=options" "ss_optall all SCEN_BASE=options_all" "ss_optsave all SCEN_BASE=options_save"
        "ss_pause none SCEN_BASE=pause_all SCEN_MODE=training" "ss_arc_c0 none SCEN_PIN=1 SCEN_BASE=arcade SCEN_DRIVE=31"
        "ss_arc_win none SCEN_BASE=arcade SCEN_DRIVE=41 SCEN_AFTER=win SCEN_END=now" "ss_arc_lose none SCEN_BASE=arcade SCEN_DRIVE=73 SCEN_AFTER=end SCEN_END=phase:10"
        "ss_cup_win none SCEN_BASE=cup SCEN_DRIVE=33 SCEN_AFTER=end SCEN_END=phase:10" "ss_cup_lose none SCEN_BASE=cup SCEN_DRIVE=65 SCEN_AFTER=win SCEN_END=menu:5"
        "ss_tt none SCEN_PIN=1 SCEN_BASE=timetrial SCEN_SECS=1 SCEN_END=freeze:900" "ss_multi none SCEN_PIN=1 SCEN_BASE=multi" "ss_team none SCEN_BASE=multi SCEN_TEAM=1"
        "ss_loadicon none SCEN_BASE=load_icon SCEN_NOLIMIT=0" "ss_tworaces none SCEN_PIN=1 SCEN_BASE=tworaces"
        "ss_les0 all SCEN_BASE=lesson_page SCEN_ROW=0" "ss_les1 all SCEN_BASE=lesson_page SCEN_ROW=1" "ss_les2 all SCEN_BASE=lesson_page SCEN_ROW=2"
        "ss_les3 all SCEN_BASE=lesson_page SCEN_ROW=3" "ss_les4 all SCEN_BASE=lesson_page SCEN_ROW=4" "ss_les5 all SCEN_BASE=lesson_page SCEN_ROW=5"
        "ss_arcpages all SCEN_BASE=arcade_pages SCEN_ROW=2 SCEN_RIGHT=1" "ss_cuppages all SCEN_BASE=cup_pages SCEN_ROW=1 SCEN_RIGHT=1"
        "ss_multipages all SCEN_BASE=multi_pages" "ss_teampages all SCEN_BASE=multi_pages SCEN_TEAM=1" "ss_anims none SCEN_BASE=menus_anims")
    # W12 TS2: SS_SET=gate = the stable race-free subset (about 30 min at SS_P=2); the full set (26 entries, about 65 min) runs at the freeze
    [ "$SS_SET" = gate ] && [ -z "$SS_ONLY" ] && SS_ONLY="ss_menus ss_options ss_optsave ss_pause ss_team ss_loadicon ss_les0 ss_les2 ss_les3 ss_arcpages ss_cuppages ss_multipages ss_teampages ss_arc_win"      # W14 A1: ss_arc_win back (end event phase 10 + the results-page key, arc_results in lib.sh); ss_cup_lose still out
    [ -n "$SS_ONLY" ] && { X=(); for e in "${SS[@]}"; do case " $SS_ONLY " in *" ${e%% *} "*) X+=("$e");; esac; done; SS=("${X[@]}"); }
    for e in "${SS[@]}"; do set -- $e; for h in a b; do while [ $(jobs -rp | wc -l) -ge ${SS_P:-2} ]; do wait -n; done
      $S $A NOCACHE=1 SCEN_IMGTHRESH=off SCEN_TAG=$1_$h "${@:3}" scenset > $O/$1_$h.log 2>&1 & done; done; wait
    # W12 TS2 rule: only race-free entries (no SCEN_PIN=1) are counted against SS_MIN; the pinned race entries are printed as info (blocked by the sound pin
    # until ours' DirectSound creation is hooked).  SS_MIN ratchet (session 2, 22 of 22 and the 13 gate entries all passed twice): default SS_MIN = every race-free entry of the run (22 full set, 13 SS_SET=gate); a new entry raises it by itself, SS_MIN=n only lowers it by hand for a diagnosis run.
    pp=0; pn=0
    for e in "${SS[@]}"; do set -- $e; pin=; case "$e" in *SCEN_PIN=1*) pin=1;; esac; [ -n "$pin" ] && pn=$((pn+1)) || nj=$((nj+1))
      python3 -I $R/tools/wine-run/scen_judge.py --shots $2 $O/scen/scenset_sa_$1_a $O/scen/scenset_sa_$1_b > $O/$1.judge 2>&1
      echo "scenset $1${pin:+ (race-pinned, info only)}: $(tail -1 $O/$1.judge)"
      if grep -q ": PASS$" $O/$1.judge; then [ -n "$pin" ] && pp=$((pp+1)) || np=$((np+1)); else grep "^  " $O/$1.judge | head -3; fi; done
    echo "scenset: $np of $nj race-free scenarios passed (want ${SS_MIN:-$nj}); race-pinned $pp of $pn (info only)"; [ $np -ge ${SS_MIN:-$nj} ] || rc=1; exit $rc;;
  *) sed -n 2,27p $0; exit 2;;
esac
