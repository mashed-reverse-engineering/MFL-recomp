"""Black-frame check (gate.sh standalone): colorcheck.py <dir> [MIN] [glob...]: every file of the globs below <dir> (default c*/run/press_start.png and
c*/run/race_end.png, the delink sweep) needs more than MIN distinct colours (default 1000; a black frame has 1, a rendered one 10k+).  No file matched = fail."""
import glob, sys
from PIL import Image
MIN = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
fs = sorted(f for g in (sys.argv[3:] or ["c*/run/press_start.png", "c*/run/race_end.png"]) for f in glob.glob(sys.argv[1] + "/" + g))
bad = 0
for f in fs:
    n = len(set(Image.open(f).convert("RGB").getdata()))
    if n <= MIN: print("BLACK/flat frame: %s has %d colours" % (f, n)); bad += 1
print("colorcheck: %d of %d frame(s) at or below %d colours" % (bad, len(fs), MIN)); sys.exit(1 if bad or not fs else 0)
