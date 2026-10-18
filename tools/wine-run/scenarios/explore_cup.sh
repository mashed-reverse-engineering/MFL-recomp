scenario() {
  to_single
  echo "keywait Down $(sel 3) 1 3"
  echo "shot c_single"
  echo "keywait Return $CUR 4 3"; echo "wait 1.5"; echo "shot c_d4"
  for i in 5 6 7 8; do echo "key Return"; echo "wait 2"; echo "shot c_r$i"; done
  echo "key Right"; echo "wait 1"; echo "shot c_right"
  echo "key Down"; echo "wait 1"; echo "shot c_down"
  echo "probe 40 $CUR $PHASE $COLUMN $ROW 0x69cfb0"
}
