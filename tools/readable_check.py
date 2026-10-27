#!/usr/bin/env python3
"""readable_check.py <before.c> <after.c> [function ...] [--emu]
Invariance check for readability edits of tier 2 functions (no byte match with the original exists, so check_one cannot guard them):
compile both texts with the diffemu / t2_auto flags (cl /nologo /c /Gy /O2 /Ob0, VC7 container, headers beside each file), and compare per function
(every function of the file, or only the named ones): code bytes AND relocations (offset, type, target symbol name; the addend is part of the code bytes).
Also compared: the set of functions, and every other COMDAT section (data, jump tables, string / float literals) by name.
Prints `SAME <fn>` per function, or `DIFF <fn>: ...` with the first differing offsets; the last line is `SAME` / `NOT SAME`.  Exit 0 = SAME, 1 = differs, 2 = compile error / function missing.
--emu: when SAME, run `diffemu.py --src after.c <fn>` for the functions and require EQUAL (sanity; slow, needs the diffemu venv)."""
import os, re, shutil, struct, subprocess, sys, tempfile
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HERE)
os.environ.setdefault("VC7_CONTAINER", "cmpfn")
import hashlib
VC7 = os.path.join(HERE, "vc7.sh")
WORKROOT = os.path.join(os.environ.get("TMPDIR", "/tmp/claude-%d" % os.getuid()), "readable_check")
OBJCACHE = os.environ.get("DIFFEMU_OBJ", os.path.expanduser("~/.cache/mfl_diffemu_obj"))      # shared with diffemu (same key)
FLAGS = ["/nologo", "/c", "/Gy", "/O2", "/Ob0"]

def sha(*parts):
    h = hashlib.sha1()
    for p in parts: h.update(p if isinstance(p, bytes) else str(p).encode())
    return h.hexdigest()[:16]

def compile_obj(src):
    d = os.path.dirname(os.path.abspath(src))
    hs = sha(*[open(os.path.join(d, x), "rb").read() for x in sorted(os.listdir(d)) if x.endswith(".h")])
    key = sha(open(src, "rb").read(), hs, " ".join(FLAGS)); obj = os.path.join(OBJCACHE, key + ".obj")
    if os.path.exists(obj): return obj
    os.makedirs(OBJCACHE, exist_ok=True); os.makedirs(WORKROOT, exist_ok=True)
    w = tempfile.mkdtemp(prefix="rc_", dir=WORKROOT)
    try:
        for x in os.listdir(d):
            if x.endswith(".h"): shutil.copy(os.path.join(d, x), w)
        shutil.copy(src, os.path.join(w, "f.c"))
        r = subprocess.run([VC7, w, "cl.exe"] + FLAGS + ["f.c"], capture_output=True, text=True, timeout=600)
        if not os.path.exists(os.path.join(w, "f.obj")): raise RuntimeError("compile error in %s:\n%s" % (src, (r.stdout + r.stderr)[-1500:]))
        shutil.copy(os.path.join(w, "f.obj"), obj + ".tmp"); os.replace(obj + ".tmp", obj)
    finally: shutil.rmtree(w, ignore_errors=True)
    return obj

def parse(obj):
    """-> {section key: (is_code, bytes, [(offset, type, target name)])}; key = name of the first symbol defined in the section (COMDAT name)"""
    d = open(obj, "rb").read()
    _, nsec, _, symoff, nsym, opt, _ = struct.unpack_from("<HHIIIHH", d, 0)
    strtab = symoff + nsym * 18
    def nm(raw):
        if raw[:4] == b"\0\0\0\0":
            o = struct.unpack_from("<I", raw, 4)[0]; return d[strtab + o:d.index(b"\0", strtab + o)].decode()
        return raw.rstrip(b"\0").decode()
    syms = {}; first = {}; i = 0
    while i < nsym:
        o = symoff + i * 18; n = nm(d[o:o + 8]); value, sec, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        if re.fullmatch(r'\$L\d+', n) and sec > 0: n = '$L#%d+%d' % (sec, value)      # compiler-numbered local label (jump tables, switch cases): identify by place, not by number
        syms[i] = n
        if sec > 0 and cls in (2, 3) and not n.startswith(".") and sec not in first: first[sec] = n
        i += 1 + aux
    out = {}
    for s in range(nsec):
        o = 20 + opt + s * 40
        sname, _, _, rawsz, rawptr, relptr, _, nrel, _, flags = struct.unpack_from("<8sIIIIIIHHI", d, o)
        if flags & 0x00000800 or sname.startswith((b".drectve", b".debug")): continue      # info / debug
        raw = d[rawptr:rawptr + rawsz] if not flags & 0x80 else b"bss%d" % rawsz
        rel = sorted((struct.unpack_from("<I", d, relptr + k * 10)[0], struct.unpack_from("<H", d, relptr + k * 10 + 8)[0],
                      syms[struct.unpack_from("<I", d, relptr + k * 10 + 4)[0]]) for k in range(nrel))
        key = first.get(s + 1, "%s#%d" % (sname.rstrip(b"\0").decode(), s))
        out[key] = (bool(flags & 0x20), raw, rel)
    return out

def undec(n): return re.sub(r"^[_@]|@\d+$", "", n) if re.match(r"^[_@]\w+(@\d+)?$", n) else n

def compare(a, b, fns):
    """-> (list of lines, all_same)"""
    lines = []; same = True
    keys = [f for f in (fns or sorted(set(a) | set(b)))]
    for fn in keys:
        ka = next((k for k in (fn, "_" + fn) if k in a), None) or next((k for k in a if re.fullmatch(r"[_@]%s(@\d+)?" % re.escape(fn), k)), None)
        kb = next((k for k in (fn, "_" + fn) if k in b), None) or next((k for k in b if re.fullmatch(r"[_@]%s(@\d+)?" % re.escape(fn), k)), None)
        if fns is None: ka = ka or (fn if fn in a else None); kb = kb or (fn if fn in b else None)
        if ka is None or kb is None:
            lines.append("DIFF %s: missing in %s" % (fn, "before" if ka is None else "after")); same = False; continue
        ca, cb = a[ka], b[kb]; why = []
        if ca[1] != cb[1]:
            if len(ca[1]) != len(cb[1]): why.append("size %d -> %d" % (len(ca[1]), len(cb[1])))
            offs = [i for i in range(min(len(ca[1]), len(cb[1]))) if ca[1][i] != cb[1][i]]
            if offs: why.append("code bytes differ at %s" % ", ".join("+0x%x" % o for o in offs[:6]) + (" (+%d more)" % (len(offs) - 6) if len(offs) > 6 else ""))
        if ca[2] != cb[2]:
            sa, sb = set(ca[2]), set(cb[2])
            for r in sorted(sa - sb)[:4]: why.append("reloc only before: +0x%x type %d %s" % r)
            for r in sorted(sb - sa)[:4]: why.append("reloc only after:  +0x%x type %d %s" % r)
        if why: lines.append("DIFF %s: %s" % (fn, "; ".join(why))); same = False
        else: lines.append("SAME %s (%d bytes, %d relocs)" % (fn, len(ca[1]), len(ca[2])))
    return lines, same

def main():
    args = [x for x in sys.argv[1:] if not x.startswith("-")]; emu = "--emu" in sys.argv
    if len(args) < 2: sys.exit(__doc__)
    before, after, fns = args[0], args[1], args[2:] or None
    try: oa, ob = compile_obj(before), compile_obj(after)
    except RuntimeError as e: print(e); return 2
    a, b = parse(oa), parse(ob)
    # default: every section of the file (functions are the code sections; the rest is data / literals)
    if fns is None:
        lines, same = compare(a, b, None)
        fnames = [k.lstrip("_@").split("@")[0] for k in sorted(a) if a[k][0]]
    else:
        lines, same = compare(a, b, fns); fnames = fns
    for l in lines:
        if l.startswith("DIFF") or "-v" in sys.argv or fns is not None: print(l)
    nsame = sum(l.startswith("SAME") for l in lines)
    print("%d sections/functions SAME, %d DIFF" % (nsame, len(lines) - nsame))
    if same and emu:
        py = os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python")
        for f in fnames:
            r = subprocess.run([py if os.path.exists(py) else sys.executable, os.path.join(HERE, "diffemu", "diffemu.py"), "--src", os.path.abspath(after), f], capture_output=True, text=True, env=dict(os.environ, DIFFEMU_OUT=os.path.join(WORKROOT, "diffemu")))
            o = (r.stdout + r.stderr).strip().splitlines()
            print("emu %s: %s" % (f, o[-1] if o else "?"))
            if r.returncode: same = False
    print("SAME" if same else "NOT SAME")
    return 0 if same else 1

if __name__ == "__main__": sys.exit(main())
