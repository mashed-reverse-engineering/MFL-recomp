#!/bin/bash
# Run the Visual C++ .NET 2002 tools (cl 13.00.9466, the compiler of MFL.exe) under Wine in Docker.
# Usage: vc7.sh <workdir> cl.exe /c /O2 file.c      (workdir is mounted read-write as Z:\work and is the current directory)
#        vc7.sh <workdir> link.exe ... / dumpbin.exe /disasm file.obj
# VC7_SHELL=1: the arguments are a shell command line run inside the container (used for parallel cl runs).
# Needs: docker, the image tianon/wine, and the extracted compiler in toolchain/vc7 (VC7_ROOT) (see ghidra/README.md).
set -e
# VC7_CL=1310: the alternate compiler cl 13.10 (VC++ Toolkit 2003, toolchain/vc71) with its own container (<name>_71); default stays cl 13.00
if [ "$VC7_CL" = 1310 ]; then VC7_ROOT=${VC7_ROOT:-$(dirname "$(dirname "$(readlink -f "$0")")")/toolchain/vc71}; VC7_PFX=${VC7_PFX:-wine_pfx_cc}; [ -n "$VC7_CONTAINER" ] && VC7_CONTAINER=${VC7_CONTAINER}_71; fi
# the compiler has its own Wine prefix volume (a clone of wine_pfx): a compile and a game run must never use the same prefix at the same time
VC7V=${VC7_PFX:-wine_pfx_cc}; docker volume inspect $VC7V >/dev/null 2>&1 || "$(dirname "$(readlink -f "$0")")/wine-run/mkpfx.sh" ${VC7V#wine_pfx_}
# VC7_CONTAINER=<name>: run in a long lived container (docker exec) instead of a new one per call: `docker create` alone costs 2-15 s on this host.
# The workdir must be below $TMPDIR or $MASHED_ROOT (mounted at the same path).
TMPDIR=${TMPDIR:-/tmp/claude-$(id -u)}   # scratch dir, mounted into the containers (never plain /tmp: X11 socket)
REPO=$(dirname "$(dirname "$(readlink -f "$0")")")
MASHED_ROOT=${MASHED_ROOT:-$(dirname "$REPO")}   # mounted into the container at the same path
ROOT=$(readlink -f "${VC7_ROOT:-$REPO/toolchain/vc7}")   # extracted Visual C++ .NET 2002 ("Microsoft Visual Studio .NET" directory)
WORK=$(readlink -f "$1"); shift
if [ -n "$VC7_CONTAINER" ]; then
  C=mfl_$VC7_CONTAINER
  if ! docker inspect -f '{{.State.Running}}' $C 2>/dev/null | grep -q true; then
    docker rm -f $C >/dev/null 2>&1
    docker run -d --init --name $C -v "$ROOT":/vs:ro -v "$TMPDIR":"$TMPDIR" -v "$MASHED_ROOT":"$MASHED_ROOT" -v ${VC7_PFX:-wine_pfx_cc}:/wpfx tianon/wine:latest sleep infinity >/dev/null 2>&1 || true
  fi
  exec docker exec -w "$WORK" -e WINEPREFIX=/wpfx -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all -e VC7_SHELL="$VC7_SHELL" \
    -e 'WINEPATH=Z:\vs\Common7\IDE;Z:\vs\Vc7\bin' -e 'INCLUDE=Z:\vs\Vc7\include;Z:\vs\Vc7\atlmfc\include' -e 'LIB=Z:\vs\Vc7\lib;Z:\vs\Vc7\atlmfc\lib' \
    $C bash -c 'mkdir -p /tmp/xdg; if [ -n "$VC7_SHELL" ]; then eval "$@"; else wine "$@"; fi' _ "$@"
fi
docker run --rm -e VC7_SHELL -v "$ROOT":/vs:ro -v "$WORK":/work -v ${VC7_PFX:-wine_pfx_cc}:/wpfx \
  -e WINEPREFIX=/wpfx -e XDG_RUNTIME_DIR=/tmp/xdg -e WINEDEBUG=-all \
  -e 'WINEPATH=Z:\vs\Common7\IDE;Z:\vs\Vc7\bin' \
  -e 'INCLUDE=Z:\vs\Vc7\include;Z:\vs\Vc7\atlmfc\include' \
  -e 'LIB=Z:\vs\Vc7\lib;Z:\vs\Vc7\atlmfc\lib' \
  -w /work tianon/wine:latest bash -c 'mkdir -p /tmp/xdg; if [ -n "$VC7_SHELL" ]; then eval "$@"; else wine "$@"; fi' _ "$@"
