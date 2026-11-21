#!/usr/bin/env python3
"""no_original.py: the release guard (plan W16 section 4): FAIL, and nothing is released, when original-derived files or bytes are present.
usage: no_original.py <checkout> [--release <dir>] [--manifest <file>] | --selftest
  1. every tracked file of <checkout> (git ls-files) passes the public export's deny rules: extension, name, size, binary, text patterns, total
  2. <checkout>/data/zero_original_baseline.txt exists and every code: / data: / rsrc row is 0 (original bytes per layer of the gated link)
  3. --release <dir>: only the allow-listed release files (no MFL.exe, no Microsoft DLL); every .exe holds the RT_MANIFEST byte-equal to
     --manifest (default <checkout>/data/link/MFLng.manifest) and, when <checkout>/assets/icon/mflng.ico exists, exactly the images of that
     icon (RT_ICON / RT_GROUP_ICON, our own drawing) and nothing else; every .dll holds no resource
     (assets/icon/mflng.ico is the one tracked binary file the deny rules let through)
Static only: it cannot see original code bytes inside a CI-built exe (no MFL.exe in CI); the gated VC7 link proves those zero (item 2)."""
import fnmatch, os, re, struct, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from rw_stub_guard import parse_pe, make_pe, icon_bad

# the deny rules of the private tools/export_public.py (selftest compares them when that file is beside tools/ci); literals that would
# match their own pattern are split so this file passes its own scan
BAD_EXT = {".exe", ".dll", ".obj", ".lib", ".pdb", ".fidb", ".gdt", ".bin", ".piz", ".whl", ".pkl", ".iso", ".png", ".ppm", ".wav", ".dff", ".txd", ".bmp", ".res"}
BAD_NAME = ["CO-AGENT-*", "ghidra_types.h", "decls.h", "*.bak*"]
BAD_TEXT = [r"/ho" r"me/", r"/tmp/claude-[0-9]", r"reference" r"s/sdk", r"rwsd" r"k-", r"reference" r"s/", r"en_vs\.net", r"[\w.+-]+@(?!users\.noreply\.github\.com)[\w-]+\.[a-z]{2,}",
            r"^struct R[wpt][A-Z]\w* \{", r"[0-9a-fA-F]{65,}", r'^#include "(ghidra_types|prelude)\.h"']
CODFH_HDR = re.compile(r"Adapted from codfh-win32 by L2KSolkov, https://github\.com/L2KSolkov/codfh-win32, \S+ @[0-9a-f]{7,}")
MAX_FILE, MAX_TOTAL = 2_000_000, 60_000_000
TP_TEXT = [r"\b(thunk_FUN|FUN|DAT|LAB|PTR)_00[0-9a-fA-F]{6}\b"]      # third_party/: the email rule is off, every other rule on, plus this
# W16 VIDCI: + the movie backend and the shared LGPL FFmpeg DLLs it imports
RELEASE_OK = ["MFLng.exe", "MFLng.nofix.exe", "MFLng-launch.exe", "mflrw.dll","mfl_audio_miniaudio.dll",
              "mfl_video_ffmpeg.dll", "avformat-*.dll", "avcodec-*.dll", "avutil-*.dll", "swscale-*.dll", "INSTALL.txt", "BUILDINFO.txt", "SHA256SUMS", "LICENSES/*.txt", "*.map"]
# W16 VIDCI vd3: a shipped file needs the licence notices of the third-party code inside it (static winpthreads: CI's -static / -l:libwinpthread.a)
NEEDS_LICENSE = [("mflrw.dll", ["winpthreads.txt"]), ("avutil-*.dll", ["ffmpeg.txt", "winpthreads.txt"]), ("av*-*.dll", ["ffmpeg.txt"]),
                 ("swscale-*.dll", ["ffmpeg.txt"]), ("mfl_video_ffmpeg.dll", ["ffmpeg.txt", "winpthreads.txt"])]
ICON = "assets/icon/mflng.ico"
ROW = re.compile(r"^(code:\S+|data:\S+|rsrc) (\d+)$")


def deny(root, files):
    bad, total = [], 0
    rx = [re.compile(p, re.M) for p in BAD_TEXT]
    tp = [r for r in rx if "@" not in r.pattern] + [re.compile(p, re.M) for p in TP_TEXT]
    for rel in files:
        b = open(os.path.join(root, rel), "rb").read(); total += len(b); n = os.path.basename(rel)
        if os.path.splitext(n)[1].lower() in BAD_EXT: bad.append((rel, "forbidden extension"))
        elif any(fnmatch.fnmatch(n, p) for p in BAD_NAME): bad.append((rel, "forbidden name"))
        elif len(b) > MAX_FILE: bad.append((rel, "over %d bytes" % MAX_FILE))
        elif b"\0" in b[:8192] and rel != ICON: bad.append((rel, "binary content"))      # ICON: our own drawing (assets/icon/mflng.svg), the one binary file allowed
        else:
            t = b.decode("utf-8", "replace"); rs = tp if rel.startswith("third_party/") else rx
            for r in (rs if not CODFH_HDR.search(t[:2000]) else [r for r in rs if not r.pattern.startswith("^struct")]):
                m = r.search(t)
                if m: bad.append((rel, "text %r: %r" % (r.pattern, m.group(0)[:40]))); break
    if total > MAX_TOTAL: bad.append(("*", "total %d bytes over cap" % total))
    return bad


def baseline(root):
    p = os.path.join(root, "data/zero_original_baseline.txt")
    if not os.path.exists(p):
        return [("data/zero_original_baseline.txt", "missing")]
    rows = [ROW.match(l.strip()) for l in open(p)]
    rows = [m for m in rows if m]
    if not rows:
        return [("data/zero_original_baseline.txt", "no code:/data:/rsrc rows")]
    return [("data/zero_original_baseline.txt", "%s = %s original bytes" % m.groups()) for m in rows if m.group(2) != "0"]


def release(d, manifest, icon=None):
    bad = []
    for top, _, fs in os.walk(d):
        for f in fs:
            rel = os.path.relpath(os.path.join(top, f), d).replace(os.sep, "/")
            if not any(fnmatch.fnmatch(rel, a) for a in RELEASE_OK):
                bad.append((rel, "not an allowed release file")); continue
            if rel.endswith((".exe", ".dll")):
                p = parse_pe(open(os.path.join(d, rel), "rb").read())
                want = ([24], [manifest]) if rel.endswith(".exe") else ([], [])
                if icon and rel.endswith(".exe"):      # our own icon (assets/icon/mflng.ico) beside the manifest: RT_ICON 3, RT_MANIFEST 24, RT_GROUP_ICON 14
                    ib = icon_bad(p, icon); p = dict(p, res_types=[t for t in p["res_types"] if t not in (3, 14)])
                    if ib: bad.append((rel, ib))
                if (p["res_types"], p["manifests"]) != want:
                    bad.append((rel, "resources %s / manifest sizes %s, want %s" % (p["res_types"], [len(m) for m in p["manifests"]], "the committed manifest only" if want[0] else "none")))
            for pat, lic in NEEDS_LICENSE:
                if fnmatch.fnmatch(rel, pat):
                    bad += [(rel, "needs LICENSES/" + l) for l in lic if not os.path.exists(os.path.join(d, "LICENSES", l))]
    return bad


def run(root, rel_dir=None, manifest=None):
    files = [f for f in subprocess.run(["git", "-C", root, "ls-files", "-z"], capture_output=True, check=True).stdout.decode().split("\0") if f]
    bad = deny(root, files) + baseline(root)
    if rel_dir:
        ip = os.path.join(root, ICON)
        bad += release(rel_dir, open(manifest or os.path.join(root, "data/link/MFLng.manifest"), "rb").read(), open(ip, "rb").read() if os.path.exists(ip) else None)
    for f, why in bad:
        print("FAIL %s - %s" % (f, why))
    print("no_original: %d tracked files, %d failures%s" % (len(files), len(bad), ", release dir " + rel_dir if rel_dir else ""))
    return 1 if bad else 0


def selftest():
    import shutil, tempfile
    d = tempfile.mkdtemp()
    try:
        def put(rel, b):
            p = os.path.join(d, rel); os.makedirs(os.path.dirname(p), exist_ok=True); open(p, "wb").write(b)
        put("ok.c", b"int x;\n"); put("data/zero_original_baseline.txt", b"# c\ncode:app 0\ndata:rdata:lib 0\nrsrc 0\nstate cars 1\n")
        assert deny(d, ["ok.c"]) == [] and baseline(d) == []
        for rel, body in [("MFL.exe", b"MZ"), ("a.txt", b"/ho" b"me/x/y"), ("b.bin2", b"a\0b"), ("c.h", b"struct RwFoo {\n};"), ("e.txt", b"x" * (MAX_FILE + 1))]:
            put(rel, body); assert [r for r, _ in deny(d, ["ok.c", rel])] == [rel], "planted %s not caught" % rel
        put("i.c", b"/* Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/a.c @e578672, changes: x */\nstruct RwFoo {\n};")
        assert deny(d, ["i.c"]) == [], "credited codfh file denied"
        put("m.txt", b"mail jloup@" b"example.org"); put("third_party/z/m.c", b"/* jloup@" b"example.org */")
        assert [r for r, _ in deny(d, ["m.txt", "third_party/z/m.c"])] == ["m.txt"], "email rule wrong inside / outside third_party/"
        for rel, body in [("third_party/z/h.c", b"/ho" b"me/x"), ("third_party/z/f.c", b"int FUN_" b"004da0e0;"), ("third_party/z/s.h", b"struct RwFoo {\n};"), ("third_party/z/b.c", b"a\0b"), ("third_party/z/x.dll", b"MZ")]:
            put(rel, body); assert [r for r, _ in deny(d, [rel])] == [rel], "planted %s not caught" % rel
        put("data/zero_original_baseline.txt", b"code:app 0\ndata:data:lib 12\n")
        assert [w for _, w in baseline(d)] == ["data:data:lib = 12 original bytes"], "planted non-zero row not caught"
        os.remove(os.path.join(d, "data/zero_original_baseline.txt")); assert baseline(d), "missing baseline not caught"
        m = b"<assembly/>"; r = os.path.join(d, "rel")
        put("rel/MFLng.exe", make_pe({"kernel32.dll": ["A"]}, res={24: m})); put("rel/mflrw.dll", make_pe({"kernel32.dll": ["A"]})); put("rel/LICENSES/zlib.txt", b"z")
        put("rel/avcodec-61.dll", make_pe({"kernel32.dll": ["A"]}))
        assert sorted(w for _, w in release(r, m)) == ["needs LICENSES/ffmpeg.txt", "needs LICENSES/winpthreads.txt"], "missing licence not caught"
        put("rel/LICENSES/ffmpeg.txt", b"f"); put("rel/LICENSES/winpthreads.txt", b"w")
        assert release(r, m) == [], release(r, m)
        put("rel/avcodec-61.dll", make_pe({"kernel32.dll": ["A"]}, res=[16])); assert [f for f, _ in release(r, m)] == ["avcodec-61.dll"], "DLL resource not caught"
        put("rel/avcodec-61.dll", make_pe({"kernel32.dll": ["A"]}))
        put("rel/vcruntime140.dll", make_pe({"kernel32.dll": ["A"]})); assert [f for f, _ in release(r, m)] == ["vcruntime140.dll"], "Microsoft DLL not caught"
        os.remove(os.path.join(r, "vcruntime140.dll"))
        put("rel/MFLng.nofix.exe", make_pe({"kernel32.dll": ["A"]}, res={24: m, 3: b"icon"})); assert [f for f, _ in release(r, m)] == ["MFLng.nofix.exe"], "extra resource not caught"
        img = b"PNGx"; ico = struct.pack("<HHH", 0, 1, 1) + struct.pack("<BBBBHHII", 16, 16, 0, 0, 1, 32, len(img), 22) + img      # our own icon is the one extra allowed
        put("rel/MFLng.nofix.exe", make_pe({"kernel32.dll": ["A"]}, res={24: m, 3: img, 14: b"g"})); assert [f for f, _ in release(r, m, ico)] == ["MFLng.exe"], "own icon refused (MFLng.exe has none: caught)"
        assert [f for f, _ in release(r, m)] == ["MFLng.nofix.exe"], "icon accepted with no icon file given"
        put("rel/MFLng.nofix.exe", make_pe({"kernel32.dll": ["A"]}, res={24: m, 3: img + b"y", 14: b"g"})); assert sorted(f for f, _ in release(r, m, ico)) == ["MFLng.exe", "MFLng.nofix.exe"], "foreign icon not caught"
        put("data/ico.bin", b"\0\1"); assert [f for f, _ in deny(d, ["data/ico.bin"])] == ["data/ico.bin"], "binary elsewhere not caught"
        put(ICON, b"\0\1"); assert deny(d, [ICON]) == [], "the icon is denied"
        put("rel/MFLng.nofix.exe", make_pe({"kernel32.dll": ["A"]}, res={24: m + b" "})); assert [f for f, _ in release(r, m)] == ["MFLng.nofix.exe"], "changed manifest not caught"
    finally:
        shutil.rmtree(d)
    ep = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "export_public.py")
    if os.path.exists(ep):      # private tree: the rules must be the export's, value for value
        import importlib.util
        s = importlib.util.spec_from_file_location("export_public", ep); e = importlib.util.module_from_spec(s); s.loader.exec_module(e)
        assert (BAD_EXT, BAD_NAME, BAD_TEXT, TP_TEXT, MAX_FILE, MAX_TOTAL, CODFH_HDR.pattern) == (e.BAD_EXT, e.BAD_NAME, e.BAD_TEXT, e.TP_TEXT, e.MAX_FILE, e.MAX_TOTAL, e.CODFH_HDR.pattern), "deny rules differ from export_public.py"
        print("rules equal to export_public.py")
    print("no_original selftest OK")


def main(a):
    if a[:1] == ["--selftest"]:
        return selftest()
    opt = {}
    for k in ("--release", "--manifest"):
        if k in a:
            i = a.index(k); opt[k] = a[i + 1]; a = a[:i] + a[i + 2:]
    if len(a) != 1:
        sys.exit(__doc__)
    sys.exit(run(a[0], opt.get("--release"), opt.get("--manifest")))


if __name__ == "__main__":
    main(sys.argv[1:])
