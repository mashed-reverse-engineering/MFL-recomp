#!/bin/bash
# install_check.sh: strict relocation-checked build of the tier 1 tree (build_overlay --automatch --bridges) right after an install; exit 0 = the tree links.
# An install step that leaves unresolved symbols (a caller of an undeclared RenderWare function, a duplicate definition) must be undone by the caller (propagate_dups.py --install does it).
T=$(dirname "$(readlink -f "$0")"); D=$(mktemp -d "${TMPDIR:-/tmp/claude-$(id -u)}"/instchk_XXXXXX)
VC7_CONTAINER=${VC7_CONTAINER:-ac} python3 $T/build_overlay.py $D/b.exe --automatch --bridges > $D/log 2>&1; rc=$?
grep -E "relocation check|rror|more than one|unresolved" $D/log | cut -c1-200 | tail -5
exit $rc
