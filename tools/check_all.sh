#!/bin/bash
# check_all.sh [work dir]: the static gate (no game run): hand written manifest compare, strict relocation-checked builds of every tier combination, tier ledger.  Exit 1 on the first failure.
# Order (plan_speedup_2026-11-03 item 2): the seconds-long checks first, then the largest strict build (b<N>) + objscan + trapcheck on it, then the other builds + the lints, the ledgers last.
# Strict builds: BUILD_PAR=1 serial, otherwise (default 4) all side by side (the object cache writes are atomic since 2026-11-03).  They are skipped when tools/strict_key.py equals
# output/gate/strict_ok.key (written after all builds passed; objscan / trapcheck then use the stored object list output/gate/strict_ok.objs.txt); FORCE_STRICT=1 always builds.
# The game gates are separate: tools/wine-run/courses.sh (union sweep), DRIVE=31 crash check, shadow sweep.
T=$(dirname "$(readlink -f "$0")"); W=${1:-$(mktemp -d)}; mkdir -p $W
trap 'kill $(jobs -p) 2>/dev/null' EXIT      # a failing check leaves no strict build running
$T/provenance.sh $W
# ---- seconds-long checks
python3 $T/cheatgrep.py | tail -5; [ ${PIPESTATUS[0]} -eq 0 ] || { echo "cheatgrep: forbidden constructs (src/cheat_allow.txt for legitimate ones)"; exit 1; }
echo "manifest:"; python3 $T/cmpfn.py --manifest $T/../src/manifest.txt > $W/manifest.txt 2>&1; rc=$?      # (was `| tail -2 || exit 1`: no pipefail, the exit status was tail's, a hand function that stopped matching passed, found 2026-11-03)
grep -E "^DIFFER|compile error" $W/manifest.txt | head -8; tail -2 $W/manifest.txt; [ $rc -eq 0 ] || { echo "manifest: a hand function no longer matches ($W/manifest.txt)"; exit 1; }
# aliascheck (2026-11-03): an alias that gives a lib_names/symbols name another address moves the library forwarder jmp (RwFclose 005bfbf0 hung the standalone exe on an uninitialised lock)
python3 $T/aliascheck.py | tail -6; [ ${PIPESTATUS[0]} -eq 0 ] || { echo "aliascheck: an alias shadows a library name (src/aliascheck_known.txt for deliberate ones)"; exit 1; }
python3 $T/declcheck.py > $W/declcheck.txt; [ $? -eq 0 ] || { grep HARD $W/declcheck.txt; echo "declcheck: hard mismatches"; exit 1; }
echo "declcheck clean"
# portability gate v0 (AArch64 LP64 / Switch): pointer<->int casts, x87 / asm, strides; fails only when a class count grows past output/portcheck_baseline.txt (`portcheck.py --update-baseline` is the only writer)
python3 $T/portcheck.py --quiet 2>&1 | tail -2 | sed 's/^/WARNING /'; [ ${PIPESTATUS[0]} -eq 0 ] || { echo "portcheck: a class grew past the baseline (output/portcheck.csv, README 'Portability gate')"; exit 1; }
# negative fixtures (plan_testing item 2, ~5 s): every defectlint class, stack_lint, the harness drift lint and the whole-program oracles (.data length, drawcmp, scenario verdict) must still FAIL on their stored bad inputs (tools/negtests, tools/d3dtrace/ref/neg_*)
$T/delink/negtest.sh > $W/negtest.txt 2>&1 || { grep -E "FAIL" $W/negtest.txt | head -8; echo "negtest: an oracle no longer fires on its negative fixture ($W/negtest.txt)"; exit 1; }
echo "negtest: $(grep -c '^negtest ok' $W/negtest.txt) negative / positive fixtures behave"
# ---- strict builds (independent of each other; compile jobs inside build_overlay are parallel as well)
FL=("" "--automatch --bridges" "--automatch --automatch-near --bridges")
[ -s $T/../src/clusters/enabled.txt ] && FL+=("--automatch --automatch-near --bridges --clusters")      # installed clusters (tools/cluster_install.py): a 4th strict build, only when some are enabled
N=${#FL[@]}; KF=$T/../output/gate/strict_ok.key; KO=$T/../output/gate/strict_ok.objs.txt
KEY=$(python3 $T/strict_key.py "${FL[@]}"); SKIP=
if [ "${FORCE_STRICT:-0}" != 1 ] && [ "$KEY" = "$(cat $KF 2>/dev/null)" ] && [ -s $KO ] && python3 -I $T/strict_key.py --objs-exist $KO; then
  SKIP=1; mkdir -p $W/b$N; cp $KO $W/b$N/objs.txt; echo "strict builds: skipped, inputs equal to the last pass ($KF; FORCE_STRICT=1 rebuilds)"
fi
build() {      # build <i> <flags>
  mkdir -p $W/b$1
  ( echo "build [$2]:"; OBJLIST=$W/b$1/objs.txt python3 $T/build_overlay.py $W/b$1/b.exe $2 2>&1 | grep -E "relocation check|RELCHECK|rror|more than one" | cut -c1-160; [ ${PIPESTATUS[0]} -eq 0 ] || echo "FAILED build $1" ) > $W/b$1.log 2>&1
}
BG=; [ "${BUILD_PAR:-4}" != 1 ] && BG=1
if [ -z "$SKIP" ]; then
  build $N "${FL[$((N-1))]}" & BP=$!
  [ -n "$BG" ] && for ((i=1; i<N; i++)); do build $i "${FL[$((i-1))]}" & done
  wait $BP; cat $W/b$N.log; grep -q "^FAILED" $W/b$N.log && { echo FAILED; exit 1; }
fi
# object-level raw image data addresses (unrelocated operands in the compiled objects of the largest build): a linked build (tools/delink/link_ours.sh) reads / writes the wrong memory there (boot crash 2026-10-30).  Masks: tools/objscan.py, checked false positives: src/objscan_allow.txt
python3 $T/objscan.py $W/b$N/objs.txt | tail -8; [ ${PIPESTATUS[0]} -eq 0 ] || { echo "objscan: raw image data addresses in compiled objects (use &symbol, see tools/objscan.py)"; exit 1; }
# trapcheck (2026-11-03): no installed C may reference original code that the standalone link fills with int3 (rw_trap, rw_dead, dshow_dead, dead_tables); RtDictDestroy / RpSplineDestroy and 8 more were trapped while live
python3 $T/trapcheck.py --objs $W/b$N/objs.txt --list $T/delink/rw_trap.txt --list $T/delink/rw_dead.txt --list $T/delink/dshow_dead.txt --list $T/delink/dead_tables.txt > $W/trapcheck.txt; rc=$?; tail -1 $W/trapcheck.txt; [ $rc -eq 0 ] || { grep VIOLATION $W/trapcheck.txt | head -8; echo "trapcheck: installed C references int3-filled original code ($W/trapcheck.txt)"; exit 1; }
[ -z "$SKIP" ] && [ -z "$BG" ] && for ((i=1; i<N; i++)); do build $i "${FL[$((i-1))]}"; done
# ---- lints (the other builds may still run); stack_lint (~60 s) runs beside rawaddr + defectlint (~90 s)
# stack / call-boundary blind spots of diffemu (frame overflow by a callee's store extent, stack address in a value slot, calls with fewer arguments than the callee's original reads, interior register callees, float literals, cluster register+stack arity): installed code only, warning (output/stack_lint.csv, README 'stack_lint')
python3 $T/stack_lint.py --csv $W/stack_lint.csv src/automatch_near src/clusters src/bridges src/tier1 > $W/stack_lint.txt 2>&1 & SLP=$!
# raw image data addresses in the C (`return 0x68b028;`) break in a linked build (tools/delink/link_ours.sh): warn only until all are rewritten (tools/rawaddr.py --rewrite --check)
python3 $T/rawaddr.py --quiet 2>&1 | tail -1 | sed 's/^/WARNING /'
# diffemu defect classes (unsigned compare of undefined4, float/int bit confusion, _ftol() without argument, wrong prototypes) in installed tier 2 / enabled bridges / candidates: warning only (output/defectlint.csv)
python3 $T/defectlint.py --quiet 2>&1 | grep "^defectlint:" | sed 's/^/WARNING /'
wait $SLP; grep -E "^stack_lint:|^recomp.*: [A-H][0-9]? \(" $W/stack_lint.txt | head -12 | sed 's/^/WARNING /'
# defectlint class 10 (any: definition arity T-L3, call arity) in installed non-identical code is a FAIL; originally on a CALL ("is called with N arguments, its original body reads M stack slots") in installed non-identical code is a FAIL as well (typed or `()` declaration alike; the same lost-argument bug as below, an_hk_1036 -> 56e990/56e9b0)
python3 - $T/../output/defectlint.csv <<'PY' || { echo "defectlint: class 10 call-arity / class 14-15 (implicit float call, float/double constant width) findings in installed code (src/defectlint_allow.txt for checked false positives)"; exit 1; }
import csv, sys
bad = [r for r in csv.DictReader(open(sys.argv[1])) if r["enabled"] == "yes" and r["byte_identical"] == "no" and (r["cls"] in ("10", "14", "15") or r["tier"] == "cluster" and r["cls"] not in ("21", "22"))]      # 14/15 (T-L1), every class in an enabled cluster (T-L2: triaged to zero 2026-10-29): float argument to an undeclared callee / float-vs-double constant width: the T-H2 defects.  21 argkind / 22 retwidth (2026-11-03): warning only until their first triage
for r in bad[:5]: print("%s:%s %s" % (r["file"], r["line"], r["note"][:150]))
sys.exit(1 if bad else 0)
PY
# class C (callarity) is a FAIL: a call with fewer arguments than the callee's original reads hands it stack garbage (course_superg_cb4_sub_45c1f0 -> 45c140: NULL matrix, course 7 crash, 2026-10-22); diffemu cannot see it.  Suppress a checked false positive in src/stacklint_allow.txt
grep -E "^recomp.*: C \(" $W/stack_lint.txt | head -5; ! grep -qE "^recomp.*: C \(" $W/stack_lint.txt || { echo "stack_lint: callarity (class C) findings in installed code"; exit 1; }
# class A (frameoverflow) is a FAIL too: a callee that stores more bytes through a pointer into our frame than the frame struct has left smashes the neighbour (return address / saved registers).  diffemu cannot see it unless its spec writes the whole record (course_world_ray_hit_light_sample: RtBaryGetTransform 64 bytes into 60, course 14 never froze, 2026-10-22).  Suppress a checked false positive in src/stacklint_allow.txt WITH a reason
grep -E "^recomp.*: A \(" $W/stack_lint.txt | head -5; ! grep -qE "^recomp.*: A \(" $W/stack_lint.txt || { echo "stack_lint: frameoverflow (class A) findings in installed code"; exit 1; }
# ---- the other strict builds' verdicts; the key of a full pass (computed again: an edit during the builds must not be recorded as passed)
if [ -z "$SKIP" ]; then
  wait; for ((i=1; i<N; i++)); do cat $W/b$i.log; done; grep -q "^FAILED" $W/b?.log && { echo FAILED; exit 1; }
  [ "$(python3 $T/strict_key.py "${FL[@]}")" = "$KEY" ] && { mkdir -p $(dirname $KF); cp $W/b$N/objs.txt $KO; echo $KEY > $KF; }
fi
# ---- ledgers
python3 $T/agent_verify.py > $W/agent_verify.txt 2>&1; tail -1 $W/agent_verify.txt      # before the ledger: the ledger reads its evidence
python3 $T/tier_ledger.py
python3 $T/evidence_ledger.py
