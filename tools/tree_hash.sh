#!/bin/bash
# tree_hash.sh: git tree hash of the tracked working tree (the tree `git commit -a` would record; equals HEAD^{tree} on a clean tree).
# gate.sh writes it to <gate dir>/tree.txt at the start; publish.sh compares it with the newest passed `full` gate.
G=$(dirname "$(readlink -f "$0")")/..; I=$(mktemp); cp "$(git -C $G rev-parse --absolute-git-dir)/index" $I
# regenerated caches are left out (they change on every gate and are not exported): committing them no longer invalidates a full gate (2026-11-05)
X="output/diffemu_cache.csv output/diffemu_results.csv output/frameaudit.csv output/ledger_state.csv output/shadow_cache.csv"
GIT_INDEX_FILE=$I git -C $G add -u && GIT_INDEX_FILE=$I git -C $G rm --cached -q --ignore-unmatch -- $X && GIT_INDEX_FILE=$I git -C $G write-tree; rc=$?; rm -f $I; exit $rc
