SCEN_DRIVE=${SCEN_DRIVE-8}
scenario() {
  to_single; unlock_all
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"
  echo "waitmem $PHASE 6 90"
  echo "probe 60 $PHASE $CUR $MSG"
  echo "wait 4"; echo "key Escape"; echo "wait 1"; echo "shot p1"
  echo "key Down"; echo "wait 0.6"; echo "shot p2"; echo "key Down"; echo "wait 0.6"; echo "key Down"; echo "wait 0.6"; echo "shot p3"
  echo "key Return"; echo "wait 2"; echo "shot p4"; echo "wait 1.5"; echo "key Return"; echo "wait 3"; echo "shot p5"; echo "wait 6"; echo "shot p6"; echo "gdbstack 1"
}
