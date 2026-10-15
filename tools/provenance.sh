#!/bin/bash
# provenance.sh <dir>: writes <dir>/provenance.txt (what produced this result): MFL.exe sha256, compiler, flags, sha1 of tools/*.py, git HEAD + dirty flag.
T=$(dirname "$(readlink -f "$0")"); mkdir -p "$1"
CL=${VC7_ROOT:-$T/../toolchain/vc7}/Vc7/bin/cl.exe; [ -f "$CL" ] || CL=
{ echo "MFL.exe sha256 $(sha256sum "${MFL_EXE:-$T/../game/MFL.exe}" | cut -c1-16)"
  echo "cl 13.00.9466 sha1 $( [ -n "$CL" ] && sha1sum "$CL" | cut -c1-12 || echo none)"
  echo "flags /nologo /c /O2 /Gy"
  echo "tools sha1 $(cd $T && cat *.py | sha1sum | cut -c1-12)"
  echo "git $(git -C $T rev-parse --short HEAD 2>/dev/null || echo none) $([ -n "$(git -C $T status --porcelain 2>/dev/null)" ] && echo dirty || echo clean)"; } > "$1/provenance.txt"
