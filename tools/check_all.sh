#!/bin/bash
# check_all.sh [work dir]: the static gate (no game run): hand written manifest compare, strict relocation-checked builds of every tier combination, tier ledger.  Exit 1 on the first failure.
# The game gates are separate: tools/wine-run/courses.sh (union sweep), DRIVE=31 crash check, shadow sweep.
T=$(dirname "$(readlink -f "$0")"); W=${1:-$(mktemp -d)}; mkdir -p $W
$T/provenance.sh $W
python3 $T/cheatgrep.py | tail -5; [ ${PIPESTATUS[0]} -eq 0 ] || { echo "cheatgrep: forbidden constructs (src/cheat_allow.txt for legitimate ones)"; exit 1; }
echo "manifest:"; python3 $T/cmpfn.py --manifest $T/../src/manifest.txt 2>&1 | tail -2 || exit 1
# raw image data addresses in the C (`return 0x68b028;`) break in a linked build (tools/delink/link_ours.sh): warn only until all are rewritten (tools/rawaddr.py --rewrite --check)
python3 $T/rawaddr.py --quiet 2>&1 | tail -1 | sed 's/^/WARNING /'
# diffemu defect classes (unsigned compare of undefined4, float/int bit confusion, _ftol() without argument, wrong prototypes) in installed tier 2 / enabled bridges / candidates: warning only (output/defectlint.csv)
python3 $T/defectlint.py --quiet 2>&1 | grep "^defectlint:" | sed 's/^/WARNING /'
# stack / call-boundary blind spots of diffemu (frame overflow by a callee's store extent, stack address in a value slot, calls with fewer arguments than the callee's original reads, interior register callees, float literals, cluster register+stack arity): installed code only, warning (output/stack_lint.csv, README 'stack_lint')
python3 $T/stack_lint.py --csv $W/stack_lint.csv src/automatch_near src/clusters src/bridges src/tier1 > $W/stack_lint.txt 2>&1; grep -E "^stack_lint:|^recomp.*: [A-H][0-9]? \(" $W/stack_lint.txt | head -12 | sed 's/^/WARNING /'
# defectlint class 10 (any: definition arity T-L3, call arity) in installed non-identical code is a FAIL; originally on a CALL ("is called with N arguments, its original body reads M stack slots") in installed non-identical code is a FAIL as well (typed or `()` declaration alike; the same lost-argument bug as below, an_hk_1036 -> 56e990/56e9b0)
python3 - $T/../output/defectlint.csv <<'PY' || { echo "defectlint: class 10 call-arity / class 14-15 (implicit float call, float/double constant width) findings in installed code (src/defectlint_allow.txt for checked false positives)"; exit 1; }
import csv, sys
bad = [r for r in csv.DictReader(open(sys.argv[1])) if r["enabled"] == "yes" and r["byte_identical"] == "no" and (r["cls"] in ("10", "14", "15") or r["tier"] == "cluster")]      # 14/15 (T-L1), every class in an enabled cluster (T-L2: triaged to zero 2026-10-29): float argument to an undeclared callee / float-vs-double constant width: the T-H2 defects
for r in bad[:5]: print("%s:%s %s" % (r["file"], r["line"], r["note"][:150]))
sys.exit(1 if bad else 0)
PY
# class C (callarity) is a FAIL: a call with fewer arguments than the callee's original reads hands it stack garbage (course_superg_cb4_sub_45c1f0 -> 45c140: NULL matrix, course 7 crash, 2026-10-22); diffemu cannot see it.  Suppress a checked false positive in src/stacklint_allow.txt
grep -E "^recomp.*: C \(" $W/stack_lint.txt | head -5; ! grep -qE "^recomp.*: C \(" $W/stack_lint.txt || { echo "stack_lint: callarity (class C) findings in installed code"; exit 1; }
# class A (frameoverflow) is a FAIL too: a callee that stores more bytes through a pointer into our frame than the frame struct has left smashes the neighbour (return address / saved registers).  diffemu cannot see it unless its spec writes the whole record (course_world_ray_hit_light_sample: RtBaryGetTransform 64 bytes into 60, course 14 never froze, 2026-10-22).  Suppress a checked false positive in src/stacklint_allow.txt WITH a reason
grep -E "^recomp.*: A \(" $W/stack_lint.txt | head -5; ! grep -qE "^recomp.*: A \(" $W/stack_lint.txt || { echo "stack_lint: frameoverflow (class A) findings in installed code"; exit 1; }
# portability gate v0 (AArch64 LP64 / Switch): pointer<->int casts, x87 / asm, strides; fails only when a class count grows past output/portcheck_baseline.txt (`portcheck.py --update-baseline` is the only writer)
python3 $T/portcheck.py --quiet 2>&1 | tail -2 | sed 's/^/WARNING /'; [ ${PIPESTATUS[0]} -eq 0 ] || { echo "portcheck: a class grew past the baseline (output/portcheck.csv, README 'Portability gate')"; exit 1; }
# the three strict builds are independent: run them side by side (BUILD_PAR=1 for the old serial order; compile jobs inside build_overlay are already parallel, the default is 1 because concurrent builds poisoned ~/.cache/mfl_objcache; make the cache writes atomic before raising it)
i=0; PIDS=()
FL=("" "--automatch --bridges" "--automatch --automatch-near --bridges")
[ -s $T/../src/clusters/enabled.txt ] && FL+=("--automatch --automatch-near --bridges --clusters")      # installed clusters (tools/cluster_install.py): a 4th strict build, only when some are enabled
for flags in "${FL[@]}"; do
  i=$((i+1)); mkdir -p $W/b$i
  ( echo "build [$flags]:"; python3 $T/build_overlay.py $W/b$i/b.exe $flags 2>&1 | grep -E "relocation check|RELCHECK |rror|more than one" | cut -c1-160; [ ${PIPESTATUS[0]} -eq 0 ] || echo "FAILED build $i" ) > $W/b$i.log 2>&1 &
  PIDS+=($!)
  if [ ${#PIDS[@]} -ge ${BUILD_PAR:-1} ]; then wait ${PIDS[0]}; PIDS=("${PIDS[@]:1}"); fi
done
wait
cat $W/b?.log; grep -q "^FAILED" $W/b?.log && { echo FAILED; exit 1; }
python3 $T/declcheck.py > $W/declcheck.txt; [ $? -eq 0 ] || { grep HARD $W/declcheck.txt; echo "declcheck: hard mismatches"; exit 1; }
echo "declcheck clean"
python3 $T/agent_verify.py > $W/agent_verify.txt 2>&1; tail -1 $W/agent_verify.txt      # before the ledger: the ledger reads its evidence
python3 $T/tier_ledger.py
