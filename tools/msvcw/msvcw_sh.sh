#!/bin/bash
# msvcw_sh.sh <work dir> <shell command line>: run the command in the msvc-wine image (MSVC v143 x86: /opt/msvc/bin/x86/cl, link, lib) with the
# current directory = <work dir>.  W16 V143 item 2, CC=v143 only (build_overlay.py compile, link_ours.sh libs / shim / link).
# Mounts as tools/vc7.sh: $TMPDIR and the mashed root at their own paths; toolset + prefix from ~/.cache/mfl/msvc-wine (tools/msvcw/msvcw_install.sh).
# The prefix is shared with run.py docker msvcw-compile / msvcw-libs: one of them at a time.
# MSVCW_NATIVE=1 (W16 CIN, the windows-2022 runner: msys2 bash inside a VS 2022 x86 developer environment): no docker, no wine; the command runs
# in <work dir> with the directory of cl.exe first on PATH (msys2's /usr/bin/link would shadow link.exe) and the /opt/msvc/bin/x86 prefix of the
# callers' command lines removed (MSVC_BIN=/opt/msvc/bin/x86 becomes empty = cl / lib from PATH); MSYS2_ARG_CONV_EXCL='*' keeps /nologo, /c ...
# as written (the callers pass relative paths only).  Unset = the docker form below, unchanged.
set -e
if [ -n "$MSVCW_NATIVE" ]; then
  command -v cl.exe > /dev/null || { echo "msvcw_sh.sh: MSVCW_NATIVE=1 but no cl.exe on PATH (ilammy/msvc-dev-cmd arch amd64_x86)"; exit 1; }
  B=$(dirname "$(command -v cl.exe)")
  c=${2//\/opt\/msvc\/bin\/x86\//}; c=${c//\/opt\/msvc\/bin\/x86/}
  cd "$1"; PATH="$B:$PATH" MSYS2_ARG_CONV_EXCL='*' exec bash -c "$c"
fi
C=$HOME/.cache/mfl/msvc-wine
[ -f $C/msvc/bin/x86/cl ] || { echo "msvcw_sh.sh: no toolset at $C (tools/msvcw/msvcw_install.sh)"; exit 1; }
TMPDIR=${TMPDIR:-/tmp/claude-$(id -u)}
S=$(readlink -f "$0"); ROOT=${MASHED_ROOT:-${S%%/ghidra/*}}      # the mashed root, also from a scratch tree below ghidra/agents/
WORK=$(readlink -f "$1")
exec docker run --rm --user "$(id -u):$(id -g)" -e HOME=/tmp -e WINEPREFIX=/wpfx -e WINEDEBUG=-all \
  -v "$C/msvc":/opt/msvc:ro -v "$C/wpfx":/wpfx -v "$TMPDIR":"$TMPDIR" -v "$ROOT":"$ROOT" -w "$WORK" mfl-msvc-wine:local bash -c "$2"
