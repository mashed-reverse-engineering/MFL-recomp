#!/usr/bin/env python3
"""Compile every C file of ghidra/src with the game's compiler (cl 13.00.9466 /O2 /Gy under Wine, see vc7.sh) and overlay the
objects onto MFL.exe (overlay.py).  Usage: build_overlay.py <out.exe> [symbols.txt] [extra overlay.py args]
Options: --harness adds src/harness/*.c (+ the hook list in src/harness/hooks.txt), --no-recomp builds only the harness (control exe). Files compiled: every .c in ghidra/src except those listed in src/skip_build.txt.
src/aliases.txt ("<name> <hex VA>") adds names the C code uses that are not Ghidra symbols."""
import glob, hashlib, os, re, shutil, subprocess, sys, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
RECOMP = os.path.join(HERE, "..", "src")
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")


def copy_hdr(h, work):
    """ghidra_types.h and prelude.h exist in several directories (copies made by the generators); the build must use ONE: src/auto (copied first in main()).  The other copies are ignored."""
    if os.path.basename(h) in ("ghidra_types.h", "prelude.h"): return
    shutil.copy(h, work)


def cl_strip(text, names):
    """text without the top-level DEFINITIONS (not prototypes) of `names`: a cluster owns its entries, the older tier 2 / tier 1 / bridge-stub definitions of the same functions are dropped from the build copy (the files on disk stay)"""
    for n in names:
        while True:
            m = re.search(r"^(?!extern|static|typedef)(?:__declspec\(naked\) )?[A-Za-z_][\w \t\*]*?\b%s\s*\([^;{}]*\)\s*\{" % re.escape(n), text, re.M)
            if not m: break
            i, depth = m.end(), 1
            while depth and i < len(text):
                depth += (text[i] == "{") - (text[i] == "}"); i += 1
            text = text[:m.start()] + "/* %s: replaced by its cluster (src/clusters) */" % n + text[i:]
    return text


def cl_verify(out, entries, thunks):
    """every cluster entry (and thunk) must be defined by the overlay and patched with `jmp new` at its original address; the original must have room for the 5 bytes"""
    import struct
    mp = {l.split()[0]: int(l.split()[1], 16) for l in open(out + ".map") if len(l.split()) == 2}
    sizes = {l.split()[1]: int(l.split()[2]) for l in open(os.path.join(HERE, "..", "output", "fnsizes.txt")) if len(l.split()) == 3}
    for f_ in glob.glob(os.path.join(HERE, "..", "output", "cluster_pilot*", "fnsizes_extra.txt")):      # sizes of gap_ items that fnsizes.txt lacks (cluster_install reads the same files)
        for l_ in open(f_):
            p_ = l_.split()
            if len(p_) == 3 and p_[1] not in sizes: sizes[p_[1]] = int(p_[2])
    syms = {l.split()[0]: int(l.split()[1], 16) for l in open(os.path.join(RECOMP, "symbols.txt")) if len(l.split()) == 2}
    _al = os.path.join(RECOMP, "aliases.txt")      # gap_<va> entries (unnamed items) are known by src/aliases.txt only
    if os.path.exists(_al):
        for l_ in open(_al):
            p_ = l_.split()
            if len(p_) == 2 and p_[0] not in syms:
                try: syms[p_[0]] = int(p_[1], 16)
                except ValueError: pass
    exe = open(out, "rb").read()
    e = struct.unpack_from("<I", exe, 0x3C)[0]
    nsec, osz = struct.unpack_from("<H", exe, e + 6)[0], struct.unpack_from("<H", exe, e + 20)[0]
    secs = [struct.unpack_from("<IIII", exe, e + 24 + osz + k * 40 + 8) for k in range(nsec)]      # vsize, rva, rawsize, rawptr
    bad = []
    for n in entries:
        if n not in mp or n not in syms: bad.append("%s: not defined by the overlay / no original address" % n); continue
        if sizes.get(n, 0) < 5: bad.append("%s: original has %s bytes, no room for the jmp" % (n, sizes.get(n))); continue
        rva = syms[n] - 0x400000
        for vs, va, rs, rp in secs:
            if va <= rva < va + max(vs, rs):
                o = rp + rva - va; break
        tgt = (syms[n] + 5 + struct.unpack_from("<i", exe, o + 1)[0]) & 0xFFFFFFFF if exe[o] == 0xE9 else None
        if tgt is not None and tgt != mp[n]:      # --count: the jmp goes to a counter thunk `pushfd; inc [cnt]; popfd; jmp new`
            r_ = tgt - 0x400000
            for vs, va, rs, rp in secs:
                if va <= r_ < va + max(vs, rs) and exe[rp + r_ - va:rp + r_ - va + 3] == b"\x9c\xff\x05":
                    tgt = (tgt + 13 + struct.unpack_from("<i", exe, rp + r_ - va + 9)[0]) & 0xFFFFFFFF
        if tgt != mp[n]: bad.append("%s: original entry is not patched with a jmp to the cluster code" % n)
    for n in thunks:
        if n + "_n" not in mp: bad.append("%s_n (natural root) not defined" % n)
    if bad: sys.exit("cluster check failed:\n  " + "\n  ".join(bad))
    print("cluster check: %d entries patched with a jmp to their cluster code, %d entry thunks" % (len(entries), len(thunks)), file=sys.stderr)


def main():
    out = sys.argv[1]
    sym = sys.argv[2] if len(sys.argv) > 2 and not sys.argv[2].startswith("--") else os.path.join(RECOMP, "symbols.txt")
    extra = sys.argv[3:] if len(sys.argv) > 2 and not sys.argv[2].startswith("--") else sys.argv[2:]
    _en_b = {l.strip() for l in open(os.path.join(RECOMP, "bridges", "enabled.txt")) if l.strip()} if os.path.exists(os.path.join(RECOMP, "bridges", "enabled.txt")) else set()
    _nn = os.path.join(RECOMP, "automatch_near_names.txt")
    t2near_off = {l.split()[1] for l in open(_nn) if len(l.split()) >= 2} - _en_b if os.path.exists(_nn) else set()
    skip = set()
    p = os.path.join(RECOMP, "skip_build.txt")
    if os.path.exists(p):
        skip = {l.split()[0] for l in open(p) if l.strip() and not l.startswith("#")}
    skip |= {x for x in os.environ.get("SKIP_FILES", "").split(",") if x}   # e.g. SKIP_FILES=batch7.c,batch8.c (bisecting)
    work = tempfile.mkdtemp(prefix="overlay_", dir=os.environ.get("TMPDIR", "/tmp/claude-%d" % os.getuid()))
    try:
        for h in glob.glob(os.path.join(RECOMP, "*.h")):
            copy_hdr(h, work)
        for h in ("ghidra_types.h", "prelude.h"):
            shutil.copy(os.path.join(RECOMP, "auto", h), work)
        objs = []
        srcs = sorted(glob.glob(os.path.join(RECOMP, "*.c")))
        if "--auto" in extra:            # src/auto: autocompile.py chunks (unverified machine output, six of the seven bugs of the course sweep came from them): opt-in only
            srcs += sorted(glob.glob(os.path.join(RECOMP, "auto", "*.c")))
            for h in glob.glob(os.path.join(RECOMP, "auto", "*.h")):
                copy_hdr(h, work)
        if "--automatch" in extra:       # src/automatch: autocompiled functions that are byte identical to the original (tools/automatch.py, automatch_promote.py), the others are extern declarations
            srcs += sorted(glob.glob(os.path.join(RECOMP, "automatch", "*.c"))) + sorted(glob.glob(os.path.join(RECOMP, "automatch_agent", "*.c"))) + sorted(glob.glob(os.path.join(RECOMP, "tier1", "*.c")))      # tier1: one file per function (tools/split_tier1.py), the chunks in automatch/ only hold new matches      # automatch_agent: functions matched by agents (tools/check_one.py), standalone files
            for h in glob.glob(os.path.join(RECOMP, "automatch_agent", "*.h")):
                copy_hdr(h, work)
            for h in glob.glob(os.path.join(RECOMP, "automatch", "*.h")):
                copy_hdr(h, work)
        if "--automatch-near" in extra:  # src/automatch_near: compiled autocompiled functions that are NOT byte identical (tier 2: only the sweeps / conformance tests vouch for them)
            t2dir = os.environ.get("T2_SHADOW_DIR") or os.path.join(RECOMP, "automatch_near")      # T2_SHADOW_DIR: tools/shadowgen2.py output (original vs C per call), needs harness_shadow/shadow.c
            srcs += sorted(glob.glob(os.path.join(t2dir, "*.c")))
            if os.environ.get("T2_SHADOW_DIR") and "--shadow" not in extra and not os.path.exists(os.path.join(t2dir, "an_shadow_rt.c")): srcs.append(os.path.join(RECOMP, "harness_shadow", "shadow.c"))
            for h in glob.glob(os.path.join(t2dir, "*.h")):
                copy_hdr(h, work)
        cl_files, cl_entries = [], set()
        if "--clusters" in extra:        # src/clusters: tools/cluster_install.py output (natural C of a usercall root + its callers, entry thunk of the root); enabled.txt or CLUSTER_ENABLED=a,b
            en_ = os.environ.get("CLUSTER_ENABLED")
            en_ = [x for x in en_.split(",") if x] if en_ is not None else [l.strip() for l in open(os.path.join(RECOMP, "clusters", "enabled.txt")) if l.strip() and not l.startswith("#")]
            for n_ in en_:
                f_ = os.path.join(RECOMP, "clusters", "cl_%s.c" % n_)
                cl_files.append(f_); cl_entries |= set(open(f_).readline().split(":", 1)[1].split())
            if "--bridges" in extra:
                bad_ = sorted(cl_entries & set(l.strip() for l in open(os.path.join(RECOMP, "bridges", "enabled.txt")) if l.strip()))
                if os.environ.get("BRIDGE_ENABLED") is not None: bad_ = sorted(cl_entries & set(os.environ["BRIDGE_ENABLED"].split(",")))
                if bad_: sys.exit("cluster entries that are ENABLED bridges (two implementations): " + " ".join(bad_))
            srcs += cl_files
        if "--bridges" in extra and "--shadow" not in extra:         # (--shadow replaces src/bridges by src/bridges_shadow: same file names) src/bridges: tools/bridgegen.py output (register functions: C implementation + naked stub), opt-in
            srcs += sorted(glob.glob(os.path.join(RECOMP, "bridges", "*.c")))
            for h in glob.glob(os.path.join(RECOMP, "bridges", "*.h")):
                copy_hdr(h, work)
            en = os.environ.get("BRIDGE_ENABLED")        # default: src/bridges/enabled.txt (functions that passed the game test and the shadow test)
            if en is None: en = ",".join(l.strip() for l in open(os.path.join(RECOMP, "bridges", "enabled.txt")))
            # comma list: only these bridged functions run as C, calls to the others go to the original (bridge_sel.h)
            if en is not None:
                keep = set(x for x in en.split(",") if x)
                sel = os.path.join(work, "bridge_sel.h")
                txt = open(sel).read()
                txt = re.sub(r"#define BR_(\w+) \1_c", lambda m: m.group(0) if m.group(1) in keep else "#define BR_%s %s_r" % (m.group(1), m.group(1)), txt)
                open(sel, "w").write(txt)
                off = [m for m in re.findall(r"#define BR_(\w+) \w+_r", txt)]      # disabled bridges: leave the original function unpatched
                off = [x for x in off if x not in cl_entries]      # a disabled bridge stub of a cluster root is stripped below, the cluster thunk is patched in
                if off: extra += ["--skip", ",".join(off)]
        if "--shadow" in extra:          # src/bridges_shadow (bridgegen.py --shadow) + the shadow test runtime: every bridged function compares the original with the C version
            srcs += sorted(glob.glob(os.path.join(RECOMP, "bridges_shadow", "*.c"))) + [os.path.join(RECOMP, "harness_shadow", "shadow.c")]
            for h in glob.glob(os.path.join(RECOMP, "bridges_shadow", "*.h")):
                copy_hdr(h, work)
            if "--bridges" in extra and os.environ.get("BRIDGE_ENABLED") is not None:      # gate.sh bridge: shadow only the named bridges, the others stay original
                keep = set(x for x in os.environ["BRIDGE_ENABLED"].split(",") if x)
                off = [l.strip() for l in open(os.path.join(RECOMP, "bridges_shadow", "shadow_names.txt")) if l.strip() and l.strip() not in keep]
                if off: extra += ["--skip", ",".join(off)]
        if "--conftest" in extra:        # src/harness_conf: conformance test runtime + generated table (tools/conftest_gen.py), see tools/wine-run/conftest.sh
            srcs += sorted(glob.glob(os.path.join(RECOMP, "harness_conf", "*.c")))
        extra = [x for x in extra if x not in ("--no-auto", "--auto", "--bridges", "--shadow", "--conftest", "--automatch", "--automatch-near", "--clusters")]
        if "--harness" in extra:    # test hooks (src/harness/*.c) for deterministic runs; the call-site hooks are passed on
            extra = [x for x in extra if x != "--harness"]
            srcs += sorted(glob.glob(os.path.join(RECOMP, "harness", "*.c")))
        for x in [x for x in extra if x.startswith("--src=")]:   # extra C file (e.g. autocompile output); its *.h are copied too
            extra.remove(x)
            srcs.append(x[6:])
            for h in glob.glob(os.path.join(os.path.dirname(x[6:]), "*.h")):
                copy_hdr(h, work)
        harness_fns = []
        if "--harness-used" not in extra and any(os.sep + "harness" + os.sep in x for x in srcs):
            for x in srcs:
                if os.sep + "harness" + os.sep in x:
                    harness_fns += re.findall(r"^[A-Za-z_][\w\s\*]*?\b(\w+)\([^;{]*\)\s*\n\{", open(x).read(), re.M)
        if "--no-recomp" in extra:   # only the harness hooks, no replaced functions (control build)
            extra = [x for x in extra if x != "--no-recomp"]
            srcs = [x for x in srcs if os.sep + "harness" + os.sep in x]
            extra += ["--no-patch"]
        # object cache: key = source + all headers + flags; only the changed files are compiled, in ONE cl call (a docker/wine start costs ~8 s)
        flags = ["/nologo", "/c", "/O2", "/Gy"]
        hdr = hashlib.sha1()
        for h in sorted(glob.glob(os.path.join(work, "*.h"))):
            hdr.update(open(h, "rb").read())
        cache = os.environ.get("OBJCACHE", os.path.expanduser("~/.cache/mfl_objcache"))
        os.makedirs(cache, exist_ok=True)
        todo, keyed = [], []; gs_files = set()
        for c in srcs:
            nm = os.path.basename(c)
            if nm in skip:
                continue
            if os.path.exists(os.path.join(work, nm)):
                sys.exit("two sources with the same file name %s (they would overwrite each other in the compile directory and poison the object cache)" % nm)
            shutil.copy(c, os.path.join(work, nm))
            if b"compile: /GS" in open(c, "rb").read(): gs_files.add(nm)      # /GS stack-cookie functions (tools/gsfix.py): this file alone gets cl /GS
            if cl_entries and c not in cl_files:
                t_ = open(c, errors="replace").read(); t2_ = cl_strip(t_, [n_ for n_ in cl_entries if re.search(r"\b%s\b" % n_, t_)])
                if t2_ != t_: open(os.path.join(work, nm), "w").write(t2_)
            if os.sep + "bridges" + os.sep in c and "bridge_sel" not in nm:      # a bridge that moved to tier 2 (automatch_near, bridge disabled): its stub / C body definitions are dropped, the tier 2 function owns the symbol
                t_ = open(os.path.join(work, nm), errors="replace").read(); t2_ = cl_strip(t_, [n_ for n_ in t2near_off if re.search(r"\b%s\b" % n_, t_)])
                if t2_ != t_: open(os.path.join(work, nm), "w").write(t2_)
            key = hashlib.sha1(open(os.path.join(work, nm), "rb").read() + hdr.digest() + " ".join(flags + (["/GS"] if nm in gs_files else [])).encode()).hexdigest()
            keyed.append((nm, os.path.join(cache, key + ".obj")))
            if not os.path.exists(keyed[-1][1]):
                todo.append(nm)
        if todo:
            # one container, several wine cl processes in parallel (same prefix/wineserver); 1 file per cl would waste the 8 s start per process
            njobs = min(len(todo), int(os.environ.get("JOBS", os.cpu_count() or 1)))
            chunks = [todo[i::njobs] for i in range(njobs)]
            cmd = " ".join("wine cl.exe %s %s &" % (" ".join(flags + (["/GS"] if g else [])), " ".join(f_)) for c in chunks for g in (0, 1) for f_ in [[x for x in c if (x in gs_files) == bool(g)]] if f_) + " wait"
            r = subprocess.run([os.path.join(HERE, "vc7.sh"), work, cmd], capture_output=True, text=True, env=dict(os.environ, VC7_SHELL="1"))
            for nm in todo:
                o = os.path.join(work, nm[:-2] + ".obj")
                if not os.path.exists(o):
                    print("compile error in", nm, (r.stdout + r.stderr)[-int(os.environ.get("ERRLEN", "600")):], file=sys.stderr)
                    sys.exit(1)
            for nm, cp in keyed:
                if nm in todo:
                    shutil.copy(os.path.join(work, nm[:-2] + ".obj"), cp)
        for nm, cp in keyed:
            o = os.path.join(work, nm[:-2] + ".obj")
            if os.path.exists(o):
                os.unlink(o)          # written by the container (root); replace by our copy
            shutil.copy(cp, o)
            objs.append(o)
        if os.environ.get("OBJLIST"):      # tools/delink: the cached objects (source name, object path) for linking with the delinked image instead of overlaying
            with open(os.environ["OBJLIST"], "w") as f:
                for nm, cp in keyed: f.write("%s %s\n" % (nm, cp))
        hk = os.path.join(RECOMP, "harness", "hooks.txt")
        if any(os.sep + "harness" + os.sep in x for x in srcs) and os.path.exists(hk):
            for l in open(hk):
                if l.strip() and not l.startswith("#"):
                    extra += ["--hook-call", l.split()[0]]
        dw = os.path.join(RECOMP, "harness", "dwords.txt")      # operand patches: "<VA of the 4 byte operand>=<symbol>"
        if any(os.sep + "harness" + os.sep in x for x in srcs) and os.path.exists(dw):
            for l in open(dw):
                if l.strip() and not l.startswith("#"):
                    extra += ["--patch-dword", l.split()[0]]
        if os.environ.get("VERBOSE"): print("sources (%d):" % len(srcs), " ".join(os.path.basename(x) for x in srcs if "/src/" in x.replace(chr(92), "/") and os.path.dirname(x).endswith("automatch_near")), file=sys.stderr)
        merged = os.path.join(work, "symbols_all.txt")
        with open(merged, "w") as f:
            f.write(open(sym).read())
            al = os.path.join(RECOMP, "aliases.txt")
            if os.path.exists(al):
                f.write(open(al).read())
        libobjs = sorted(glob.glob(os.path.join(os.environ["LIBOBJ_DIR"], "*.obj"))) if os.environ.get("LIBOBJ_DIR") else []
        if libobjs:      # SDK library objects (tools/lib_extract.py): their external symbols resolve through the relocation derived map output/rw_libsyms.csv (tools/libsyms.py)
            objs += libobjs
            sk_ = os.path.join(os.environ["LIBOBJ_DIR"], "skip.txt")      # library functions that stay original (not verified): comma list
            if os.path.exists(sk_) and open(sk_).read().strip(): extra += ["--skip", open(sk_).read().strip()]
            os.environ["OVERLAY_EXTDATA"] = os.environ["LIBOBJ_DIR"]
            with open(merged, "a") as f:
                for fn_ in sorted(glob.glob(os.path.join(HERE, "..", "output", "*_libsyms.csv"))):      # rw_libsyms.csv, crt_libsyms.csv ...
                  for l_ in open(fn_):
                    q_ = l_.strip().split(",")
                    if len(q_) >= 5 and int(q_[2]) >= 2 * int(q_[3]) and not q_[0].startswith("$L"):      # majority of the votes; `$L` jump labels are local
                        n_ = q_[0][1:] if q_[0].startswith("_") and not q_[0].startswith("__imp") else q_[0]
                        f.write("%s %s\n" % (re.sub(r"@\d+$", "", n_), q_[1]))
        with open(merged, "a") as f:      # decompiler names for functions without a symbol: func_0x00449c40 -> that address
            for fn in sorted({m for c in srcs for m in re.findall(r"\bfunc_0x[0-9a-f]{8}\b", open(c, errors="replace").read())}):
                f.write("%s %s\n" % (fn, fn[7:]))
        if "--replace" in extra and harness_fns:    # a --replace list (bisect) must not switch off the harness functions that replace game functions (timer_ticks)
            k = extra.index("--replace")
            extra[k + 1] = extra[k + 1] + "," + ",".join(sorted(set(harness_fns)))
        if "--no-patch" in extra and harness_fns:   # a control must still run the harness functions that replace game functions (timer_ticks)
            extra = [x for x in extra if x != "--no-patch"] + ["--replace", ",".join(sorted(set(harness_fns)))]
        subprocess.run([sys.executable, os.path.join(HERE, "overlay.py"), EXE, out, merged, "--pad", os.environ.get("SECTION_PAD", "0x20000")] + extra + objs, check=True)
        if cl_entries and "--no-patch" not in extra: cl_verify(out, sorted(cl_entries), [n for f_ in cl_files for n in [os.path.basename(f_)[3:-2]] if os.path.exists(f_) and ("%s_n" % n) in open(f_).read()])
        try:      # build provenance (idea from REA: every result names the identity of its inputs): <out>.build.json
            import json, time
            sha = lambda b: hashlib.sha1(b).hexdigest()[:12]
            json.dump({"time": time.strftime("%F %T"), "base_exe_sha256": hashlib.sha256(open(EXE, "rb").read()).hexdigest(), "args": sys.argv[1:], "fix_rules": os.environ.get("FIX_RULES", "default"),
                       "t2_shadow_dir": os.environ.get("T2_SHADOW_DIR"), "sources": {os.path.relpath(x, RECOMP) if x.startswith(RECOMP) else x: sha(open(x, "rb").read()) for x in srcs}}, open(out + ".build.json", "w"), indent=1)
        except Exception as e:
            print("build.json not written:", e, file=sys.stderr)
    finally:
        shutil.rmtree(work, ignore_errors=True)


main()
