#!/bin/bash
# gate.sh [--dry-run] <kind> [names...]   the checks of ROADMAP "When to run which game check", nothing more.
#   kind: readability | tier1 | header | infra | tier2 | bridge | cluster | milestone
#   names: functions touched (tier2 / bridge / tier1; cluster: ignored, all enabled clusters are always gated); tier2 without names = the functions tools/shadow_cache.py says changed. '@file' = names from a file.
#   readability  check_all.sh
#   tier1        check_all + course 0 crash smoke
#   header       check_all + course 0 smoke (a full sweep only when tier 2 build bytes changed: run `gate.sh infra` then)
#   infra        check_all + union sweep, 15 courses (courses_par, P=2 x PAR=2 = 4 games; every game start takes one of 4 global slots, tools/wine-run/gameslot.sh; an INVALID course is repeated once with PAR=1; timeouts scale with load/4) + DRIVE=31 on courses 0,3
#   tier2        check_all + diffemu of the names (when tools/diffemu/diffemu.py exists; DIFF fails, INCONCLUSIVE warns) + targeted courses (tools/fn_courses.py) + DRIVE=31 courses 0,3 + shadow of the names (race, targeted courses; recorded in the shadow cache) + EARLY shadow, course 0
#   bridge       check_all + shadow of the bridges (BRIDGE_ENABLED=names, --shadow, course 0 + targeted courses) + targeted courses
#   cluster      check_all (4th strict build with --clusters) + targeted courses + DRIVE=31 courses 0,3 of a build with --clusters (names = cluster names = roots; none = all enabled in src/clusters/enabled.txt); no shadow (a cluster has no original-vs-C shadow), diffemu --cluster is the conformance proof
#   milestone    check_all + full sweep RUNS=2 with --count (refreshes output/fn_courses.csv) + DRIVE=31 sweep + win/lose (DRIVE=33 / 65) crash-only + full tier 2 shadow
# Game checks are crash-only (RUNS=1, SHOTS_RUNS=0, THRESH=100000: a run that does not freeze at the race frame / a stall / crash fails). Refuses to start at load > LOADMAX (default 6),
# every step has a hard timeout (STEP_TIMEOUT minutes, default 3x the estimate), one progress line per course, results in $GATE_DIR (default output/gate/<time>_<kind>).
# --dry-run prints the plan with the estimated minutes (rough: check_all 4, course 0.5 incl. cached control, sweep of 15 on P=2 about 6).  Exit 0 = all steps passed.
export LC_ALL=C
T=$(dirname "$(readlink -f "$0")"); R=$T/..; W=$T/wine-run
DRY=; [ "$1" = --dry-run ] && { DRY=1; shift; }
KIND=$1; shift; NAMES=(); for x in "$@"; do [ "${x:0:1}" = @ ] && NAMES+=($(cat "${x:1}")) || NAMES+=("$x"); done
case $KIND in readability|tier1|header|infra|tier2|bridge|cluster|milestone) ;; *) sed -n 2,3p "$0"; exit 2;; esac
D=${GATE_DIR:-$R/output/gate/$(date +%m%d_%H%M)_$KIND}
EXTRA="--automatch --automatch-near --bridges"
[ $KIND != cluster ] && [ -s $R/src/clusters/enabled.txt ] && EXTRA="$EXTRA --clusters"      # the shipped configuration: installed clusters own their entries (the older tier 2 definitions of the same functions are only stripped when --clusters is on); without it these gates tested a build nobody ships (2026-10-18)
CRASH="RUNS=1 SHOTS_RUNS=0 THRESH=100000 COUNT= COURSE_TIMEOUT=300"      # COUNT= : no coverage counters unless a step wants them
declare -a PLAN; est=0
step() { PLAN+=("$1|$2|$3"); est=$(awk "BEGIN{print $est+$2}"); }      # label | minutes | command (run with eval)
[ $KIND = bridge ] && [ ${#NAMES[@]} = 0 ] && { echo "bridge needs the bridge names"; exit 2; }
# ---- course lists
if [ $KIND = cluster ]; then      # names = cluster roots; the course lists use their entries; only those clusters are built in
  [ ${#NAMES[@]} -gt 0 ] && echo "note: cluster names ignored ('${NAMES[*]}'): a cluster whose callee is another installed cluster crashes when built alone; always gating ALL enabled clusters" && NAMES=()
  NAMES=($(grep -v '^#' $R/src/clusters/enabled.txt)); [ ${#NAMES[@]} = 0 ] && { echo "cluster: nothing enabled / no names"; exit 2; }
  export CLUSTER_ENABLED=$(IFS=,; echo "${NAMES[*]}"); EXTRA="$EXTRA --clusters"; ENT=($(python3 $T/cluster_install.py --entries "${NAMES[@]}")) || exit 2
  CL=$(python3 $T/fn_courses.py "${ENT[@]}" 2>/dev/null || echo "0 3"); [ -n "$CL" ] || CL="0 3"
  [ -n "${CLUSTER_COURSES:-}" ] && CL="$CLUSTER_COURSES"      # e.g. CLUSTER_COURSES="$(seq -s' ' 0 14)": the entries of most clusters run in few courses (fn_courses.csv is missing, the default is 0 3)
  [ -n "${CLUSTER_COUNT:-}" ] && CRASH="${CRASH/COUNT= /COUNT=--count }"      # CLUSTER_COUNT=1: coverage counters, never_executed.txt of the targeted step lists the entries no course ran
fi
if [ $KIND = tier2 -o $KIND = bridge ]; then
  if [ ${#NAMES[@]} = 0 -a $KIND = tier2 ]; then NAMES=($(python3 $T/shadow_cache.py changed)); echo "no names given: ${#NAMES[@]} tier 2 functions changed since the shadow cache"; fi
  CL=$(python3 $T/fn_courses.py "${NAMES[@]}" || echo "0 3"); [ -n "$CL" ] || CL="0 3"      # fn_courses notes (no course executes X) go to stderr; table missing -> courses 0 and 3
fi
nc() { echo $#; }
# names of tier2 go to a file (long lists); NF = that file, "-" = no list (full shadow)
NF=-; [ ${#NAMES[@]} -gt 0 -o $KIND = tier2 ] && NF=$D/names.txt
# ---- steps
step "check_all" 4 "$T/check_all.sh $D/check"
if [ $KIND = tier2 -a -f $T/diffemu/diffemu.py -a ${#NAMES[@]} -gt 0 ]; then step "diffemu ${#NAMES[@]} names" 3 "diffemu $NF"; fi
case $KIND in
  tier1|header) step "smoke course 0" 0.7 "courses 'smoke' '0' $CRASH";;
  infra)     step "union sweep 0-14" 6 "sweep union 0 14 $CRASH"; step "DRIVE=31 courses 0,3" 1.4 "courses drive31 '0 3' $CRASH DRIVE=31";;
  tier2)     step "targeted courses ($CL)" $(awk "BEGIN{print 0.6*$(nc $CL)}") "courses targeted '$CL' $CRASH"
             step "DRIVE=31 courses 0,3" 1.4 "courses drive31 '0 3' $CRASH DRIVE=31"
             step "shadow race ($CL)" $(awk "BEGIN{print 2+0.7*$(nc $CL)}") "shadow race '$CL' $NF"
             # EARLY shadow (boot-time wrapping) crashes today (known, see ROADMAP): opt-in with GATE_EARLY=1 until the crash search is done
             [ "${GATE_EARLY:-0}" = 1 ] && step "shadow EARLY course 0" 2.5 "shadow early '0' $NF";;
  bridge)    step "bridge shadow (course $CL)" $(awk "BEGIN{print 2+0.7*$(nc $CL)}") "bridgeshadow '$CL' ${NAMES[*]}"
             step "targeted courses ($CL)" $(awk "BEGIN{print 0.6*$(nc $CL)}") "courses targeted '$CL' $CRASH";;
  cluster)   step "targeted courses ($CL), clusters ${NAMES[*]}" $(awk "BEGIN{print 0.6*$(nc $CL)}") "courses targeted '$CL' $CRASH"
             step "DRIVE=31 courses 0,3" 1.4 "courses drive31 '0 3' $CRASH DRIVE=31";;
  milestone) step "full sweep 0-14 RUNS=2 --count" 12 "sweep full 0 14 RUNS=2 SHOTS_RUNS=1 COURSE_TIMEOUT=420 COUNT=--count"
             step "fn_courses refresh" 0.1 "python3 $T/fn_courses.py build $D/full"
             step "DRIVE=31 sweep" 6 "sweep drive31 0 14 $CRASH DRIVE=31"
             step "win crash-only (DRIVE=33, courses 0,3,7)" 2 "courses win '0 3 7' $CRASH DRIVE=33"
             step "lose crash-only (DRIVE=65, courses 0,3,7)" 2 "courses lose '0 3 7' $CRASH DRIVE=65"
             step "AI scenarios (cup rows 0 7 10 12 13)" 15 "scen_check cup '0:1 7:1 10:1 12:2 13:2'"
             step "full tier 2 shadow, all courses" 10 "shadow race '$(seq -s' ' 0 14)' -";;
esac
[ $KIND = tier2 -a ${#NAMES[@]} = 0 ] && echo "note: no changed tier 2 functions, shadow steps will be skipped"
echo "gate $KIND: ${#PLAN[@]} steps, about $est min, output $D"
i=0; for p in "${PLAN[@]}"; do i=$((i+1)); IFS='|' read -r l m c <<< "$p"; echo "  $i. $l  (~$m min)"; [ -n "$DRY" ] && echo "       ${c:0:230}"; done
[ -n "$DRY" ] && exit 0
# ---- guards
L=$(cut -d' ' -f1 /proc/loadavg | cut -d. -f1); [ $L -gt ${LOADMAX:-6} ] && { echo "load $L too high (LOADMAX ${LOADMAX:-6}), not starting"; exit 1; }
mkdir -p $D; $T/provenance.sh $D; exec > >(tee -a $D/gate.log) 2>&1; [ $NF != - ] && printf "%s\n" "${NAMES[@]}" > $NF; CHILD=; trap '[ -n "$CHILD" ] && kill -TERM $CHILD 2>/dev/null; echo "gate interrupted"; exit 130' INT TERM
failcheck() { grep -E "regress failed|timed out|INVALID| [1-9][0-9]* warnings" "$1"; }      # lines that mean a crash / stall / non-frozen run
courses() {       # courses <tag> '<list>' [ENV=..]...: one courses.sh call per course (sequential, PAR=4 inside), a progress line each
  local tag=$1 list=$2; shift 2; local c n=0 tot=$(nc $list)
  for c in $list; do n=$((n+1)); echo "[$(date +%H:%M)] $tag: course $c ($n/$tot)"
    env EXTRA_OVL="$EXTRA" "$@" PAR=4 $W/courses.sh $D/$tag $c $c 90 >> $D/$tag.log 2>&1; tail -1 $D/$tag.log | cut -c1-200; done
  ! failcheck $D/$tag.log
}
sweep() {         # sweep <tag> <a> <b> [ENV=..]...: courses_par.sh P=2 (2 x PAR 2 game processes), progress lines polled from the part logs
  local tag=$1 a=$2 b=$3; shift 3; local seen=0 n
  env EXTRA_OVL="$EXTRA" "$@" WALL=${WALL:-$(awk -v l=$(cut -d" " -f1 /proc/loadavg) "BEGIN{s=l/4; if(s<1)s=1; if(s>3)s=3; print int(2400*s)}")} $W/courses_par.sh $D/$tag 2 $a $b 90 > $D/$tag.log 2>&1 & CHILD=$!
  while kill -0 $CHILD 2>/dev/null; do sleep 15; n=$(grep -h "^course" $D/$tag/p*.log 2>/dev/null | wc -l)
    [ $n -gt $seen ] && { echo "[$(date +%H:%M)] $tag: $n/$((b-a+1)) courses done"; seen=$n; }; done
  wait $CHILD; CHILD=; cat $D/$tag.log | cut -c1-200
  ! failcheck $D/$tag.log
}
scen_check() {   # scen_check <scenario> '<row>:<right> ...': tools/wine-run/scenario.sh runs on the shipped build; a Wine crash of the overlay (control clean) fails.  AI route-following (cup 'Race Game' events) was not reached by any course sweep: ai_find_target_b crashed there while every other gate passed (2026-10-18)
  local sc=$1 spec rc=0 row right out; for spec in $2; do row=${spec%%:*}; right=${spec##*:}
    out=$(SCEN_ROW=$row SCEN_RIGHT=$right NOGDB=1 SCEN_TAG=g$$ SCEN_OUT=$D/scen $W/scenario.sh $sc 2>&1 | grep -E '^(ctrl|ovl)'); echo "scenario $sc row $row right $right: $(echo $out | tr '\n' ' ')"
    echo "$out" | grep -q '^ovl : clean' || rc=1; done; return $rc
}
shadow() {        # shadow <race|early> '<courses>' <names file|->: shadowgen2 --changed (tier 2 cache), build with T2_SHADOW_DIR, merge the logs; DIFF = fail; race results go to the cache
  local mode=$1 list=$2 nf=$3 g=$D/shadow_$mode a=()
  [ "$nf" != - ] && a=($nf); [ "$nf" != - -a $mode = race ] && a+=(--changed)      # EARLY re-tests the named functions regardless of the cache (the race step has just recorded them)
  [ $mode = early ] && export EARLY=1
  python3 $T/shadowgen2.py $R/src/automatch_near $g/src "${a[@]}" || return 1
  [ -s $g/src/shadow_names.txt ] || { echo "nothing to shadow ($mode)"; return 0; }
  courses shadow_$mode "$list" $CRASH T2_SHADOW_DIR=$g/src RELCHECK=off || return 1
  python3 $W/shadow_merge.py $D/shadow_$mode $g/src/shadow_names.txt $g/table.txt $([ $mode = race ] && echo --record) | tee $g/summary.txt
  ! grep -q " DIFF " $g/table.txt
}
diffemu() {       # diffemu <names file>: DIFF fails, INCONCLUSIVE only warns (needs the unicorn venv, see README)
  python3 $T/diffemu/diffemu.py --names $1 2>&1 | tee $D/diffemu.log | tail -15
  if [ -n "${HS:-}" ]; then      # HS=1 (opt-in): additionally the state-seeded harness of tools/diffemu_audit/tweak (README there); same DIFF / INCONCLUSIVE rules
    ~/.cache/mfl_diffemu_venv/bin/python $T/diffemu_audit/tweak/gate_hs.py $1 ${HS_CASES:-1500} ${HS_BUDGET:-300} 2>&1 | tee -a $D/diffemu.log | tail -15
  fi
  # NaN-form ratchet (T-weak3): a tier 2 function with a compare that treats NaN the other way than the original (tools/diffemu_audit/tweak/nancmp.py --flips; src/nancmp_allow.txt for justified cases)
  ~/.cache/mfl_diffemu_venv/bin/python $T/diffemu_audit/tweak/nancmp.py --flips $(cat $1) 2>&1 | tee -a $D/diffemu.log | tail -8
  ! grep "^FLIP " $D/diffemu.log | grep -qv "(allowed)" || { echo "nancmp: NaN-form FLIP (a compare takes NaN the other side than the original)"; return 1; }
  ! grep -q "^DIFF " $D/diffemu.log || { echo "diffemu: DIFF verdict"; return 1; }
  grep -q "^INCONCLUSIVE " $D/diffemu.log && echo "WARNING diffemu: INCONCLUSIVE verdicts (see $D/diffemu.log)"; return 0
}
bridgeshadow() {  # bridgeshadow '<courses>' names...: the bridged functions as shadow build (--shadow, only the named bridges enabled)
  local list=$1; shift; local en; en=$(IFS=,; echo "$*")
  courses bridge_shadow "$list" $CRASH EXTRA_OVL="--automatch --bridges --shadow" BRIDGE_ENABLED="$en" RELCHECK=off || return 1
  local f ok=0; for f in $D/bridge_shadow/c*/out_ovl_1/shadowlog.bin; do python3 $W/shadow_report.py $f $R/src/bridges_shadow/shadow_names.txt | tee -a $D/bridge_shadow/report.txt | grep -iE "diff" | grep -E "$(IFS='|'; echo "$*")" && ok=1; done
  [ $ok = 0 ]
}
export -f courses sweep shadow diffemu bridgeshadow failcheck nc scen_check; export T R W D CRASH EXTRA
i=0
for p in "${PLAN[@]}"; do i=$((i+1)); IFS='|' read -r l m c <<< "$p"
  echo "=== [$(date +%H:%M)] step $i/${#PLAN[@]}: $l"
  timeout -k 10 $(awk -v l=$(cut -d" " -f1 /proc/loadavg) "BEGIN{s=l/4; if(s<1)s=1; if(s>2)s=2; print int(${STEP_TIMEOUT:-$(awk "BEGIN{print 3*$m+5}")}*60*s)}") bash -c "$c" & CHILD=$!; wait $CHILD; rc=$?; CHILD=
  [ $rc = 0 ] || { echo "GATE FAILED at step $i ($l), exit $rc; logs in $D"; echo "  next: groupsearch only on the failing course (wine-run/percourse.sh / groupsearch.py)"; exit 1; }
done
echo "GATE $KIND PASSED ($i steps)"
