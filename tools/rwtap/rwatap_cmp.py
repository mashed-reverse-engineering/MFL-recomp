#!/usr/bin/env python3
"""rwatap_cmp.py A.log B.log [--min-ratio R]: compare the RWA audio boundary of two rwtap logs recorded with aud_funcs.txt (AU1).
Checks (exit 1 if one fails):
  open    the soft output create params (numVoices, sampleRate, periodMs; the object pointer is masked) are equal
  period  every P record of both logs has the same n (period in bytes) and the same bus count
  env     the sequence of distinct I3DL2 parameter sets (AUD_setEnvironment p2) is equal
  mix     the sequences of (dry, reverb) bus hashes, aligned as sequences (difflib), match with ratio >= R (default 0.9);
          the mixer is paced by wall time on both sides, so run ctrl vs ctrl first and use its ratio as the noise floor.
Prints the equal prefix length, the ratio and the first differing period."""
import difflib, re, sys

def load(p):
    soft, env, mix, ns = [], [], [], set()
    for l in open(p, errors="replace"):
        if l.startswith("P "):
            m = re.search(r"n=(\d+) b=(\d+) d=(\w+) w=(\w+)", l)
            ns.add((m.group(1), m.group(2))); mix.append((m.group(3), m.group(4)))
        elif l.startswith("C ") and " AUD_softOutputInit " in l:
            soft.append(re.search(r" p2=(\S+)", l).group(1)[8:])          # drop the object pointer
        elif l.startswith("C ") and " AUD_setEnvironment " in l:
            e = re.search(r" p2=(\S+)", l).group(1)
            if not env or env[-1] != e: env.append(e)
    return soft, env, mix, ns

def main():
    a, b = sys.argv[1], sys.argv[2]
    rmin = float(sys.argv[sys.argv.index("--min-ratio") + 1]) if "--min-ratio" in sys.argv else 0.9
    sa, ea, ma, na = load(a); sb, eb, mb, nb = load(b)
    bad = []
    if sa != sb: bad.append("open: %s vs %s" % (sa, sb))
    if na != nb or len(na) != 1: bad.append("period: %s vs %s" % (sorted(na), sorted(nb)))
    if ea != eb: bad.append("env: %d vs %d distinct sets, first difference at %d" % (len(ea), len(eb), next((i for i, (x, y) in enumerate(zip(ea, eb)) if x != y), min(len(ea), len(eb)))))
    pre = next((i for i, (x, y) in enumerate(zip(ma, mb)) if x != y), min(len(ma), len(mb)))
    ratio = difflib.SequenceMatcher(None, ma, mb, autojunk=False).ratio() if ma and mb else 0.0
    print("periods %d / %d, period %s, equal prefix %d, match ratio %.3f, env sets %d / %d" % (len(ma), len(mb), sorted(na), pre, ratio, len(ea), len(eb)))
    if ratio < rmin: bad.append("mix: ratio %.3f < %.3f (first difference at period %d)" % (ratio, rmin, pre))
    for x in bad: print("DIFF", x)
    print("RWATAP", "EQUAL" if not bad else "DIFF")
    sys.exit(1 if bad else 0)

if __name__ == "__main__":
    main()
