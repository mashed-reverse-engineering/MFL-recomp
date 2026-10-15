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
# portability gate v0 (AArch64 LP64 / Switch): pointer<->int casts, x87 / asm, strides; fails only when a class count grows past output/portcheck_baseline.txt (`portcheck.py --update-baseline` is the only writer)
python3 $T/portcheck.py --quiet 2>&1 | tail -2 | sed 's/^/WARNING /'; [ ${PIPESTATUS[0]} -eq 0 ] || { echo "portcheck: a class grew past the baseline (output/portcheck.csv, README 'Portability gate')"; exit 1; }
# the three strict builds are independent: run them side by side (BUILD_PAR=1 for the old serial order; compile jobs inside build_overlay are already parallel, the default is 1 because concurrent builds poisoned ~/.cache/mfl_objcache; make the cache writes atomic before raising it)
i=0; PIDS=()
for flags in "" "--automatch --bridges" "--automatch --automatch-near --bridges"; do
  i=$((i+1)); mkdir -p $W/b$i
  ( echo "build [$flags]:"; python3 $T/build_overlay.py $W/b$i/b.exe $flags 2>&1 | grep -E "relocation check|RELCHECK |rror|more than one" | cut -c1-160; [ ${PIPESTATUS[0]} -eq 0 ] || echo "FAILED build $i" ) > $W/b$i.log 2>&1 &
  PIDS+=($!)
  if [ ${#PIDS[@]} -ge ${BUILD_PAR:-1} ]; then wait ${PIDS[0]}; PIDS=("${PIDS[@]:1}"); fi
done
wait
cat $W/b1.log $W/b2.log $W/b3.log; grep -q "^FAILED" $W/b?.log && { echo FAILED; exit 1; }
python3 $T/declcheck.py > $W/declcheck.txt; [ $? -eq 0 ] || { grep HARD $W/declcheck.txt; echo "declcheck: hard mismatches"; exit 1; }
echo "declcheck clean"
python3 $T/agent_verify.py > $W/agent_verify.txt 2>&1; tail -1 $W/agent_verify.txt      # before the ledger: the ledger reads its evidence
python3 $T/tier_ledger.py
