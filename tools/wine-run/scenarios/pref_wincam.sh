# W15 PREF: round-end winner camera (cup_scene_update_object).  Arcade Quick Battle as arcade.sh (SCEN_DRIVE 41 = the arcade_win drive),
# race_end (phase leaves 6), then two dumps of the cup scene camera words 0x64CB04..0x64CB14 (angle, distance, speed, side, blend) 120 frames
# apart with no key pressed (phase 7 waits for menu_select).  Original: the angle word changes between the dumps (orbit).
# MFL_FIX_PREFS link with winner_cam_rotation=0 (SCEN_PREFS file): both angle words = 0x3EC90FDB (0.3926991, the start angle).
SCEN_DRIVE=${SCEN_DRIVE-41}
scenario() {
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "shot course_page"
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"
  echo "waitmem $PHASE 6 90"; echo "shot race_start"
  race_end
  echo "waitframes 0x$(mapva frames_total) 150"; echo "dump wincam_a 0x64CB04 0x14"; echo "shot wincam_a"
  echo "waitframes 0x$(mapva frames_total) 120"; echo "dump wincam_b 0x64CB04 0x14"; echo "shot wincam_b"
}
