#!/usr/bin/env python3
"""linkrepro.py <work dir> [harness|release|both]   |   --compare <a dir> <b dir>   |   --selftest
Link reproducibility (W11 B1): two links of the tree with PYTHONHASHSEED=1 and =2, side by side, compared byte for byte except the PE
TimeDateStamp (ours.exe) and the map's "Timestamp is" line (ours.map).  A set / dict iteration choice in a link tool shows up as a difference.
  harness  the gate's standalone link (gate.sh standalone_link env, HARNESS=1, DELTA from output/gate/standalone_delta.txt; one rerun on
           "DELTA mismatch" with the printed value)        -> <work dir>/harness_s1, harness_s2
  release  tools/delink/link_release.sh (no harness)          -> <work dir>/release_s1, release_s2
Default both.  Exit 0 = every form EQUAL, 1 = a DIFF or a failed link.  Logs: <work dir>/<form>_s<seed>.log.  Never while a gate holds the lock
(it is a scratch check, not a gate step)."""
import os, re, struct, subprocess, sys, tempfile
from concurrent.futures import ThreadPoolExecutor
T = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(T, "..", ".."))

def gate_env():
    lr = re.search(r"lr=\$\{LIB_RW-([^}]*)\}", open(os.path.join(G, "tools", "gate.sh")).read())      # one owner of the default group list: gate.sh (as run.py preflight)
    if not lr: sys.exit("linkrepro: no LIB_RW default in tools/gate.sh")
    return {"LIB_LUA": "src", "LIB_QHULL": "src", "LIB_ZLIB": "src", "LIB_CRT": "libcmt", "LIB_RW": lr.group(1), "DROP_RW": "1",
            "DROP_RWTRAP": "1", "DROP_DINPUT": "1", "MKRANGE": "1", "GEN_DATA": "1", "IMPLIBS": "1", "HARNESS": "1"}

def link(form, seed, w):
    """-> the dir holding ours.exe / ours.map, or None (log in w/<form>_s<seed>.log)"""
    env = dict(os.environ, PYTHONHASHSEED=str(seed)); log = open(os.path.join(w, "%s_s%d.log" % (form, seed)), "w")
    d = os.path.join(w, "%s_s%d" % (form, seed))
    if form == "release":
        return d if subprocess.call(["bash", os.path.join(T, "link_release.sh"), d], env=env, stdout=log, stderr=subprocess.STDOUT) == 0 else None
    sd = os.path.join(G, "output", "gate", "standalone_delta.txt"); delta = open(sd).read().strip() if os.path.exists(sd) else "ba000"
    for k in range(2):
        dk = d if k == 0 else d + "_r"
        p = subprocess.run(["bash", os.path.join(T, "link_ours.sh"), dk, "0x400000", "--automatch", "--automatch-near", "--bridges", "--clusters"],
                           env=dict(env, DELTA=delta, **gate_env()), capture_output=True, text=True)
        log.write(p.stdout + p.stderr)
        if p.returncode == 0: return dk
        m = re.search(r"DELTA mismatch: rerun with DELTA=([0-9a-f]+)", p.stdout)
        if not m: return None
        delta = m.group(1)
    return None

def exe_masked(p):
    b = bytearray(open(p, "rb").read()); pe = struct.unpack_from("<I", b, 0x3c)[0]; ts = b[pe + 8:pe + 12]
    b[pe + 8:pe + 12] = bytes(4)      # IMAGE_FILE_HEADER.TimeDateStamp
    # SK2 W16: link 14.x copies it into each debug directory entry (v143 exe: one POGO entry); VC7 link.exe writes no debug directory (no-op)
    ns, osz = struct.unpack_from("<H", b, pe + 6)[0], struct.unpack_from("<H", b, pe + 20)[0]
    rva, sz = struct.unpack_from("<II", b, pe + 24 + 96 + 48) if osz >= 96 + 56 else (0, 0)
    for k in range(ns if sz else 0):
        va, rs, rp = struct.unpack_from("<III", b, pe + 24 + osz + 40 * k + 12)
        if va <= rva < va + rs:
            for o in range(rp + rva - va + 4, rp + rva - va + sz, 28):
                if b[o:o + 4] == ts: b[o:o + 4] = bytes(4)      # only a copy of the header stamp
    return bytes(b)

def map_masked(p):
    return [l for l in open(p, errors="replace").read().splitlines() if not l.startswith(" Timestamp is")]

def compare(a, b):
    """-> list of differences between two link dirs (empty = EQUAL except the timestamp)"""
    out = []
    ea, eb = exe_masked(os.path.join(a, "ours.exe")), exe_masked(os.path.join(b, "ours.exe"))
    if ea != eb:
        n = sum(x != y for x, y in zip(ea, eb)) + abs(len(ea) - len(eb))
        first = next((i for i, (x, y) in enumerate(zip(ea, eb)) if x != y), min(len(ea), len(eb)))
        out.append("ours.exe: %d byte(s) differ (sizes %d / %d), first at file offset 0x%x" % (n, len(ea), len(eb), first))
    ma, mb = map_masked(os.path.join(a, "ours.map")), map_masked(os.path.join(b, "ours.map"))
    if ma != mb:
        i = next((i for i, (x, y) in enumerate(zip(ma, mb)) if x != y), min(len(ma), len(mb)))
        out.append("ours.map: line %d differs: %r / %r" % (i + 1, ma[i] if i < len(ma) else "<end>", mb[i] if i < len(mb) else "<end>"))
    return out

def selftest():
    """pairs built here: timestamp only -> EQUAL, one code byte -> DIFF, one map line -> DIFF"""
    def pe(ts, code):
        b = bytearray(0x200); b[0:2] = b"MZ"; struct.pack_into("<I", b, 0x3c, 0x80); b[0x80:0x84] = b"PE\0\0"; struct.pack_into("<I", b, 0x88, ts); b[0x100] = code
        return bytes(b)
    def mk(d, ts, code, line):
        os.makedirs(d); open(os.path.join(d, "ours.exe"), "wb").write(pe(ts, code))
        open(os.path.join(d, "ours.map"), "w").write(" ours\n\n Timestamp is %08x (Mon Nov 16 00:00:00 2026)\n\n  0001:00000000 %s\n" % (ts, line))
    w = tempfile.mkdtemp(); mk(w + "/a", 1, 0x90, "_f"); mk(w + "/b", 2, 0x90, "_f"); mk(w + "/c", 1, 0xcc, "_f"); mk(w + "/d", 1, 0x90, "_g")
    assert compare(w + "/a", w + "/b") == [], compare(w + "/a", w + "/b")
    assert len(compare(w + "/a", w + "/c")) == 1 and "ours.exe" in compare(w + "/a", w + "/c")[0]
    assert len(compare(w + "/a", w + "/d")) == 1 and "ours.map" in compare(w + "/a", w + "/d")[0]
    def dbg(d, ts, ets):      # SK2: one section at rva 0x1000 (file 0x1c0) holding a debug directory entry whose stamp is ets
        mk(d, ts, 0x90, "_f"); b = bytearray(open(d + "/ours.exe", "rb").read())
        struct.pack_into("<H", b, 0x86, 1); struct.pack_into("<H", b, 0x94, 0xe0); struct.pack_into("<II", b, 0x80 + 24 + 96 + 48, 0x1000, 28)
        struct.pack_into("<III", b, 0x80 + 24 + 0xe0 + 12, 0x1000, 0x40, 0x1c0); struct.pack_into("<I", b, 0x1c4, ets)
        open(d + "/ours.exe", "wb").write(b)
    dbg(w + "/e", 1, 1); dbg(w + "/f", 2, 2); dbg(w + "/g", 2, 3)
    assert compare(w + "/e", w + "/f") == [], compare(w + "/e", w + "/f")
    assert len(compare(w + "/e", w + "/g")) == 1, "a debug stamp that is not the header stamp must count"
    print("linkrepro selftest: ok (timestamp-only pair EQUAL incl. debug directory copy, code byte DIFF, other debug stamp DIFF, map line DIFF)")

def main(a):
    if a[:1] == ["--selftest"]: return selftest()
    if a[:1] == ["--compare"] and len(a) == 3:
        d = compare(a[1], a[2]); print("\n".join(d) or "EQUAL except the timestamp"); return 1 if d else 0
    if not a or a[0].startswith("-"): print(__doc__); return 2
    w = os.path.abspath(a[0]); forms = ["harness", "release"] if len(a) < 2 or a[1] == "both" else [a[1]]
    if os.path.exists(w) and os.listdir(w): sys.exit("linkrepro: %s is not empty: use a fresh dir" % w)
    os.makedirs(w, exist_ok=True); rc = 0
    for f in forms:
        with ThreadPoolExecutor(2) as ex:
            d1, d2 = ex.map(lambda s: link(f, s, w), (1, 2))
        if not d1 or not d2:
            print("linkrepro %s: link FAILED (%s)" % (f, ", ".join("%s_s%d.log" % (f, s) for s, d in ((1, d1), (2, d2)) if not d))); rc = 1; continue
        d = compare(d1, d2)
        print("linkrepro %s: %s" % (f, "EQUAL except the timestamp (PYTHONHASHSEED 1 / 2)" if not d else "DIFF"))
        for l in d: print("  " + l)
        rc |= bool(d)
    return rc

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
