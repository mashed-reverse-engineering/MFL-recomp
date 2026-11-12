#!/usr/bin/env python3
"""negtest_key.py [--key | --check | --store KEY | --selftest]  (strict_key.py pattern, engine switch window 5 A1)

check_all.sh skips the DLL groups of tools/delink/negtest.sh (the docker blocks, ~30 min) when the key over everything they depend on equals
output/gate/negtest_ok.key (written only after all groups passed).  Inputs, by class:
  dll      output/librw/mflrw.dll
  exe      game/MFL.exe
  harness  tools/delink/negtest.sh, every rw_*_check.sh, every file a check script names ($T/x), the #include "x" closure of the .c/.h among them,
           librw-mfl files named as $L/ or $LIBRW/ (a directory = its top-level headers)   [derived by reading the scripts, no hard-coded subset]
  negfix   tools/negtests/**, tools/d3dtrace/ref/neg_*
  gamedata the game's *.png (path + size; rw_pngread_check.sh copies them)
  image    every `<image> wine /t/` reference in the rw_*_check.sh (the pinned Wine image)
  compiler `i686-w64-mingw32-gcc --version` and g++ (the harness exes are built by the host compiler on every run)
--key prints the key (nothing and exit 1 on any error), --check prints `HIT <key>` or `MISS <key>` (`MISS error` on any error: fail-open),
--store writes the key file."""
import glob, hashlib, os, re, subprocess, sys

T = os.path.dirname(os.path.abspath(__file__))
if os.path.basename(os.path.dirname(T)) == "agents":
    T = os.path.join(os.path.dirname(os.path.dirname(T)), "tools")      # run from agents/a12_w5/: read the live tools tree (selftest before the install)
G = os.path.dirname(T)
D = os.path.join(T, "delink")
KF = os.path.join(G, "output", "gate", "negtest_ok.key")
GAME = os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed")


def rd(p):
    with open(p, "rb") as f:
        return f.read()


def closure(files):
    """files + the local #include "x" closure of the .c/.h among them"""
    out, todo = [], list(files)
    while todo:
        f = todo.pop()
        if f in out:
            continue
        out.append(f)
        if f.endswith((".c", ".h")):
            for m in re.findall(r'^\s*#\s*include\s+"([^"]+)"', rd(f).decode("latin-1"), re.M):
                p = os.path.normpath(os.path.join(os.path.dirname(f), m))
                if os.path.isfile(p):
                    todo.append(p)
    return sorted(out)


def items(G=G, D=D):
    """-> [(class, label, bytes)]"""
    it = []
    L = os.path.normpath(os.path.join(G, "..", "librw-mfl"))
    it.append(("dll", "mflrw.dll", rd(os.path.join(G, "output", "librw", "mflrw.dll"))))
    it.append(("exe", "MFL.exe", rd(os.path.join(G, "game", "MFL.exe"))))
    scripts = sorted(glob.glob(os.path.join(D, "rw_*_check.sh")))
    assert scripts, "no rw_*_check.sh"
    dep = [os.path.join(D, "negtest.sh")] + scripts
    refs = set()
    for s in scripts:
        txt = rd(s).decode("latin-1")
        refs.update(re.findall(r"(\S+) wine /t/", txt))
        for m in re.findall(r"\$T/([A-Za-z0-9_][A-Za-z0-9_./-]*)", txt):
            if os.path.isfile(os.path.join(D, m)):
                dep.append(os.path.join(D, m))
        for m in re.findall(r"\$\{?(?:L|LIBRW)\}?/([A-Za-z0-9_][A-Za-z0-9_./-]*)", txt):
            p = os.path.join(L, m)
            if os.path.isdir(p):
                dep += glob.glob(os.path.join(p, "*.h")) + glob.glob(os.path.join(p, "*.hpp"))
            elif os.path.isfile(p):
                dep.append(p)
    for f in closure(dep):
        it.append(("harness", os.path.relpath(f, G), rd(f)))
    nf = [os.path.join(dp, f) for dp, dn, fn in os.walk(os.path.join(T, "negtests")) for f in fn]
    nf += glob.glob(os.path.join(T, "d3dtrace", "ref", "neg_*"))
    for f in sorted(nf):
        it.append(("negfix", os.path.relpath(f, G), rd(f)))
    for dp, dn, fn in os.walk(GAME):
        for f in fn:
            if f.lower().endswith(".png"):
                p = os.path.join(dp, f)
                it.append(("gamedata", os.path.relpath(p, GAME), str(os.path.getsize(p)).encode()))
    it.sort(key=lambda x: (x[0], x[1]))      # os.walk order must not matter
    assert refs, "no image reference found"
    it.append(("image", "refs", "\n".join(sorted(refs)).encode()))
    for c in ("gcc", "g++"):
        it.append(("compiler", c, subprocess.run(["i686-w64-mingw32-" + c, "--version"], capture_output=True, check=True).stdout))
    return it


def key(it):
    h = hashlib.sha256()
    for c, lab, data in it:
        h.update(("%s\0%s\0%d\0" % (c, lab, len(data))).encode() + data)
    return h.hexdigest()


def decide(kf, k):
    try:
        return "HIT" if rd(kf).decode().strip() == k else "MISS"
    except OSError:
        return "MISS"      # a missing key file is a MISS


def selftest():
    it = items()
    base = key(it)
    assert base == key(items()), "key not stable between two runs"
    classes = sorted({c for c, _, _ in it})
    assert classes == ["compiler", "dll", "exe", "gamedata", "harness", "image", "negfix"], classes
    bad = 0
    labs = {l for c, l, _ in it if c == "harness"}      # derived, not hard-coded: the include closure, a .def, a python helper, the negtest script itself
    for need in ("tools/delink/fakedev.c", "tools/delink/fakedev.h", "tools/delink/fakedev_raster.c", "tools/delink/rw_lod_fixture.def", "tools/delink/rw_g0_check.py",
                 "tools/delink/rw_g0_replay.c", "tools/delink/negtest.sh", "tools/delink/rw_d3dras_check.c", "tools/delink/rw_d3dras_check.sh"):
        print("%-4s harness derives %s" % ("ok" if need in labs else "FAIL", need)); bad += need not in labs
    for c in classes:
        i = next(n for n, x in enumerate(it) if x[0] == c)
        d = it[i][2]
        for how, nd in (("first byte", bytes([d[0] ^ 1]) + d[1:]), ("last byte", d[:-1] + bytes([d[-1] ^ 1]))):
            k = key(it[:i] + [(c, it[i][1], nd)] + it[i + 1:])
            print("%-4s %-8s one %s of %s changes the key: %s" % ("ok" if k != base else "FAIL", c, how, it[i][1], k[:8]))
            bad += k == base
    # a removed / added file and a renamed one change it too
    for what, alt in (("file removed", it[1:]), ("file added", it + [("harness", "x", b"")])):
        k = key(alt)
        print("%-4s %s changes the key" % ("ok" if k != base else "FAIL", what)); bad += k == base
    tmp = KF + ".selftest_missing"
    print("%-4s missing key file -> %s" % ("ok" if decide(tmp, base) == "MISS" else "FAIL", decide(tmp, base))); bad += decide(tmp, base) != "MISS"
    os.makedirs(os.path.dirname(tmp), exist_ok=True)
    open(tmp, "w").write(base + "\n")
    r = (decide(tmp, base), decide(tmp, "0" * 64))
    os.remove(tmp)
    print("%-4s key file equal -> %s, different -> %s" % ("ok" if r == ("HIT", "MISS") else "FAIL", *r)); bad += r != ("HIT", "MISS")
    print("negtest_key selftest: %d inputs, %d classes, key %s -> %s" % (len(it), len(classes), base[:8], "PASS" if not bad else "FAIL (%d)" % bad))
    return 1 if bad else 0


if __name__ == "__main__":
    a = sys.argv[1:2]
    if a == ["--selftest"]:
        sys.exit(selftest())
    try:
        k = key(items())
    except Exception as e:      # fail-open: the caller runs negtest
        print("negtest_key: %s: %s" % (type(e).__name__, e), file=sys.stderr)
        if a == ["--check"]:
            print("MISS error")
            sys.exit(0)
        sys.exit(1)
    if a == ["--key"]:
        print(k)
    elif a == ["--check"]:
        print(decide(KF, k), k)
    elif a == ["--store"] and re.fullmatch(r"[0-9a-f]{64}", sys.argv[2] if len(sys.argv) > 2 else ""):
        os.makedirs(os.path.dirname(KF), exist_ok=True)
        open(KF, "w").write(sys.argv[2] + "\n")
    else:
        sys.exit(__doc__)
