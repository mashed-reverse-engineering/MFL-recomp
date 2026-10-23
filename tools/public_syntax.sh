#!/bin/bash
# public_syntax.sh [root]: syntax-only check (cl /Zs, original compiler under Wine, tools/vc7.sh) of every published src/tier2 and src/clusters source against the curated headers.
R=$(readlink -f "${1:-$(dirname "$(readlink -f "$0")")/..}"); cd "$R" || exit 1
ls src/tier2/an_*.c src/clusters/cl_*.c > .syntax_list.txt
VC7_SHELL=1 "$R/tools/vc7.sh" "$R" 'wine cl.exe /nologo /Zs /Isrc $(cat .syntax_list.txt | tr "\n" " ")' > .syntax_out.txt 2>&1
rm -f .syntax_list.txt; n=$(grep -c -E "error C|fatal error" .syntax_out.txt); rm -f .syntax_out.txt
echo "public_syntax: $n errors"; [ "$n" = 0 ]
