#!/usr/bin/env python3
"""defectlint.py [--ref REV] [--all] [files...]: lightweight C lint for the defect classes the differential emulator (tools/diffemu) found in decompiler output.
Scope (default): src/{automatch_near,automatch_agent,tier1,bridges}, output/tier2_candidates, output/usercall_candidates*.  Output: output/defectlint.csv + summary.
  1 signed    `<` `>` `<=` `>=` where one operand is an `undefined4` object (compiles UNSIGNED) and the other a literal / signed object, without an `(int)` cast
  2 floatbits int literal bit pattern (>= 7 hex digits) assigned to a float object; `(float)` of an undefined4/int dword element (converts, original copies bits);
              float value stored into an undefined4/int dword element (`(int)expr`, `(float)expr`, float local) instead of `*(float *)&x = ..`
  3 ftol      `_ftol()` / `_ftol2()` without its ST0 argument
  4 proto     call with a different argument count than its prototype; one address declared with different parameter counts in different files (or against output/cdecl_args.txt /
              output/stdcall.txt); float argument passed to an integer parameter (compiler emits _ftol) or to a `()` declaration (pushed as double)
  6 vtable_conv  `(**(code **)(*p + N))(args)` / `(*(T (__stdcall **)(..))(*p + N))(args)` where the ORIGINAL caller (output/call_indirect.txt, objdump of the function: `call [reg+N]`, no `add esp` / `pop` cleanup on the path to the next `ret`) shows the method pops its own arguments (__stdcall COM / DirectX style method), but the C call is cdecl (`code *`), or __stdcall with fewer arguments than the original pushes
  9 framelayout  `&local_N` (Ghidra stack slot, named by its offset) passed to a call / stored while another local within 0x10 bytes (local_N+-4, +-8, ..) is also used, or `&local_N` indexed / offset (`(&local_N)[1]`, `&local_N + 1`, `(char *)&local_N + k`, `p = &local_N; p[1]`): the original had ONE frame object whose members a callee reads past the passed address (clump_has_uv_anim: {flag, callback, flag ptr}), separate C locals are reordered by the compiler; fix = a struct local
  10 defarity  parameters the C definition declares < stack argument slots the ORIGINAL reads / rewrites ([esp+4+4k] / [ebp+8+4k], `ret N`, tools/origargs.py -> output/orig_stackargs.txt), < the arguments of the function it tail-jumps to, < what its callers push; a tail call in the C body with fewer arguments than the jump target takes (player_equip_weapon_by_index: `void f(void){ g(); }` for a thunk that rewrites [esp+4] and jmps)
  7 regcallee  call of a function with register inputs (output/regsigs.csv, joined by name or by `/*addr*/` of the extern) by its plain name: it is compiled as a cdecl call (needs the F_r adapter or the cluster route); the summary lists the callers per callee
Class 5 (x87 precision of a float local) is not lintable.  tier 1 / agent sources are byte identical (check_all manifest + relocation check), so classes 1-3 cannot hide there:
they are listed (column `byte_identical`) but a byte identical function never counts as an open finding.  --ref REV lints the files as of a git revision (tuning on the pre-fix text).
Exit 0 always (warning tool)."""
import csv, glob, os, re, subprocess, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, ".."))
SCOPE = ["src/automatch_near", "src/automatch_agent", "src/tier1", "src/bridges", "output/tier2_candidates", "output/tier2_candidates2", "output/usercall_candidates*"]
INT_T = {"int", "short", "long", "char", "signed", "ushort_s", "int8_t", "int16_t", "int32_t", "SHORT", "LONG", "INT", "BOOL", "RwInt32", "RwBool", "RwInt16"}
UNS_T = {"uint", "ulong", "ushort", "uchar", "byte", "DWORD", "UINT", "WORD", "BYTE", "ULONG", "size_t", "unsigned", "RwUInt32", "RwUInt16", "undefined1", "undefined2", "dword", "word"}
FLT_T = {"float", "double", "float10", "RwReal", "FLOAT", "longdouble"}
KW = {"if", "while", "for", "return", "else", "switch", "sizeof", "do", "case", "goto"}


def exact_slots(va, k):
    """the ORIGINAL body reads exactly k stack argument slots (orig_stackargs.txt row without tail / addr / dyn / ijmp / w<N> flags): fewer pending pushes than the callers' minimum are then batched `add esp` cleanups, not missing arguments (2026-10-16)"""
    o = ORIG.get(va)
    return bool(o) and o[0] == k and not o[2] and not (o[3] & {"dyn", "addr", "ijmp"}) and not any(re.match(r"w\d", x) for x in o[3])
def strip_comments(t):
    def blank(m): return re.sub(r"[^\n]", " ", m.group(0))
    t = re.sub(r"/\*.*?\*/", blank, t, flags=re.S)
    t = re.sub(r"//[^\n]*", blank, t)
    return re.sub(r"'(?:\\.|[^'\\])+'|\"(?:\\.|[^\"\\])*\"", lambda m: re.sub(r"[^\n]", "x", m.group(0)), t)    # char / string literals: content cannot matter


def base_type(decl):
    """'extern __cdecl undefined4 *' -> 'undefined4 *'"""
    d = re.sub(r"\b(extern|static|const|struct|union|enum|__cdecl|__stdcall|__fastcall|__thiscall|__declspec\([^)]*\)|unsigned|signed)\b", lambda m: "unsigned_" if m.group(1) == "unsigned" else "", decl)
    d = d.replace("unsigned_", "uint ") if "unsigned_" in d else d
    stars = d.count("*")
    w = [x for x in re.findall(r"\w+", d)]
    return (w[-1] if w else "?") + (" *" * stars)


DECL_RE = re.compile(r"^\s*(?:/\*[0-9a-fA-F]{8}\*/\s*)?(?:extern\s+)?((?:(?:const|struct|unsigned|signed|undefined\d?|[A-Za-z_]\w*)\s+)*[A-Za-z_]\w*(?:\s*\*+\s*|\s+))([A-Za-z_]\w*)\s*((?:\[[^\]]*\])*)\s*(?:=[^;]*)?;")


def load_decls():
    """globals of output/decls.h: name -> type"""
    g = {}
    p = os.path.join(G, "output", "decls.h")
    for l in open(p, errors="replace"):
        if "(" in l: continue
        m = DECL_RE.match(l)
        if m and "extern" in l: g[m.group(2)] = base_type(m.group(1))
    return g


def split_args(s):
    out, d, cur = [], 0, ""
    for c in s:
        if c in "([": d += 1
        elif c in ")]": d -= 1
        if c == "," and d == 0: out.append(cur); cur = ""
        else: cur += c
    if cur.strip(): out.append(cur)
    return out


def balanced_after(t, i):
    """index of the ')' matching the '(' at t[i]"""
    d = 0
    for j in range(i, len(t)):
        if t[j] == "(": d += 1
        elif t[j] == ")":
            d -= 1
            if d == 0: return j
    return -1


class Lint:
    def __init__(self, path, text, gdecl):
        self.path, self.raw = path, text
        self.t = strip_comments(text)
        self.lines = self.t.split("\n")
        self.gdecl = gdecl
        self.glob = dict(gdecl)           # file externs override decls.h
        self.protos = {}                  # name -> (nparams or None for (), [types], va)
        self.finds = []
        self.hints = []                   # mechanical fixes for tools/t2_postfix.py: (kind, line, symbol / function, [argument texts])
        self.parse_top()

    # ---- declarations
    def parse_top(self):
        raw = self.raw.split("\n")
        for i, l in enumerate(self.t.split("\n")):
            if raw[i].startswith((" ", "\t", "#")) or "extern" not in l: continue
            m = re.match(r"\s*extern\s+(?:__declspec\([^)]*\)\s*)?(.*?)\b([A-Za-z_]\w*)\s*\((.*)\)\s*;", l)
            if m:
                ps = [p.strip() for p in split_args(m.group(3))]
                va = re.search(r"/\*([0-9a-fA-F]{8})\*/", self.raw.split("\n")[i])
                if not ps or ps == [""]: n = None
                elif ps == ["void"]: n = 0
                else: n = len(ps)
                self.protos[m.group(2)] = (n, ps if n else [], va.group(1).lower() if va else None, base_type(m.group(1)), i + 1)
                continue
            m = DECL_RE.match(l)
            if m: self.glob[m.group(2)] = base_type(m.group(1))

    def functions(self):
        """yield (name, params-string, body_start_line_index, body_end_line_index)"""
        L = self.lines; i = 0
        while i < len(L):
            l = L[i]
            if l and not l[0].isspace() and not l.startswith(("#", "}", "extern", "typedef", "struct", "union", "enum")) and "(" in l and not l.rstrip().endswith(";"):
                hdr, j = l, i
                while "{" not in "".join(L[j + 1:j + 2]).strip()[:1] and j + 1 < len(L) and ")" not in hdr: j += 1; hdr += L[j]
                k = j + 1
                while k < len(L) - 1 and not L[k].strip() and "{" not in hdr: k += 1       # Ghidra output has a blank line between the header and `{`
                if k < len(L) and L[k].strip() == "{" or l.rstrip().endswith("{"):
                    s = k if L[k].strip() == "{" else j
                    d, e = 0, s
                    while e < len(L):
                        d += L[e].count("{") - L[e].count("}")
                        if d == 0 and e > s - 1 and ("{" in "".join(L[s:e + 1])): break
                        e += 1
                    m = re.match(r"\s*(?:[\w\s\*]*?)\b(\w+)\s*\((.*)\)\s*(?:\{)?\s*$", re.sub(r"__declspec\([^)]*\)|__naked", "", hdr))
                    if m: yield m.group(1), m.group(2), s, e
                    i = e
            i += 1

    def local_env(self, params, s, e):
        env = {}
        for p in split_args(params):
            m = re.match(r"\s*(.*?)(\w+)\s*(?:\[[^\]]*\])?\s*$", p)
            if m and m.group(1).strip() and m.group(1).strip() != "void": env[m.group(2)] = base_type(m.group(1))
        for l in self.lines[s:e + 1]:
            m = re.match(r"^\s+((?:[A-Za-z_]\w*\s+)*[A-Za-z_]\w*)(\s*\*+\s*|\s+)(\*?\s*[A-Za-z_]\w*(?:\s*(?:\[[^\]]*\])*\s*,\s*\*?\s*[A-Za-z_]\w*)*)\s*(?:\[[^\]]*\])*\s*;", l)
            if m and not re.match(r"^\s*(return|goto|else|extern)\b", l):
                base = base_type(m.group(1)); st0 = m.group(2).count("*")
                for v in m.group(3).split(","):
                    v = re.sub(r"\[[^\]]*\]", "", v); st = v.count("*") + st0; n = v.replace("*", "").strip()
                    env[n] = base.split(" *")[0] + (" *" * (st + base.count("*")))
        return env

    # ---- typing
    def tkind(self, ty):
        if not ty: return "?"
        b = ty.replace("*", "").strip()
        if "*" in ty: return "P"
        if b == "undefined4": return "U"
        if b in UNS_T: return "u"
        if b in INT_T: return "S"
        if b in FLT_T: return "F"
        return "?"

    _FM = None
    def float_members(self):
        """member names that are float / double / RwReal in EVERY struct of src/auto/ghidra_types.h that has a member of that name (scalar members only)"""
        if Lint._FM is None:
            fl, other = set(), set()
            for l in open(os.path.join(G, "src", "auto", "ghidra_types.h"), errors="replace"):
                m = re.match(r"^\s+(?:const\s+)?([A-Za-z_]\w*(?:\s*\*+)?)\s+\**(\w+)(\[[^\]]*\])?;", l)
                if not m: continue
                (fl if m.group(1) in FLT_T else other).add(m.group(2) + ("[]" if m.group(3) else ""))
            Lint._FM = fl - other - {x.rstrip("[]") for x in other}
        return Lint._FM
    def fexpr(self, a, env):
        """True when `a` is a float VALUE built from float operands: `g_tab[i].radius`, `p->x * k_f_0_1`, `a + b` (every operand a float atom / float struct member / literal, at least one float)"""
        a = a.strip()
        while a.startswith("(") and a.endswith(")") and balanced_after(a, 0) == len(a) - 1: a = a[1:-1].strip()
        parts, d, cur = [], 0, ""
        for c in a:
            if c in "([": d += 1
            elif c in ")]": d -= 1
            if d == 0 and c in "*/+-" and cur.strip(): parts.append(cur); cur = ""; continue
            cur += c
        parts.append(cur); fl = False
        for q in parts:
            q = q.strip()
            if not q: return False
            m = re.search(r"(?:\.|->)(\w+)(\[[^\]]*\])?$", q)
            if m and re.match(r"^[\w\[\]\.\->]+$", q) and m.group(1) + ("[]" if m.group(2) else "") in self.float_members(): fl = True; continue
            k, n = self.atom(q, env)
            if k == "F" and n != "(cast)": fl = True
            elif k != "L": return False
        return fl
    def atom(self, s, env):
        """-> (kind, name) of a simple operand; kind in L literal, U undefined4 object, u unsigned, S signed, F float, P pointer, ? unknown, C cast-protected"""
        s = s.strip()
        while s.startswith("(") and s.endswith(")") and balanced_after(s, 0) == len(s) - 1: s = s[1:-1].strip()
        m = re.match(r"^\(\s*((?:unsigned\s+|signed\s+)?[A-Za-z_]\w*)\s*\)\s*(.*)$", s)
        if m and m.group(1) in {"int", "short", "long", "char", "uint", "ushort", "ulong", "byte", "float", "double", "unsigned", "float10"} | INT_T | UNS_T | FLT_T:
            return ("S" if m.group(1) in INT_T else "u" if m.group(1) in UNS_T else "F"), "(cast)"
        if re.match(r"^-?(0[xX][0-9a-fA-F]+|\d+)[uUlL]*$", s): return "L", s
        if re.match(r"^-?(\d+\.\d*|\.\d+|\d+)([eE][-+]?\d+)?[fF]?$", s): return "F", s
        m = re.match(r"^\(\s*&\s*(\w+)\s*\)\s*\[.*\]$", s)                        # (&g_x)[i]
        if m: return self.tkind(env.get(m.group(1)) or self.glob.get(m.group(1))), m.group(1)
        m = re.match(r"^(\w+)\s*\[.*\]$", s)                                       # p[i]
        if m:
            ty = env.get(m.group(1)) or self.glob.get(m.group(1))
            if ty and "*" in ty: return self.tkind(ty.rsplit(" *", 1)[0] if " *" in ty else ty.replace("*", "")), m.group(1) + "[]"
            return self.tkind(ty), m.group(1) + "[]"
        m = re.match(r"^\*\s*(\w+)$", s)
        if m:
            ty = env.get(m.group(1)) or self.glob.get(m.group(1))
            if ty and "*" in ty: return self.tkind(ty.rsplit(" *", 1)[0] if " *" in ty else ty.replace("*", "")), "*" + m.group(1)
            return "?", s
        if re.match(r"^\w+$", s):
            ty = env.get(s) or self.glob.get(s)
            return self.tkind(ty), s
        return "?", s

    # ---- statements
    def statements(self, s, e):
        """(line number, text) of statements and control headers in lines s..e (0 based, inclusive)"""
        out, cur, start = [], "", None
        for i in range(s, e + 1):
            for c in self.lines[i] + "\n":
                if c in ";{}":
                    if cur.strip(): out.append((start, cur.strip()))
                    cur, start = "", None
                else:
                    if start is None and not c.isspace(): start = i + 1
                    cur += c
        if cur.strip(): out.append((start, cur.strip()))
        return out

    def add(self, cls, sev, line, fn, sym, text, note):
        self.finds.append(dict(cls=cls, sev=sev, file=self.path, line=line, function=fn, symbol=sym, text=" ".join(text.split())[:140], note=note))

    def operand(self, text, i, direction):
        """text of the operand left (-1) / right (+1) of the operator at text[i]"""
        d = 0
        if direction < 0:
            j = i - 1
            while j >= 0:
                c = text[j]
                if c in ")]": d += 1
                elif c in "([":
                    if d == 0: break
                    d -= 1
                elif d == 0 and (c in ",?:;=" and not (c == "=" and j and text[j - 1] in "<>=!")  or text[j - 1:j + 1] in ("&&", "||")): break
                j -= 1
            return text[j + 1:i].rstrip("&|").strip()
        j = i
        while j < len(text):
            c = text[j]
            if c in "([": d += 1
            elif c in ")]":
                if d == 0: break
                d -= 1
            elif d == 0 and (c in ",?:;" or text[j:j + 2] in ("&&", "||", "==", "!=")): break
            j += 1
        return text[i:j].strip()

    def check_stmt(self, fn, line, st, env):
        # class 3
        if re.search(r"\b_ftol2?(?:_sse)?\s*\(\s*\)", st):
            self.add(3, "high", line, fn, "_ftol", st, "_ftol() without its ST0 argument (decompiler artefact, the value comes from an empty x87 stack)")
        # class 1
        for m in re.finditer(r"(?<![<>\-=!])(<=|>=|<|>)(?![<>=])", st):
            if re.match(r"\s*(if|while|for|return|else)?\b", st) is None: pass
            a, b = self.operand(st, m.start(), -1), self.operand(st, m.end(), 1)
            if not a or not b: continue
            ka, na = self.atom(a, env); kb, nb = self.atom(b, env)
            if (ka == "U" and kb in "LS") or (kb == "U" and ka in "LS"):
                u = na if ka == "U" else nb
                where = "local" if u.split("[")[0].lstrip("*") in env else "global"
                self.hints.append(("sgn", line, u, []))
                self.add(1, "high" if where == "global" else "medium", line, fn, u, st, "%s `%s` is undefined4 (unsigned compare); the original may be signed: needs (int)" % (where, u))
        # class 2
        for m in re.finditer(r"(?<![\w.])(\d+\.\d*(?:[eE]-?\d+)|\d+[eE]-\d+)[fF]?\b", st):
            if 0 < abs(float(m.group(1))) < 1e-30: self.add(2, "high", line, fn, m.group(1), st, "denormal float literal %s: an integer bit pattern (counter / flags) read as a float" % m.group(1))
        st = re.sub(r"^LAB_\w+:\s*", "", st)      # a statement behind a `LAB_x:` label (goto target)
        m = re.match(r"^(?:\w+\s+)?(.+?)\s*=\s*(.+)$", st, flags=re.S)
        if m and "==" not in st[:st.find("=") + 2] and not re.match(r"^(if|while|for|return)\b", st):
            lv, rhs = m.group(1).strip(), m.group(2).strip()
            lv = re.sub(r"^(?:const\s+|register\s+)*(?:float|int|undefined4|uint|double)\s+", "", lv)
            kl, nl = self.atom(lv, env) if not lv.startswith("*(float") else ("F", lv)
            kr, nr = self.atom(rhs, env)
            if kl == "F" and kr == "L" and re.match(r"^0[xX][0-9a-fA-F]{7,8}$", rhs.strip("uUlL")) and int(rhs.rstrip("uUlL"), 16) >= 0x1000000:
                self.hints.append(("flit", line, lv, [rhs.rstrip("uUlL")])); self.add(2, "high", line, fn, nl, st, "int literal %s assigned to float `%s`: converts, does not copy the bit pattern" % (rhs, nl))
            elif kl in "UP?S" and not lv.startswith("*(float") and re.search(r"\[", lv) and kl in "US":
                ifloat = re.match(r"^\(\s*(?:float|double)\s*\)", rhs) or (kr == "F" and nr != "(cast)" or kr == "?" and self.fexpr(rhs, env)) and not re.match(r"^-?\d+$", rhs) \
                    or re.search(r"\(\s*int\s*\)\s*\(?.*?(\(\s*float\s*\)|\bf[A-Z]?Var\d|\bfVar|\bRVar)", rhs)
                if ifloat and not re.match(r"^\(\s*(int|uint)\s*\)\s*[\w\.]+$", rhs): self.hints.append(("fstore", line, lv, [rhs])); self.add(2, "high", line, fn, nl, st, "float value stored into the dword object `%s` by conversion (use *(float *)&...)" % nl)
                elif re.match(r"^\(\s*int\s*\)\s*\w+(\.\w+)*\.[xyz]\s*$", rhs) or (re.match(r"^\(\s*int\s*\)\s*(\w+(\.\w+)*)\s*$", rhs) and kr == "?" and self.is_float_name(rhs, env)): self.hints.append(("fstore", line, lv, [rhs])); self.add(2, "high", line, fn, nl, st, "float stored into the dword object `%s` by conversion" % nl)
        for m in re.finditer(r"\(\s*(?:float|RwReal)\s*\)\s*((?:\(\s*&\s*\w+\s*\)|[\w.]+)\s*\[[^\]]*\]|\w+)", st):
            k, n = self.atom(m.group(1), env)
            if k in "US" and "[" in m.group(1) and not re.search(r"\[\s*[a-z]?Var\d*\s*\]|\[\s*(i|j|k)\s*\]", m.group(1)) or k == "U" and "[" in m.group(1): self.hints.append(("fcast", line, m.group(0), [m.group(1)])); self.add(2, "medium", line, fn, n, st, "(float) of the dword element %s converts (fild); a float kept in a dword object needs *(float *)&..." % m.group(1))
            elif k == "U" and n in self.glob and self.glob[n] == "undefined4" and n not in env and "[" not in m.group(1) and not re.search(r"\(\s*float\s*\)\s*" + re.escape(n) + r"\s*[\*/]", st) and False: pass
        # class 4
        for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", st):
            f = m.group(1)
            if f in KW or f not in self.protos or re.search(r"printf", f) or st[:m.start()].rstrip().endswith(("extern",)): continue
            if re.match(r"^\s*(?:extern\b)", st): continue
            n, ptypes = self.protos[f][0], self.protos[f][1]
            j = balanced_after(st, m.end() - 1)
            if j < 0: continue
            args = split_args(st[m.end():j])
            if n is None:
                fa = [a for a in args if self.atom(a, env)[0] == "F" and self.atom(a, env)[1] != "(cast)" or re.match(r"^\s*\(\s*float\s*\)", a) or (os.environ.get("DL_FEXPR") and not re.fullmatch(r"\s*[\w.]+\s*", a) and self.fexpr(a, env))]      # float typed arithmetic (`g + k_f_80`) too: ONLY when DL_FEXPR is set (t2_postfix.retry): installed code passes such expressions to K&R callees that really take a double (sound_volume_to_directsound_db)
                if fa:
                    self.hints.append(("fbits", line, f, [a.strip() for a in fa]))
                    self.add(4, "high", line, fn, f, st, "float argument passed to `%s()` (no prototype): pushed as double" % f)
                continue
            if len(args) != n and not (n == 0 and not args):
                if len(args) > n > 0: self.hints.append(("dropextra", line, f, [n]))
                self.add(4, "high", line, fn, f, st, "%s called with %d argument(s), prototype has %d" % (f, len(args), n))
                continue
            for a, p in zip(args, ptypes):
                pk = self.tkind(base_type(re.sub(r"\w+$", "", p.strip())) if re.search(r"\s\w+$", p.strip()) else base_type(p))
                ak, an = self.atom(a, env)
                fl = (ak == "F" and an != "(cast)") or re.match(r"^\s*\(\s*float\s*\)", a) or self.fexpr(a, env)
                if fl and re.fullmatch(r"\s*[\w.]+\s*", a) and re.search(r"\b(?:float|RwReal|double)\s+%s\s*\[" % re.escape(a.strip().split(".")[-1]), self.t): fl = False      # a float ARRAY decays to a pointer: not a float argument
                if pk in "UuS" and fl and "*" not in p and not re.match(r"^\s*\(\s*(int|uint)\s*\)", a):
                    self.hints.append(("fbits", line, f, [a.strip()]))
                    self.add(4, "high", line, fn, f, st, "float argument `%s` for the integer parameter `%s` of %s: the compiler emits _ftol" % (a.strip(), p.strip(), f))

    def vtable_calls(self, st):
        """(disp, nargs, stdcall?) of the indirect `(*(cast)(expr + N))(args)` calls in a statement"""
        out = []
        for m in re.finditer(r"\*\*\s*\)", st):
            i = m.end() - 1; d = 0; o = -1
            for j in range(i, -1, -1):                         # '(' of the cast group
                if st[j] == ")": d += 1
                elif st[j] == "(":
                    d -= 1
                    if d == 0: o = j; break
            if o < 0: continue
            inner = st[o + 1:m.start() + 2]; pos = i + 1; std = "__stdcall" in inner
            if re.match(r"^\s*__\w+\s*\*\*$", inner):               # `ret (conv **)(params)`: skip the parameter list
                k = pos
                while k < len(st) and st[k].isspace(): k += 1
                if k >= len(st) or st[k] != "(": continue
                pos = balanced_after(st, k) + 1
                # the cast paren that opened before the return type closes here
                while pos < len(st) and st[pos].isspace(): pos += 1
                if pos < len(st) and st[pos] == ")": pos += 1
            while pos < len(st) and st[pos].isspace(): pos += 1
            d = 0; e = -1                                      # end of the callee expression = the ')' closing the outer `(*`
            for j in range(pos, len(st)):
                if st[j] == "(": d += 1
                elif st[j] == ")":
                    if d == 0: e = j; break
                    d -= 1
            if e < 0: continue
            ex = st[pos:e].strip()
            n = re.search(r"\+\s*(0x[0-9a-fA-F]+|\d+)\s*\)?\s*$", ex)
            if not n: continue
            a = e + 1
            while a < len(st) and st[a].isspace(): a += 1
            if a >= len(st) or st[a] != "(": continue
            b = balanced_after(st, a)
            if b < 0: continue
            out.append((int(n.group(1), 0), len(split_args(st[a + 1:b])), std))
        return out

    def check_vtable(self, fn, calls):
        """calls: [(line, st, disp, nargs, std)] of one function, in source order; paired with the original indirect calls of the same displacement"""
        va = NAMEVA.get(fn) or NAMEVA.get(fn.removesuffix("_c"))
        if not va or va not in IND: return
        for disp in {c[2] for c in calls}:
            orig = IND[va].get(disp, []); cs = [c for c in calls if c[2] == disp]
            if not orig: continue
            for i, (line, st, _, nargs, std) in enumerate(cs):
                if len(orig) == len(cs): o = orig[i]
                elif all(x == orig[0] for x in orig): o = orig[0]
                else: continue
                k, ev = o
                if ev == "pop" and nargs >= 1 and not std:
                    self.add(6, "high", line, fn, "+0x%x" % disp, st, "indirect call [+0x%x] is cdecl in C, but the original pops its arguments (no `add esp` after the call): needs `(*(int (__stdcall **)(int *))(*p + 0x%x))(p, ..)`" % (disp, disp))
                elif ev == "pop" and k >= 1 and nargs < k:
                    self.add(6, "high", line, fn, "+0x%x" % disp, st, "indirect call [+0x%x] passes %d argument(s), the original pushes %d (stdcall, callee pops): the `this` / an argument is missing" % (disp, nargs, k))
                elif ev == "clean" and std:
                    self.add(6, "medium", line, fn, "+0x%x" % disp, st, "indirect call [+0x%x] is __stdcall in C, but the original caller cleans the arguments (`add esp`): cdecl" % disp)

    def check_regcallee(self, fn, line, st):
        if re.match(r"^\s*extern\b", st): return
        for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", st):
            f = m.group(1)
            if f in KW or f == fn: continue
            va = (self.protos[f][2] if f in self.protos and self.protos[f][2] else None) or REGNAME.get(f)
            if not va:
                h = re.search(r"_([0-9a-f]{8})$", f)
                va = h.group(1) if h and h.group(1) in REGVA else None
            if va in REGVA:
                self.add(7, "high", line, fn, f, st, "`%s` takes its input in registers (%s, regsigs.csv) but is called as cdecl: needs the F_r adapter or the cluster route" % (f, REGVA[va]))

    def check_krarity(self, fn, line, st):
        """class 8: call through a `()` prototype (file extern or decls.h): argument count vs the pushes every original call site has pending (call_pushmin.txt) / cdecl_args.txt / stdcall.txt"""
        if re.match(r"^\s*extern\b", st): return
        for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", st):
            f = m.group(1)
            if f in KW or f == fn or re.search(r"printf", f): continue
            pr = self.protos.get(f)
            if pr: n, va = pr[0], pr[2]
            elif f in DPROTO: n, va = DPROTO[f]
            else: continue
            if n is not None or not va or va in REGVA: continue
            j = balanced_after(st, m.end() - 1)
            if j < 0: continue
            k = len([a for a in split_args(st[m.end():j]) if a.strip()])
            if va in GT and GT[va] != k:
                self.add(8, "high", line, fn, f, st, "`%s` is declared `()`; called with %d argument(s), the original callers pass %d (cdecl_args.txt / stdcall.txt)" % (f, k, GT[va]))
            elif va in PMALL and k < PMALL[va][0] and not exact_slots(va, k):
                self.add(8, "high" if PMALL[va][1] >= 3 else "medium", line, fn, f, st, "`%s` is declared `()`; called with %d argument(s), every original call site (%d) has >= %d pushes pending (call_pushmin.txt)" % (f, k, PMALL[va][1], PMALL[va][0]))

    def check_calleearity(self, fn, line, st):
        """class 10: a call passes fewer arguments than the stack slots the CALLEE's original body reads (output/orig_stackargs.txt; exact only: no address taken / lost esp; gfx_set_vec3_6f2aa8(x) for a 3 argument setter)"""
        if re.match(r"^\s*extern\b", st): return
        for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", st):
            f = m.group(1)
            if f in KW or f == fn or re.search(r"printf", f): continue
            pr = self.protos.get(f)
            if pr: va = pr[2]
            elif f in DPROTO: va = DPROTO[f][1]
            else: continue
            if not va or va in REGVA or va not in ORIG: continue
            sl, retn, tails, fl = ORIG[va]
            if fl & {"addr", "dyn"} or not sl: continue
            j = balanced_after(st, m.end() - 1)
            if j < 0: continue
            k = len([a for a in split_args(st[m.end():j]) if a.strip()])
            if k < sl: self.add(10, "high", line, fn, f, st, "defarity: `%s` is called with %d argument(s), its original body reads %d stack argument slot(s)" % (f, k, sl))

    def is_float_name(self, expr, env):
        n = re.sub(r"^\(\s*int\s*\)\s*", "", expr).strip().split(".")[0]
        ty = env.get(n) or self.glob.get(n)
        return bool(ty) and self.tkind(ty) == "F"

    def check_framelayout(self, fn, s, e):
        body = "\n".join(self.lines[s:e + 1])
        loc = {}
        for m in re.finditer(r"\b(?:local|[a-z]*Stack)_([0-9a-fA-F]+)\b", body): loc[m.group(0)] = int(m.group(1), 16)
        if not loc: return
        SC = re.compile(r"^\s+(?:(?:unsigned|signed|const)\s+)*(undefined\d?|u?int|short|ushort|long|ulong|char|uchar|byte|float|double|float10|code|bool|BOOL|\w+)\s*(\*+)?\s*\**\s*((?:local|[a-z]*Stack)_[0-9a-fA-F]+)\s*;", re.M)
        sty = {m.group(3): (m.group(1) if not m.group(2) else "ptr") for m in SC.finditer(body)}
        SCALAR = {"undefined", "undefined1", "undefined2", "undefined4", "undefined8", "int", "uint", "short", "ushort", "long", "ulong", "char", "uchar", "byte", "float", "double", "float10", "code", "bool", "BOOL", "ptr"}
        def line_of(i): return s + 1 + body[:i].count("\n")
        flagged = set()
        for m in re.finditer(r"&\s*((?:local|[a-z]*Stack)_[0-9a-fA-F]+)\b", body):
            x = m.group(1); pre = body[max(0, m.start() - 120):m.start()]; post = body[m.end():m.end() + 12]
            # pointer arithmetic from the address of one slot
            ar = re.match(r"\s*\)?\s*(\[\s*[1-9]|\+\s*[1-9\(a-zA-Z]|\)\s*\[\s*[1-9])", post) or re.search(r"\(\s*(?:char|byte|undefined1|uchar)\s*\*\s*\)\s*$", pre) and re.match(r"\s*\+", post)
            if ar and (x, "ar") not in flagged:
                flagged.add((x, "ar")); self.add(9, "high", line_of(m.start()), fn, x, body[max(0, m.start() - 30):m.end() + 20], "pointer arithmetic / indexing past the address of a single stack slot: the original has an array / struct there, declare it as one")
                continue
            ctx = re.search(r"([\w]+)\s*\(([^;]*)$", pre + m.group(0)) # inside a call argument list?
            stored = re.search(r"=\s*(\([^)]*\)\s*)?$", pre)
            ps_ = re.split(r"[;{}]", pre)[-1]      # parentheses of the CURRENT statement only (the 120 character window can start inside an earlier one: `...(param_1 + 0x38);` left a stray `)` and hid trigger_sphere_register(&local_8c))
            incall = bool(ctx) and ctx.group(1) not in KW and ps_.count("(") > ps_.count(")")
            if not (incall or stored) or (x, "nb") in flagged: continue
            if sty.get(x) not in SCALAR: continue          # a struct / array local (RwV3d, RwMatrix, ..): the whole object is passed, its members are contiguous by construction
            ox = loc[x]
            nb = sorted(y for y, oy in loc.items() if y != x and abs(oy - ox) <= 0x10 and sty.get(y) in SCALAR)
            if nb:
                flagged.add((x, "nb"))
                self.add(9, "high" if stored or len(nb) > 1 else "medium", line_of(m.start()), fn, x, body[max(0, m.start() - 30):m.end() + 20], "address of %s is %s while the neighbouring slot(s) %s are also used: a callee may read them through the pointer; the compiler does not keep separate locals adjacent (use one struct local)" % (x, "stored" if stored else "passed to a call", ",".join(nb)))
        # a stack ARRAY passed to a call (bare name) that is immediately followed by more scalar locals of the same frame object (`undefined1 auStack_2c [8]; undefined4 uStack_24; ..`): the original has one buffer, the callee reads past the array
        ES = {"undefined": 1, "undefined1": 1, "undefined2": 2, "undefined4": 4, "undefined8": 8, "char": 1, "uchar": 1, "byte": 1, "short": 2, "ushort": 2, "int": 4, "uint": 4, "float": 4, "RwReal": 4, "double": 8}
        for m in re.finditer(r"^\s+(?:(?:unsigned|signed|const)\s+)*(\w+)\s+((?:local|[a-z]*Stack)_([0-9a-fA-F]+))\s*\[\s*(\d+|0x[0-9a-fA-F]+)\s*\]\s*;", body, flags=re.M):
            ty, nm, off, n = m.group(1), m.group(2), int(m.group(3), 16), int(m.group(4), 0)
            end = off - n * ES.get(ty, 4)
            above = [y for y, oy in loc.items() if y != nm and oy == end and sty.get(y) in SCALAR]      # offsets are negative distances from the frame top: the slot that starts where the array ends
            call = re.search(r"\b\w+\s*\([^;]*[(,]\s*%s\s*[,)]" % re.escape(nm), body)
            if above and call and (nm, "arr") not in flagged:
                flagged.add((nm, "arr")); self.add(9, "high", line_of(m.start()), fn, nm, m.group(0).strip(), "array %s is passed to a call and the scalar local(s) %s follow it directly: one frame object in the original, the compiler may drop / move the separate locals (declare one array / struct)" % (nm, ",".join(above)))
        # p = &local_N; ... p[k] / *(p + k) with k >= 1
        for m in re.finditer(r"\b(\w+)\s*=\s*(?:\([^)]*\)\s*)?&\s*((?:local|[a-z]*Stack)_[0-9a-fA-F]+)\b\s*;", body):
            p_, x = m.group(1), m.group(2)
            if re.search(r"\b%s\s*\[\s*[1-9]|\b%s\s*\+\s*[1-9]" % (p_, p_), body[m.end():]) and (x, "ar") not in flagged:
                flagged.add((x, "ar")); self.add(9, "high", line_of(m.start()), fn, x, m.group(0), "%s = &%s is indexed / offset beyond element 0 later" % (p_, x))

    def check_defarity(self, fn, params, s, e):
        """class 10: parameters declared by the C definition vs the stack argument slots the ORIGINAL reads / rewrites (output/orig_stackargs.txt: tools/origargs.py), the
        argument count of its tail-jump target and what its callers push (cdecl_args.txt, call_pushmin.txt with >= 3 sites)"""
        va = NAMEVA.get(fn) or NAMEVA.get(fn[:-2] if fn.endswith("_c") else fn)
        if not va:
            h = re.match(r"FUN_([0-9a-f]{8})$", fn); va = h.group(1) if h else None
        if not va or va not in ORIG or "..." in params: return
        slots, retn, tails, fl = ORIG[va]
        if any("__asm" in l for l in self.lines[s:e + 1]):      # naked bridge wrapper: its `_c` function is the definition that is checked; the wrapper must forward every stack slot the original reads and pop `ret N` like it
            body = "\n".join(self.lines[s:e + 1]); fw = len(re.findall(r"push\s+dword ptr \[esp\s*\+", body)); rm = re.search(r"\bret\s+(\d+)", body); rn = int(rm.group(1)) if rm else 0
            if va in ORIG and "..." not in params:
                sl, retn = ORIG[va][:2]
                if fw < sl: self.add(10, "high", s + 1, fn, fn, "wrapper %s" % fn, "defarity: the naked wrapper forwards %d stack argument(s) but the original reads / rewrites %d" % (fw, sl))
                if rn != retn: self.add(10, "high", s + 1, fn, fn, "wrapper %s" % fn, "defarity: the naked wrapper returns with `ret %d`, the original with `ret %d`" % (rn, retn))
            return
        ps = [p for p in split_args(params) if p.strip() and p.strip() != "void" and not re.search(r"\breg_\w+$", p.strip())]      # register inputs of a bridge `_c` function are not stack arguments
        cn = sum(2 if re.search(r"\b(double|longlong|ulonglong|undefined8|__int64|int64_t|uint64_t)\b", p) and "*" not in p else 1 for p in ps)
        ev = []
        if slots: ev.append((slots, "the original reads / rewrites %d stack argument slot(s)%s" % (slots, " (address taken: lower bound)" if "addr" in fl else "")))
        if retn >= 4 and not fn.endswith("_c"): ev.append((retn // 4, "the original returns with `ret %d`" % retn))
        for t in tails:
            n = max(ORIG.get(t, (0, 0))[0], ORIG.get(t, (0, 0, 0))[1] // 4, GT.get(t, 0), *[int(f[1:]) for f in fl if re.fullmatch(r"w\d+", f)])
            if n: ev.append((n, "it tail-jumps to 0x%s which takes %d argument(s)" % (t, n)))
        if va not in REGVA:
            n = GT.get(va) if va in GT else PMALL[va][0] if va in PMALL and PMALL[va][1] >= 3 else 0
            if n: ev.append((n, "the original callers push %d argument(s)" % n))
        for n, why in ev:
            if cn < n:
                self.add(10, "high", s + 1, fn, fn, "%s(%s)" % (fn, params), "defarity: the C definition declares %d parameter(s) but %s" % (cn, why)); break
        # vtable thunk (`jmp [reg+N]`): the C indirect call must pass the frame arguments on
        if "ijmp" in fl:
            need = max(cn, slots, GT.get(va, 0))
            for ln, st in self.statements(s, e):
                m = re.search(r"\(\*\*?\(\w[\w\s]*\*+\)[^;]*?\)\s*\(", st)
                if not m: continue
                j = balanced_after(st, m.end() - 1)
                k = len([a for a in split_args(st[m.end():j]) if a.strip()]) if j > 0 else 0
                if k < need: self.add(10, "high", ln, fn, fn, st, "defarity: vtable thunk (the original is `jmp [reg+N]`, %d frame argument(s) pass through) but the C indirect call passes %d argument(s)" % (need, k))
        # tail call from the C body with fewer arguments than the tail target takes
        for t in tails:
            tn = NAMEOF.get(t)
            n = max(ORIG.get(t, (0, 0))[0], GT.get(t, 0), *[int(f[1:]) for f in fl if re.fullmatch(r"w\d+", f)])
            if not tn or not n: continue
            for ln, st in self.statements(s, e):
                for m in re.finditer(r"\b%s\s*\(" % re.escape(tn), st):
                    j = balanced_after(st, m.end() - 1)
                    k = len([a for a in split_args(st[m.end():j]) if a.strip()]) if j > 0 else 0
                    if k < n: self.add(10, "high", ln, fn, tn, st, "defarity: tail call `%s` passes %d argument(s), the original jumps there with %d (frame arguments)" % (tn, k, n))

    def run(self):
        for fn, params, s, e in self.functions():
            self.check_framelayout(fn, s, e); self.check_defarity(fn, params, s, e)
            env = self.local_env(params, s, e)
            vc = []
            for line, st in self.statements(s, e):
                self.check_stmt(fn, line, st, env); self.check_regcallee(fn, line, st); self.check_krarity(fn, line, st); self.check_calleearity(fn, line, st)
                vc += [(line, st) + c for c in self.vtable_calls(st)]
            self.check_vtable(fn, vc)
        for m in re.finditer(r"^([A-Za-z_][\w \*]*?)\b(__stdcall |__cdecl )?(\w+)\(([^;{]*)\)\s*\{?\s*$", self.t, flags=re.M):
            n = m.group(3)
            if n in NAMEVA and NAMEVA[n] in GTCONV and "__stdcall" not in (m.group(2) or "") and not m.group(1).lstrip().startswith(("extern", "typedef", "return", "else")) and n not in KW and "(" not in m.group(1):
                self.add(4, "high", self.t[:m.start()].count("\n") + 1, n, n, m.group(0), "definition is not __stdcall but the original pops its arguments (`ret %d`, output/stdcall.txt)" % (GTCONV[NAMEVA[n]]))
        used = {f for f in self.protos if re.search(r"(?<![\w])%s\s*\(" % re.escape(f), "\n".join(l for l in self.t.split("\n") if not re.match(r"\s*extern\b", l)))}
        for f, (n, ps, va, rt, line) in self.protos.items():
            if f not in used: continue      # an unused extern cannot change the code
            if va and n is not None and va in PM and n < PM[va] and va not in GT and not re.search(r"printf", f) and not exact_slots(va, n):
                self.add(4, "high", line, f, f, self.raw.split("\n")[line - 1], "prototype has %d parameter(s) but every original call site has >= %d pushes pending (call_pushmin.txt): an argument is missing" % (n, PM[va]))
        # file level: a prototype with a different parameter count than output/cdecl_args.txt / stdcall.txt
        for f, (n, ps, va, rt, line) in self.protos.items():
            if f in used and va and n is not None and va in GT and GT[va] != n:
                self.add(4, "medium", line, f, f, self.raw.split("\n")[line - 1], "prototype has %d parameter(s), the original callers pass %d (cdecl_args.txt / stdcall.txt)" % (n, GT[va]))
        return self.finds


EXE = os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe")
PUSHMIN = os.path.join(G, "output", "call_pushmin.txt")


def gen_pushmin():
    """output/call_pushmin.txt `va min_pushes sites`: for every call target, the smallest number of `push`es pending (since the last call / branch / ret, net of pop / add esp) over its call sites.
    A cdecl / stdcall callee with n arguments has at least n pending pushes at EVERY call site, so a prototype with fewer parameters than min_pushes (>= 3 sites) lost an argument
    (the diffemu clump_set_flag_on_atomics(clump, flag) case: declared with one parameter, every caller pushes two)."""
    out = subprocess.run(["objdump", "-d", "-M", "intel", "--start-address=0x401000", "--stop-address=0x5d7000", EXE], capture_output=True, text=True).stdout.split("\n")
    mn, cnt, p = {}, {}, 0; fs = {int(l.split()[0], 16) for l in open(os.path.join(G, "output", "fnsizes.txt")) if l.split()}; pro = True
    for l in out:
        m = re.match(r"\s*([0-9a-f]+):\t[0-9a-f ]+\t(\S+)\s*(.*)$", l)
        if not m: continue
        if int(m.group(1), 16) in fs: pro = True; p = 0
        op, rest = m.group(2), m.group(3).strip()
        if pro and op == "push" and rest in ("esi", "edi", "ebx", "ebp"): continue      # saved registers of the prologue are no arguments (as in gen_indirect)
        if op != "nop": pro = False
        if op == "push": p += 1
        elif op == "pop": p = max(0, p - 1)
        elif op == "add" and rest.startswith("esp,"):
            try: p = max(0, p - int(rest.split(",")[1], 0) // 4)
            except ValueError: p = 0
        elif op == "call":
            t = re.match(r"0x([0-9a-f]{6})\b", rest)
            if t:
                a = t.group(1); mn[a] = min(mn.get(a, 99), p); cnt[a] = cnt.get(a, 0) + 1
            p = 0
        elif op.startswith("j") or op.startswith("ret") or op == "leave": p = 0
    with open(PUSHMIN + ".tmp", "w") as f:
        for a in sorted(mn): f.write("%08x %d %d\n" % (int(a, 16), mn[a], cnt[a]))
    os.replace(PUSHMIN + ".tmp", PUSHMIN)


INDIRECT = os.path.join(G, "output", "call_indirect.txt")
ORIGARGS = os.path.join(G, "output", "orig_stackargs.txt")


def gen_indirect():
    """output/call_indirect.txt `funcva disp pushes ev`: every `call [reg+disp]` of the original, with the pushes pending (since the last call / ret / jmp; a conditional branch does not reset: `push eax; je; call [ecx+0x1c]`)
    and ev = `pop` (no `add esp` / `pop ecx|edx` cleanup of its arguments on the straight-line path to the next `ret`, conditional branches fall through, net of the pushes of later calls: the callee pops its own arguments = __stdcall method),
    `clean` (`add esp` right after the call) or `?` (the path ends in `jmp` / a frame restore `leave` / `mov esp,ebp`: the cleanup may be elsewhere)."""
    out = subprocess.run(["objdump", "-d", "-M", "intel", "--start-address=0x401000", "--stop-address=0x5d7000", EXE], capture_output=True, text=True).stdout.split("\n")
    ins = []
    for l in out:
        m = re.match(r"\s*([0-9a-f]+):\t[0-9a-f ]+\t(\S+)\s*(.*)$", l)
        if m: ins.append((int(m.group(1), 16), m.group(2), m.group(3).strip()))
    starts = sorted(int(l.split()[0], 16) for l in open(os.path.join(G, "output", "fnsizes.txt")) if l.split())
    import bisect
    rows = []; p = 0; pro = True; fs = set(starts)
    for i, (a, op, rest) in enumerate(ins):
        if a in fs: pro = True; p = 0
        if pro and op == "push" and rest in ("esi", "edi", "ebx", "ebp"): continue       # saved registers of the prologue are no arguments
        if op != "nop": pro = False
        if op == "push": p += 1
        elif op == "pop": p = max(0, p - 1)
        elif op == "add" and rest.startswith("esp,"):
            try: p = max(0, p - int(rest.split(",")[1], 0) // 4)
            except ValueError: p = 0
        elif op in ("ret", "jmp", "retn") or op == "leave": p = 0
        elif op == "call":
            m = re.match(r"DWORD PTR \[(e[a-d]x|e[sd]i|ebx|ebp)\+0x([0-9a-f]+)\]$", rest) or re.match(r"DWORD PTR \[(e[a-d]x|e[sd]i|ebx|ebp)\]$", rest)
            if m:
                disp = int(m.group(2), 16) if m.lastindex == 2 else 0
                fa = starts[bisect.bisect_right(starts, a) - 1]
                clean = later = 0; ev = "?"
                for b, op2, r2 in ins[i + 1:i + 200]:
                    if b in fs: break
                    if op2 == "push": later += 1
                    elif op2 == "add" and r2.startswith("esp,"):
                        try: clean += int(r2.split(",")[1], 0) // 4
                        except ValueError: break
                        if b == ins[i + 1][0]: ev = "clean"; break
                    elif op2 == "pop" and r2 in ("ecx", "edx"): clean += 1
                    elif op2 == "leave" or (op2 in ("mov", "lea") and r2.startswith("esp,")): break
                    elif op2 in ("ret", "retn"): ev = "pop"; break      # a window that ends in jmp / a frame restore / the next function is unknown: the cleanup may follow at the target
                    elif op2 == "jmp": break
                if ev == "pop" and clean - later >= 1: ev = "clean"
                rows.append("%08x %d %d %s" % (fa, disp, p, ev))
            p = 0
        elif op == "call": p = 0
    open(INDIRECT, "w").write("\n".join(rows) + "\n")


ORIG = {}; NAMEOF = {}
ENABLED = set(); IND = {}; REGVA = {}; REGNAME = {}; GT = {}; GTCONV = {}; NAMEVA = {}; PM = {}; PMALL = {}; DPROTO = {}
def load_gt():
    if not os.path.exists(PUSHMIN) or "--regen" in sys.argv: gen_pushmin()
    for l in open(PUSHMIN):
        q = l.split()
        PMALL[q[0]] = (int(q[1]), int(q[2]))
        if int(q[2]) >= 3: PM[q[0]] = int(q[1])
    for l in open(os.path.join(G, "output", "decls.h"), errors="replace"):      # `()` externs of decls.h
        m = re.match(r"\s*/\*([0-9a-fA-F]{8})\*/\s*extern\s+.*?\b(\w+)\s*\(\s*\)\s*;", l)
        if m: DPROTO[m.group(2)] = (None, m.group(1).lower())
    for l in open(os.path.join(G, "output", "fnsizes.txt")):
        q = l.split()
        if len(q) >= 2: NAMEVA[q[1]] = q[0].lower()
    if not os.path.exists(ORIGARGS) or "--regen" in sys.argv: subprocess.run([sys.executable, os.path.join(H, "origargs.py")], check=True)
    for l in open(ORIGARGS):
        q = l.split()
        ORIG[q[0]] = (int(q[1]), int(q[2]), [] if q[3] == "-" else q[3].split(","), set(q[4].split(",")))
    NAMEOF.update({v: k for k, v in NAMEVA.items()})
    if not os.path.exists(INDIRECT) or "--regen" in sys.argv: gen_indirect()
    for l in open(INDIRECT):
        q = l.split()
        if len(q) == 4: IND.setdefault(q[0], {}).setdefault(int(q[1]), []).append((int(q[2]), q[3]))
    for r in csv.DictReader(open(os.path.join(G, "output", "regsigs.csv"))):
        REGVA[r["addr"].lower()] = r["regs"]; REGNAME[r["name"]] = r["addr"].lower()
    for l in open(os.path.join(G, "output", "stdcall.txt")):
        q = l.split()
        if len(q) >= 2 and int(q[1]) > 0: GTCONV[q[0].lower()] = int(q[1])
    for fn_, div in (("cdecl_args.txt", 1), ("stdcall.txt", 4)):
        p = os.path.join(G, "output", fn_)
        if os.path.exists(p):
            for l in open(p):
                q = l.split()
                if len(q) >= 2: GT.setdefault(q[0].lower(), int(q[1]) // (4 if div == 4 else 1))


def files_of(args):
    out = []
    for pat in (args or SCOPE):
        pat = pat if os.path.isabs(pat) else os.path.join(G, pat)
        for p in sorted(glob.glob(pat)):
            out += sorted(glob.glob(os.path.join(p, "*.c"))) if os.path.isdir(p) else [p]
    return out


def main():
    a = sys.argv[1:]; ref = None; out_csv = os.environ.get("DEFECTLINT_OUT") or os.path.join(G, "output", "defectlint.csv"); quiet = False
    if "--ref" in a: i = a.index("--ref"); ref = a[i + 1]; del a[i:i + 2]; out_csv = os.path.join(os.environ.get("TMPDIR", "/tmp/claude-%d" % os.getuid()), "defectlint_ref.csv")
    a_all = "--all" in a
    if a_all: a.remove("--all")
    if "--regen" in a: a.remove("--regen")
    if "--quiet" in a: a.remove("--quiet"); quiet = True
    load_gt(); gd = load_decls()
    ENABLED.update(l.strip() for l in open(os.path.join(G, "src", "bridges", "enabled.txt")))
    files = files_of(a)
    # diffemu verdict per function, and which tiers are byte identical
    verdict = {}
    p = os.path.join(G, "output", "diffemu_results.csv")
    if os.path.exists(p):
        for r in csv.DictReader(open(p)): verdict[r["name"]] = r["verdict"]
    finds = []; texts = {}
    for f in files:
        rel = os.path.relpath(f, G)
        if ref:
            r = subprocess.run(["git", "-C", G, "show", "%s:%s" % (ref, rel)], capture_output=True, text=True, errors="replace")
            if r.returncode: continue
            t = r.stdout
        else: t = open(f, errors="replace").read()
        texts[rel] = t
        finds += Lint(rel, t, gd).run()
    # cross file: one address declared with different parameter counts
    decl = {}
    for rel, t in texts.items():
        L = Lint(rel, t, gd)
        for f, (n, ps, va, rt, line) in L.protos.items():
            if va and n is not None and re.search(r"(?<![\w])%s\s*\(" % re.escape(f), "\n".join(l for l in L.t.split("\n") if not re.match(r"\s*extern\b", l))): decl.setdefault(va, {}).setdefault(n, []).append((rel, line, f))
    for va, d in decl.items():
        if len(d) > 1:
            mx = max(d)
            for n, sites in d.items():
                if n < mx:
                    for rel, line, f in sites:
                        finds.append(dict(cls=4, sev="medium", file=rel, line=line, function=f, symbol=f, text="/*%s*/ %s declared with %d parameter(s)" % (va, f, n), note="other files declare it with %d parameters (%s)" % (mx, d[mx][0][0])))
    allow = set()      # src/defectlint_allow.txt: `function class reason` (a finding that was checked against the original and is fine)
    ap = os.path.join(G, "src", "defectlint_allow.txt")
    if os.path.exists(ap):
        for l in open(ap):
            q = l.split(None, 2)
            if len(q) == 3 and not l.startswith("#"): allow.add((q[0], q[1]))
            elif q and not l.startswith("#"): sys.exit("defectlint_allow.txt: `%s` has no reason" % l.strip())
    finds = [f for f in finds if (f["function"], str(f["cls"])) not in allow]
    rows = []
    for f in finds:
        tier = "bridge" if "/bridges" in f["file"] or "usercall" in f["file"] else "tier1" if "/tier1/" in f["file"] else "agent" if "automatch_agent" in f["file"] else "tier2" if "automatch_near" in f["file"] else "candidate"
        f["tier"] = tier
        f["byte_identical"] = "yes" if tier in ("tier1", "agent") else "no"
        f["diffemu"] = verdict.get(f["function"], "")
        f["enabled"] = ("yes" if f["function"].removesuffix("_c") in ENABLED else "no") if f["file"].startswith("src/bridges/") else "yes" if tier in ("tier2", "tier1", "agent") else "no"
        rows.append(f)
    rows.sort(key=lambda r: (r["cls"], r["file"], r["line"]))
    cols = ["cls", "sev", "tier", "file", "line", "function", "symbol", "diffemu", "enabled", "byte_identical", "text", "note"]
    with open(out_csv, "w", newline="") as fh:
        w = csv.DictWriter(fh, cols, extrasaction="ignore"); w.writeheader(); w.writerows(rows)
    names = {1: "signed", 2: "floatbits", 3: "ftol", 4: "proto", 6: "vtable_conv", 7: "regcallee", 8: "krarity", 9: "framelayout", 10: "defarity"}
    live = [r for r in rows if r["byte_identical"] == "no" and r["enabled"] == "yes"]       # installed tier 2 / enabled bridges (a diffemu EQUAL does not exclude: random inputs never made frontend_enter_menu negative, nor passed a float to a `()` callee)
    rest = [r for r in rows if r["byte_identical"] == "no" and r not in live]
    summ = "defectlint: %d finding(s): %d in installed / enabled code [%s], %d in candidates / disabled bridges, %d in byte identical tier 1 / agent code" % (
        len(rows), len(live), ", ".join("%s %d" % (names[c], sum(1 for r in live if r["cls"] == c)) for c in names), len(rest), sum(1 for r in rows if r["byte_identical"] == "yes"))
    print(summ + "  -> " + os.path.relpath(out_csv, G))
    open_ = live if not a_all else rows
    if not quiet:
        for r in (rows if ref else open_):
            print("%s:%s: class %d (%s) %s %s: %s" % (r["file"], r["line"], r["cls"], names[r["cls"]], r["sev"], r["function"], r["note"]))
    rc = {}
    for r in rows:
        if r["cls"] == 7: rc.setdefault(r["symbol"], []).append("%s (%s)" % (r["function"], r["file"]))
    for callee, cs in sorted(rc.items()): print("regcallee %s (%s): called as cdecl from %s" % (callee, REGVA.get(REGNAME.get(callee) or (re.search(r"_([0-9a-f]{8})$", callee) or [0, ""])[1], "?"), "; ".join(sorted(set(cs)))))
    return 0


if __name__ == "__main__": sys.exit(main())
