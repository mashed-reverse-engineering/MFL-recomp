#!/bin/sh
# run_test.sh [movie dir]: offline acceptance. Decodes renderware.mpg + intro.mpg twice with the native harness (same source as the DLL),
# compares frame counts + per-frame hashes, cross-checks the frame count with the system ffprobe. Results in $WORK/test_results.
# Movie dir default: $MOVIES, else MashedGame/FullyLoaded/installed/App_Executables/TOASTART/PC/MOVIES under the repo parent.
set -e
HERE=$(dirname "$(readlink -f "$0")")
WORK=${WORK:-$HERE/../work}
M=${1:-${MOVIES:-$(readlink -f "$HERE/../../../../MashedGame/FullyLoaded/installed/App_Executables/TOASTART/PC/MOVIES")}}
H=$WORK/out/harness_native
R=$WORK/test_results
mkdir -p "$R"
for m in renderware intro; do
  "$H" "$M/$m.mpg" > "$R/$m.run1.txt"
  "$H" "$M/$m.mpg" > "$R/$m.run2.txt"
  if cmp -s "$R/$m.run1.txt" "$R/$m.run2.txt"; then v=IDENTICAL; else v=DIFFERENT; fi
  echo "$m: $(tail -1 "$R/$m.run1.txt" | sed "s|$M/||") hashes_run1_vs_run2=$v sha256=$(sha256sum < "$R/$m.run1.txt" | cut -c1-16)"
  echo "  ffprobe frames: $(ffprobe -v error -select_streams v:0 -count_frames -show_entries stream=nb_read_frames -of csv=p=0 "$M/$m.mpg")"
done
"$H" -dump "$R/last_ours.bgra" "$M/renderware.mpg" > /dev/null
ffmpeg -v error -y -i "$M/renderware.mpg" -vf "select=eq(n\,240)" -frames:v 1 -pix_fmt bgra -f rawvideo "$R/last_ref.bgra"
python3 -I "$HERE/pixdiff.py" "$R/last_ours.bgra" "$R/last_ref.bgra"
"$H" -rt "$M/renderware.mpg" | head -1
echo "  ffprobe duration: $(ffprobe -v error -show_entries format=duration -of csv=p=0 "$M/renderware.mpg")"
