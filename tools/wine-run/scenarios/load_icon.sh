# W10 Z-rsrc: the RWTEXDICTIONARY load icon (resource 0x194, video_init_main_camera -> resource_load) on the loading screen of a Quick Battle.
# Like arcade.sh up to the course page, then shots while the race loads.  Run with SCEN_NOLIMIT=0 (loading at the 60 fps limit, so the shots catch it)
# and SCEN_IMGTHRESH as for races; compare the loading shots ctrl vs ours with imgdiff.py by hand (scenario.sh judges only final.png).
SCEN_DRIVE=${SCEN_DRIVE-23}
scenario() {
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"
  for i in 1 2 3 4 5 6; do echo "shot loading$i"; echo "wait 0.4"; done
  echo "waitmem $PHASE 6 90"; echo "shot race_start"
  race_freeze ${SCEN_FRAMES:-120}
}
