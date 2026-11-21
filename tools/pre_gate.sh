#!/bin/bash
# pre_gate.sh: the cheap checks before a full gate, about 2 minutes, no Wine, no docker.  Run: bash tools/pre_gate.sh from the tree root
# Stops at the first failure with one line "PRE-GATE FAIL (<step>): <reason>" (+ the last log lines, logs in a /tmp/pre_gate.* dir); exit 0 = all ok.
# Order: 1 tracked tree clean (files tree_hash.sh leaves out ignored) | 2 tier_ledger.py --retag, the tree must still be clean (a changed ledger / retagged source must be
# committed first: gate.sh's tree guard and publish.sh compare the tree hash) | 3 readability_ratchet.py --quiet | 4 fixlint.py | 5 selftests no_original, pe_check, statecmp,
# adapter_inline | 6 dry export into a scratch dir (the deny rules: hard-coded home-folder paths, original bytes, decompiler names) | 7 bash -n on the tools/*.sh changed in the last
# PRE_GATE_COMMITS (default 5) commits and on this script.
export LC_ALL=C
T=$(dirname "$(readlink -f "$0")"); R=$T/..; cd $R || exit 2
L=$(mktemp -d /tmp/pre_gate.XXXXXX)
fail() { echo "PRE-GATE FAIL ($1): $2"; [ -f $L/$1.log ] && tail -5 $L/$1.log; echo "logs: $L"; exit 1; }
run() { local n=$1 w=$2; shift 2; local t=$SECONDS; "$@" > $L/$n.log 2>&1 || fail $n "$w"; echo "pre_gate: $n ok ($((SECONDS-t)) s)"; }
dirty() { git status --short -uno | grep -vE '^.. output/((diffemu_cache|diffemu_results|frameaudit|ledger_state|shadow_cache|portcheck)\.csv|agent_verify\.txt)$'; }      # the paths tree_hash.sh excludes

D=$(dirty); [ -z "$D" ] || { echo "$D" | head -8; fail tree "tracked tree not clean (commit or revert first; untracked files are fine)"; }
echo "pre_gate: tree clean ok"

run retag "tools/tier_ledger.py --retag failed" python3 $T/tier_ledger.py --retag
D=$(dirty)
if [ -n "$D" ]; then
  echo "$D" | head -8
  [ -z "$(echo "$D" | grep -v ' output/tier_ledger\.csv$')" ] && fail retag "tier_ledger --retag changed output/tier_ledger.csv: commit the ledger first"
  fail retag "tier_ledger --retag changed tracked files (retagged sources?): commit them first"
fi

run ratchet "readability ratchet: a count grew past output/readability_baseline.txt" python3 $T/readability_ratchet.py --quiet
run fixlint "MFL_FIX_ switch misuse (python3 tools/fixlint.py)" python3 $T/fixlint.py
run st_no_original "tools/ci/no_original.py --selftest failed" python3 $T/ci/no_original.py --selftest
run st_pe_check "tools/ci/pe_check.py --selftest failed" python3 $T/ci/pe_check.py --selftest
run st_statecmp "tools/statehash/statecmp.py --selftest failed" python3 $T/statehash/statecmp.py --selftest
run st_adapter_inline "tools/adapter_inline.py --selftest failed" python3 $T/adapter_inline.py --selftest

E=$(mktemp -d /tmp/pre_gate_export.XXXXXX)
run export "dry export tools/export_public.py denied a file (DENY lines in the log)" python3 $T/export_public.py $E
rm -rf $E

NC=${PRE_GATE_COMMITS:-5}; SH=$(git diff --name-only HEAD~$NC HEAD -- 'tools/*.sh'); SH="$SH tools/pre_gate.sh"; bad=
for f in $SH; do [ -f $f ] && { bash -n $f 2> $L/bash_n.log || bad="$bad $f"; }; done
[ -z "$bad" ] || fail bash_n "bash -n failed on:$bad"
echo "pre_gate: bash -n ok ($SH)"
rm -rf $L; echo "PRE-GATE OK"
