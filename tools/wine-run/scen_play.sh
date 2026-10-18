#!/bin/bash
# scen_play.sh: copy of play.sh that runs scen_inner.sh (play_inner.sh + gdbcov command); see scenario.sh
# Scripted headless run of a prepared MFL copy (see prepare.py).  Usage: play.sh <game dir> <script file> <out dir>
# Script lines:  wait N | key NAME [NAME...] (xdotool key names, e.g. Return Up Down space) | hold NAME SECONDS |
#                shot NAME (screenshot out/NAME.png) | dump NAME 0xSTART 0xLEN (out/NAME.bin from the live process) |
#                poke 0xADDR 0xVALUE (write a 32 bit value into the live process) |
#                probe SECONDS 0xADDR... (background: every 0.25 s log elapsed seconds + the 32 bit values to out/probe.txt) |
#                keywait KEY 0xADDR VALUE [TIMEOUT=4] (press the key, wait for the memory value; repeat the key once more if it is not reached: no lost key presses) |
#                pressuntil KEY 0xADDR VALUE [TIMEOUT s=20] [INTERVAL s=0.4] (press the key every INTERVAL until the value is reached; for message boxes whose input gate opens late) |
#                trace SECONDS (background strace of file opens -> out/trace.txt) | gdbsample N (N stack samples of all threads -> out/gdb_N.txt, slows the game)
#                keyabove KEY 0xADDR VALUE (press the key only while the value is above VALUE) |
#                keyunless KEY 0xADDR VALUE (press the key unless the 32 bit value equals VALUE) |
#                keychange KEY 0xADDR [TIMEOUT=2] (press the key, wait until the value differs from the one before the press; one retry)
#                waitframes 0xADDR N [TIMEOUT=5] (wait until the 32 bit counter at ADDR (harness frames_total) grew by N: animations are per frame, a fixed time is not reproducible under load)
#                waitmem 0xADDR VALUE TIMEOUT [STALL [PROGRESS_ADDR]] (STALL: give up when the value (or the value at PROGRESS_ADDR) did not change for that many seconds; poll the live process until the 32 bit value equals VALUE; logs the wait to out/waitmem.txt)
[ -n "$GAMESLOT_HELD" ] || exec "$(dirname "$(readlink -f "$0")")/gameslot.sh" "$0" "$@"      # global limit of concurrent game containers
GAME=$(readlink -f "$1"); SCRIPT=$(readlink -f "$2"); OUT=$(readlink -f "$3")
mkdir -p "$OUT"
# callers name their container / prefix by a LOCAL index (regress.sh: slot1.., two parallel sweeps both use slot1): two games in one container (wineserver -k, pgrep MFL.exe) kill each other.
# The global game slot is exclusive, so a persistent "slotN" container and its prefix are renamed after it.
case "$PERSIST" in slot[0-9]*) PERSIST=slot$GAMESLOT_HELD; PFX=wine_pfx_$GAMESLOT_HELD; "$(dirname "$(readlink -f "$0")")/mkpfx.sh" $GAMESLOT_HELD; echo $GAMESLOT_HELD > "$OUT/slot.txt";; esac
T=$(dirname "$(readlink -f "$0")")
OVR="mscoree,mshtml="; [ -n "$NOAUDIO" ] && OVR="mscoree,mshtml=;winepulse.drv=;winealsa.drv=;wineoss.drv=;winejack.drv="   # NOAUDIO=1: no audio device at all (sound completion events are timing dependent)
ENVS="-e WINEDLLOVERRIDES=$OVR -e WINEPREFIX=/wpfx -e WINEDEBUG=${WDBG:--all} -e XDG_RUNTIME_DIR=/tmp/xdg -e LIBGL_ALWAYS_SOFTWARE=1 -e vblank_mode=${VBLANK:-1} -e DBG=$DBG -e CRASHWAIT=$CRASHWAIT"
# PERSIST=<name>: use a long lived container (docker create alone costs 2-15 s on this host, docker exec ~0.3 s).  $TMPDIR and
# $MASHED_ROOT are mounted at the same paths (NOT /tmp: its X11 socket must stay private per container), so game dir, script and out dir
# must live below one of them, otherwise a normal per-run container is used.
TMPDIR=${TMPDIR:-/tmp/claude-$(id -u)}   # scratch dir, mounted into the containers (never plain /tmp: X11 socket)
MASHED_ROOT=${MASHED_ROOT:-$(dirname "$(dirname "$(dirname "$T")")")}
under() { case "$1" in "$TMPDIR"/*|"$MASHED_ROOT"/*) true;; *) false;; esac; }
if [ -n "$PERSIST" ] && under "$GAME" && under "$SCRIPT" && under "$OUT" && under "$T"; then
  C=mfl_$PERSIST
  if ! docker inspect -f '{{.State.Running}}' $C 2>/dev/null | grep -q true; then
    docker rm -f $C >/dev/null 2>&1
    docker run -d --init --name $C $DENV --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -v "$TMPDIR":"$TMPDIR" -v "$MASHED_ROOT":"$MASHED_ROOT" -v ${PFX:-wine_pfx}:/wpfx mfl-wine sleep infinity >/dev/null
  fi
  exec docker exec $ENVS -e GAME="$GAME" -e OUT="$OUT" -e SCRIPT="$SCRIPT" -e TOOLS="$T" $C bash "$T/scen_inner.sh"
fi
exec docker run --rm $DENV --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -v "$GAME":/game -v "$OUT":/out -v "$SCRIPT":/script.txt:ro -v ${PFX:-wine_pfx}:/wpfx -v "$T":/tools:ro \
  $ENVS -e GAME=/game -e OUT=/out -e SCRIPT=/script.txt -e TOOLS=/tools mfl-wine bash /tools/scen_inner.sh
