#!/usr/bin/env python3
"""v143md_plan.py <link work dir>   |   --selftest
W16 V143 item 2 (CC=v143 LIB_CRT=v143md only; tools/delink/link_ours.sh runs it before mkobj): which names of our objects the v143 /MD CRT
(msvcrt.lib vcruntime.lib ucrt.lib legacy_stdio_definitions.lib, read with binutils nm from the msvc-wine toolset) must supply, by name.
Writes <dir>/v143/:
  crt_extern.txt  for mkobj --crt-extern: "name" = a CRT library defines it (mkobj leaves it undefined, not bound to the VC7 bytes of the image);
                  "name=crt" = our C calls a CRT function of the image by its Ghidra spelling (one more leading underscore, stdcall @N:
                  __fclose, ___alldiv@16) and the CRT has it as crt -> weak external; MAP below = the two heap thunks the skipped an_crt_heap_thunks.c defined
  dups.txt        "name our-object crt-lib crt-member": a non-COMDAT name our objects AND a CRT library define (LNK2005 / LNK4006 candidates)
  image_crt.txt   "name VA object": names that still bind to the image's VC7 CRT bytes (no v143 CRT counterpart; run-time hazards, not link errors)"""
import collections, csv, os, re, subprocess, sys

MS = os.path.expanduser("~/.cache/mfl/msvc-wine/msvc")
LIBS = [MS + "/vc/tools/msvc/14.44.35207/lib/x86/" + x for x in ("msvcrt.lib", "vcruntime.lib", "legacy_stdio_definitions.lib")] + [MS + "/kits/10/lib/10.0.26100.0/ucrt/x86/ucrt.lib"]
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
MAP = {"_operator_new": "_malloc", "__free_004b45fd": "_free",      # an_crt_heap_thunks.c (skipped): operator new / the free thunk of the image -> UCRT heap
       "_FUN_004b322b": "_exit"}      # V143c8: src/bridges/b004.c calls the image's VC7 exit (0x4b322b) by its decompiler name


def nm(paths):
    """-> {path: (defined {name: type}, undefined set)}"""
    r = collections.defaultdict(lambda: ({}, set()))
    for i in range(0, len(paths), 300):
        out = subprocess.run(["nm", "-A", "-g"] + paths[i:i + 300], capture_output=True, text=True, errors="replace").stdout
        for l in out.splitlines():
            p = l.rsplit(" ", 2)
            if len(p) != 3: continue
            f = p[0].split(":")[0]      # "<file>:<addr>" or "<lib>:<member (may hold D:\...)>:<addr>"
            mem = p[0].split(":", 1)[1].rsplit(":", 1)[0] if p[0].count(":") >= 2 else ""
            if p[1] == "U": r[f][1].add(p[2])
            elif p[1] != "C": r[f][0].setdefault(p[2], (p[1], mem))
    return r


def decide(und, crt, crtva, sym):
    """und: undefined names of our objects (none of ours defines them) -> (extern lines, image hazards [(name, VA)])"""
    ext, haz = [], []
    for n in sorted(und):
        if n.startswith("__imp_"): continue      # import libs (S2 IMPLIBS) or the CRT import members
        if n in crt: ext.append(n); continue
        if n in MAP: ext.append("%s=%s" % (n, MAP[n])); continue
        c = re.sub(r"@\d+$", "", n)
        m = re.match(r"_?func_0x([0-9a-f]{8})$", c)
        va = int(m.group(1), 16) if m else sym.get(c[1:], sym.get(c))
        if va not in crtva: continue
        if c[1:] in crt and c.startswith("__"): ext.append("%s=%s" % (n, c[1:]))
        else: haz.append((n, va))
    return ext, haz


# V143e1: VC7 EH / SEH frame helpers stay VC7 (stateless asm; the image's scope tables and FuncInfo records are VC7 format, one consistent set;
# setjmp3 stores the unwinder only when the frame handler IS this _except_handler3); _except1 = the VC7 libm internal error path
KEEP_VC7 = {"except_handler3", "SEH_prolog", "SEH_epilog", "global_unwind2", "local_unwind2", "NLG_Notify", "abnormal_termination",
            "CxxFrameHandler", "CxxThrowException", "FindAndUnlinkFrame", "DestructExceptionObject", "CallSettingFrame",
            "setjmp3", "longjmp", "except1"}
# VC7 time_t is 32 bit: its plain time functions are the UCRT's *32 exports (the UCRT's plain names are 64 bit header inlines)
ITEM_MAP = {"time": "__time32", "gmtime": "__gmtime32", "localtime": "__localtime32", "ctime": "__ctime32", "mktime": "__mktime32", "difftime": "__difftime32"}


def norm(n):
    return n.lstrip("_@").split("@")[0]


def redirects(items, labels, crt):
    """V143e1: the image's VC7 CRT functions -> v143 CRT names.  items: [(lo, hi, item name)] (ownership_items.csv code rows owned by the CRT);
    labels: {va: set of image label names (as mkobj writes them)}.  An item gets an entry jmp to the v143 function when a label at its start
    is in MAP or names a v143 CRT function with the item's own (normalised) name: FID conflict labels (_iswctype on _nh_malloc) do not
    match.  First tier with a hit wins: MAP, the label, the label minus one leading underscore (Ghidra spelling __fseek), "_" + the item
    name (FUN_ labels; only when no other CRT item has that name: exit / _exit).  -> (["va name"], ["va item why"] skipped)"""
    red, skip = [], []
    cnt = collections.Counter(norm(it) for _, _, it in items)
    for lo, hi, it in sorted(items):
        ls = labels.get(lo, set())
        hit, m = [], {MAP[n] for n in ls if n in MAP} | ({ITEM_MAP[it]} if ITEM_MAP.get(it) in crt else set())
        for t in (m, {n for n in ls if n in crt and norm(n) == norm(it)},
                  {n[1:] for n in ls if n.startswith("__") and n[1:] in crt and norm(n) == norm(it)},
                  {"_" + it} if cnt[norm(it)] == 1 and "_" + it in crt else set()):
            hit = sorted(t)
            if hit: break
        why = ("EH/SEH helper stays VC7" if norm(it) in KEEP_VC7 else "shorter than the 5 byte jmp" if hi - lo < 5
               else "no v143 name" if not hit else "ambiguous " + ",".join(hit) if len(hit) > 1 else None)
        if why: skip.append("%08x %s %s" % (lo, it, why))
        else: red.append("%08x %s" % (lo, hit[0]))
    return red, skip


def image_items():
    """-> (CRT code items of output/delink_sdk/ownership_items.csv, {va: image labels}) for redirects()"""
    items = [(int(r["lo"], 16), int(r["hi"], 16), r["name"]) for r in csv.DictReader(open(G + "/output/delink_sdk/ownership_items.csv"))
             if r["kind"] == "code" and r["owner"].startswith("crt")]
    labels = collections.defaultdict(set)
    for l in open(G + "/src/symbols.txt"):
        q = l.split()
        if len(q) == 2: labels[int(q[1], 16)].add("_" + q[0])
    for l in open(G + "/output/fnsizes.txt"):
        q = l.split()
        if len(q) == 3: labels[int(q[0], 16)].add("_" + q[1])
    for l in open(G + "/output/crt_libsyms.csv"):
        q = l.split(",")
        if len(q) >= 5 and q[4].strip() == "code": labels[int(q[1], 16)].add(q[0])
    return items, labels


def main(w):
    os.makedirs(w + "/v143", exist_ok=True)
    crt = {}
    for lib in LIBS:
        for n, (t, mem) in nm([lib])[lib][0].items():
            crt.setdefault(n, (os.path.basename(lib), mem, t))
    objs = [l.split()[1] for l in open(w + "/objs_local.txt") if l.strip()]
    syms = nm(objs)
    ours = {n for d, u in syms.values() for n in d}
    und = set().union(*(u for d, u in syms.values())) - ours
    crtva = {int(l.split(",")[1], 16) for l in open(G + "/output/crt_libsyms.csv") if l.count(",") >= 4 and l.split(",")[4].strip() == "code"}
    sym = {}
    for f in ("src/symbols.txt", "src/aliases.txt", "output/delink_sdk/lib_names.txt"):
        if not os.path.exists(os.path.join(G, f)): continue
        for l in open(os.path.join(G, f)):
            q = l.split()
            if len(q) >= 2 and not q[0].startswith("#") and re.match(r"^[0-9a-fA-F]+$", q[1]): sym.setdefault(q[0], int(q[1], 16))
    ext, haz = decide(und, crt, crtva, sym)
    who = collections.defaultdict(list)
    for o, (d, u) in syms.items():
        for n in u: who[n].append(os.path.basename(o))
    # COMDAT inline stdio helpers of the UCRT headers (legacy_stdio_definitions.lib has the same COMDATs) and string literals are not duplicates
    dups = sorted("%s %s %s %s" % (n, os.path.basename(o), crt[n][0], crt[n][1]) for o, (d, u) in syms.items() for n in d
                  if n in crt and not n.startswith(("??_C@", "___local_stdio_")) and crt[n][0] != "legacy_stdio_definitions.lib")
    open(w + "/v143/crt_extern.txt", "w").write("".join(x + "\n" for x in ext))
    open(w + "/v143/crt_names.txt", "w").write("".join(x + "\n" for x in sorted(crt)))      # V143c8: mkobj --crt-names: the image's labels of these names get a _vc7 suffix
    open(w + "/v143/dups.txt", "w").write("".join(x + "\n" for x in dups))
    open(w + "/v143/image_crt.txt", "w").write("".join("%s %08x %s\n" % (n, va, ",".join(sorted(who[n]))) for n, va in haz))
    red, skip = redirects(*image_items(), crt)
    open(w + "/v143/crt_redirect.txt", "w").write("".join(x + "\n" for x in red))      # V143e1: mkobj --crt-redirect
    open(w + "/v143/crt_redirect_skip.txt", "w").write("".join(x + "\n" for x in skip))
    print("v143md_plan: %d CRT names, crt_extern %d (%d by Ghidra spelling), dups %d, image_crt %d, crt_redirect %d (skipped %d)  (%s/v143)"
          % (len(crt), len(ext), sum("=" in x for x in ext), len(dups), len(haz), len(red), len(skip), w))


def selftest():
    crt = {"_fclose": 1, "__alldiv": 1, "_malloc": 1, "__ftol": 1}
    sym = {"_fclose": 0x4b4358, "__alldiv": 0x4b4160, "_nh_malloc": 0x4b45bf, "game_fn": 0x401000}
    va = {0x4b4358, 0x4b4160, 0x4b45bf}
    ext, haz = decide({"__fclose", "___alldiv@16", "__nh_malloc", "_game_fn", "__ftol", "__imp__malloc", "_operator_new", "_FUN_004b322b"}, crt, va, sym)
    assert ext == ["_FUN_004b322b=_exit", "___alldiv@16=__alldiv", "__fclose=_fclose", "__ftol", "_operator_new=_malloc"], ext
    assert haz == [("__nh_malloc", 0x4b45bf)], haz
    red, skip = redirects([(0x4b45bf, 0x4b45eb, "nh_malloc"), (0x4b45eb, 0x4b45fd, "malloc"), (0x4b3c0c, 0x4b3c1a, "operator_new"),
                           (0x4bd500, 0x4bd5bd, "except_handler3"), (0x4bd7d1, 0x4bd7f0, "tolower"), (0x4be080, 0x4be083, "fptrap"), (0x4b322b, 0x4b323c, "exit"),
                           (0x4b323c, 0x4b324d, "exit"), (0x4b4531, 0x4b4544, "fopen"), (0x5d3133, 0x5d3172, "fseek"), (0x5d3805, 0x5d3880, "time")],
                          {0x4b45bf: {"__nh_malloc", "_iswctype"}, 0x4b45eb: {"_malloc", "__malloc"}, 0x4b3c0c: {"_operator_new"}, 0x4bd500: {"__except_handler3"},
                           0x4bd7d1: {"_tolower", "__tolower"}, 0x4be080: {"__fptrap"}, 0x4b322b: {"_exit"}, 0x4b323c: {"___exit", "__exit"},
                           0x4b4531: {"_FUN_004b4531"}, 0x5d3133: {"__fseek"}, 0x5d3805: {"__time"}},
                          {"_malloc", "_iswctype", "__except_handler3", "_tolower", "__tolower", "__fptrap", "_exit", "__exit", "_fopen", "_fseek", "__time32"})
    assert red == ["004b322b _exit", "004b323c __exit", "004b3c0c _malloc", "004b4531 _fopen", "004b45eb _malloc", "005d3133 _fseek", "005d3805 __time32"], red
    assert [s.split(None, 2)[2] for s in skip] == ["no v143 name", "EH/SEH helper stays VC7", "ambiguous __tolower,_tolower", "shorter than the 5 byte jmp"], skip
    print("v143md_plan selftest: ok")


if __name__ == "__main__":
    selftest() if sys.argv[1:] == ["--selftest"] else main(sys.argv[1])
