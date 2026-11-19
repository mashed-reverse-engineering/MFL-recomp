#!/usr/bin/env python3
"""gatestep.py <out dir>: the gate's statehash step (W15 A5, report only).  No Wine: compares the STORED pairs named in data/gate_pair.txt,
one `course N` block each (ctrl, ctrl2, and optionally ours + map).  Per course it runs statecmp with the gate's option set
(--ptr --mask --rwpad --sound --uninit --env; ours also --map-b --dead-layers) and writes <out>/cc_c<N>.txt (ctrl vs ctrl2) and
<out>/oc_c<N>.txt (ctrl vs ours).  Prints both VERDICT lines, the SOUND / UNINIT / ENV lines, the words that differ in ours but not in the
control pair (split: "static" = differ from frame 0 in every frame, never written by the control = link layout constants; "moving" = the
rest, with their first frame), and one `STATEHASH course N:` line per course:
  UNJUDGED  the control pair differs or an ALOCK timeout (statecmp exit 3) -- ours is then read against the control pair's words
            (ENV words, data/state_env.txt, are outside the verdict: a control pair that differs only in them is IDENTICAL)
  IDENTICAL / DIVERGENT  ours vs control
  CONTROL PAIR ONLY  the block names no ours run
Exit 0 unless an input is missing or statecmp breaks (rule 30: a skipped compare is a FAIL inside a gate); the verdict never fails the gate."""
import io, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
GH = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import statecmp

DATA = os.path.join(HERE, "data")
DEAD = "Lua,qhull,zlib/PNG/TIFF,CRT,DirectShow"     # the standalone link's replaced layers (README usage)
ROW = re.compile(r"^  ([0-9a-f]{8}) .*?\s(\d+)\s+(\d+) [0-9a-f]{8} [0-9a-f]{8} writes (\d+) ")
KEYS = ("VERDICT:", "SOUND:", "UNINIT:", "ENV:", "ALOCK:")


def run(a, b, path, **kw):
    buf = io.StringIO()
    r = statecmp.compare(a, b, ptr=DATA + "/state_ptrwords.txt", mask=DATA + "/state_mask.txt", rwpad=DATA + "/state_rwpad.txt",
                         sound=DATA + "/state_sound.txt", uninit=DATA + "/state_uninit.txt", env=DATA + "/state_env.txt",
                         show=1 << 30, out=buf, **kw)
    txt = buf.getvalue()
    open(path, "w").write(txt)
    words, part = {}, False
    for l in txt.splitlines():
        if l.startswith("words that ever differ"):
            part = True
        elif part and not l.startswith("  "):
            part = False
        elif part:
            m = ROW.match(l)
            words[int(m.group(1), 16)] = (int(m.group(2)), int(m.group(3)), int(m.group(4)))
    nfr = int(re.search(r"^compared (\d+) frames", txt, re.M).group(1))
    lines = {k: re.search(r"^%s.*$" % k, txt, re.M) for k in KEYS}
    return r, words, nfr, {k: (m.group(0) if m else "") for k, m in lines.items()}


def blocks():
    """gate_pair.txt -> [{course, ctrl, ctrl2[, ours, map]}]; each `course N` line opens a block"""
    out = []
    for l in open(DATA + "/gate_pair.txt"):
        if not l.strip() or l[0] == "#":
            continue
        k, v = l.split(None, 1)
        if k == "course":
            out.append({})
        elif not out:
            sys.exit("STATEHASH FAIL: data/gate_pair.txt: %s row before the first `course` line" % k)
        out[-1][k] = v.strip()
    bad = [b.get("course", "?") for b in out if not ("ctrl" in b and "ctrl2" in b and ("ours" in b) == ("map" in b))]
    if not out or bad:
        sys.exit("STATEHASH FAIL: data/gate_pair.txt: no course block, or a block without ctrl + ctrl2 (+ ours with map): %s" % " ".join(bad))
    return out


def course(cfg, out):
    c = cfg["course"]
    p = {k: os.path.join(GH, cfg[k]) for k in ("ctrl", "ctrl2", "ours", "map") if k in cfg}
    rc, wc, _, lc = run(p["ctrl"], p["ctrl2"], "%s/cc_c%s.txt" % (out, c))
    print("statehash course %s (stored pair, gate_pair.txt): ctrl %s, ours %s" % (c, cfg["ctrl"], cfg.get("ours", "none stored")))
    print("  control pair: " + lc["VERDICT:"])
    if "ours" not in p:
        for k in KEYS[1:]:
            if lc[k]:
                print("  control pair " + lc[k])
        print("STATEHASH course %s: %s (report only; detail %s/cc_c%s.txt)" % (c, "CONTROL PAIR ONLY, no ours run stored; control pair " + (
            "ALOCK timeout" if lc["ALOCK:"] else "IDENTICAL" if rc < 0 else "differs from frame %d" % rc), out, c))
        return
    ro, wo, nfr, lo = run(p["ctrl"], p["ours"], "%s/oc_c%s.txt" % (out, c), mapb=p["map"], dead=DEAD)
    print("  ours:         " + lo["VERDICT:"])
    for k in KEYS[1:]:
        if lo[k] or lc[k]:
            print("  ours %s / control pair %s" % (lo[k] or "-", lc[k] or "-"))
    extra = {va: w for va, w in wo.items() if va not in wc}
    static = [va for va, (f0, k, wr) in extra.items() if f0 == 0 and k == nfr and wr == 0]
    moving = sorted((f0, va) for va, (f0, k, wr) in extra.items() if va not in static)
    print("  ours-only words (not in the control pair's %d): %d moving%s, %d static (frame 0 on, never written by the control)" % (
        len(wc), len(moving), " (first frame %d: %08x %s)" % (moving[0][0], moving[0][1], statecmp.Syms().name(moving[0][1])) if moving else "", len(static)))
    unj = rc >= 0 or lc["ALOCK:"] or lo["ALOCK:"]
    print("STATEHASH course %s: %s (report only, never fails; detail %s/oc_c%s.txt, %s/cc_c%s.txt)" % (c,
        "UNJUDGED (control pair %s)" % ("ALOCK timeout" if lc["ALOCK:"] or lo["ALOCK:"] else "differs from frame %d" % rc) if unj else
        "IDENTICAL" if ro < 0 else "DIVERGENT from frame %d" % ro, out, c, out, c))


def main():
    out = sys.argv[1]
    os.makedirs(out, exist_ok=True)
    bl = blocks()
    miss = [os.path.join(GH, b[k]) for b in bl for k in ("ctrl", "ctrl2", "ours", "map") if k in b and not os.path.isfile(os.path.join(GH, b[k]))]
    if miss:
        sys.exit("STATEHASH FAIL: stored input missing (data/gate_pair.txt): " + " ".join(miss))
    for b in bl:
        course(b, out)


if __name__ == "__main__":
    main()
