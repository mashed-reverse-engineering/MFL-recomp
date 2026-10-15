#!/bin/bash
# Run MFL.exe headless in Docker (Wine + Xvfb).  Usage: run.sh <game dir that contains MFL.exe> <seconds> [out dir]
# The game dir is mounted read-write (the game writes saves next to the exe), so pass a copy.
# Registry (MashedRunner/VersionMFL.cs): HKLM\Software\empire interactive\mfl  installpath, installstatus=2, language.
GAME=$(readlink -f "$1"); SECS=${2:-30}; OUT=$(readlink -f "${3:-/tmp}")
mkdir -p "$OUT"
docker run --rm -v "$GAME":/game -v "$OUT":/out -v wine_pfx:/wpfx -e WINEDLLOVERRIDES="mscoree,mshtml=" \
  -e SCREEN=${SCREEN:-640x480x24} -e WINEPREFIX=/wpfx -e WINEDEBUG=${WINEDEBUG:-err+all,fixme-all} -e XDG_RUNTIME_DIR=/tmp/xdg -e LIBGL_ALWAYS_SOFTWARE=1 \
  mfl-wine bash -c '
mkdir -p /tmp/xdg; chmod 700 /tmp/xdg
pulseaudio -D --exit-idle-time=-1 --disallow-exit --load=module-null-sink >/dev/null 2>&1
Xvfb :99 -screen 0 ${SCREEN:-640x480x24} >/out/xvfb.log 2>&1 &
export DISPLAY=:99; sleep 2
wine reg add "HKLM\\Software\\empire interactive\\mfl" /v installpath /t REG_SZ /d "Z:\\game\\" /f >/dev/null 2>&1
wine reg add "HKLM\\Software\\empire interactive\\mfl" /v installstatus /t REG_SZ /d 2 /f >/dev/null 2>&1
wine reg add "HKLM\\Software\\empire interactive\\mfl" /v language /t REG_SZ /d English /f >/dev/null 2>&1
cd /game/App_Executables
wine MFL.exe -VS0 -CS0 -L0 2>&1 | head -200 >/out/game.log &
for t in $(seq 5 5 '"$SECS"'); do sleep 5; import -window root /out/shot_$t.png 2>/dev/null; done
xwininfo -root -tree > /out/windows.txt 2>&1
ps aux > /out/ps.txt
'
