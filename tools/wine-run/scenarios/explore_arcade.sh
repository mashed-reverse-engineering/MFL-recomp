# exploration: Arcade Modes pages, challenge select with Left/Right/Down, no race started
scenario() {
  to_single
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "shot a_single"
  echo "keywait Return $CUR 4 3"; echo "wait 1.5"; echo "shot a_depth4"
  echo "keywait Return $CUR 5 3"; echo "wait 1.5"; echo "shot a_depth5"
  echo "keywait Return $CUR 6 3"; echo "wait 1.5"; echo "shot a_depth6"
  for k in Right Right Right Left; do echo "key $k"; echo "wait 1.2"; echo "shot a_$k$RANDOM"; done
  echo "key Down"; echo "wait 1.2"; echo "shot a_down"
  echo "key Return"; echo "wait 1.5"; echo "shot a_ret"
}
