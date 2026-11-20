#!/usr/bin/env python3
"""objnames.py <objs.txt> [<extra objs file>]   |   --rsp <objs_local.txt>   |   --selftest
W11 B2: stable object names for tools/delink/link_ours.sh (was o/<n>.obj, numbered in list order: one file more or less renumbered every later object).
Default form: one line "src obj name" per input line ("src obj": build_overlay's objs.txt, then the EXTRA_OBJS chain), name =
  our C "<file>.c"                          -> "<file>.obj"            (build_overlay.py refuses two sources with one file name)
  "lib:<lib>:src"                           -> "<lib>__<object file>"  (source-built Lua / qhull / zlib members)
  "lib:<lib>.lib:<member path>"             -> "<lib>__<member file>"  (split CRT members)
  "<kind>:<name>[:<member path>]"           -> "<kind>__<last part>"   (stub:stubs, pull:..., rw:thunks, dinput:formats)
A duplicate name (case-insensitive, as link.exe sees files) is a hard error: exit 1, nothing printed.
--rsp: the link.rsp lines for the objects of objs_local.txt: our C sorted by name (byte order, no locale), then the extra objects in their
chain order.  That order keeps every LNK4006 winner of the numbered form (first definition wins: stub__stubs.obj must stay in front of
the libcmt / pull objects that define _fgetc / _ungetc too; agents/asm_w11/b2_dupwin.py), and it does not change when an object leaves the list."""
import os, re, sys

def name(src, obj):
    if src.endswith(".c"): return src[:-2] + ".obj"
    p = src.split(":")
    if p[0] == "lib" and len(p) == 3 and p[2] == "src": return "%s__%s" % (p[1], os.path.basename(obj))
    if p[0] == "lib": return "%s__%s" % (p[1].split(".")[0], re.split(r"[\\/]", ":".join(p[2:]))[-1])
    last = re.split(r"[\\/:]", ":".join(p[1:]))[-1]
    return "%s__%s" % (p[0], last if last.endswith(".obj") else last + ".obj")

def names(lines):
    out, seen = [], {}
    for src, obj in lines:
        n = name(src, obj); k = n.lower()
        if k in seen: raise SystemExit("objnames: duplicate object name %s (%s and %s)" % (n, seen[k], src))
        seen[k] = src; out.append((src, obj, n))
    return out

def rsp(local):
    c = sorted((o for s, o in local if s.endswith(".c")), key=lambda o: os.path.basename(o).encode())
    return ["o/" + os.path.basename(o) for o in c] + ["o/" + os.path.basename(o) for s, o in local if not s.endswith(".c")]

def selftest():
    ok = names([("a.c", "/x/1.obj"), ("lib:lua:src", "/l/_o/lapi.obj"), ("lib:libcmt.lib:..\\build\\intel\\mt_obj\\87cdisp.obj", "/c/x.obj"),
                ("stub:stubs", "/s/stubs.obj"), ("pull:libcmt.lib:build\\intel\\mt_obj\\woutput.obj", "/p/w.obj"), ("rw:thunks", "/r/t.obj")])
    assert [n for _, _, n in ok] == ["a.obj", "lua__lapi.obj", "libcmt__87cdisp.obj", "stub__stubs.obj", "pull__woutput.obj", "rw__thunks.obj"], ok
    try: names([("Ab.c", "/1.obj"), ("x:ab", "/2.obj"), ("aB.c", "/3.obj")]); raise AssertionError("duplicate not refused")
    except SystemExit as e: assert "duplicate" in str(e)
    loc = [("b.c", "/w/o/b.obj"), ("a.c", "/w/o/a.obj"), ("stub:stubs", "/w/o/stub__stubs.obj"), ("lib:libcmt.lib:x\\fgetc.obj", "/w/o/libcmt__fgetc.obj")]
    assert rsp(loc) == ["o/a.obj", "o/b.obj", "o/stub__stubs.obj", "o/libcmt__fgetc.obj"], rsp(loc)
    assert rsp(loc[1:]) == ["o/a.obj", "o/stub__stubs.obj", "o/libcmt__fgetc.obj"]      # one object less: the others keep name and order
    print("objnames selftest: ok (names per class, duplicate refused, rsp order C-by-name then chain)")

if __name__ == "__main__":
    a = sys.argv[1:]
    if a == ["--selftest"]: selftest(); sys.exit(0)
    if a[:1] == ["--copy"]:      # W16 SPD: --copy <W> <objnames.txt>: copy each object to <W>/o/<name>, print the objs_local.txt lines (was a bash cp loop: one process per object, ~4,200 on the CI runner)
        import shutil
        for l in open(a[2]):
            if not l.strip(): continue
            s, o, n = l.split(None, 2); n = n.strip(); shutil.copy(o, os.path.join(a[1], "o", n)); print("%s %s/o/%s" % (s, a[1], n))
        sys.exit(0)
    if a[:1] == ["--rsp"]:
        print("\n".join(rsp([l.split(None, 1) for l in (x.strip() for x in open(a[1])) if l]))); sys.exit(0)
    lines = [l.split(None, 1) for f in a for l in (x.strip() for x in open(f)) if l]
    print("\n".join("%s %s %s" % x for x in names([(s, o.strip()) for s, o in lines])))
