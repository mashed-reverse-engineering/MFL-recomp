#!/bin/bash
# negtest.sh: negative fixtures of the whole-program oracles (plan_testing item 2), no game run, about 5 s.  Every oracle must FAIL on its stored bad input and pass on the good one:
#   data    sa_oracles.sh data on tools/negtests/data/{good,bad}/ours.map (bad = T-P15: .data 00052004H)
#   render  d3dtrace/drawcmp.py with the gate's course:0 limits (tools/d3dtrace/render_limits.txt; 0 1 0 without it) on frames 0-2 of kept traces (tools/d3dtrace/ref/neg_*.d3d.gz):
#           neg_tp18 = link 1556 (DrawIndexedPrimitive Base/MinVertexIndex swapped), neg_tp19 = link 2152 (timer_ticks high dword), neg_good = gate 1103_0951 ours, neg_ctrl = its control
#   scen    the verdict() function of sa_oracles.sh (extracted, not copied) on saved scenario logs tools/negtests/scen/*.log (crash = link 1228 T-P17)
#   lints   defectlint.py --selftest, stack_lint.py --selftest, harness_drift.py --selftest (M4; skipped without a standalone link under output/gate)
# Exit 1 on the first oracle that does not behave.
T=$(dirname "$(readlink -f "$0")"); G=$(readlink -f $T/../..); N=$G/tools/negtests; O=$(mktemp -d); rc=0
ok() { echo "negtest ok   $*"; }; bad() { echo "negtest FAIL $*"; rc=1; }
# data
$T/sa_oracles.sh data $N/data/good 0 $O/d > $O/dg.txt 2>&1 && ok "data good: $(cat $O/dg.txt)" || bad "data good map rejected: $(cat $O/dg.txt)"
$T/sa_oracles.sh data $N/data/bad 0 $O/d > $O/db.txt 2>&1 && bad "data bad map accepted: $(cat $O/db.txt)" || ok "data bad: $(cat $O/db.txt)"
# render
LF=$G/tools/d3dtrace/render_limits.txt; LIM=$(awk '$1=="course:0" {print $3, $4, $5; exit}' $LF 2>/dev/null); [ -n "$LIM" ] || { echo "negtest note: no course:0 row in $LF, limits 0 1 0 (noise floor of T-P19)"; LIM="0 1 0"; }
R=$G/tools/d3dtrace/ref
python3 $G/tools/d3dtrace/drawcmp.py $R/neg_good.d3d.gz $R/neg_ctrl.d3d.gz $LIM > $O/rg.txt && ok "render good (limits $LIM): $(head -1 $O/rg.txt | cut -c1-140)" || bad "render good trace rejected: $(head -2 $O/rg.txt)"
for t in tp18 tp19; do
  python3 $G/tools/d3dtrace/drawcmp.py $R/neg_$t.d3d.gz $R/neg_ctrl.d3d.gz $LIM > $O/r$t.txt && bad "render neg_$t accepted: $(head -1 $O/r$t.txt)" || ok "render neg_$t: $(head -1 $O/r$t.txt | cut -c1-140)"
done
# scenario verdict
eval "$(sed -n '/^ *verdict() {/,/; }$/p' $T/sa_oracles.sh)"
if ! type verdict > /dev/null 2>&1; then bad "scen: verdict() not found in sa_oracles.sh"; else
  for f in crash:crashed gsdiff:GameState noresult:"no result" shotonly:screenshot; do      # shotonly: the final menu screenshot is judged since 2026-11-03 (race scenarios run with SCEN_IMGTHRESH=off)
    v=$(verdict $N/scen/${f%%:*}.log | tr '\n' ';'); echo "$v" | grep -q "${f#*:}" && ok "scen ${f%%:*}.log -> $v" || bad "scen ${f%%:*}.log not judged a failure (verdict '$v')"
  done
  v=$(verdict $N/scen/pass.log); [ -z "$v" ] && ok "scen pass.log -> pass" || bad "scen pass.log judged a failure: $v"
fi
# lints
python3 $G/tools/defectlint.py --selftest > $O/dl.txt 2>&1 && ok "$(tail -1 $O/dl.txt)" || { grep -E "FAIL" $O/dl.txt | head -5; bad "$(tail -1 $O/dl.txt)"; }
python3 $G/tools/stack_lint.py --selftest > $O/sl.txt 2>&1 && ok "$(tail -1 $O/sl.txt)" || bad "stack_lint selftest: $(tail -2 $O/sl.txt)"
python3 $T/harness_drift.py --selftest > $O/hd.txt 2>&1 && ok "$(tail -1 $O/hd.txt)" || bad "$(tail -1 $O/hd.txt)"
python3 $T/../trapcheck.py --selftest > $O/tc.txt 2>&1 && ok "$(tail -1 $O/tc.txt)" || { grep FAIL $O/tc.txt | head -5; bad "$(tail -1 $O/tc.txt)"; }
python3 $T/../clean_scan.py --selftest > $O/cs.txt 2>&1 && ok "$(tail -1 $O/cs.txt)" || bad "$(tail -1 $O/cs.txt)"
rm -r $O
exit $rc
