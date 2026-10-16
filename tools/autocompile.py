#!/usr/bin/env python3
"""Compile the named decompilation function by function (in chunks) with cl 13.00.9466 and report which functions compile.
Usage: autocompile.py <workdir> <decls.h> <first VA hex> <last VA hex> [chunk size]
Needs <workdir>/ghidra_types.h (fix_ghidra_types.py output) and prelude.h.  Writes <workdir>/ok.c (all compiling functions,
one translation unit) and <workdir>/failed.txt (function, first error)."""
import os, re, subprocess, sys, time

HERE = os.path.dirname(os.path.abspath(__file__))
work, decls, lo, hi = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
chunk = int(sys.argv[5]) if len(sys.argv) > 5 else 40
import shutil
for _h in ("ghidra_types.h", "prelude.h"):      # always the CURRENT headers (a copy in the work dir from an older round hid fields added since: 94 wave-0 "compile failures" of the first census)
    _src = os.path.join(HERE, "..", "src", "auto", _h)
    _ov = os.path.join(os.environ.get("T2_OVERLAY_DIR", "/nonexistent"), _h)      # t2_auto: header patched with the newer struct specs (tools/spec_header.py)
    if os.path.exists(_ov): _src = _ov
    if os.path.exists(_src) and (not os.path.exists(os.path.join(work, _h)) or open(_src, "rb").read() != open(os.path.join(work, _h), "rb").read()): os.makedirs(work, exist_ok=True); shutil.copy(_src, os.path.join(work, _h)); print("refreshed", _h, file=sys.stderr)
text = open(os.environ.get("AC_TEXT") or os.path.join(HERE, "..", "output", "named", "MFL_app_named.c"), errors="replace").read()      # AC_TEXT=<file>: another decompilation (tools/t2_auto.py: bodies with scripted fixes / a Ghidra export of FUN_ functions)
REG = re.compile(r"\b(reg_e(?:ax|bx|cx|dx|si|di|bp)|in_[A-Z]{2,3}|unaff_[A-Za-z]+|extraout_\w+|stack0x\w+|param_\d+_00\w+|unaff_retaddr|[iu]?Stack[0-9a-f]{8}|in_stack_\w+|POPCOUNT|SBORROW\d|CARRY\d|halt_baddata|ABS|fpatan|fsin|fcos|fptan|fyl2x|f2xm1)\b")
# functions with a hand-written C file (src/manifest.txt) are not regenerated
# functions that read registers at entry (tools/reginputs.py, from the disassembly: the decompiler often loses such parameters without leaving an in_/unaff_ marker)
regin = set()
rp = os.path.join(HERE, "..", "output", "reginputs.csv")
if os.path.exists(rp):
    for l in open(rp).read().splitlines()[1:]:
        p = l.split(",")
        if len(p) >= 3 and p[2].strip("|"):
            regin.add(p[0])
have = {l.split()[1] for l in open(os.path.join(HERE, "..", "src", "manifest.txt")) if len(l.split()) > 3 and l.split()[0] not in ("#", "(none)")}
decl_lines = open(decls, errors="replace").read().split("\n")
# type overrides for globals whose Ghidra type is wrong for the compiler (`undefined4` compares unsigned, the original compares signed): names/MFL_global_types.txt, "name type" per line,
# found by tools/variants.py (rule gsign) and collected by tools/variants_globals.py
GT = {}
gtp = os.path.join(HERE, "..", "names", "MFL_global_types.txt")
FIX_RULES = set(os.environ.get("FIX_RULES", "widths,ret_wide,conv,locals,imports,idxscale,callees,subpiece,externs,underscore").split(","))      # systematic fixers that can be switched off for an A/B measurement (tools/rule_ab.sh)
if os.path.exists(gtp):
    for gf in [gtp] + ([os.path.join(HERE, "..", "names", "MFL_global_widths.txt")] if "widths" in FIX_RULES else []):
      for l in open(gf):
        p_ = l.split()
        if len(p_) == 2 and not l.startswith("#"): GT[p_[0]] = p_[1]
    decl_lines = [re.sub(r"(extern )(?:\w+ )+?(%s);" % re.escape(m_.group(1)), lambda mm: "%s%s %s;" % (mm.group(1), GT[m_.group(1)], m_.group(1)), l) if (m_ := re.search(r"extern (?:\w+ )+?(\w+);", l)) and m_.group(1) in GT and "[" not in l else l for l in decl_lines]


# FUN_ callees with a prototype (tools/callee_protos.py: names/MFL_callee_protos.txt, hand names names/MFL_callee_names.csv): the declaration of that address is replaced, the hand named callees are renamed in
# every body, and a call to such a callee no longer makes its caller "unresolved-call" (rule `callees`)
CALLEE_NAME, CALLEE_OK = {}, set()
if "callees" in FIX_RULES:
    _np, _pp = os.path.join(HERE, "..", "names", "MFL_callee_names.csv"), os.path.join(HERE, "..", "names", "MFL_callee_protos.txt")
    if os.path.exists(_np):
        for l_ in open(_np):
            p_ = l_.rstrip("\n").split(",", 2)
            if len(p_) >= 2 and not l_.startswith("#"): CALLEE_NAME["FUN_" + p_[0]] = p_[1]
    if os.path.exists(_pp):
        _pv = {}
        for _pf in (_pp, os.path.join(HERE, "..", "names", "MFL_callee_protos_auto.txt"), os.path.join(os.environ.get("T2_OVERLAY_DIR", "/nonexistent"), "MFL_callee_protos_auto.txt")):      # the third: the overlay of a t2_auto run      # the second: tools/callee_auto.py (t2_auto.py), exact-confidence only
          if not os.path.exists(_pf): continue
          for l_ in open(_pf):
            m_ = re.match(r"/\*([0-9a-f]{8})\*/ extern ", l_)
            if m_: _pv[m_.group(1)] = l_.rstrip("\n"); CALLEE_OK.add("FUN_" + m_.group(1))
        _seen = set()
        decl_lines = [(_pv[m_.group(1)] if (m_ := re.match(r"/\*([0-9a-f]{8})\*/ extern ", l_)) and m_.group(1) in _pv and not _seen.add(m_.group(1)) else l_) for l_ in decl_lines]
        decl_lines += [l_ for v_, l_ in _pv.items() if v_ not in _seen]
_CNRE = re.compile(r"\b(" + "|".join(sorted(CALLEE_NAME)) + r")\b") if CALLEE_NAME else None

EXTRA_DECL = []
_xp = os.path.join(HERE, "..", "names", "MFL_extern_fix.txt")
if "externs" in FIX_RULES:
    for _xf in (_xp, os.path.join(os.environ.get("T2_OVERLAY_DIR", "/nonexistent"), "MFL_extern_fix.txt")):      # the second: the overlay of a t2_auto run
        if os.path.exists(_xf): EXTRA_DECL += [l_.rstrip("\n") for l_ in open(_xf) if l_.strip() and not l_.startswith("#") and l_.rstrip("\n") not in EXTRA_DECL]
# functions that pop their own arguments (`ret N`, tools/stdcall_scan.py) but are declared without __stdcall: the C would add `add esp,N` after the call
STD = {}
_sp = os.path.join(HERE, "..", "output", "stdcall.txt")
if os.path.exists(_sp):
    for l_ in open(_sp):
        p_ = l_.split()
        if len(p_) == 2: STD[p_[0]] = int(p_[1])
def _stdfix(l_):
    m_ = re.match(r"^(/\*([0-9a-f]{8})\*/ extern )([^(]*?)(\w+)\(([^)]*)\);", l_)
    if not m_ or m_.group(2) not in STD or re.search(r"__(?:stdcall|fastcall|thiscall)", l_): return l_
    n_ = STD[m_.group(2)] // 4
    params = m_.group(5).strip()
    have = 0 if params in ("", "void") else params.count(",") + 1
    if have != n_: params = ", ".join("undefined4 p%d" % i for i in range(n_))
    return "%s%s__stdcall %s(%s);" % (m_.group(1), m_.group(3), m_.group(4), params)
decl_lines = [_stdfix(l_) for l_ in decl_lines]
decl_lines = [re.sub(r"extern undefined (float_\w+);", r"extern float \1;", l_) for l_ in decl_lines]      # float literals Ghidra left untyped
DECLARED = set()
for l_ in decl_lines:
    m_ = re.search(r"extern [^;(]*?(\w+)(?:\[[^\]]*\])?;", l_)
    if m_: DECLARED.add(m_.group(1))
UNDERSCORED = re.compile(r"\b_((?:g_|float_|double_|k_f_|DAT_)\w*)\b" if "underscore" not in FIX_RULES else r"\b_([A-Za-z]\w*)\b")      # rule `underscore`: every `_name` whose `name` is declared (`_distance_to_leader`)

def strip_underscores(b):
    """the decompiler prints `_g_x` / `_float_1_0` for a global that overlaps smaller symbols at its address; the declaration exists without the underscore (same address)"""
    return UNDERSCORED.sub(lambda m: m.group(1) if m.group(1) in DECLARED and m.group(0) not in DECLARED else m.group(0), b)

ARR = {}      # global struct arrays: name -> struct type (from decls.h)
for l in open(decls, errors="replace"):
    m = re.search(r"extern (struct \w+) (g_\w+)\[\d+\];", l)
    if m: ARR[m.group(2)] = m.group(1)

SUBTYPE = {1: "byte", 2: "ushort", 4: "uint", 8: "double"}
SUBPIECE = re.compile(r"((?:\w+)(?:\[[^\[\]]*\]|\.[A-Za-z]\w*|->[A-Za-z]\w*)*)(\.|->)_(\d+)_([1248])_")
def subpiece(b):
    """Ghidra writes the bytes OFF..OFF+SZ of a variable / structure that has no field there as `x._OFF_SZ_` (`param_1._2_2_`, `g_game._2796_4_`, `p->_58_1_`): not C.  Same bytes through a pointer:
    `(*(ushort *)((char *)&x + 2))` / `(*(byte *)((char *)p + 58))` (a 3 byte piece `_0_3_` has no C type and stays an error).  Rule `subpiece`: only touches text that never compiled."""
    return SUBPIECE.sub(lambda m: "(*(%s *)((char *)%s%s + %s))" % (SUBTYPE[int(m.group(4))], "&" if m.group(2) == "." else "", m.group(1), m.group(3)), b)

def ptr_locals(b):
    """`g_arr[i].a = ..; g_arr[i].b = ..` (same global struct array, same index variable, 2+ uses): the original keeps `&g_arr[i]` in one register (one `imul`), write a local pointer
    (the README lesson `struct *p = &arr[i]`); declared right in front of the first use."""
    for name, typ in ARR.items():
        for idx in sorted(set(re.findall(r"\b%s\[(\w+)\]\." % name, b))):
            pat = r"\b%s\[%s\]\." % (name, re.escape(idx))
            if len(re.findall(pat, b)) < 2: continue
            first = re.search(pat, b)
            ls = b.rfind("\n", 0, first.start()) + 1
            indent = re.match(r"\s*", b[ls:]).group(0)
            if b[ls:first.start()].strip(): continue          # the first use is inside a larger expression: leave it
            pv = "p_%s" % idx
            b = b[:ls] + "%s%s *%s = &%s[%s];\n" % (indent, typ, pv, name, idx) + re.sub(pat, pv + "->", b[ls:])
            # a declaration in the middle of a block is not C89: move it to the top of the function
            line = "%s%s *%s = &%s[%s];\n" % (indent, typ, pv, name, idx)
            b = b.replace(line, "%s%s = &%s[%s];\n" % (indent, pv, name, idx), 1)
            b = re.sub(r"\{\n", "{\n  %s *%s;\n" % (typ, pv), b, count=1)
    return b

def fix_body(b):
    """the decompiler shows `f(x); v = _ftol();` for a call returning a double that is truncated (the value travels in st(0)): write `v = (int)f(x);`
    (compiles to the same call + _ftol, but the C version no longer pops st(0) after the call: 25 lua_* functions differed by one `fstp st(0)`)"""
    b = ptr_locals(strip_underscores(b))
    b = re.sub(r"^(\s*\w+:)[ \t]*\n(\s*\})", r"\1;\n\2", b, flags=re.M)      # a label directly before `}` is not C (Ghidra emits it at the end of a block): `LAB_x:;`
    if "idxscale" in FIX_RULES:      # `(&g + i * 4)` is a BYTE offset in Ghidra's output (g was 1 byte there), but once global_widths widened g to 4 bytes C scales it by 4 again: 11 near misses (shl 4 vs [eax*4+g])
        b = re.sub(r"\(&(g_\w+) \+ ", r"((char *)&\1 + ", b)
    if "subpiece" in FIX_RULES: b = subpiece(b)
    return re.sub(r"^(\s*)(\w+)\(([^;\n]*)\);\n\s*(\w+) = _ftol\(\);", lambda m: "%s%s = (int)%s(%s);" % (m.group(1), m.group(4), m.group(2), m.group(3)), b, flags=re.M)

# AC_PLAIN=1: also take cdecl functions whose only pseudo registers are `reg_e??` LOCALS (Ghidra's name for a local passed in a register to a callee with register parameters, tools/tier2_census.py class
# plain-regcallee): the calls to register functions go through the F_r call adapters of tools/bridgegen.py (emitted into the chunk, static), nothing else changes.
# AC_X87=1: also take functions whose only pseudo variables are fsin / fcos / fpatan / ABS (x87 instructions the original executes inline): written as CRT calls sin / cos / atan2 / fabs
# (equal behaviour for in-range arguments, never the same bytes: tier 2 candidates only; atan2 / fabs may have no address in symbols.txt: the census build drops such functions).  Both are opt-in: the default flow and its results are unchanged.
PLAIN, X87 = os.environ.get("AC_PLAIN") == "1", os.environ.get("AC_X87") == "1"
if PLAIN: sys.path.insert(0, HERE); from callee_auto import float_literal_ok as _fl_ok
X87_NAME = {"fsin": "sin", "fcos": "cos", "ABS": "fabs", "fpatan": "atan2"}      # CRT calls (no inline asm: tools/cheatgrep.py): fpatan(st1, st0) = atan2(st1, st0); the original is the inline x87 instruction
X87_PROTO = {"sin": "double sin(double);", "cos": "double cos(double);", "fabs": "double fabs(double);", "atan2": "double atan2(double, double);"}
ADAPT = {}      # register function -> bridgegen description (F_r adapters)
if PLAIN:
    sys.path.insert(0, HERE)
    import bridgegen as bg
    _bl, _sg, _ri, _man = bg.load()
    _ad = sorted(a_ for a_, _ in _bl.values()); _nx = {a_: _ad[i_ + 1] if i_ + 1 < len(_ad) else a_ + 0x400 for i_, a_ in enumerate(_ad)}
    for _n in bg.adapter_ok(_bl, _sg, _ri, _man, _nx): ADAPT[_n] = bg.analyse(_n, _bl, _sg, _ri, _man, _nx)[0]
    if os.environ.get("AC_SYNTH", "1") == "1": ADAPT.update({n_: f_ for n_, f_ in bg.synth_adapters(_bl, _ri, {})[0].items() if n_ not in ADAPT})      # register callees without a decompiled signature: F_r from regsigs.csv (as usercall_adapters.py)
    for _n, _f in ADAPT.items():      # the decompiled prototype says `void` for a register callee whose result travels in eax and a caller uses (`iVar1 = f(x)`): the naked F_r leaves eax alone, so `int` is free
        if _f["ret"] == "void": ADAPT[_n] = dict(_f, ret="int")      # a caller may use the value (`v = f(x)`: eax after the call; ret_regs does not always list it): the naked F_r returns whatever eax the callee left, `int` costs nothing
    NOADAPT = {c_ for c_, (r_, _) in _ri.items() if r_.strip("|") and c_ not in ADAPT}      # register callees without adapter: their callers stay out
ONLY = {l_.split()[0] for l_ in open(os.environ["AC_ONLY"]) if l_.strip()} if os.environ.get("AC_ONLY") else None      # AC_ONLY=<file with function names>: compile just these (subset runs, tools/tier2_census.py reruns)
funcs, funcs_all = [], []
for m in re.finditer(r"// ==== (\S+) @ ([0-9a-f]+)\n(.*?)(?=\n// ==== |\Z)", text, re.S):
    n, a, body = m.group(1), int(m.group(2), 16), m.group(3)
    if _CNRE: body = _CNRE.sub(lambda m_: CALLEE_NAME[m_.group(1)], body)
    flat = re.sub(r"\s+", " ", body)
    unsafe = any(c_ not in CALLEE_OK for c_ in re.findall(r"\b(FUN_[0-9a-f]{8}) ?\(", flat)) or any(      # unprototyped calls: unknown callee signature, float literals become 8 byte doubles
        re.search(r"(?<![\w.])\d+\.\d+(?![\w.])", a) and c not in ("if", "while", "for", "switch", "sizeof", "return") and not (PLAIN and _fl_ok(c, a)) for c, a in re.findall(r"\b(\w+) ?\(([^;{}]*)\);", flat))      # t2_auto (AC_PLAIN): a float literal is fine where the callee prototype has float / double there (tools/callee_auto.py float_literal_ok)
    rf_ = set(REG.findall(body))
    funcs_all.append((n, a, fix_body(body.replace("__thiscall", "__fastcall"))))
    allow_ = ({r_ for r_ in rf_ if r_.startswith("reg_e")} if PLAIN else set()) | ({"fsin", "fcos", "fpatan", "ABS"} & rf_ if X87 else set())
    called_ = set(re.findall(r"\b(\w+) ?\(", flat)) - {n}
    if PLAIN and called_ & NOADAPT: continue
    if (ONLY is None or n in ONLY) and n not in have and n not in regin and not unsafe and int(lo, 16) <= a <= int(hi, 16) and not (rf_ - allow_):
        if X87: body = re.sub(r"\b(fsin|fcos|fpatan|ABS)\(", lambda m_: X87_NAME[m_.group(1)] + "(", body)      # before fix_body: `x87_sin(x); v = _ftol();` becomes `v = (int)x87_sin(x);`
        if PLAIN and called_ & set(ADAPT): body = re.sub(r"\b(%s)(\s*)\(" % "|".join(map(re.escape, called_ & set(ADAPT))), r"\1_r\2(", body)      # the decompiler may break the line between name and `(` ; guard: an empty alternation would rewrite every `(`
        funcs.append((n, a, fix_body(body.replace("__thiscall", "__fastcall"))))


# AC_FIX=rule,rule (or all): retry pass over the functions that did NOT compile (functions that compiled are never touched: the matched set cannot change), each one alone in its chunk.  Rules:
#   subword    `x._2_2_` / `x._0_1_` on a Ghidra 4 byte variable (local_ / uVar / param_ ...): value arithmetic `((uint)x >> 16 & 0xffff)`; lvalue `*(undefined1 *)&x = v` (3 byte store: mask and or)
#   floatbits  `lhs = (RwReal)u;` with u an `undefined4` (or element / deref of an `undefined4 *`): Ghidra's cast is a bit copy, C would convert: `*(undefined4 *)&(lhs) = u;`
#   extptr     `extern undefined4 g;` that the body indexes / dereferences (`g[i]`, `*g`): declared `undefined4 *g` for that chunk
#   locals     `_local_5c` (Ghidra's name for an overlapping stack slot) without declaration: `undefined4 _local_5c;` at the top of the function
FIXSET = set(os.environ.get("AC_FIX", "").split(",")) - {""}
if "all" in FIXSET: FIXSET = {"subword", "floatbits", "extptr", "locals"}
_GV = r"(?:local_|_local_|uVar|iVar|param_|uStack_|in_|stack)\w*"
def _sub_rv(m, b):
    x, o, sz = m.group(1), int(m.group(2)), int(m.group(3))
    if sz == 4: return x
    mask = {1: "0xff", 2: "0xffff", 3: "0xffffff"}[sz]
    return "((uint)%s >> %d & %s)" % (x, 8 * o, mask) if o else "((uint)%s & %s)" % (x, mask)
PROTO = {}
for _l in decl_lines:
    _m = re.match(r"(?:/\*[0-9a-f]+\*/ )?extern [^;(]*?(\w+)\(([^()]*)\);", _l)
    if _m and _m.group(1) not in PROTO: PROTO[_m.group(1)] = [x.strip() for x in _m.group(2).split(",")]
def _argtype(g, b):
    for f, args in re.findall(r"\b(\w+)\(([^;()]*\b%s\b[^;()]*)\)" % g, b):
        for i, a in enumerate(x.strip() for x in args.split(",")):
            if a == g and f in PROTO and i < len(PROTO[f]):
                m = re.match(r"^((?:const )?\w+(?: \w+)*?) ?\*\s*\w*$", PROTO[f][i])
                if m and m.group(1) not in ("void", "undefined4", "char"): return m.group(1)
def fix2(b, ext):
    """-> (new body, {global: new type}) for the AC_FIX rules"""
    retype = {}
    if "locals" in FIXSET:
        for x in sorted(set(re.findall(r"\b_local_[0-9a-f]+\b", b))):
            if not re.search(r"\b\w[\w ]*[ \*]%s\s*[;\[,]" % x, b): b = b.replace("{\n", "{\n  undefined4 %s;\n" % x, 1)
    if "subword" in FIXSET:
        four = set(re.findall(r"\b(?:undefined4|uint|int|ulong|dword) (%s)\b" % _GV, b))
        def lv(m):
            x, o, sz, v = m.group(1), int(m.group(2)), int(m.group(3)), m.group(4)
            if x not in four: return m.group(0)
            if sz in (1, 2, 4): return "*(undefined%d *)((char *)&%s + %d) = %s;" % (sz, x, o, v)
            return "%s = (%s & 0x%08x) | ((uint)(%s) & 0xffffff);" % (x, x, 0xff000000 if o == 0 else 0xff, v) if o == 0 else m.group(0)
        b = re.sub(r"\b(%s)\._(\d+)_(\d)_ = ([^;]+);" % _GV, lv, b)
        b = re.sub(r"\b(%s)\._(\d+)_(\d)_" % _GV, lambda m: _sub_rv(m, b) if m.group(1) in four else m.group(0), b)
    if "floatbits" in FIXSET:
        u4 = set(re.findall(r"\bundefined4 (\w+)\b", b)); u4p = set(re.findall(r"\bundefined4 \*(\w+)\b", b))
        def fb(m):
            r = m.group(3); base = re.match(r"\*?(\w+)", r).group(1)
            ok = (base in u4p and ("[" in r or r.startswith("*"))) or (base in u4 and r == base)
            return "%s*(undefined4 *)&%s = %s;" % (m.group(1), m.group(2), r) if ok else m.group(0)
        b = re.sub(r"^(\s*)([^=;\n]+?) = \((?:RwReal|float)\)(\*?\w+(?:\[-?\d+\])?);", fb, b, flags=re.M)
    if "extptr" in FIXSET:
        for g in ext:
            t_ = re.search(r"\b%s (?:!=|==|=) \((\w[\w ]*?) \*\)0x0\b|\((\w[\w ]*?) \*\)%s\b(?!\s*\+)" % (g, g), b)      # the cast of the null compare / of the use names the pointee
            if t_ and re.search(r"\b%s->|\b%s ?(?:!=|==) \(" % (g, g), b): retype[g] = (t_.group(1) or t_.group(2)) + " *"
            elif re.search(r"\b%s->" % g, b) and (t2_ := _argtype(g, b)): retype[g] = t2_ + " *"      # no cast: the type of the callee parameter that receives it
            elif re.search(r"\b%s\[|(?<![\w\]])\*%s\b" % (g, g), b): retype[g] = "undefined4 *"
    return b, retype

def build(fs, path, retype=None):
    defined = {n for n, _, _ in fs}
    used = set(re.findall(r"[A-Za-z_]\w*", " ".join(b for _, _, b in fs)))
    out = ['#include "prelude.h"', '#include "ghidra_types.h"']
    seen_decl = set()
    if "unkbyte10" in used: out.append("typedef double unkbyte10;")      # Ghidra's type for an x87 value of unknown width (st0 return of the fsin helpers)
    for l in EXTRA_DECL:      # names/MFL_extern_fix.txt (tools/undeclared_fix.py)
        m = re.search(r"(\w+)(?:\[\d*\])?;$", l)
        if m and m.group(1) in used and m.group(1) not in defined and m.group(1) not in seen_decl: seen_decl.add(m.group(1)); out.append(l)
    for l in decl_lines:
        m = re.search(r"extern [^;]*?([A-Za-z_]\w*)(\(|\s*\[|;)", l)
        if not m or m.group(1) in defined or (PLAIN and m.group(1) in ADAPT) or m.group(1) not in used or m.group(1) in seen_decl:      # a name at several addresses (`start`, `default`) is declared once: a second declaration with another type is error C2371
            continue
        seen_decl.add(m.group(1))
        out.append(re.sub(r"extern (?:undefined4|undefined|int|uint) (\w+);", lambda mm: "extern %s%s;" % (retype[mm.group(1)], mm.group(1)), l) if retype and (m2_ := re.search(r"extern (?:undefined4|undefined|int|uint) (\w+);", l)) and m2_.group(1) in retype else l)
    lines_of = {}
    for k in sorted(v for v in X87_NAME.values() if X87 and re.search(r"\b%s\(" % v, " ".join(b for _, _, b in fs))): out.append("extern " + X87_PROTO[k])
    if PLAIN:
        for n_ in sorted(n_ for n_ in ADAPT if n_ + "_r" in used and n_ not in defined):
            out.append("static %s;" % bg.proto_of(ADAPT[n_], n_ + "_r")); out.append("extern void %s(void);" % n_); out.append(bg.gen_adapter(ADAPT[n_]))
    for n, a, b in fs:
        out.append("/* %s */" % n)
        start = len(out) + 1
        out.extend(b.split("\n"))
        lines_of[n] = (start, len(out))
    # line numbers are 1 based in the compiler output; we recompute by position after joining
    src = "\n".join(out) + "\n"
    from fix_decls import fix      # `extern undefined f();` would truncate every call result to a byte
    src = fix(src)
    open(path, "w").write(src)
    spans = {}
    ln = 0
    pos = 0
    cur = None
    for i, l in enumerate(src.split("\n"), 1):
        mm = re.match(r"/\* (\S+) \*/$", l)
        if mm:
            cur = mm.group(1)
        if cur:
            spans.setdefault(cur, [i, i])[1] = i
    return spans


def compile_chunk(fs, tag, retype=None):
    path = os.path.join(work, tag + ".c")
    spans = build(fs, path, retype)
    for _try in range(4):      # several cl processes in one Wine prefix can die silently (no output at all): a run counts only when cl echoed the file name
        r = subprocess.run([os.path.join(HERE, "vc7.sh"), work, "cl.exe", "/nologo", "/c", "/Zs", "/W0", os.path.basename(path)], capture_output=True, text=True)
        if os.path.basename(path) in (r.stdout + r.stderr): break
        time.sleep(1 + _try)
    errs = {}
    for l in (r.stdout + r.stderr).split("\n"):
        m = re.match(r"\S+\((\d+)\) : (?:fatal )?error (\w+): (.*)", l)
        if not m:
            continue
        ln = int(m.group(1))
        for n, (a, b) in spans.items():
            if a <= ln <= b:
                errs.setdefault(n, m.group(2) + " " + m.group(3))
                break
    return errs, "C1003" in (r.stdout + r.stderr)


ok, failed = [], {}
queue = [funcs[i:i + chunk] for i in range(0, len(funcs), chunk)]
ci = 0
# the first compile of every initial chunk does not depend on the others: run them in parallel (one container, several wine cl processes); retries after a failure stay sequential
from concurrent.futures import ThreadPoolExecutor
memo = {}
with ThreadPoolExecutor(int(os.environ.get("AC_JOBS", "4"))) as ex_:
    futs = {tuple(f[0] for f in fs_): ex_.submit(compile_chunk, fs_, "pchunk%d" % i_) for i_, fs_ in enumerate(queue)}
    for k_, fu_ in futs.items(): memo[k_] = fu_.result()
while queue:
    fs = queue.pop(0)
    ci += 1
    key_ = tuple(f[0] for f in fs)
    errs, toomany = memo.pop(key_) if key_ in memo else compile_chunk(fs, "chunk")
    if errs:
        for n, e in errs.items():
            failed[n] = e
        rest = [f for f in fs if f[0] not in errs]
        if toomany and len(rest) == len(fs):
            half = len(fs) // 2
            queue = [fs[:half], fs[half:]] + queue
            continue
        if len(rest) == len(fs):      # errors outside any function (declaration clashes): bisect the chunk instead of dropping all of it (90 functions were lost that way)
            if len(fs) > 1:
                half = len(fs) // 2
                queue = [fs[:half], fs[half:]] + queue
                continue
            failed[fs[0][0]] = "unattributed error"
            rest = []
        if rest:
            queue.insert(0, rest)    # recompile without the failures (more errors may be hidden behind the 100 limit)
    else:
        ok.extend(fs)
        build(fs, os.path.join(work, "ok_%03d.c" % ci))     # each passing chunk is its own TU (a merged TU hits cross-chunk prototype clashes)
    print("chunk", ci, "ok", len(ok), "failed", len(failed), "queue", len(queue), file=sys.stderr)
rescued = []
if FIXSET and failed:
    byname = {n_: (n_, a_, b_) for n_, a_, b_ in funcs_all}
    ext_ = {m_.group(1) for l_ in decl_lines if (m_ := re.match(r"(?:/\*[0-9a-f]+\*/ )?extern (?:undefined4|undefined|int|uint) (\w+);", l_))}
    jobs_ = {}
    for n_ in failed:
        if n_ not in byname: continue
        nb_, rt_ = fix2(byname[n_][2], ext_)
        if nb_ != byname[n_][2] or rt_: jobs_[n_] = ((n_, byname[n_][1], nb_), rt_)
    def _try(n_):
        f_, rt_ = jobs_[n_]
        e_, _t = compile_chunk([f_], "fix2_%s" % re.sub(r"\W", "_", n_)[:40], rt_)
        return n_, f_, rt_, e_
    with ThreadPoolExecutor(int(os.environ.get("AC_JOBS", "4"))) as ex_:
        for n_, f_, rt_, e_ in ex_.map(_try, list(jobs_)):
            if not e_:
                rescued.append(n_); ok.append(f_); del failed[n_]
                build([f_], os.path.join(work, "ok_fix2_%s.c" % re.sub(r"\W", "_", n_)[:40]), rt_)
    print("AC_FIX %s: %d retried, %d rescued" % (",".join(sorted(FIXSET)), len(jobs_), len(rescued)), file=sys.stderr)
    open(os.path.join(work, "rescued.txt"), "w").write("".join(n_ + "\n" for n_ in rescued))
build([f_ for f_ in ok if f_[0] not in set(rescued)], os.path.join(work, "ok.c"))      # rescued functions live in their own ok_fix2_*.c (retyped externs)
with open(os.path.join(work, "addrs.txt"), "w") as f_:        # name -> original address from the named decompilation headers (a name can exist at two addresses: automatch must compare against the right one)
    for f in ok: f_.write("%s %08x\n" % (f[0], f[1]))
with open(os.path.join(work, "failed.txt"), "w") as f:
    for n, e in sorted(failed.items()):
        f.write("%s\t%s\n" % (n, e))
print("compiling %d / failed %d / register or label skipped %d" % (len(ok), len(failed), 0))
