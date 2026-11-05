#!/usr/bin/env python3
"""rwatap_cmp.py A.log B.log [--min-ratio R] [--count-tol F]: compare the RWA audio boundary of two rwtap logs recorded with aud_funcs.txt (AU1).
Checks (exit 1 if one fails):
  open    the soft output create params (numVoices, sampleRate, periodMs; the object pointer is masked) are equal
  period  every P record of both logs has the same n (period in bytes) and the same bus count
  seq     the P records of each log are numbered 0, 1, 2, ... without a gap (a lost / dropped period record fails)
  count   the period counts differ by at most max(2, F * the larger count), default F 0.25 (a pacing failure, not run-time noise)
  env     the sequence of distinct I3DL2 parameter sets (AUD_setEnvironment p2) is equal
  mix     the sequences of (dry, reverb) bus hashes, aligned as sequences (difflib), match with ratio >= R (default 0.10)
The mixer is paced by real time on both sides (the original by the DirectSound cursor, ours by mfl_audio_null's device clock), so neither
the count nor the hashes are reproducible: the count is the wall time the run's audio was open, and a slower exe gets more periods.
Measured 2026-11-05 (d3dtrace cup / arcade, 200 frames, vclock 0 and 1): control vs control counts within 2..7 %, ours vs control
within 4 % (vclock 0) and 7..16 % (vclock 1: ours.exe ran 16 / 20 s, the control 15 / 17 s); before the device clock fix ours ran
9039 periods under vclock 1 against the control's 141.  A rate check (periods per second from a timestamp in the P records) would
remove the run-time dependence; the tap does not record one yet.  Mix ratio within a scenario: control vs control 0.24..0.43, ours vs control 0.12..0.43;
cup vs arcade 0.17..0.26.  So the ratio separates only a garbage mix (random hashes give ~0.0; a silent mix ~0.1 = the silent share
of the control) and the default floor 0.10 is that gross check; the old default 0.9 failed control vs control.  The structural
checks (open, period, seq, count, env) carry the verdict.
Prints the counts, the equal prefix length, the ratio and the first differing period."""
import difflib, re, sys

def load(p):
    soft, env, mix, ns, seq = [], [], [], set(), []
    for l in open(p, errors="replace"):
        if l.startswith("P "):
            m = re.search(r"^P (\d+) n=(\d+) b=(\d+) d=(\w+) w=(\w+)", l)
            seq.append(int(m.group(1))); ns.add((m.group(2), m.group(3))); mix.append((m.group(4), m.group(5)))
        elif l.startswith("C ") and " AUD_softOutputInit " in l:
            soft.append(re.search(r" p2=(\S+)", l).group(1)[8:])          # drop the object pointer
        elif l.startswith("C ") and " AUD_setEnvironment " in l:
            e = re.search(r" p2=(\S+)", l).group(1)
            if not env or env[-1] != e: env.append(e)
    gap = next((i for i, s in enumerate(seq) if s != i), None)
    return soft, env, mix, ns, gap

def arg(name, default):
    return float(sys.argv[sys.argv.index(name) + 1]) if name in sys.argv else default

def main():
    a, b = sys.argv[1], sys.argv[2]
    rmin, ctol = arg("--min-ratio", 0.10), arg("--count-tol", 0.25)
    sa, ea, ma, na, ga = load(a); sb, eb, mb, nb, gb = load(b)
    bad = []
    if sa != sb: bad.append("open: %s vs %s" % (sa, sb))
    if na != nb or len(na) != 1: bad.append("period: %s vs %s" % (sorted(na), sorted(nb)))
    for p, g in ((a, ga), (b, gb)):
        if g is not None: bad.append("seq: %s: period record %d missing (records are not numbered 0..n-1)" % (p, g))
    if abs(len(ma) - len(mb)) > max(2, ctol * max(len(ma), len(mb))): bad.append("count: %d vs %d periods (tolerance %.0f %%)" % (len(ma), len(mb), 100 * ctol))
    if ea != eb: bad.append("env: %d vs %d distinct sets, first difference at %d" % (len(ea), len(eb), next((i for i, (x, y) in enumerate(zip(ea, eb)) if x != y), min(len(ea), len(eb)))))
    pre = next((i for i, (x, y) in enumerate(zip(ma, mb)) if x != y), min(len(ma), len(mb)))
    ratio = difflib.SequenceMatcher(None, ma, mb, autojunk=False).ratio() if ma and mb else 0.0
    print("periods %d / %d, period %s, equal prefix %d, match ratio %.3f, env sets %d / %d" % (len(ma), len(mb), sorted(na), pre, ratio, len(ea), len(eb)))
    if ratio < rmin: bad.append("mix: ratio %.3f < %.3f (first difference at period %d)" % (ratio, rmin, pre))
    for x in bad: print("DIFF", x)
    print("RWATAP", "EQUAL" if not bad else "DIFF")
    sys.exit(1 if bad else 0)

def selftest():
    """--selftest: synthetic logs; the copy must pass, a log with one period dropped / a wall-vs-virtual-clock count must fail."""
    import os, subprocess, tempfile
    hdr = "H 0 AUD_mix 5d8d00\nC 0 AUD_softOutputInit d=0 f=0 a=1,2,3 p2=108bc4022000000080bb000050000000\n"
    per = lambda n: "".join("P %d n=15360 b=2 d=%08x w=5f74cdc5\n" % (i, i % 7) for i in range(n))
    drop = "".join(l for i, l in enumerate(per(140).splitlines(True)) if i != 70)        # period record 70 lost
    junk = "".join("P %d n=15360 b=2 d=%08x w=%08x\n" % (i, 0x1000 + i, 0x2000 + i) for i in range(140))      # garbage mix, right count
    cases = [("copy", per(140), 0), ("dropped period", drop, 1), ("9039 vs 141", per(9039), 1), ("garbage mix", junk, 1)]
    d = tempfile.mkdtemp(); ref = os.path.join(d, "ref.log"); open(ref, "w").write(hdr + per(140)); ok = True
    for name, body, want in cases:
        p = os.path.join(d, "t.log"); open(p, "w").write(hdr + body)
        rc = subprocess.run([sys.executable, os.path.abspath(__file__), ref, p], capture_output=True).returncode
        print("selftest %-15s exit %d (want %d) %s" % (name, rc, want, "ok" if rc == want else "WRONG")); ok &= rc == want
    sys.exit(0 if ok else 1)

if __name__ == "__main__":
    selftest() if "--selftest" in sys.argv else main()
