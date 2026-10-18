scenario() {
  to_single
  echo "shot t_single"
  echo "key Return"; echo "wait 2"; echo "shot t_r1"
  for i in 2 3 4; do echo "key Return"; echo "wait 2.5"; echo "shot t_r$i"; done
  echo "wait 10"; echo "shot t_race1"; echo "wait 10"; echo "shot t_race2"
}
