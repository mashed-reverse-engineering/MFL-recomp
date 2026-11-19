#!/usr/bin/env python3
"""msvcw_compile.py --cl <cl> --out <dir> [--wine] [--root <tree>] [--jobs N] [--chunk N]
REPORT ONLY (exit 0 unless cl itself cannot run): compile-only check of the link's C source set with a modern MSVC
(v143, x86 target, /arch:IA32 = x87 only, like the original).  No link; nothing is written into the tree.
The same script runs in both places, so the local result is the CI result:
  local : tools/run.py docker msvcw-compile <out dir>   (msvc-wine in Docker, --wine: paths handed to cl as Z:\\...)
  CI    : .github/workflows/build.yml on windows-latest after ilammy/msvc-dev-cmd (arch amd64_x86), cl from PATH
Source set = tools/portsyntax.py groups (tier1, codfh, tier2 = automatch_near, enabled clusters, hand-written top level);
the export layout (src/, src/tier2, src/clusters/enabled.txt) is detected.  MFL_FIX_* stay at their src/mfl_fix.h default (off).
Writes <out>/cl.log, <out>/msvcw_files.csv (file, group, ok, errors, first_code, first_class, first_msg)
and <out>/msvcw_classes.csv (class, code, files, errors, example file, example msg)."""
import collections, csv, glob, os, re, subprocess, sys
from concurrent.futures import ThreadPoolExecutor

# /O2 /Gy as build_overlay.py; /arch:IA32 x87 code (v143 x86 default is SSE2); /GS- because the original has no cookies outside the
# files marked `compile: /GS`; /Zl: no default-lib records (the link layer chooses the CRT); /RTC is off unless asked (command-line default).
# /MD (W16 V143 item 1): the CC=v143 variant links the DLL CRT first (_DLL set, CRT functions dllimport); /MT is V143 item 4.
# Dropped markers: /G5 and /Op no longer exist in v143 (D9002 if passed); `compile: cl1310` has no meaning for a modern compiler.
BASE = ["/nologo", "/c", "/TC", "/O2", "/Gy", "/arch:IA32", "/GS-", "/MD", "/Zl", "/W3", "/diagnostics:classic",
        "/D_CRT_SECURE_NO_WARNINGS", "/D_CRT_NONSTDC_NO_DEPRECATE"]
DIAG = re.compile(r"^(.*?)\((\d+)(?:,\d+)?\)\s*:\s*(?:fatal )?error (C\d+):\s*(.*)$")
GLOBAL_ERR = re.compile(r"^(cl|LINK)\s*:\s*.*(error|Command line error) [CDL]\d+")


def lines(p):
    return [l.split()[0] for l in open(p) if l.strip() and not l.startswith("#")] if os.path.exists(p) else []


def groups(root):
    """-> ({group: [abs .c]}, [include dirs after the file's own dir]); private tree (src/) or public export (src/)"""
    if os.path.isdir(os.path.join(root, "src")):
        R = os.path.join(root, "src")
        skip = set(lines(R + "/skip_build.txt")) | {"an_hk_975.c"}      # as tools/portsyntax.py
        t2 = [f for f in sorted(glob.glob(R + "/automatch_near/*.c")) if os.path.basename(f) not in skip]
        cl = [R + "/clusters/cl_%s.c" % n for n in lines(R + "/clusters/enabled.txt")]
        tier1, inc = sorted(glob.glob(R + "/tier1/*.c")), [R + "/auto", R, R + "/tier1"]
    else:
        R = os.path.join(root, "src")
        t2 = sorted(glob.glob(R + "/tier2/an_*.c"))
        cl = [R + "/clusters/cl_%s.c" % n for n in lines(R + "/clusters/enabled.txt")]
        tier1, inc = sorted(glob.glob(R + "/tier1/*.c")), [R, R + "/include", R + "/tier1", R + "/tier2"]
    G = {"tier1": tier1,
         "codfh": [f for f in t2 if os.path.basename(f).startswith("an_codfh_")],
         "tier2": [f for f in t2 if not os.path.basename(f).startswith("an_codfh_")],
         "clusters": [f for f in cl if os.path.exists(f)],
         "hand": sorted(glob.glob(R + "/*.c"))}
    return G, [d for d in inc if os.path.isdir(d)]


def klass(code, msg, where):
    m = msg.lower()
    if "inline assembler" in m or "__asm" in m or code[:3] == "C24": return "inline asm"
    if "naked" in m: return "naked"
    if code == "C1083": return "missing include"
    if code == "C2169" or "intrinsic" in m: return "intrinsic redefined"
    if re.search(r"windows kits|microsoft visual studio|[\\/]msvc[\\/]|[\\/]vc[\\/]tools", where.lower()): return "CRT/SDK header clash"
    if code in ("C2197", "C2198", "C4020"): return "call arity"
    if code in ("C2371", "C2084", "C2373", "C2375", "C2365", "C2370"): return "redefinition"
    if code in ("C2065", "C2061", "C2079"): return "undeclared"
    if code in ("C2143", "C2059", "C2146", "C2054", "C2449", "C2085"): return "syntax"
    if code in ("C2440", "C2664", "C2100", "C2223", "C2232", "C2037", "C2036"): return "type"
    return "other"


def main(argv):
    a = {"--root": os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))), "--jobs": "4", "--chunk": "120"}
    wine, it = False, iter(argv)
    for k in it:
        if k == "--wine": wine = True
        elif k in ("--cl", "--out", "--root", "--jobs", "--chunk"): a[k] = next(it)
        else: print(__doc__); return 2
    if "--cl" not in a or "--out" not in a: print(__doc__); return 2
    out = os.path.abspath(a["--out"]); os.makedirs(out + "/obj", exist_ok=True)
    wp = (lambda p: "Z:" + p.replace("/", "\\")) if wine else (lambda p: p)
    G, inc = groups(os.path.abspath(a["--root"]))
    grp = {f: g for g, fl in G.items() for f in fl}
    # one cl call per (dir, flags) chunk: a cl start under Wine costs seconds; cl goes on to the next file after an error
    buckets = collections.defaultdict(list)
    for f in grp:
        src = open(f, "rb").read()
        extra = (("/GS",) if b"compile: /GS" in src else ()) + (("/Ob0",) if grp[f] in ("tier2", "codfh", "clusters") else ())
        buckets[(os.path.dirname(f), extra)].append(f)      # /Ob0: clusters + tier 2, as build_overlay.py
    jobs, n = [], 0
    for (d, extra), fl in sorted(buckets.items()):
        for i in range(0, len(fl), int(a["--chunk"])):
            n += 1
            rsp = "%s/c%03d.rsp" % (out, n)
            opts = BASE + list(extra) + ["/I" + wp(x) for x in [d] + inc] + ["/Fo" + wp(out + "/obj") + "\\"]
            open(rsp, "w").write("\n".join(opts + ['"%s"' % wp(x) for x in fl[i:i + int(a["--chunk"])]]) + "\n")
            jobs.append((rsp, fl[i:i + int(a["--chunk"])]))

    def echoed(fl, txt):
        return any(os.path.basename(f) in txt for f in fl)

    def run(j):
        for _ in range(2):      # a batch cl never started (rc != 0, no source name echoed) is run once more
            r = subprocess.run([a["--cl"], "@" + wp(j[0])], capture_output=True, text=True, errors="replace")
            txt = (r.stdout + r.stderr).replace("\r", "")
            if r.returncode == 0 or echoed(j[1], txt): break
        return j, r.returncode, txt
    # run2 (W15): 3 of the 4 first parallel cl starts died with "wine: error opening file descriptor for wineserver tmpdir file"
    # while the first one was still creating the prefix -> 281 files 'no object'. The first batch runs alone, then the pool.
    res = [run(jobs[0])] if jobs else []
    with ThreadPoolExecutor(int(a["--jobs"])) as ex:
        res += list(ex.map(run, jobs[1:]))

    errs, bad_global, lost = collections.defaultdict(list), [], {}
    with open(out + "/cl.log", "w") as log:
        for (rsp, fl), rc, txt in res:
            log.write("## %s rc %d\n%s\n" % (os.path.basename(rsp), rc, txt))
            if rc and not echoed(fl, txt):      # cl never compiled this batch: a harness failure, not a source verdict
                why = "batch lost: %s rc %d: %s" % (os.path.basename(rsp), rc, (txt.strip().split("\n") or [""])[0][:120])
                bad_global.append(why); lost.update((f, why) for f in fl)
                continue
            names, cur = {os.path.basename(f): f for f in fl}, None
            for l in txt.split("\n"):
                s = l.strip()
                if s in names: cur = names[s]; continue      # cl echoes each source name before compiling it
                m = DIAG.match(s)
                if m and cur: errs[cur].append((m.group(3), m.group(4)[:160], m.group(1)))
                elif GLOBAL_ERR.match(s): bad_global.append(s)
    cls = collections.defaultdict(lambda: [0, 0, "", ""])
    with open(out + "/msvcw_files.csv", "w", newline="") as fo:
        w = csv.writer(fo); w.writerow(["file", "group", "ok", "errors", "first_code", "first_class", "first_msg"])
        for f in sorted(grp):
            e = errs.get(f, [])
            ok = not e and os.path.exists(os.path.join(out, "obj", os.path.basename(f)[:-2] + ".obj"))
            c0, m0, w0 = e[0] if e else ("", "" if ok else lost.get(f, "no object, no diagnostic"), "")
            k0 = klass(c0, m0, w0) if e else ""
            w.writerow([os.path.relpath(f, a["--root"]), grp[f], int(ok), len(e), c0, k0, m0])
            seen = set()
            for c, m_, wh in e:
                kc = (klass(c, m_, wh), c); k = cls[kc]; k[1] += 1
                if kc not in seen: seen.add(kc); k[0] += 1
                if not k[2]: k[2], k[3] = os.path.relpath(f, a["--root"]), m_
    with open(out + "/msvcw_classes.csv", "w", newline="") as fo:
        w = csv.writer(fo); w.writerow(["class", "code", "files", "errors", "example_file", "example_msg"])
        for (k, c), v in sorted(cls.items(), key=lambda x: -x[1][0]): w.writerow([k, c] + v)
    good = lambda f: not errs.get(f) and os.path.exists(os.path.join(out, "obj", os.path.basename(f)[:-2] + ".obj"))
    tot, okn = len(grp), sum(map(good, grp))
    for g, fl in G.items():
        print("msvcw %-8s files %4d  ok %4d" % (g, len(fl), sum(map(good, fl))))
    print("msvcw TOTAL ok %d total %d  (mingw portsyntax baseline: output/portsyntax_baseline.txt)  csv %s/msvcw_files.csv  (report only)" % (okn, tot, out))
    if bad_global:
        print("msvcw: cl did not run cleanly (first: %s); see %s/cl.log" % (bad_global[0], out))
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
