"""t2_mwfix.py: error driven text fixes for t2_auto compile errors (called like t2_stfix.fix(path, err) -> number of edits).
 fnptr   C2064 `term does not evaluate to a function` at `(*g_x)(..)`: Ghidra declared the function pointer global as `undefined4 *g_x` / `int g_x` -> `int (__cdecl *g_x)()` (same 4 byte object;
         K&R `()` takes any arguments, cdecl is the default guess, diffemu's stack delta check catches a stdcall target)"""
import re
def fix(path, err):
    t = open(path, errors="replace").read(); n = 0
    ls = t.split("\n")
    for m in re.finditer(r"f\.c\((\d+)\) : error C2064", err):
        L = int(m.group(1))
        for g in set(re.findall(r"\(\*(\w+)\)\s*\(", "\n".join(ls[max(0, L - 2):L + 1]))):
            t2 = re.sub(r"^((?:/\*[0-9a-f]{8}\*/ )?extern) (?:undefined4 \*|int |uint |undefined4 )%s;" % g, r"\1 int (__cdecl *%s)();" % g, t, flags=re.M)
            if t2 != t: t = t2; n += 1
    if n: open(path, "w").write(t)
    return n

def gbitcopy(t, nm):
    """kind `gbitcopy` (REAL): a dword copied to / from an `extern float G;` scalar global.  Ghidra types the global float; the compiler then converts (fild / fistp) where the original moves the bits.
    Decided like t2_postfix.bitcopy from the ORIGINAL's conversion counts (orig_conv: fistp+__ftol count, fild count):
    (a) `G = <int expr>;` (a `param_N` / int local / `*(undefined4 *)X` / `(float)v`-free dword) and the original has no fild -> `*(int *)&G = e;` (`*(undefined4 *)X` -> `*(float *)X`)
    (b) `<dword lvalue> = G;` and the original has no fistp / __ftol -> `<lvalue> = *(int *)&G;`   -> (text, number of edits)"""
    import t2_postfix as PF
    oc = PF.orig_conv(nm)
    if oc is None: return t, 0
    nf2i, ni2f = oc
    body = re.sub(r"/\*.*?\*/", "", t, flags=re.S)
    fg = set(re.findall(r"^(?:/\*[0-9a-f]{8}\*/ )?extern float (\w+);", t, re.M))
    if not fg: return t, 0
    ints = set(re.findall(r"\b(?:undefined4|int|uint)\s+(\w+)\s*[;,)=]", body))
    dptr = set(re.findall(r"\b(?:undefined4|int|uint)\s*\*\s*(\w+)", body))
    n = 0
    def a_(m):
        nonlocal n
        g, e = m.group(2), m.group(3).strip()
        if g not in fg: return m.group(0)
        d = re.fullmatch(r"\*\((?:undefined4|int|uint) \*\)(.+)", e)
        if d: n += 1; return "%s%s = *(float *)%s;" % (m.group(1), g, d.group(1))
        if re.fullmatch(r"\w+", e) and e in ints: n += 1; return "%s*(int *)&%s = %s;" % (m.group(1), g, e)
        return m.group(0)
    def b_(m):
        nonlocal n
        lv, g = m.group(2), m.group(3)
        if g not in fg: return m.group(0)
        base = re.match(r"\*?\(?\(?\w+", lv)
        ok = re.fullmatch(r"\*(\w+)|(\w+)\[[^\]\n]+\]|\*\((?:undefined4|int|uint) \*\)\(.+\)", lv) and (re.fullmatch(r"\*(\w+)", lv) and re.fullmatch(r"\*(\w+)", lv).group(1) in dptr or re.match(r"(\w+)\[", lv) and re.match(r"(\w+)\[", lv).group(1) in dptr or lv.startswith("*("))
        if ok: n += 1; return "%s%s = *(int *)&%s;" % (m.group(1), lv, g)
        return m.group(0)
    if ni2f == 0: t = re.sub(r"^([ \t]*)(\w+) = ([^;\n]+);", a_, t, flags=re.M)
    if nf2i == 0: t = re.sub(r"^([ \t]*)([^=\n;]+?) = (\w+);", b_, t, flags=re.M)
    return t, n

def stepvar(t):
    """the one pointer variable the text steps through a loop (`piVar4 = piVar4 + 9;`): the opaque register of a recovered x87 operand `[edi-0x14]` (t2_postfix.ftol retry); None when there is no or more than one"""
    vs = {m.group(1) for m in re.finditer(r"^[ \t]*(\w+) = \1 \+ -?\d+;", t, re.M) if re.search(r"^\s+[\w ]+\*\s*%s;" % m.group(1), t, re.M)}
    return vs.pop() if len(vs) == 1 else None

def _uns(nm):
    """True when the ORIGINAL converts an unsigned int to float (fild + fadd/fsub of 2^32), False when it has fild and no such constant, None unknown"""
    import struct, t2_postfix as PF
    try:
        import core
        PF.orig_const_args(nm, set())
        if nm not in PF._FN: return None
        lo, sz = PF._FN[nm]; fild = False
        for x in core.CS.disasm(core.img_read(lo, sz), lo):
            if x.mnemonic == "fild": fild = True
            m = re.fullmatch(r"dword ptr \[0x([0-9a-f]+)\]", x.op_str)
            if m and x.mnemonic in ("fadd", "fsub", "fsubr", "fmul", "fld") and struct.unpack("<f", core.img_read(int(m.group(1), 16), 4))[0] == 4294967296.0: return True
        return False if fild else None
    except Exception: return None

def u2f(t, nm):
    """kind `u2f` (REAL): `(float10)g` / `(float)v` of an `undefined4` / `uint` object converts UNSIGNED (the compiler adds 2^32 for the sign); the original has fild and no 2^32 constant = a signed int:
    `(float)(int)v`.  -> (text, number of edits)"""
    if _uns(nm) is not False: return t, 0
    body = re.sub(r"/\*.*?\*/", "", t, flags=re.S)
    un = set(re.findall(r"\b(?:undefined4|uint)\s+(\w+)\s*[;,)=\[]", body)) | set(re.findall(r"^extern (?:undefined4|uint) (\w+);", t, re.M))
    un -= set(re.findall(r"\b(?:undefined4|uint)\s*\*\s*(\w+)", body))
    if not un: return t, 0
    n = [0]
    def r(m):
        if m.group(2) not in un: return m.group(0)
        n[0] += 1; return "(%s)(int)%s" % (m.group(1), m.group(2))
    t2 = re.sub(r"\((float10|float|double)\)(\w+)\b(?!\s*[\[(.])", r, t)
    return t2, n[0]

def ptrtab(t):
    """kind `ptrtab` (REAL): `(&PTR_s_x_0062ad3c)[i]` = element i of a table of pointers (Ghidra names pointer data PTR_...): declared `extern char PTR_x[];` by undeclared_fix, which makes `(&PTR_x)[i]` an array of
    unknown size -> `extern char *PTR_x[];` and `PTR_x[i]`.  -> (text, number of tables)"""
    ns = set(re.findall(r"\(&(PTR_\w+)\)\[", t)); n = 0
    for x in ns:
        t2 = re.sub(r"extern char %s\[\];" % x, "extern char *%s[];" % x, t)
        if t2 == t: continue
        t = t2.replace("(&%s)[" % x, "%s[" % x); n += 1
    return t, n
