#!/bin/bash
# Runs inside the container (called by play.sh).  Env: GAME OUT SCRIPT TOOLS (container paths), DBG.
mkdir -p /tmp/xdg; chmod 700 /tmp/xdg
wineserver -k >/dev/null 2>&1; while pgrep -f '(^|\\)MFL.exe' >/dev/null; do sleep 0.1; done
T_BOOT=$(date +%s.%N)
# Xvfb and pulseaudio stay up between runs in a persistent container
[ -S /tmp/.X11-unix/X99 ] || Xvfb :99 -screen 0 640x480x24 >/dev/null 2>&1 &
[ -n "$NOAUDIO" ] || pgrep -x pulseaudio >/dev/null || pulseaudio -D --exit-idle-time=-1 --disallow-exit --load=module-null-sink >/dev/null 2>&1
export DISPLAY=:99; until [ -S /tmp/.X11-unix/X99 ]; do sleep 0.05; done
echo "x11+pulse ready after $(echo "$(date +%s.%N) $T_BOOT" | awk "{printf \"%.1f\", \$1-\$2}")s" >> $OUT/timing.txt
cd $GAME/App_Executables
if [ -n "$DBG" ]; then printf "cont\nbt\ninfo reg\nquit\n" > /tmp/dbg.cmd; [ -f "$OUT/dbg.cmd" ] && cp "$OUT/dbg.cmd" /tmp/dbg.cmd;   # own winedbg commands: put dbg.cmd into the out dir
  wine winedbg --file /tmp/dbg.cmd Z:$GAME/App_Executables/MFL.exe -VS0 -CS0 -L0 </dev/null >$OUT/dbg.txt 2>&1 &
else wine MFL.exe -VS0 -CS0 -L0 >$OUT/wine.log 2>&1 & fi
# wait for the game process (no fixed sleep); scripts use waitmem to wait for the title screen
rm -f $OUT/crashed $OUT/done
# crash watcher: Wine logs "Unhandled ..." the moment the game faults; the run is then aborted at once (a crashing build used to wait out every timeout of the script, ~65 s)
( until grep -q "Unhandled" $OUT/wine.log 2>/dev/null || [ -f $OUT/done ]; do sleep 0.25; done; [ -f $OUT/done ] || { sleep ${CRASHWAIT:-0}; touch $OUT/crashed; kill -TERM $$ 2>/dev/null; } ) &   # CRASHWAIT=s: let winedbg --auto write the crash dump to wine.log first
WATCH=$!
cleanup() { kill $WATCH 2>/dev/null; wineserver -k >/dev/null 2>&1; while pgrep -f '(^|\\)MFL.exe' >/dev/null; do sleep 0.1; done; }
trap 'echo "crash: aborted" >> $OUT/steps.txt; cleanup; exit 3' TERM
T_START=$(date +%s.%N)
until PID=$(pgrep -n -f '(^|\\)MFL.exe'); [ -n "$PID" ]; do sleep 0.2; done
echo "process up after $(echo "$(date +%s.%N) $T_START" | awk "{printf \"%.1f\", \$1-\$2}")s (after xvfb)" >> $OUT/timing.txt
lsc() { awk -v t=$1 '{s=$1/4; if(s<1)s=1; if(s>4)s=4; printf "%d", t*s+0.999}' /proc/loadavg; }      # timeout scaled by the load (x1 up to load 4, max x4)
while read -r cmd a b c d e f; do
  case "$cmd" in
    wait) sleep $a ;;
    key) xdotool key --delay 150 $a $b $c ;;
    hold) xdotool keydown $a; sleep $b; xdotool keyup $a ;;
    shot) import -window root $OUT/$a.png 2>/dev/null; [ "$a" = end ] && import -window root PPM:$OUT/$a.ppm 2>/dev/null ;;
    poke) v=$((b)); printf "\\x$(printf %02x $((v&255)))\\x$(printf %02x $(((v>>8)&255)))\\x$(printf %02x $(((v>>16)&255)))\\x$(printf %02x $(((v>>24)&255)))" | dd of=/proc/$PID/mem bs=1 seek=$((a)) conv=notrunc 2>/dev/null ;;
    probe) perl $TOOLS/probe.pl $PID $a $b $c $d $e $f & ;;
    keywait) for try in 1 2; do xdotool key --delay 150 $a; perl $TOOLS/memwait.pl $PID $b $c ${d:-4} && break; done ;;
    pressuntil) end=$(( $(date +%s) + $(lsc ${d:-20}) )); until perl $TOOLS/memwait.pl $PID $b $c 0.05; do [ $(date +%s) -ge $end ] && break; xdotool key --delay 100 $a; sleep ${e:-0.4}; done ;;
    trace) timeout $a strace -f -tt -e trace=openat -p $PID -o $OUT/trace.txt >/dev/null 2>&1 & ;;
    gdbfile) gdb -batch -p $PID -x $OUT/$a > $OUT/gdbfile.txt 2>&1 & ;;   # gdbfile NAME: background gdb with the command file $OUT/NAME (breakpoint dumps etc.); output $OUT/gdbfile.txt
    gdbcrash) { echo "handle SIGSEGV stop nopass"; for i in $(seq 1 ${a:-20}); do echo "continue"; echo "info registers eip eax ebx ecx edx esi edi esp ebp"; echo "x/48xw \$esp"; echo "signal SIGSEGV"; done; } > /tmp/gdbc.cmd; gdb -batch -p $PID -x /tmp/gdbc.cmd > $OUT/gdbcrash.txt 2>&1 & ;;   # gdbcrash N: background gdb that dumps registers + stack at each of the next N SIGSEGVs (Wine exceptions) and passes them on
    gdbsample) for i in $(seq 1 $a); do gdb -batch -p $PID -ex "thread apply all bt 8" -ex "info sharedlibrary" > $OUT/gdb_$i.txt 2>&1; done ;;
    fill32) perl -e 'print pack("V*", (oct($ARGV[1]) ) x $ARGV[0])' $b $c | dd of=/proc/$PID/mem bs=1 seek=$((a)) conv=notrunc 2>/dev/null ;;
    keyabove) if [ "$(perl $TOOLS/memread.pl $PID $b)" -gt "$c" ]; then xdotool key --delay 150 $a; fi ;;       # keyabove KEY ADDR VALUE: press the key only while the 32 bit value is above VALUE (Return outside the race phases 5/6, where it would pause)
    keyunless) if [ "$(perl $TOOLS/memread.pl $PID $b)" != "$c" ]; then xdotool key --delay 150 $a; fi ;;       # keyunless KEY ADDR VALUE: press the key unless the 32 bit value equals VALUE (e.g. Return only outside the race, where it would pause)
    keychange) old=$(perl $TOOLS/memread.pl $PID $b); xdotool key --delay 150 $a; perl $TOOLS/memwait.pl $PID $b !$old ${c:-2} || { xdotool key --delay 150 $a; perl $TOOLS/memwait.pl $PID $b !$old ${c:-2}; } ;;
    waitframes) f0=$(perl $TOOLS/memread.pl $PID $a); end=$(( $(date +%s) + $(lsc ${c:-5}) )); until [ $(perl $TOOLS/memread.pl $PID $a) -ge $((f0 + b)) ] || [ $(date +%s) -ge $end ]; do sleep 0.02; done ;;
    waitmem) perl $TOOLS/memwait.pl $PID $a $b ${c:-30} $d $e ;;
    dumpall) : > $OUT/$a.maps; while read -r rng perm _; do case "$perm" in rw*) ;; *) continue;; esac; s=$((0x${rng%-*})); e=$((0x${rng#*-})); [ $e -le $((0x7fff0000)) ] && [ $((e-s)) -le $((0x10000000)) ] || continue; echo "$rng" >> $OUT/$a.maps; dd if=/proc/$PID/mem of=$OUT/$a.$(printf %08x $s).bin bs=65536 iflag=skip_bytes,count_bytes skip=$s count=$((e-s)) 2>/dev/null; done < /proc/$PID/maps ;;   # dumpall NAME: every rw* region below 0x7fff0000 (<= 256 MB) -> NAME.<start hex>.bin + NAME.maps (S2-b asset oracle, tools/asset_oracle/walk.py)
    dump) dd if=/proc/$PID/mem of=$OUT/$a.bin bs=4096 iflag=skip_bytes,count_bytes skip=$((b)) count=$((c)) 2>/dev/null ;;
  esac
  echo "$(echo "$(date +%s.%N) $T_START" | awk "{printf \"%.1f\", \$1-\$2}") $cmd $a $b" >> $OUT/steps.txt
done <"$SCRIPT"

touch $OUT/done; cleanup   # the next run in a persistent container starts from a clean prefix state
