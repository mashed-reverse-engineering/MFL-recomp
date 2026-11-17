# Main menu > Options, every page / entry (Sound: Music, SFX, Insults volume, Insults; Gamma Correction; Load / Save: Load, Save, Autosave): each setting is moved Left to its minimum,
# Right to its maximum and Left a few steps back; GameState (0x7eebc0, 0xB64) is dumped after each step (out/gs_*.bin, compared with the control by scenario.sh: cmpdump.py gs), then back out
# to the main menu, where the final shot is taken (until 2026-11-03 a Quick Battle followed: its race shot differed run to run, 37..157 mean, a flaky verdict).
gs() { echo "dump gs_$1 0x7eebc0 0xB64"; }
# sweep <name> : Left x12, dump; Right x24, dump; Left x4, dump
# (W13 hx5: each key waits 4 game frames, keysf, instead of 0.3 s)
sweep() { keysf Left Left Left Left Left Left Left Left Left Left Left Left; gs ${1}_min; keysf Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right Right; gs ${1}_max; keysf Left Left Left Left; gs ${1}_mid; }
scenario() {
  to_single; to_main
  keys 0.6 Down Down; gs start
  # (W13 hx5: page opens / closes = pagekey, every one changes the menu depth: probe of C3 run 1; the Load / Save confirm boxes do not, they keep their waits)
  pagekey Return; echo "shot opt_page"
  # Sound
  pagekey Return; echo "shot sound_page"
  sweep music; keys 0.6 Down; sweep sfx; keys 0.6 Down; sweep insvol; keys 0.6 Down; sweep insults
  echo "shot sound_after"; pagekey Escape
  # Gamma Correction
  echo "key Down"; echo "wait 0.6"; pagekey Return; echo "shot gamma_page"
  sweep gamma; echo "shot gamma_after"; pagekey Escape
  # Load / Save: Load Game, Save Game (Yes, Continue), Autosave (Left / Right)
  keys 0.6 Down; pagekey Return; echo "shot ls_page"
  echo "key Return"; echo "wait 1.5"; echo "key Return"; echo "wait 2.5"; echo "key Return"; echo "wait 2"; gs load
  keys 0.6 Down; echo "key Return"; echo "wait 1.5"; echo "key Return"; echo "wait 2.5"; echo "key Return"; echo "wait 2"; gs save
  keys 0.6 Down; pagekey Return; echo "shot ls_autosave"; sweep autosave; pagekey Return; gs autosave_ok
  pagekey Escape; pagekey Escape; echo "shot opt_back"
  # back to the main menu: the footer's final shot is taken there (a menu is deterministic, so scenario.sh judges it with the default imgdiff limit)
  echo "key Escape"; echo "wait 1.5"; keys 0.6 Up Up; gs main; echo "waitframes 0x$(mapva frames_total) 40"
}
