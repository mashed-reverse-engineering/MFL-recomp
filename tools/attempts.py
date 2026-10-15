#!/usr/bin/env python3
"""attempts.py add <fn> <outcome> "<note>" | show <fn> | tried <fn>: per-function attempt notes (output/attempts/<fn>.md, one line per attempt: date, who, outcome, note).
Idea from decomp.wtf (per-function brief + accumulated attempt notes): a later agent reads `show <fn>` first and does not repeat a failed lever; `tried <fn>` prints the number of attempts (agents stop after 4 without progress).
outcome: MATCH | EQUAL | DIFF | NO | NOTE."""
import os, sys, time
D = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "output", "attempts"); os.makedirs(D, exist_ok=True)
a = sys.argv[1:]
if len(a) >= 3 and a[0] == "add":
    fn, out, note = a[1], a[2], " ".join(a[3:])
    with open(os.path.join(D, fn + ".md"), "a") as f: f.write("%s | %s | %s | %s\n" % (time.strftime("%F %H:%M"), os.environ.get("ATTEMPT_WHO", "?"), out, note.replace("\n", " ")))
elif len(a) == 2 and a[0] in ("show", "tried"):
    p = os.path.join(D, a[1] + ".md"); L = open(p).read().splitlines() if os.path.exists(p) else []
    print("\n".join(L) if a[0] == "show" else len(L))
else: sys.exit(__doc__)
