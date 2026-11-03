#!/bin/bash
# tree_hash.sh: git tree hash of the tracked working tree (the tree `git commit -a` would record; equals HEAD^{tree} on a clean tree).
# gate.sh writes it to <gate dir>/tree.txt at the start; publish.sh compares it with the newest passed `full` gate.
G=$(dirname "$(readlink -f "$0")")/..; I=$(mktemp); cp "$(git -C $G rev-parse --absolute-git-dir)/index" $I
GIT_INDEX_FILE=$I git -C $G add -u && GIT_INDEX_FILE=$I git -C $G write-tree; rc=$?; rm -f $I; exit $rc
