# Main menu > Options, every page / entry (Sound: Music, SFX, Insults volume, Insults; Gamma Correction; Load / Save: Load, Save, Autosave): each setting is moved Left to its minimum,
# Right to its maximum and Left a few steps back; GameState (0x7eebc0, 0xB64) is dumped after each step (out/gs_*.bin, compared with the control by scenario.sh: cmpdump.py gs), then back out
# and a Quick Battle (row 0) with the changed settings is raced until race frame SCEN_FRAMES (default 1200 = ~20 s) and frozen like the arcade scenario.
SCEN_DRIVE=${SCEN_DRIVE-23}
export SCEN_IMGTHRESH=${SCEN_IMGTHRESH-70}      # the final shot is a race frame after 20 s: camera / AI positions differ run to run (37 measured ctrl vs ovl); a crash / black screen is ~100+
gs() { echo "dump gs_$1 0x7eebc0 0xB64"; }
# sweep <name> : Left x12, dump; Right x24, dump; Left x4, dump
sweep() { keys 0.3 Left Left Left Left Left Left Left Left Left Left Left Left; gs ${1}_min; keys 0.3 Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right; gs ${1}_max; keys 0.3 Left Left Left Left; gs ${1}_mid; }
scenario() {
  to_single; to_main
  keys 0.6 Down Down; gs start
  echo "key Return"; echo "wait 2"; echo "shot opt_page"
  # Sound
  echo "key Return"; echo "wait 2"; echo "shot sound_page"
  sweep music; keys 0.6 Down; sweep sfx; keys 0.6 Down; sweep insvol; keys 0.6 Down; sweep insults
  echo "shot sound_after"; echo "key Escape"; echo "wait 1.5"
  # Gamma Correction
  echo "key Down"; echo "wait 0.6"; echo "key Return"; echo "wait 2"; echo "shot gamma_page"
  sweep gamma; echo "shot gamma_after"; echo "key Escape"; echo "wait 1.5"
  # Load / Save: Load Game, Save Game (Yes, Continue), Autosave (Left / Right)
  keys 0.6 Down; echo "key Return"; echo "wait 2"; echo "shot ls_page"
  echo "key Return"; echo "wait 1.5"; echo "key Return"; echo "wait 2.5"; echo "key Return"; echo "wait 2"; gs load
  keys 0.6 Down; echo "key Return"; echo "wait 1.5"; echo "key Return"; echo "wait 2.5"; echo "key Return"; echo "wait 2"; gs save
  keys 0.6 Down; echo "key Return"; echo "wait 1.5"; echo "shot ls_autosave"; sweep autosave; echo "key Return"; echo "wait 1"; gs autosave_ok
  keys 1 Escape Escape; echo "shot opt_back"
  # back to the main menu, Quick Battle with the changed settings
  echo "key Escape"; echo "wait 1.5"; keys 0.6 Up Up; gs main
  echo "keywait Return $CUR 3 3"; echo "waitframes 0x$(mapva frames_total) 40"; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "shot course_page"
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"
  echo "waitmem $PHASE 6 90"; echo "shot race_start"
  race_freeze ${SCEN_FRAMES:-1200}
}
