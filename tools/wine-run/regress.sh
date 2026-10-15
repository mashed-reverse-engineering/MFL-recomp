#!/bin/bash
# Differential test: run a control exe (harness hooks only) and the overlay exe (harness + recompiled functions) through the same
# scripted race, freeze both at the same race frame and compare memory dumps.   Usage: regress.sh <work dir> [race frames] [runs]
# Default: the harness speed-ups are on (fast_menu_scale, skip_autosave, see src/harness/freeze.c); FULL=1 runs without them.
# OVL_ARGS (env) adds build_overlay.py arguments to the overlay build, e.g. --src=<autocompile ok_NNN.c>.
# Needs the docker image mfl-wine.  Prints the number of differing bytes per dumped structure; compare with the control-vs-control noise.
MODE=fast; [ -n "$FULL" ] && MODE=slow
[ $MODE = fast ] && export NOSHOW=${NOSHOW-1}   # fast mode: do not draw the loading phase (NOSHOW=0 to draw it)    # FULL=1: no harness speed-ups (message boxes, normal frame time): the periodic last confirmation
W=$(readlink -f "$1"); FR=${2:-150}; RUNS=${3:-2}; SHOTS_RUNS=${SHOTS_RUNS:-$RUNS}   # runs>=2 shows the control-vs-control noise
T=$(dirname "$(readlink -f "$0")")
TS=$(date +%s.%N); ts() { echo "$(echo "$(date +%s.%N) $TS" | awk "{printf \"%.1f\", \$1-\$2}")s $1" >> ${W:-/tmp}/phases.txt; }
mkdir -p "$W"
echo building > $W/stage.txt
# the parameters this control was made with: bisect_files.sh / ddmin_multi.py refuse to compare other parameters against these dumps (a FRAMES default of 150 against 90 frame dumps made every group look BAD)
{ echo "P_FR=$FR"; echo "P_MODE=$MODE"; echo "P_COURSE=${COURSE:-0}"; echo "P_DRIVE=${DRIVE:-}"; echo "P_NOLIMIT=${NOLIMIT:-}"; echo "P_VARIANT=${VARIANT:-}"; echo "P_OVL_ARGS='$OVL_ARGS'"; echo "P_SAMELAYOUT=${SAMELAYOUT-1}"; } > $W/params.env
# SAMELAYOUT=1 (set by courses.sh): the control has the same objects and section size as the overlay but patches no function (--no-patch). Some behaviour
# depends on the memory layout (a bigger exe shifts the heap): on course 5 an overlay that patches NOTHING already differs from the small control (1037 car bytes).
SAMELAYOUT=${SAMELAYOUT-1}; export SAMELAYOUT      # default 1; SAMELAYOUT= (empty) or 0 uses the small harness-only control (cached, but its layout differs: menus pick different random backgrounds)
if [ "$SAMELAYOUT" = 1 ]; then python3 $T/../build_overlay.py $W/ctrl.exe --harness --no-patch $OVL_ARGS >/dev/null || exit 1
else python3 $T/../build_overlay.py $W/ctrl.exe --no-recomp --harness >/dev/null || exit 1; fi
ts build_ctrl
python3 $T/../build_overlay.py $W/ovl.exe --harness $OVL_ARGS >/dev/null || exit 1
ts build_ovl
for k in ctrl ovl; do
  va=$(grep freeze_target $W/$k.exe.map | cut -d' ' -f2)
  python3 $T/prepare.py $T/../../../MashedGame/FullyLoaded/installed $W/run_$k $W/$k.exe >/dev/null
  rf=$(grep race_frames $W/$k.exe.map | cut -d' ' -f2)
  $T/race_script.sh 0x$va 0x$rf $FR $W/$k.exe.map $MODE > $W/script_$k.txt
  SHOTS=0 $T/race_script.sh 0x$va 0x$rf $FR $W/$k.exe.map $MODE > $W/script_${k}_noshot.txt   # runs 2.. take no screenshots (each costs ~0.3 s); run 1 feeds LIVE.md
done
ts prepare_and_scripts
# first launch creates gamesave.bin; the menu path needs it, so warm both copies up
# the control dumps only depend on ctrl.exe (harness only) and the frame count: cache them (CTRL_CACHE, default ~/.cache/mfl_ctrl)
CK=$(cat $W/ctrl.exe $W/script_ctrl.txt $W/script_ctrl_noshot.txt $T/play_inner.sh $T/memwait.pl | sha1sum | cut -c1-12)_${FR}_${RUNS}_${MODE}$([ "$NOSHOW" = 1 ] && echo _noshow)${COURSE:+_c$COURSE}; CC=${CTRL_CACHE:-$HOME/.cache/mfl_ctrl}/$CK
KS="ovl"; if [ -d $CC ]; then for r in $(seq 1 $RUNS); do rm -rf $W/out_ctrl_$r; cp -r $CC/out_ctrl_$r $W/; done; else KS="ctrl ovl"; fi
for k in $KS; do
  [ -n "$NOWARM" ] || [ -f $HOME/.cache/mfl_warm_done ] && continue   # the first launch of a fresh Wine prefix needs this once (marker); NOWARM=1 skips it anyway
  printf 'wait 14\nkey Return\nwait 2\n' > $W/warm.txt
  PERSIST=warm $T/gameslot.sh timeout -k 5 90 $T/play.sh $W/run_$k $W/warm.txt $W/out_warm_$k >/dev/null 2>&1
done
mkdir -p $HOME/.cache; touch $HOME/.cache/mfl_warm_done
ts warmup
# every run starts from the same snapshot (saves are rewritten by each run, which would change the next one)
for k in $KS; do rm -rf $W/snap_$k; $T/clone.sh $W/run_$k $W/snap_$k; done
# a run is valid when car 0 is active in the dump (a lost key press in the menus leaves the game outside a race); retry up to 3 times
valid() { python3 -c "import struct,sys; d=open(sys.argv[1],'rb').read(); sys.exit(0 if len(d)>8 and struct.unpack_from('<I',d,4)[0]==1 else 1)" $1 2>/dev/null; }
# the runs are independent (own game copy, own Wine prefix volume wine_pfx_N, own container): PAR of them at the same time (default 3,
# one game uses ~2 cores); PAR=1 is the old sequential behaviour
PAR=${PAR:-3}
# live view: LIVE.md (open it in VS Code, Markdown preview) is rewritten every 2 s while the runs go; screenshots are copied to live/
LIVE=${LIVE:-$T/../../LIVE.md}; LD=$(dirname $LIVE)/live; mkdir -p $LD; find "$LD" -maxdepth 1 -name '*.png' -delete; echo $W > $LD/current
[ -f $LD/placeholder.png ] || python3 -c "import zlib,struct,sys; w,h=120,90; raw=b''.join(b'\\x00'+bytes([40,40,44])*w for _ in range(h)); c=lambda t,d:struct.pack('>I',len(d))+t+d+struct.pack('>I',zlib.crc32(t+d)&0xffffffff); open(sys.argv[1],'wb').write(b'\\x89PNG\\r\\n\\x1a\\n'+c(b'IHDR',struct.pack('>IIBBBBB',w,h,8,2,0,0,0))+c(b'IDAT',zlib.compress(raw))+c(b'IEND',b''))" $LD/placeholder.png
live_loop() {
  while :; do
    { echo "# Regress live view ($(basename $W), $FR frames, $MODE mode, PAR=$PAR)"; echo; echo "Updated $(date +%T). Stage: $(cat $W/stage.txt 2>/dev/null)"; echo
      echo "| run | last step | elapsed (s) | waiting now |"; echo "|---|---|---|---|"
      for d in $W/out_*_*/; do n=$(basename $d); l=$(grep -E '^[0-9]' $d/steps.txt 2>/dev/null | tail -1); cr=$(grep '^crash:' $d/steps.txt 2>/dev/null | tail -1 | cut -d' ' -f2-); [ -z "$cr" ] && [ -f $d/crashed ] && cr=aborted; rf=$(awk -v now=$(date +%s) 'NF>=6 && now-$1<3 {printf "waiting %s want %s now %s (%ss)", $2, $3, $4, int($6)}' $d/progress.txt 2>/dev/null)
        echo "| $n | $(echo "$l" | cut -d' ' -f2-3)${cr:+ **CRASHED ($cr)**} | $(echo "$l" | cut -d' ' -f1) | $rf |"
      done
      echo; echo "## Screenshots: control (top) and overlay (bottom) per run, the number is the mean pixel difference between them (same scene: < ~30)"; echo
      STAGES="title msgbox menu_single menu_colour menu_course menu_mode press_start loading countdown race_start end"
      for r in $(seq 1 $RUNS); do [ -d $W/out_ctrl_$r ] || [ -d $W/out_ovl_$r ] || continue
      if [ $r -gt ${SHOTS_RUNS:-2} ]; then echo "run $r: no screenshots (only the first ${SHOTS_RUNS:-2} runs take them, SHOTS_RUNS=N changes that)"; echo; continue; fi
      for k in ctrl ovl; do for png in $W/out_${k}_$r/*.png; do [ -f "$png" ] && cp -u "$png" $LD/out_${k}_${r}_$(basename $png); done; done
      echo "<table cellspacing=0 cellpadding=0 style=\"border-collapse:collapse\">"
      for k in ctrl ovl; do
        d=$W/out_${k}_$r; cur=1
        [ -f $d/powerups.bin ] && cur=0
        echo "<tr><td><b>$k $r</b></td>"
        for i in $STAGES; do
          if [ -f $LD/out_${k}_${r}_$i.png ]; then src="live/out_${k}_${r}_$i.png"; bd="3px solid transparent"; lab="<sub>$i</sub>"
          else src="live/placeholder.png"; bd="3px solid transparent"; lab="<sub><i>$i</i></sub>"
            if [ $cur = 1 ]; then bd="3px solid orange"; lab="<sub><b>&#9654; $i</b></sub>"; cur=0; fi; fi
          if [ $k = ovl ] && [ -f $LD/out_ovl_${r}_$i.png ] && [ -f $LD/out_ctrl_${r}_$i.png ]; then
            [ -f $LD/diff_${r}_$i.txt ] && [ $LD/diff_${r}_$i.txt -nt $LD/out_ovl_${r}_$i.png ] || python3 $T/imgdiff.py $LD/out_ovl_${r}_$i.png $LD/out_ctrl_${r}_$i.png > $LD/diff_${r}_$i.txt 2>/dev/null
            lab="$lab <sub>&#916; $(cat $LD/diff_${r}_$i.txt)</sub>"; fi
          echo "<td align=center valign=top style=\"border:$bd;width:126px;min-width:126px;padding:1px\"><img src=\"$src\" style=\"width:120px;height:90px;max-width:none;object-fit:contain;display:block\">$lab</td>"
        done
        echo "</tr>"
      done
      echo "</table>"; echo
      done
    } > $LIVE.tmp; mv $LIVE.tmp $LIVE; [ -n "$ONCE" ] && break; sleep 2
  done
}
echo 'runs' > $W/stage.txt; live_loop & LP=$!
# a run that does not finish within RUN_TIMEOUT s (a normal run takes 15-25 s, FULL=1 ~60 s) is killed and its slot is reset: the container is removed, and from the
# second failure on the slot's Wine prefix is rebuilt from the clean wine_pfx (a killed Wine can leave a prefix that never starts again)
RUN_TIMEOUT=${RUN_TIMEOUT:-$([ -n "$FULL" ] && echo 240 || echo 120)}; RUN_TIMEOUT=$(awk -v t=$RUN_TIMEOUT -v l=$(cut -d" " -f1 /proc/loadavg) "BEGIN{s=l/4; if(s<1)s=1; if(s>3)s=3; print int(t*s)}")      # scaled by the load at the start (x1 up to load 4, max x3)
reset_slot() { local g=$(cat $W/out_$3/slot.txt 2>/dev/null); g=${g:-$1}; docker rm -f mfl_slot$g >/dev/null 2>&1; if [ ${2:-1} -ge 2 ]; then docker volume rm wine_pfx_$g >/dev/null 2>&1; fi; $T/mkpfx.sh $g; }      # $3 = run name: the real (global) slot is in its slot.txt
job() {  # job <k> <r> <prefix index>
  local k=$1 r=$2
  for try in 1 2 3; do
    rm -rf $W/run_${k}_$r $W/out_${k}_$r; $T/clone.sh $W/snap_$k $W/run_${k}_$r
    PFX=wine_pfx_$3 PERSIST=slot$3 $T/gameslot.sh timeout -k 5 $RUN_TIMEOUT $T/play.sh $W/run_${k}_$r $([ $r -le ${SHOTS_RUNS:-2} ] && echo $W/script_$k.txt || echo $W/script_${k}_noshot.txt) $W/out_${k}_$r >/dev/null 2>&1 || { echo "run $k $r timed out after ${RUN_TIMEOUT}s (try $try): resetting slot $3" >&2; reset_slot $3 $try ${k}_$r; }
    valid $W/out_${k}_$r/cars.bin && break
    echo "run $k $r invalid (try $try), repeating" >&2
  done
}
n=0; PIDS=
for k in $KS; do for r in $(seq 1 $RUNS); do
  n=$((n+1)); $T/mkpfx.sh $n
  while [ $(for p in $PIDS; do kill -0 $p 2>/dev/null && echo; done | wc -l) -ge $PAR ]; do sleep 0.5; done
  job $k $r $n &
  PIDS="$PIDS $!"
done; done
wait $PIDS
kill $LP 2>/dev/null; echo 'runs finished, comparing' > $W/stage.txt; ONCE=1 live_loop
ts runs
CTRL_OK=1; for r in $(seq 1 $RUNS); do tail -1 $W/out_ctrl_$r/waitmem.txt 2>/dev/null | grep -q "==1 waited [0-9.]*s (value 1)" || CTRL_OK=0; done
if [ ! -d $CC ] && [ $CTRL_OK = 1 ]; then mkdir -p $CC; for r in $(seq 1 $RUNS); do cp -r $W/out_ctrl_$r $CC/; done; fi
echo "screenshot: ctrl1-ctrl2 $(cmp -l $W/out_ctrl_1/end.ppm $W/out_ctrl_2/end.ppm 2>/dev/null | wc -l) ctrl1-ovl1 $(cmp -l $W/out_ctrl_1/end.ppm $W/out_ovl_1/end.ppm 2>/dev/null | wc -l) (differing bytes of the 640x480 frame)"
# a run that never reached the freeze frame (waitmem timed out; the game stalled or crashed in the race) makes every diff meaningless
INVALID=0
for f in $W/out_*_*/waitmem.txt; do
  tail -1 $f | grep -q "==1 waited [0-9.]*s (value 1)" || { echo "WARNING: $(dirname $f) did not freeze (race frame $FR or end of race): $(tail -1 $f)" >&2; INVALID=1; }
done
exec > >(tee $W/result.txt) 2>&1
# fields that are not game behaviour (heap pointers, real time ticks, matrix padding) are ignored, see cmpdump.py
cmpdump() { python3 $T/cmpdump.py "$1" "$2" "$3"; }
for f in cars vehicles aibots statsrace powerups; do
  [ $RUNS -ge 2 ] && NM=" noise-masked $(python3 $T/cmpdump.py $f $W/out_ctrl_1/$f.bin $W/out_ovl_1/$f.bin $W/out_ctrl_1/$f.bin $W/out_ctrl_2/$f.bin)" || NM=""      # ctrl1-ovl1 without the bytes in which the two control runs differ
  BEST=$(for i in $(seq 1 $RUNS); do for j in $(seq 1 $RUNS); do [ -f $W/out_ovl_$j/$f.bin ] && [ -f $W/out_ctrl_$i/$f.bin ] && cmpdump $f $W/out_ctrl_$i/$f.bin $W/out_ovl_$j/$f.bin; done; done | sort -n | head -1)      # the game state is bimodal on courses 2/5/6: an overlay run is fine if it equals ANY control run
  echo "$f: ctrl1-ctrl2 $(cmpdump $f $W/out_ctrl_1/$f.bin $W/out_ctrl_2/$f.bin)  ctrl1-ovl1 $(cmpdump $f $W/out_ctrl_1/$f.bin $W/out_ovl_1/$f.bin)  ctrl1-ovl2 $(cmpdump $f $W/out_ctrl_1/$f.bin $W/out_ovl_2/$f.bin)  ovl1-ovl2 $(cmpdump $f $W/out_ovl_1/$f.bin $W/out_ovl_2/$f.bin)$NM  best ${BEST:-n/a}"
done
ts done
# the base image (prepare.py) must never change: Wine runs as root in the container and ignores the read-only bits, so check the modification times
BASE=${MFL_BASE:-$HOME/.cache/mfl_base}
if [ -f $BASE/.stamp ]; then ch=$(find $BASE -type f -newer $BASE/.stamp | head -3); [ -n "$ch" ] && echo "WARNING: the base image was modified during the run (hardlinked files shared by all game directories): $ch" >&2; fi
# an invalid run (a process that never froze) leaves numbers that mean nothing: tell the caller (courses.sh, scripts), except for the scenarios that are not frozen by design (QUIT, win / lose)
if [ "$INVALID" = 1 ] && [ -z "$QUIT" ] && [ $(( ${DRIVE:-0} & 96 )) = 0 ]; then echo "INVALID: at least one run did not freeze, the numbers above are not a result" | tee -a $W/result.txt; exit 3; fi
