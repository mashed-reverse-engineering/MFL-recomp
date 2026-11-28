#!/usr/bin/env python3
"""Compile every C file of ghidra/src with the game's compiler (cl 13.00.9466 /O2 /Gy under Wine, see vc7.sh) and overlay the
objects onto MFL.exe (overlay.py).  Usage: build_overlay.py <out.exe> [symbols.txt] [extra overlay.py args]
Options: --harness adds src/harness/*.c (+ the hook list in src/harness/hooks.txt), --no-recomp builds only the harness (control exe). Files compiled: every .c in ghidra/src except those listed in src/skip_build.txt.
src/aliases.txt ("<name> <hex VA>") adds names the C code uses that are not Ghidra symbols."""
import glob, hashlib, os, re, shutil, subprocess, sys, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.append(HERE); import incexpand
RECOMP = os.path.join(HERE, "..", "src")
EXE = os.path.join(HERE, "..", "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
V143 = os.environ.get("CC") == "v143"      # W16 V143 item 2: the CC=v143 variant compiles with MSVC v143 /MD (msvc-wine); unset = VC7 cl 13.00, unchanged
MT = V143 and os.environ.get("LIB_CRT") == "v143mt"      # W16 MT (V143 item 4): /MT instead of /MD (static CRT, no CRT dllimport); own cache keys


# W17 CCSW: CC=gcc = the hybrid swap: ONLY the files listed in data/link/gcc_files.txt (one basename per line, # comments) are compiled by
# i686-w64-mingw32-gcc (SSE2 float, float10 = float); every other file stays VC7 cl 13.00 exactly as before.  The gcc objects are plain COFF i386
# objects with the same `_name` / `_name@N` decoration, so mkobj (entry jmp), the pinned data layout and link.exe take them like any cl object.
# CC unset (or v143) = this block is inert: no list is read, no key changes.  The link scripts need no change (link_ours.sh only tests CC=v143).
GCC = os.environ.get("CC") == "gcc"
GCC_FLAGS = ["-c", "-w", "-std=gnu99", "-O2", "-msse2", "-mfpmath=sse", "-ffp-contract=off", "-fno-fast-math", "-fms-extensions", "-DNOASM", "-D__int64=long long",      # (mingw defines __int64 only in its own headers; the sources use no system header)
             "-fno-ident", "-fno-asynchronous-unwind-tables",      # these two: no .comment / .eh_frame sections in the objects (mkobj / mkrange expect .text .data .rdata .bss only)
             # W17 GCC1 (output/gcc_first_compile_2026-11-21.md): the sources are decompiler style C that cl 13 compiles literally; gcc -O2 must not use the
             # undefined behaviour in it: type punning through casts (109 tier 2 files warn), signed / pointer wrap, loops that walk past a placeholder
             # `g_x[0x1000]` extern into the next pinned global, reads through small absolute addresses
             "-fno-strict-aliasing", "-fwrapv", "-fwrapv-pointer", "-fno-aggressive-loop-optimizations", "-fno-delete-null-pointer-checks",
             "-mpreferred-stack-boundary=2",     # the cl ABI: the stack is 4 byte aligned at every call (gcc's default assumes 16 on entry: movaps on a stack slot would fault when cl code calls); gcc then realigns only a function that needs an aligned slot
             "-fno-reorder-functions",           # no .text.unlikely (cold functions)
             # W17 G-A2 (agents/gcc2_w17/G-A2.md, probe ga2_probe.c): the original calls every function of a file; gcc must too, or diffemu's callee hooks miss the call
             "-ffunction-sections",              # one .text$<name> per function: a call between two functions of the file gets a relocation (in one .text gas resolves it without one)
             "-flive-patching=inline-only-static"]      # only static functions inline: no inlining of a global callee, no IPA-RA (caller relying on a callee's clobbers), no ICF folding (same_2 -> jmp same_1), no ipa-cp/sra clones
GCC_PHYSICS = re.compile(r"(^|_)(car|cars|aibot|rwp|vehicle)(_|\.)")      # x87 study (output/x87_sse2_experiment_2026-11-21.md): car / aibot / rwp physics stay on MSVC x87
GCC_FLOAT10 = "typedef double float10;"      # src/mfl_tier2.h; the gcc copy of the header (never the tree file) says `float` so a PC=24 x87 temporary rounds per op like the original


def gcc_list():
    """basenames from data/link/gcc_files.txt; a physics file name stops the build"""
    p = os.environ.get("GCC_FILES") or os.path.join(HERE, "..", "data", "link", "gcc_files.txt")      # GCC_FILES=<file>: another list (experiments)
    names = [l.split()[0] for l in open(p) if l.strip() and not l.startswith("#")] if os.path.exists(p) else []
    bad = [n for n in names if GCC_PHYSICS.search(n)]
    if bad: sys.exit("CC=gcc: physics files stay on MSVC x87, remove from data/link/gcc_files.txt: " + " ".join(bad))
    return set(names)


_LIBGCC = None


def gcc_objcheck(o):
    """-> reasons why gcc object `o` cannot be linked by link.exe next to the cl objects: an undefined symbol that only libgcc defines
    (___chkstk_ms: a frame > 4 KiB, ___divdi3 / ___umoddi3: 64 bit division; cl calls __chkstk / __alldiv of the exe instead) or a non-empty
    section other than .text / .data / .rdata / .bss (mkobj / mkrange place only these)"""
    global _LIBGCC
    nm_ = lambda *a: subprocess.run(["i686-w64-mingw32-nm"] + list(a), capture_output=True, text=True).stdout.splitlines()
    if _LIBGCC is None:
        lib = subprocess.run(["i686-w64-mingw32-gcc", "-print-libgcc-file-name"], capture_output=True, text=True).stdout.strip()
        _LIBGCC = {l.split()[-1] for l in nm_("-g", "--defined-only", lib) if len(l.split()) == 3}
    bad = ["libgcc symbol " + s for s in sorted({l.split()[-1] for l in nm_("-u", o) if l.split()} & _LIBGCC)]
    hdr = subprocess.run(["i686-w64-mingw32-objdump", "-h", o], capture_output=True, text=True).stdout
    bad += ["section " + s for s, z in re.findall(r"^\s*\d+ (\S+)\s+([0-9a-f]{8})", hdr, re.M) if int(z, 16) and s.split("$")[0] not in (".text", ".data", ".rdata", ".bss")]
    return bad


def gcc_compile(work, todo, fix, keyed):
    """CC=gcc: compile `todo` (allowlisted names, sources already in `work`) with mingw gcc in work/gcc (headers copied, mfl_tier2.h with float10 = float),
    objects land in work/<name>.obj and go into the object cache like a cl object"""
    g = os.path.join(work, "gcc"); os.makedirs(g, exist_ok=True)
    for h in glob.glob(os.path.join(work, "*.h")) + glob.glob(os.path.join(work, "*.inc")): shutil.copy(h, g)
    t = open(os.path.join(g, "mfl_tier2.h")).read()
    if GCC_FLOAT10 not in t: sys.exit("CC=gcc: %r not found in mfl_tier2.h" % GCC_FLOAT10)
    open(os.path.join(g, "mfl_tier2.h"), "w").write(t.replace(GCC_FLOAT10, "typedef float float10;"))
    cp_ = dict(keyed)
    for nm in todo:
        shutil.copy(os.path.join(work, nm), os.path.join(g, nm))
        o = os.path.join(work, nm[:-2] + ".obj")
        r = subprocess.run(["i686-w64-mingw32-gcc"] + GCC_FLAGS + [x.replace("/D", "-D", 1) for x in fix] + ["-o", o, nm], cwd=g, capture_output=True, text=True)
        if r.returncode or not os.path.exists(o):
            print("compile error in", nm, "(gcc)", r.stderr[-600:], file=sys.stderr); sys.exit(1)
        bad = gcc_objcheck(o)      # W17 GCC1: an object link.exe / mkobj cannot take stops here with its reason, not as an unresolved symbol later
        if bad: sys.exit("CC=gcc: %s cannot join the link, remove it from data/link/gcc_files.txt: %s" % (nm, "; ".join(bad)))
        tmp_ = "%s.%d.tmp" % (cp_[nm], os.getpid()); shutil.copy(o, tmp_); os.replace(tmp_, cp_[nm])


def v143_compile(work, todo, fix, gs_files, ob0_files):
    """CC=v143: compile `todo` in `work` with MSVC v143 (tools/msvcw/msvcw_compile.py BASE flags: /O2 /Gy /arch:IA32 /GS- /MD /Zl), /GS and /Ob0 per
    file as the VC7 build, the compile: markers /G5 /Op / cl1310 dropped (no meaning for v143).  One container (tools/msvcw/msvcw_sh.sh), the first
    cl call alone (prefix start), then the rest in parallel.  -> cl output text (objects land in `work`)"""
    sys.path.append(os.path.join(HERE, "msvcw")); import msvcw_compile
    par = max(1, int(os.environ.get("JOBS", os.cpu_count() or 1)))
    jobs = []
    for g in (0, 1):
        for o in (0, 1):
            fl = [x for x in todo if (x in gs_files) == bool(g) and (x in ob0_files) == bool(o)]
            for i in range(0, len(fl), max(1, -(-len(fl) // par))):
                rsp = "v143_%d.rsp" % len(jobs)
                open(os.path.join(work, rsp), "w").write("\n".join([("/MT" if MT and x == "/MD" else x) for x in msvcw_compile.BASE] + fix + (["/GS"] if g else []) + (["/Ob0"] if o else []) + fl[i:i + max(1, -(-len(fl) // par))]) + "\n")
                jobs.append(rsp)
    cl = "/opt/msvc/bin/x86/cl"
    cmd = " ".join(["%s @%s > %s.log 2>&1;" % (cl, jobs[0], jobs[0])] + ["%s @%s > %s.log 2>&1 &" % (cl, j, j) for j in jobs[1:]] + ["wait"])
    r = subprocess.run([os.path.join(HERE, "msvcw", "msvcw_sh.sh"), work, cmd], capture_output=True, text=True)
    return r.stdout + r.stderr + "".join(open(os.path.join(work, j + ".log"), errors="replace").read() for j in jobs if os.path.exists(os.path.join(work, j + ".log")))


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
    t2near_off |= {os.path.basename(f)[:-2] for f in glob.glob(os.path.join(RECOMP, "tier1", "*.c"))} - _en_b      # W14 B0 F9: a tier 1 file (one per function) owns its name too; the export drops names.txt rows of removed tier 2 files (sphere_vs_box_test: b004 body + tier1 = two definitions)
    skip = set()
    p = os.path.join(RECOMP, "skip_build.txt")
    if os.path.exists(p):
        skip = {l.split()[0] for l in open(p) if l.strip() and not l.startswith("#")}
    skip |= {x for x in os.environ.get("SKIP_FILES", "").split(",") if x}   # e.g. SKIP_FILES=batch7.c,batch8.c (bisecting)
    if not (V143 and "/DMFL_FIX_CRASHLOG=1" in os.environ.get("MFL_FIX_DEFINES", "").split()): skip.add("mfl_crashlog.c")      # W16 CRASHLOG: v143 + switch on only, so the VC7 and every off build keep their object list
    if "/DMFL_FIX_PREFS=1" not in os.environ.get("MFL_FIX_DEFINES", "").split(): skip.add("mfl_settings_menu.c")      # W17 PREF: the Preferences page only with its switch (off builds keep their object list)
    work = tempfile.mkdtemp(prefix="overlay_", dir=os.environ.get("TMPDIR", "/tmp/claude-%d" % os.getuid()))
    try:
        for h in glob.glob(os.path.join(RECOMP, "*.h")) + glob.glob(os.path.join(RECOMP, "tier1", "*.h")):      # tier1/*.h: library headers of tier 1 sources (rwa.h / rwaint.h: RenderWare Audio, codfh-win32)
            copy_hdr(h, work)
        for h in ("ghidra_types.h", "prelude.h"):      # W14 B1: only where they exist (the private tree: bridges_shadow, --auto); the public export has no src/auto/ and no source includes them
            if os.path.exists(os.path.join(RECOMP, "auto", h)): shutil.copy(os.path.join(RECOMP, "auto", h), work)
        objs = []
        srcs = sorted(glob.glob(os.path.join(RECOMP, "*.c"))) + sorted(glob.glob(os.path.join(RECOMP, "video", "*.c")))      # src/video: mfl_video backend dispatcher + null backend
        srcs += glob.glob(os.path.join(RECOMP, "audio", "mfl_audio_null.c"))      # src/audio: only the mfl_audio dispatcher + null backend go into the exe (miniaudio = optional DLL, tests = host builds)
        if "--auto" in extra:           # src/auto: autocompile.py chunks (unverified machine output, six of the seven bugs of the course sweep came from them): opt-in only
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
            for h in glob.glob(os.path.join(t2dir, "*.inc")):      # KES group decls + bind blocks (an_hk_612_<grp>.inc): hashed into the key of the file that includes them, below
                shutil.copy(h, os.path.join(work, os.path.basename(h)))
        cl_files, cl_entries = [], set()
        if "--clusters" in extra:        # src/clusters: tools/cluster_install.py output (natural C of a usercall root + its callers, entry thunk of the root); enabled.txt or CLUSTER_ENABLED=a,b
            en_ = os.environ.get("CLUSTER_ENABLED")
            en_ = [x for x in en_.split(",") if x] if en_ is not None else [l.strip() for l in open(os.path.join(RECOMP, "clusters", "enabled.txt")) if l.strip() and not l.startswith("#")]
            hx_ = os.path.join(RECOMP, "harness", "cluster_exclude.txt")      # harness builds: hook call sites (hooks.txt) inside the ORIGINAL body of these cluster entries cannot be patched once the cluster replaces it (app_main_loop: freeze_hook / frame_end_ticks)
            if "--harness" in extra and os.path.exists(hx_):
                en_ = [x for x in en_ if x not in {l.split()[0] for l in open(hx_) if l.strip() and not l.startswith("#")}]
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
            # W13 LAY: a shadow bridge defines the function under its own name (the shadow wrapper); a tier 1 file of the same function would be a
            # second definition (overlay.py stops on it), so the shadow build leaves those tier 1 files out
            sh_ = {l.strip() for l in open(os.path.join(RECOMP, "bridges_shadow", "shadow_names.txt")) if l.strip()}
            srcs = [x for x in srcs if not (os.path.basename(os.path.dirname(x)) == "tier1" and os.path.basename(x)[:-2] in sh_)]
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
        fixd_ = os.environ.get("MFL_FIX_DEFINES", "").split()      # MFL_FIX_* switches of a flag-on build (src/mfl_fix.h), e.g. /DMFL_FIX_DETERMINISTIC_AUDIO_RNG=1: on the cl line AND in every object's cache key (flags), so a flag-on object is never reused by the default build
        known_ = set(re.findall(r"^\s*#\s*define\s+(MFL_FIX_\w+)", open(os.path.join(RECOMP, "mfl_fix.h")).read(), re.M))      # ASM: a misspelt switch would build "off" silently
        bad_ = [x for x in fixd_ if not re.match(r"^/DMFL_FIX_[A-Z0-9_]+=[01]$", x) or x[2:].split("=")[0] not in known_]
        if bad_: sys.exit("MFL_FIX_DEFINES: only /DMFL_FIX_<NAME>=0|1 entries of a switch src/mfl_fix.h defines, got %s" % bad_)
        flags += fixd_
        hdr = hashlib.sha1()
        LOCAL_H = ("mfl_video.h", "mfl_audio.h", "rwa.h", "rwaint.h", "rwa2.h", "rwaint2.h", "rw37_slim.h", "mfl_fix.h", "mfl_audio_rng.h")      # (mfl_fix.h: W11 B5; mfl_audio_rng.h: W12 rep4, only random_range.c names it) included by few files: hashed into their keys below, not into every object's (a new header must not rebuild the 3,600 objects)
        for h in sorted(glob.glob(os.path.join(work, "*.h"))):
            if os.path.basename(h) in LOCAL_H: continue
            hdr.update(open(h, "rb").read())
        lh_ = {n_: open(os.path.join(work, n_), "rb").read() for n_ in LOCAL_H if os.path.exists(os.path.join(work, n_))}
        vh_ = lambda src_: b"".join(v_ for n_, v_ in sorted(lh_.items()) if n_.encode() in src_)
        inc_ = lambda p_: incexpand.read_expanded(p_).encode("utf-8") if incexpand.incs(open(p_, errors="replace").read()) else b""      # a source with .inc includes: the inlined text joins its key (others: key unchanged)
        # W16 SPD (CC=v143 only): the MFL_FIX defines join the key only of a file that can see them (its text or .inc text names MFL_FIX_, or a local header that does);
        # any other file has the same object with and without them (agents/spd_w16/notes.md), so the flag-on build reuses the default build's objects
        fixh_ = []; sees_ = lambda t_: b"MFL_FIX_" in t_ or any(n_ in t_ for n_ in fixh_)
        for _ in LOCAL_H: fixh_ = [n_.encode() for n_, v_ in lh_.items() if sees_(v_)]      # local headers that see them, also through each other
        fixall_ = any(sees_(open(h, "rb").read()) for h in glob.glob(os.path.join(work, "*.h")) if os.path.basename(h) not in LOCAL_H)      # a shared header that can see them: every file keyed with them
        kflags_ = lambda t_: flags if not (V143 and fixd_) or fixall_ or sees_(t_) else flags[:4]
        cache = os.environ.get("OBJCACHE", os.path.expanduser("~/.cache/mfl_objcache"))
        os.makedirs(cache, exist_ok=True)
        todo, keyed = [], []; gs_files = set(); ob0_files = set(); g5_files = set(); op_files = set(); ccv = {}
        gccn = gcc_list() if GCC else set()      # W17 CCSW: allowlisted files compiled by gcc (empty = nothing changes)
        gccv = subprocess.run(["i686-w64-mingw32-gcc", "-dumpfullversion"], capture_output=True, text=True).stdout.strip().encode() if gccn else b""
        OB0 = ["/Ob0"]      # clusters / tier 2 replace functions that the original CALLS (tools/diffemu verifies them with /Ob0 and hooks every callee): no inlining of their static helpers, so the shipped object is the verified one (2026-10-24, F-o2). Tier 1 / automatch stay /O2: their byte identity was proven with inlining.
        for c in srcs:
            nm = os.path.basename(c)
            if nm in skip:
                continue
            if os.path.exists(os.path.join(work, nm)):
                sys.exit("two sources with the same file name %s (they would overwrite each other in the compile directory and poison the object cache)" % nm)
            shutil.copy(c, os.path.join(work, nm))
            if b"compile: /GS" in open(c, "rb").read(): gs_files.add(nm)      # /GS stack-cookie functions (tools/gsfix.py): this file alone gets cl /GS
            if b"compile: /G5" in open(c, "rb").read(): g5_files.add(nm)      # RenderWare SDK functions were built by cl 12 (VC6, /G5 scheduling: no partial register xor, params loaded in source order): this file alone gets cl /G5 (BYTEMATCH_PATTERNS "RW library")
            if b"compile: /Op" in open(c, "rb").read(): op_files.add(nm)      # W9 T-OP: /Op (improve float consistency: every float result stored where the source stores it), this file alone (an_p_vectormult.c, the exe's spill points)
            _m = open(c, "rb").read(); ccv[nm] = "1310_3077" if b"compile: cl1310b" in _m else "1310" if b"compile: cl1310" in _m else os.environ.get("VC7_CL", "")      # compiler variant (vc7.sh VC7_CL), same rule as tools/cmpfn.py: `/* compile: cl1310 */` = cl 13.10 (toolchain/vc71), `cl1310b` = cl 13.10.3077 (vc71b), default cl 13.00
            if any(os.sep + d_ + os.sep in c for d_ in ("clusters", "automatch_near")): ob0_files.add(nm)
            if cl_entries and c not in cl_files:
                t_ = open(c, errors="replace").read(); t2_ = cl_strip(t_, [n_ for n_ in cl_entries if re.search(r"\b%s\b" % n_, t_)])
                if t2_ != t_: open(os.path.join(work, nm), "w").write(t2_)
            if os.sep + "bridges" + os.sep in c and "bridge_sel" not in nm:      # a bridge that moved to tier 2 (automatch_near, bridge disabled): its stub / C body definitions are dropped, the tier 2 function owns the symbol
                t_ = open(os.path.join(work, nm), errors="replace").read(); t2_ = cl_strip(t_, [n_ for n_ in t2near_off if re.search(r"\b%s\b" % n_, t_)])
                if t2_ != t_: open(os.path.join(work, nm), "w").write(t2_)
            t_ = open(os.path.join(work, nm), "rb").read(); i_ = inc_(os.path.join(work, nm))
            key = hashlib.sha1(t_ + hdr.digest() + vh_(t_) + i_ + " ".join(kflags_(t_ + i_) + (["/GS"] if nm in gs_files else []) + (OB0 if nm in ob0_files else []) + (["/G5"] if nm in g5_files else []) + (["/Op"] if nm in op_files else [])).encode() + (b" cc=" + ccv[nm].encode() if ccv[nm] else b"") + (b" cc=v143" if V143 else b"") + (b" crt=mt" if MT else b"")).hexdigest()      # the compiler variant is part of the key (a cl 13.00 object of a cl1310 file must never be reused); default cl 13.00 keys unchanged
            if nm in gccn: key = hashlib.sha1(t_ + hdr.digest() + vh_(t_) + i_ + " ".join(GCC_FLAGS + fixd_).encode() + b" cc=gcc " + gccv + b" float10=float").hexdigest()      # W17 CCSW: own key space (flags, gcc version); the VC7 keys above are untouched
            keyed.append((nm, os.path.join(cache, key + ".obj")))
            if not os.path.exists(keyed[-1][1]):
                todo.append(nm)
        print("objects: %d from the cache, %d to compile%s" % (len(keyed) - len(todo), len(todo), " (/Op: %s)" % ",".join(sorted(op_files)) if op_files else ""), file=sys.stderr)      # W9 T-OP test: an unmarked file's key is unchanged = cache hit
        if os.environ.get("CC_DRYRUN"):      # W17 CCSW: list what would be compiled and with what, compile / link nothing (same output with CC unset before and after this change = default builds unchanged)
            for nm, cp in keyed: print("key", nm, os.path.basename(cp))
            for nm in todo: print("compile", nm, "gcc" if nm in gccn else "cl " + ccv[nm] + (" /GS" if nm in gs_files else "") + (" /Ob0" if nm in ob0_files else "") + (" /G5" if nm in g5_files else "") + (" /Op" if nm in op_files else ""))
            if gccn: print("gcc", " ".join(GCC_FLAGS + fixd_), "files:", " ".join(sorted(gccn)))
            return
        if any(x in gccn for x in todo):      # W17 CCSW: the allowlisted misses go to gcc, the rest to cl as before
            gcc_compile(work, [x for x in todo if x in gccn], fixd_, keyed)
            todo = [x for x in todo if x not in gccn]
        if todo:
            # one container, several wine cl processes in parallel (same prefix/wineserver); 1 file per cl would waste the 8 s start per process
            out_ = v143_compile(work, todo, flags[4:], gs_files, ob0_files) if V143 else ""
            for cv_ in ([] if V143 else sorted(set(ccv[x] for x in todo))):      # one vc7.sh call per compiler variant (each has its own toolchain root / container)
                todo_ = [x for x in todo if ccv[x] == cv_]
                par_ = int(os.environ.get("JOBS", os.cpu_count() or 1))
                # W11 B3 (lane PP's finding): a long cl line under `wine cmd /c` is cut, so a full cache miss with JOBS=4 (1,150 files per cl line) failed with a
                # silent "compile error in ai_np.c"; 5,700 characters still failed (logs of agents/asm_w11 b3_fullmiss*), JOBS=24 (~3,100) worked.  Enough
                # chunks that each file list stays under 2,500 characters; they run JOBS at a time (waves below).
                njobs = min(len(todo_), max(par_, -(-sum(len(x) + 1 for x in todo_) // 2500)))
                chunks = [todo_[i::njobs] for i in range(njobs)]
                # every cl job gets its own TMP/TEMP below the work dir: cl names its intermediate files C:\...\Temp\_CL_<pid>..., and concurrent builds
                # (check_all BUILD_PAR, each a separate container = separate wineserver on the one prefix) reuse the same Windows pids -> C1083 (G0, 2026-11-12).
                # Set inside cmd /c (Wine replaces a unix TMP/TEMP by the prefix's registry value); %CD% = the work dir in both vc7.sh modes.
                jobs_ = [(" ".join(flags + (["/GS"] if g else []) + (OB0 if o else []) + (["/G5"] if v else []) + (["/Op"] if p else [])), " ".join(f_)) for c in chunks for g in (0, 1) for o in (0, 1) for v in (0, 1) for p in (0, 1) for f_ in [[x for x in c if (x in gs_files) == bool(g) and (x in ob0_files) == bool(o) and (x in g5_files) == bool(v) and (x in op_files) == bool(p)]] if f_]
                for j_ in range(len(jobs_)): os.makedirs(os.path.join(work, "cltmp%d" % j_), exist_ok=True)
                for w_ in range(0, len(jobs_), max(par_, 1)):      # at most JOBS cl processes at once (one vc7.sh call per wave; one wave unless the lines had to be split)
                    cmd = " ".join('wine cmd /c "set TMP=%%CD%%\\cltmp%d&&set TEMP=%%CD%%\\cltmp%d&&cl.exe %s %s" &' % (j_, j_, fl_, fs_) for j_, (fl_, fs_) in list(enumerate(jobs_))[w_:w_ + max(par_, 1)]) + " wait"
                    r = subprocess.run([os.path.join(HERE, "vc7.sh"), work, cmd], capture_output=True, text=True, env=dict(os.environ, VC7_SHELL="1", VC7_CL=cv_))
                    out_ += r.stdout + r.stderr
            miss_ = [nm for nm in todo if not os.path.exists(os.path.join(work, nm[:-2] + ".obj"))] if V143 else []
            if miss_:      # CC=v143: every file without an object, each with its own cl error lines (the ERRLEN tail below shows another file's output)
                for nm in miss_:
                    print("compile error in", nm, *[l for l in out_.splitlines() if l.startswith(nm + "(") and " error " in l][:5], sep="\n  ", file=sys.stderr)
                for nm, cp in keyed:      # keep the good objects: the next run compiles only the failed files (a full v143 compile is > 30 min)
                    if nm in todo and nm not in miss_:
                        tmp_ = "%s.%d.tmp" % (cp, os.getpid()); shutil.copy(os.path.join(work, nm[:-2] + ".obj"), tmp_); os.replace(tmp_, cp)
                sys.exit("v143: %d files without an object" % len(miss_))
            for nm in todo:
                o = os.path.join(work, nm[:-2] + ".obj")
                if not os.path.exists(o):
                    print("compile error in", nm, out_[-int(os.environ.get("ERRLEN", "600")):], file=sys.stderr)
                    sys.exit(1)
            for nm, cp in keyed:
                if nm in todo:
                    tmp_ = "%s.%d.tmp" % (cp, os.getpid()); shutil.copy(os.path.join(work, nm[:-2] + ".obj"), tmp_); os.replace(tmp_, cp)      # atomic: concurrent builds (check_all BUILD_PAR) never see a half-written cache object
        for nm, cp in keyed:
            o = os.path.join(work, nm[:-2] + ".obj")
            if os.path.exists(o):
                os.unlink(o)          # written by the container (root); replace by our copy
            shutil.copy(cp, o)
            objs.append(o)
        if os.environ.get("OBJLIST"):      # tools/delink: the cached objects (source name, object path) for linking with the delinked image instead of overlaying
            with open(os.environ["OBJLIST"], "w") as f:
                for nm, cp in keyed: f.write("%s %s\n" % (nm, cp))
            if os.environ.get("OBJLIST_ONLY"): return      # SK W16 NOEXE link (link_ci.sh): objects only, no overlay exe (overlay.py reads MFL.exe)
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
