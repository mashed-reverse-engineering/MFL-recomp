#!/bin/bash
# rwreplay.sh <rwtap.log> <orig trace.d3d.gz> <librw build dir (tools/build_mflrw.sh, tests/build_rwreplay.sh done)> <work dir> [frames]
# Replays the log into librw-mfl under Wine + the d3dtrace proxy and diffs the per-draw D3D traces.  Needs docker image mfl-wine and the volume wine_pfx_rp
# (tools/wine-run/mkpfx.sh rp).  Result: <work>/replay.d3d.gz, <work>/diff.txt; the tex.N content hashes are blanked (the log has no texture data).
T=$(dirname "$(readlink -f "$0")"); LOG=$(readlink -f "$1"); ORIG=$(readlink -f "$2"); B=$(readlink -f "$3"); W=$(readlink -f "$4"); FR=${5:-100}
D3=$T/../d3dtrace
mkdir -p "$W/run" && cp "$B/rwreplay.exe" "$W/run/" && python3 -I "$D3/patch_import.py" "$W/run/rwreplay.exe" || exit 1
cp "$D3/d3d9.dll" "$W/run/d3dp.dll"
printf 'out=d3dtrace.log\nstart_phase=0\nracef_addr=0\nstop_racef=0\nstop_phase=0\ngeom=1\nmax_frames=%s\n' "$FR" > "$W/run/d3dtrace.cfg"
python3 -I "$T/rwreplay_gen.py" "$LOG" "$ORIG" > "$W/run/replay.txt" || exit 1
docker run --rm -v "$W/run":/t -v wine_pfx_rp:/wpfx -e WINEDLLOVERRIDES="mscoree,mshtml=" -e WINEPREFIX=/wpfx -e WINEDEBUG=-all -e XDG_RUNTIME_DIR=/tmp/xdg -e LIBGL_ALWAYS_SOFTWARE=1 mfl-wine \
  bash -c 'mkdir -p /tmp/xdg; chmod 700 /tmp/xdg; Xvfb :99 -screen 0 640x480x24 >/dev/null 2>&1 & export DISPLAY=:99; sleep 2; cd /t; timeout 120 wine rwreplay.exe replay.txt' > "$W/run.log" 2>&1
tail -3 "$W/run.log"
python3 -I "$D3/normalise.py" "$W/run/d3dtrace.log" "$W/replay.d3d.gz" || exit 1
blank() { python3 -I -c '
import gzip, re, sys
op = gzip.open if sys.argv[1].endswith(".gz") else open
for l in op(sys.argv[1], "rt"): sys.stdout.write(re.sub(r"(\ttex\.\d=)[0-9a-f]+\|", r"\1H|", l))' "$1" > "$2"; }
blank "$ORIG" "$W/orig.d3d"; blank "$W/replay.d3d.gz" "$W/replay.d3d"
python3 -I "$D3/diff.py" "$W/replay.d3d" "$W/orig.d3d" --ulp 16 --no-ids --max ${MAXD:-3} > "$W/diff.txt"; tail -25 "$W/diff.txt"
