#!/bin/bash
# M1 (full recompilation feasibility): link a small exe with the VC7 link.exe from the RenderWare SDK d3d9 release libs (built with cl 12) + libcmt + generated import libs, run it under Wine.
# Result 2026-10-10: links, RwEngineInit() runs and returns 1.  Usage: m1/build.sh <work dir>
T=$(dirname "$(readlink -f "$0")"); W=$(readlink -f "${1:-${TMPDIR:-/tmp/claude-$(id -u)}/m1}"); mkdir -p $W/imp; cp $T/main.c $W/
export VC7_CONTAINER=ac
# import libs of the DLLs the game imports (user32 / gdi32 ... are not in the VC7 lib directory), plus the few user32 functions the SDK libs use that the game does not import; kernel32.lib of VC7 is used as it is
EXTRA_USER32=GetWindowRect,SetWindowPos,AdjustWindowRectEx,GetMenu,SetWindowLongA,GetWindowLongA,IsIconic python3 $T/../mk_importlibs.py $W/imp > /dev/null
rm -f $W/imp/kernel32.lib
R=Z:$(readlink -f "${RWSDK:-$T/../../toolchain/rwsdk}")/lib/d3d9/release   # RenderWare 3.7 SDK (user supplied)
$T/../vc7.sh $W cl.exe /nologo /O2 /MT main.c /link /nodefaultlib:uuid.lib /LIBPATH:imp /LIBPATH:$R rwcore.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ole32.lib d3d9.lib winmm.lib
VC7_SHELL=1 timeout 120 $T/../vc7.sh $W "wine main.exe" 2>&1 | grep -v -E "fixme|winediag|Wine build|Platform|Version|Host" | head -5
