#!/bin/bash
# release_v7.sh <out dir>: V7 of the plan for window 6 (section 4), last step of `gate.sh full`.
# Release link of the tree (tools/delink/link_release.sh, AUDIO_DLL=1 TRAP=1, miniaudio + FFmpeg DLLs beside the exe), then
# release_smoke.sh title and race with MFL_TRAP_NOBOX=1 and MFL_AUDIO_DUMP (WDBG stays at the default).  PASS needs: both runs ok,
# no mfl_trap.log, the race run's PCM dump passes tools/audio/pcm_check2.py (cover = audio s / wall s from the dump's first to its last write).
# Not checked: the trap exit code (unverified under gdb, W7).  Callers put a hard timeout around it (gate.sh: 1800 s).
# Prints one "V7: PASS ..." or "V7: FAIL <why>" line last.  Exit 0 PASS, 1 FAIL.
export LC_ALL=C      # awk printf would write "36,0" under a de_* locale (first proof run 2026-11-13)
WR=$(dirname "$(readlink -f "$0")"); G=$WR/../..; O=$(readlink -f "$1"); mkdir -p $O
# video: every DLL of tools/video/work/out (W16 VIDCI: mfl_video_ffmpeg.dll + the shared FFmpeg DLLs; the glob expands in link_release.sh's `for f in $DLLS`)
AUDIO_DLL=1 TRAP=1 DLLS="$G/tools/audio/work/out/null/mfl_audio_miniaudio.dll $G/tools/video/work/out/*.dll" \
  $G/tools/delink/link_release.sh $O/rel > $O/link.log 2>&1 || { tail -5 $O/link.log; echo "V7: FAIL release link (log $O/link.log)"; exit 1; }
A=$O/rel/game/App_Executables; rm -f $A/mfl_trap.log
[ -f $A/MFL_original.exe ] || { echo "V7: FAIL no MFL_original.exe beside the release exe in $A (the resources are read from it at run time)"; exit 1; }
export DENV="-e MFL_TRAP_NOBOX=1 -e MFL_AUDIO_DUMP=/out/audio.pcm"
t=$(BOOT_S=150 $WR/release_smoke.sh $O/rel $O/title title); echo "$t"
r=$($WR/release_smoke.sh $O/rel $O/race race); echo "$r"      # race_relinked.sh exits 0 always: judged by its line
bad=
[ "$t" = "release_smoke title: ok" ] || bad="$bad title;"
echo "$r" | grep -q "^course [0-9]*: ok$" || bad="$bad race;"
[ -f $A/mfl_trap.log ] && bad="$bad mfl_trap.log: $(head -c 200 $A/mfl_trap.log);"
P=$O/race/run/audio.pcm
if [ -s $P ]; then
  # W15 PCM: cover's wall time = the dump file's birth (fopen at the stream start) -> its last write (kill), not steps.txt
  # title -> end: the dump starts at the 2nd Return of the title keywait, so the first, load-scaled try (4.0..6.0 s) was counted
  # as missing audio (cover 0.842..0.888, file-based 1.004..1.005 over 14 full gates; agents/pcm_w15/notes.md). No birth time -> old way.
  b=$(stat -c %.W $P); m=$(stat -c %.Y $P)
  s=$(awk -v b="$b" -v m="$m" 'NR==1{a=$1} END{if (b > 0) printf "%.1f", m-b; else printf "%.1f", $1-a}' $O/race/run/steps.txt)
  p=$(python3 -I $G/tools/audio/pcm_check2.py $P --scenario-seconds $s) || bad="$bad pcm_check2;"; echo "$p"
else bad="$bad no audio.pcm;"; fi
[ -z "$bad" ] && { echo "V7: PASS (title ok, race ok, no mfl_trap.log, pcm_check2 PASS; dir $O)"; exit 0; }
echo "V7: FAIL$bad (dir $O)"; exit 1
