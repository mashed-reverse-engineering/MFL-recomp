# W15 UNL: the hidden 100 % unlock.  Main menu: the 4th Down lands on entry 3 (bonus features) only when it is enabled (disabled entries are skipped);
# bonus page: Down + Right toggles one bonus flag (GameState option_flags[1..4] at 0x7EF6F0..0x7EF6FC, dumped before and after); Return leaves the page;
# then Arcade Modes > Quick Battle like arcade.sh and race_freeze.  SCEN_UNLOCK defaults to 1 (freeze.c unlock_all); the flag-on link
# (MFL_FIX_UNLOCK_ALL=1) runs with SCEN_UNLOCK= (empty) and NOCTRL=1.
SCEN_UNLOCK=${SCEN_UNLOCK-1}
SCEN_DRIVE=${SCEN_DRIVE-23}
scenario() {
  to_single; to_main; echo "shot main_menu"
  echo "keywait Down $(sel 2) 1 3"; echo "keywait Down $(sel 2) 2 3"; echo "key Down"
  echo "waitevent $(sel 2) 3 5 bonus"; echo "shot main_bonus_sel"
  pagekey Return; echo "shot bonus_page"; echo "dump optflags_before 0x7EF6EC 0x14"
  keysf Down Right; echo "shot bonus_toggled"; echo "dump optflags_toggled 0x7EF6EC 0x14"
  pagekey Return; echo "dump optflags_after 0x7EF6EC 0x14"; echo "shot bonus_left"
  echo "pressuntil Up $(sel 2) 0 10 0.5"; pagekey Return
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "shot course_page"
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"
  echo "waitevent $PHASE 6 90 end"; echo "shot race_start"
  race_freeze ${SCEN_FRAMES:-300}
}
