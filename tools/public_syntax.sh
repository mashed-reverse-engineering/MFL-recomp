#!/bin/bash
# public_syntax.sh [root]: syntax-only check (cl /Zs, original compiler under Wine, tools/vc7.sh) of every published src/tier1, src/tier2 and src/clusters source against the curated headers.
# Sources go in chunks of 300 (one command line for all of them overflows Wine's limit and then compiles nothing); a chunk that compiled fewer files than it was given counts as an error.
R=$(readlink -f "${1:-$(dirname "$(readlink -f "$0")")/..}"); cd "$R" || exit 1
ls src/tier1/*.c src/tier2/an_*.c src/clusters/cl_*.c > .syntax_all.txt; split -l 300 .syntax_all.txt .syntax_part_
n=0; total=$(wc -l < .syntax_all.txt)
for p in .syntax_part_*; do
  cp "$p" .syntax_list.txt
  VC7_SHELL=1 "$R/tools/vc7.sh" "$R" 'wine cl.exe /nologo /Zs /Isrc $(cat .syntax_list.txt | tr "\n" " ")' 2>&1 | tr -d "\r" > .syntax_out.txt
  e=$(grep -c -E "error C|fatal error" .syntax_out.txt); got=$(grep -c -E "^[A-Za-z0-9_]+\.c$" .syntax_out.txt); want=$(wc -l < "$p")
  [ "$got" = "$want" ] || { echo "public_syntax: only $got of $want sources compiled in $p"; e=$((e+1)); }
  n=$((n+e))
done
rm -f .syntax_all.txt .syntax_part_* .syntax_list.txt .syntax_out.txt
echo "public_syntax: $n errors ($total sources)"; [ "$n" = 0 ]
