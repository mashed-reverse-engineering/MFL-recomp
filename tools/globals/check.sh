#!/bin/bash
# Scratch check of the globals generator: generate, compile with VC7, add aliases (interior/second names, library labels), link with a dummy main, mapcheck (layout, bytes, pointers, reloc coverage), then types.py (T0/T1/F5).
# The real build compiles the generated TUs with the same cl command line (/Od /Zl).  Usage: check.sh <empty scratch dir below $TMPDIR>      (no files are written into the repo)
set -eo pipefail
W=$(readlink -f "$1"); HERE=$(dirname "$(readlink -f "$0")"); VC7=$HERE/../vc7.sh
export VC7_CONTAINER=${VC7_CONTAINER:-globgen}
[ -z "$(ls -A "$W" 2>/dev/null)" ] || { echo "work dir must be empty: $W"; exit 1; }
mkdir -p "$W"; python3 -I "$HERE/globgen.py" "$W" | tail -4
echo 'int main(void){return 0;}' > "$W/main.c"
cd "$W"
"$VC7" "$W" cl.exe /nologo /c /Od /Zl rdata.c data.c bss.c stubs.c main.c 2>&1 | grep -v '^[a-z0-9]*\.c$' | head -20 || true
for s in rdata data bss; do
  [ -f objcopy_$s.txt ] && objcopy $(tr '\n' ' ' < objcopy_$s.txt) $s.obj $s.o2 && mv $s.o2 $s.obj
done
"$VC7" "$W" link.exe /nologo /nodefaultlib /entry:main /fixed /base:0x400000 /map /out:g.exe rdata.obj data.obj bss.obj stubs.obj main.obj 2>&1 | tail -5 || true
python3 -I "$HERE/mapcheck.py" "$W"
python3 -I "$HERE/types.py" "$W" | grep -v '^   '      # T0 types.csv, T1 C types vs generated extents, F5 link-level name check (details in types_report.txt)
python3 -I "$HERE/lowtriage.py" "$W" | head -2              # F2: low-confidence words, how many are on src/globals_reloc_allow.txt
