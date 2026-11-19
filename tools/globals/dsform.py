#!/usr/bin/env python3
"""dsform.py (W16 DS): globgen.py's output as committed source (data/link/gen/), and back.  No exe is opened.
  split <gen dir> <out dir>              rdata.c / data.c / bss.c -> <sec>.c (globgen header + #include lines) + <sec>_<va>.inc parts
                                         (cut at the fixed VAs of CUTS, between definitions), the held-back rows (HELD, INV 3) as HELD_<va>
                                         with their initialisers in held.h (private, never exported), held_fill.inc (names, VAs, sizes:
                                         CC=v143 fills them at start-up from MFL_original.exe), objcopy_<sec>.txt copied.
  join <data dir> <out dir> [--held-zero] the exact globgen rdata.c / data.c / bss.c + objcopy lists again (byte-identical, no held.h
                                         read with --held-zero: held rows = {0} and held_fill.inc appended to data.c; CC=v143 only)
  check <gen dir> <data dir>             gate step: split(<gen dir>) == <data dir> file by file and join(<data dir>) == <gen dir>; exit 1 on drift
  selftest <gen dir>                     the check passes on a fresh split and FAILS on a changed byte / a missing held.h (negative fixture, rule 27)
Not under src/: globgen / cdecls read every .c / .h / .inc there (used_idents, decls) and would feed on their own output."""
import filecmp, os, re, shutil, sys, tempfile

SECS = ("rdata", "data", "bss")
LO = {"rdata": 0x5dd000, "data": 0x5fb000, "bss": 0x64d000}
CUTS = {"rdata": [0x5e8000], "data": [0x608000, 0x618000, 0x630000], "bss": [0x700000, 0x7a0000]}      # fixed: a new row moves no other part
HELD = {0x5e1838, 0x5e1920, 0x5e1980, 0x5e19f0, 0x5fb168, 0x600000, 0x600080, 0x602000}      # INV 3 media-like rows (default 1): not public
ROW = re.compile(r"^/\*([0-9a-f]{8})\*/ |^/\* PESPAN ([0-9a-f]{8}) ")
INC = re.compile(r'^#include "([\w.]+)"\n$')


def split(gen, out):
    os.makedirs(out, exist_ok=True)
    held_h, fill = ["/* W16 DS: initialisers of the held-back rows (INV 3). PRIVATE: never exported. */\n"], []
    lay = {int(l.split(",")[0], 16): l.split(",") for l in open(os.path.join(gen, "globals_layout.csv")).readlines()[1:]}
    for sec in SECS:
        ls = open(os.path.join(gen, sec + ".c")).read().splitlines(True)
        rows = [(i, int(m.group(1) or m.group(2), 16)) for i, l in enumerate(ls) for m in [ROW.match(l)] if m]
        def start(i):      # a definition's block starts with its struct type (#pragma pack / struct lines in front)
            while i and ls[i - 1].startswith(("#pragma pack", "struct ")): i -= 1
            return i
        bounds = [(LO[sec], start(rows[0][0]))]
        for c in CUTS[sec]: bounds.append((c, start(next(i for i, va in rows if va >= c))))
        for i, va in rows:
            if va in HELD:
                a, b = ls[i].split(" = ", 1)
                assert b.endswith(";\n"), ls[i][:80]
                ls[i] = "%s = HELD_%08x;\n" % (a, va)
                held_h.append("#define HELD_%08x %s\n" % (va, b[:-2]))
                fill.append((va, int(lay[va][1]), lay[va][3], sec))
        w = ls[:bounds[0][1]] + (['#include "held.h"\n'] if sec != "bss" else [])
        for k, (va, i) in enumerate(bounds):
            j = bounds[k + 1][1] if k + 1 < len(bounds) else len(ls)
            open(os.path.join(out, "%s_%08x.inc" % (sec, va)), "w").write("".join(ls[i:j]))
            w.append('#include "%s_%08x.inc"\n' % (sec, va))
        open(os.path.join(out, sec + ".c"), "w").write("".join(w))
        shutil.copyfile(os.path.join(gen, "objcopy_%s.txt" % sec), os.path.join(out, "objcopy_%s.txt" % sec))
    assert {f[0] for f in fill} == HELD, "held rows missing from the generated data: %s" % sorted(hex(x) for x in HELD - {f[0] for f in fill})
    open(os.path.join(out, "held.h"), "w").write("".join(held_h))
    open(os.path.join(out, "held_fill.inc"), "w").write(
        "/* W16 DS, CC=v143 only (dsform.py join --held-zero appends this to data.c): the held-back rows are {0} in this build;\n"
        " * mfl_held_fill (src/mfl_resources.c) reads each from the user's MFL_original.exe at file offset va - 0x400000. */\n"
        + "".join("extern const char %s[];\n" % n for va, sz, n, s in sorted(fill) if s == "rdata")
        + "struct mfl_held_row { const void *at; unsigned long va, size; };\nconst struct mfl_held_row mfl_held[] = {\n"
        + "".join("  {%s, 0x%08x, %d},\n" % (n, va, sz) for va, sz, n, s in sorted(fill)) + "  {0, 0, 0}\n};\n")


def join(src, out, held_zero=False):
    os.makedirs(out, exist_ok=True)
    held = {} if held_zero else dict(l[len("#define HELD_"):].rstrip("\n").split(" ", 1) for l in open(os.path.join(src, "held.h")) if l.startswith("#define HELD_"))
    for sec in SECS:
        t = []
        for l in open(os.path.join(src, sec + ".c")).readlines():
            m = INC.match(l)
            if not m: t.append(l); continue
            if m.group(1) == "held.h": continue
            t.append(re.sub(r" = HELD_([0-9a-f]{8});\n", lambda h: " = %s;\n" % ("{0}" if held_zero else held[h.group(1)]), open(os.path.join(src, m.group(1))).read()))
        if held_zero and sec == "data": t.append(open(os.path.join(src, "held_fill.inc")).read())
        open(os.path.join(out, sec + ".c"), "w").write("".join(t))
        shutil.copyfile(os.path.join(src, "objcopy_%s.txt" % sec), os.path.join(out, "objcopy_%s.txt" % sec))


def check(gen, data):
    tmp = tempfile.mkdtemp()
    try:
        split(gen, tmp + "/s")
        a, b = sorted(os.listdir(tmp + "/s")), sorted(os.listdir(data))
        bad = ["file set: generated %s, committed %s" % (sorted(set(a) - set(b)), sorted(set(b) - set(a)))] if a != b else []
        bad += ["%s differs" % f for f in a if f in b and not filecmp.cmp(os.path.join(tmp, "s", f), os.path.join(data, f), shallow=False)]
        try:
            join(data, tmp + "/j")
            bad += ["join(committed) %s != generated" % f for f in os.listdir(tmp + "/j") if not filecmp.cmp(os.path.join(tmp, "j", f), os.path.join(gen, f), shallow=False)]
        except (OSError, KeyError) as e:
            bad.append("join(committed) failed: %r" % e)
    finally:
        shutil.rmtree(tmp)
    for x in bad: print("DS DRIFT: " + x)
    print("ds check: %s (%s vs %s)" % ("FAIL" if bad else "generated == committed", gen, data))
    return 1 if bad else 0


def selftest(gen):
    tmp = tempfile.mkdtemp()
    try:
        d = tmp + "/d"; split(gen, d)
        assert check(gen, d) == 0, "fresh split does not pass"
        join(d, tmp + "/z", held_zero=True)
        z = open(tmp + "/z/data.c").read() + open(tmp + "/z/rdata.c").read()
        assert "HELD_" not in z and "mfl_held[]" in z and all(re.search(r"/\*%08x\*/ [^\n]* = \{0\};" % va, z) for va in HELD), "held-zero join"
        assert not any("#define" in open(os.path.join(d, f)).read() for f in os.listdir(d) if f.endswith(".inc")), "held values in a public part"
        p = os.path.join(d, sorted(f for f in os.listdir(d) if f.startswith("data_"))[1])
        b = open(p).read(); open(p, "w").write(b.replace("0x", "0x0", 1))      # one changed value (same number, other text)
        assert check(gen, d) == 1, "a changed part is not caught"
        open(p, "w").write(b); os.remove(os.path.join(d, "held.h"))
        assert check(gen, d) == 1, "a missing held.h is not caught"
    finally:
        shutil.rmtree(tmp)
    print("dsform selftest ok")


if __name__ == "__main__":
    a = sys.argv[1:]
    if a[:1] == ["split"]: split(a[1], a[2])
    elif a[:1] == ["join"]: join(a[1], a[2], "--held-zero" in a)
    elif a[:1] == ["check"]: sys.exit(check(a[1], a[2]))
    elif a[:1] == ["selftest"]: selftest(a[1])
    else: sys.exit(__doc__)
