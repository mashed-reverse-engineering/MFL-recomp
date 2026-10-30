#!/bin/bash
# build_res.sh <exe> <scratch dir>: extract the resources, compile mfl.res with rc.exe (VC7) and check the tree round-trips (via a dummy link).
set -e
H=$(dirname "$(readlink -f "$0")"); T=$(dirname "$(dirname "$H")"); S=$(readlink -f "$2"); mkdir -p "$S"
python3 "$H/res_extract.py" "$1" "$S"
echo 'int main(void){return 0;}' > "$S/d.c"
"$T/tools/vc7.sh" "$S" cl.exe /nologo /c d.c >/dev/null
"$T/tools/vc7.sh" "$S" rc.exe /fo mfl.res mfl.rc
python3 "$H/rsrc_cmp.py" "$1" "$S/mfl.res"
"$T/tools/vc7.sh" "$S" link.exe /nologo /entry:main /subsystem:windows /nodefaultlib /out:d.exe d.obj mfl.res
python3 "$H/rsrc_cmp.py" "$1" "$S/d.exe"
