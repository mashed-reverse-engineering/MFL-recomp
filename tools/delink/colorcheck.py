"""Black-frame check of a sweep dir (gate.sh standalone): every c*/run/{press_start,race_end}.png needs more than MIN distinct colours (default 1000; a black frame has 1, a rendered one 10k+)."""
import glob, sys
from PIL import Image
MIN = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
bad = 0
for f in sorted(glob.glob(sys.argv[1] + "/c*/run/press_start.png") + glob.glob(sys.argv[1] + "/c*/run/race_end.png")):
    n = len(set(Image.open(f).convert("RGB").getdata()))
    if n <= MIN: print("BLACK/flat frame: %s has %d colours" % (f, n)); bad += 1
print("colorcheck: %d frame(s) at or below %d colours" % (bad, MIN)); sys.exit(1 if bad else 0)
