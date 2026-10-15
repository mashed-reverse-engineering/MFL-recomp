#!/bin/bash
# Run the prepared MFL copy, wait, then dump memory ranges of the live process (virtual addresses = Ghidra addresses).
# Usage: dump.sh <game dir> <wait seconds> <out dir> name:0xSTART:0xLEN [name:0xSTART:0xLEN ...]
# Example: dump.sh run 30 out cars:0x8B06E0:0x34100 stats:0x8C8AE0:0x1000   (files out/<name>.bin + a title screenshot out/shot.png)
GAME=$(readlink -f "$1"); WAIT=$2; OUT=$(readlink -f "$3"); shift 3
mkdir -p "$OUT"
docker run --rm --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -v "$GAME":/game -v "$OUT":/out -v wine_pfx:/wpfx -e WINEDLLOVERRIDES="mscoree,mshtml=" \
  -e WINEPREFIX=/wpfx -e WINEDEBUG=-all -e XDG_RUNTIME_DIR=/tmp/xdg -e LIBGL_ALWAYS_SOFTWARE=1 \
  mfl-wine bash -c '
mkdir -p /tmp/xdg; chmod 700 /tmp/xdg
pulseaudio -D --exit-idle-time=-1 --disallow-exit --load=module-null-sink >/dev/null 2>&1
Xvfb :99 -screen 0 640x480x24 >/dev/null 2>&1 &
export DISPLAY=:99; sleep 2
cd /game/App_Executables
wine MFL.exe -VS0 -CS0 -L0 >/dev/null 2>&1 &
sleep '"$WAIT"'
import -window root /out/shot.png 2>/dev/null
PID=$(pgrep -f "^MFL.exe" | head -1); echo "pid $PID"
for r in '"$*"'; do
  IFS=: read n s l <<<"$r"
  dd if=/proc/$PID/mem of=/out/$n.bin bs=4096 iflag=skip_bytes,count_bytes skip=$((s)) count=$((l)) 2>>/out/dd.err
  ls -la /out/$n.bin
done
'
