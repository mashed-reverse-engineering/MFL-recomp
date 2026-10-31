#!/bin/bash
# A9: drives the three config dialogs of MFL.exe with xdotool in the Wine container and takes screenshots.
#   101 (0x65) video config   : -VS1  (shown by video_config_dialog(1))
#   103 (0x67) controller list: -CS1  (controller_dialog_start)
#   104 (0x68) key capture    : -CS1 then a click on the first "Reconfigure" button (495,228), key 'a' is captured, Escape closes
# Usage: dialogs_script.sh <prepared game dir (prepare.py)> <out dir> [label=run]     (needs docker image mfl-wine; one game slot at a time)
# Output: <out>/<label>_{video,ctl}/*.png, <out>/<label>.csv (shot name, size, ok); run it on the original and on the standalone exe and compare with
#   `python3 tools/wine-run/imgdiff.py a.png b.png` (the script prints the per-shot mean abs diff when DIALOGS_REF=<out dir of the reference run, same label> is set).
# play.sh / play_inner.sh of the repo have neither MFLARGS nor a mouse click: private patched copies are made in <out>/wr (the repo scripts stay untouched).
set -e
T=$(dirname "$(readlink -f "$0")"); GAME=$(readlink -f "$1"); O=$(readlink -f "$2"); L=${3:-run}; mkdir -p "$O/wr"
cp $T/play.sh $T/play_inner.sh $T/gameslot.sh $T/*.pl "$O/wr/"
sed -i 's/-VS0 -CS0 -L0/${MFLARGS:--VS0 -CS0 -L0}/g; s/^    hold) /    click) xdotool mousemove $a $b click 1 ;;\n    maps) grep -i -E "mfl|ours" \/proc\/$PID\/maps | head -${a:-6} >> $OUT\/maps.txt ;;\n    hold) /' "$O/wr/play_inner.sh"
# a relocated exe shows up as "Z:\game\App_Executables\MFL.exe" in the process list, not "MFL.exe": pgrep must accept both
sed -i "s#pgrep -n -f \"^MFL.exe\"#pgrep -n -f \"(^|[^ ])MFL.exe\"#; s#pgrep -f '^MFL.exe'#pgrep -f '(^|[^ ])MFL.exe'#g" "$O/wr/play_inner.sh"
sed -i 's/-e CRASHWAIT=\$CRASHWAIT"/-e CRASHWAIT=$CRASHWAIT -e MFLARGS"/; s#"\$(dirname "\$(readlink -f "\$0")")/mkpfx.sh"#'$T'/mkpfx.sh#' "$O/wr/play.sh"
printf 'wait 5\nshot v1_video\nkey Tab\nwait 1\nshot v2_tab\nkey Return\nwait 3\nshot v3_after_ok\n' > "$O/video.txt"
printf 'wait 5\nshot c1_list\nclick 495 228\nwait 2\nshot c2_capture\nkey a\nwait 1\nshot c3_key_a\nkey Escape\nwait 1\nshot c4_back\nkey Escape\nwait 2\nshot c5_closed\n' > "$O/ctl.txt"
: > "$O/$L.csv"
for k in video:"-VS1 -CS0 -L0" ctl:"-VS0 -CS1 -L0"; do
  n=${k%%:*}; export MFLARGS=${k#*:}; D=$O/${L}_$n; mkdir -p $D
  timeout 150 "$O/wr/play.sh" "$GAME" "$O/$n.txt" "$D" > "$D.log" 2>&1 || echo "$n: play.sh exit $?" >&2
  crash=$(grep -a -c Unhandled $D/wine.log 2>/dev/null || true)
  for f in $D/*.png; do [ -f "$f" ] || continue; b=$(basename $f .png); d=-
    [ -n "$DIALOGS_REF" ] && [ -f "$DIALOGS_REF/$(basename $D)/$b.png" ] && d=$(python3 -I $T/imgdiff.py "$DIALOGS_REF/$(basename $D)/$b.png" $f 2>/dev/null | tr -d '\n')
    [ -n "$d" ] || d=n/a; echo "$L,$n,$b,$(stat -c %s $f),crash=$crash,diff_vs_ref=$d" >> "$O/$L.csv"; done
done
cat "$O/$L.csv"
