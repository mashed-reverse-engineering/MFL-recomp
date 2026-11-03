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
  14 implicit  call of a function with NO prototype in scope (implicit declaration, or a `name()` K&R declaration in an included header) with a float / double argument: promoted to double, 8 bytes pushed, the callee reads a float (T-H2 course_warzone_cb0 func_0x0048db30)
  15 fpwidth   a float (double) typed global used by a function whose ORIGINAL instructions read that address only as a qword (dword): k_f_0 at 0x5ddc28 is the double 90.0, read as float it is 0 (T-H2 car_auto_steer_assist_step); output/fp_const_uses.txt = x87 memory operands per original function (gen_fpuse, --regen)
  16 t1hygiene tier 1 only (report only, byte identical code): no one-line `/* description */` comment before the function, or an `extern` declaration (object / function / function pointer) whose name is never used in the file (draft header residue; T-Q1)
  18 nakedaddr `&local` passed to a `__declspec(naked)` wrapper defined in the same file, local not volatile and its address going to no other (extern) function: MSVC drops the stores / reads around the call (T-P16/T-P17 menu_page_enter)
  20 regentry  callee side of regcallee: a cluster entry / tier 2 definition installed at the original address that is plain cdecl (or keeps an `in_<reg>` parameter) although the
              original reads register inputs at entry (regsigs.csv, reginputs2.csv, reginputs_interior.csv, cluster_pilot*/clusters.txt): needs `<fn>_n` + naked entry thunk (fix14 455e60)
  19 comargs   indirect `[reg+disp]` (COM vtable) call whose literal / field arguments contradict every original call site's pushes of that disp in the function (T-P18 DrawIndexedPrimitive Base/MinVertexIndex swap)
  21 argkind   DIRECT call whose literal / field arguments contradict every original call site of that callee in the function (output/call_sites.txt pushes; class 19 = the indirect COM form)
  22 retwidth  the C returns / declares <= 32 bits but the original callers read edx after the call (edx:eax result: T-P19 timer_ticks, __allmul / __alldiv declared int), or returns void while they read eax
  --selftest   runs the negative fixtures tools/negtests/defectlint/*.c (`// expect: class N` must fire, `// expect: clean class N` must not; `// path: src/..` = the path the snippet is linted as)
  17 framesize (`--framesize`, own run: needs the diffemu venv + object cache; `--all-tier2` = all automatch_near functions, default rwp_*) tier 2 function whose compiled prologue frame (sub esp / push ecx / chkstk) is > 8 bytes smaller than the ORIGINAL's: a local array / struct passed to a call is too small for the callee's stores (585950 `float res[16]` for 0xec bytes).  Output output/frameaudit.csv; baseline = output/frameaudit_2026-11-01.csv (status fixed / justified per flagged function)\nClass 5 (x87 precision of a float local) is not lintable.  tier 1 / agent sources are byte identical (check_all manifest + relocation check), so classes 1-3 cannot hide there:
they are listed (column `byte_identical`) but a byte identical function never counts as an open finding.  --ref REV lints the files as of a git revision (tuning on the pre-fix text).
Exit 0 always (warning tool)."""
import csv, glob, os, re, subprocess, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, ".."))
SCOPE = ["src/automatch_near", "src/automatch_agent", "src/tier1", "src/automatch", "src/bridges", "src/clusters", "output/tier2_candidates", "output/tier2_candidates2", "output/usercall_candidates*"]
INT_T = {"int", "short", "long", "char", "signed", "ushort_s", "int8_t", "int16_t", "int32_t", "SHORT", "LONG", "INT", "BOOL", "RwInt32", "RwBool", "RwInt16"}
UNS_T = {"uint", "ulong", "ushort", "uchar", "byte", "DWORD", "UINT", "WORD", "BYTE", "ULONG", "size_t", "unsigned", "RwUInt32", "RwUInt16", "undefined1", "undefined2", "dword", "word"}
FLT_T = {"float", "double", "float10", "RwReal", "FLOAT", "longdouble"}
KW = {"if", "while", "for", "return", "else", "switch", "sizeof", "do", "case", "goto"}


def exact_slots(va, k):
    """the ORIGINAL body reads exactly k stack argument slots (orig_stackargs.txt row without tail / addr / dyn / ijmp / w<N> flags): fewer pending pushes than the callers' minimum are then batched `add esp` cleanups, not missing arguments (2026-10-16)"""
    o = ORIG.get(va)
    return bool(o) and o[0] == k and all(t in ORIG and ORIG[t][0] <= k and not ORIG[t][2] and not (ORIG[t][3] & {"dyn", "addr", "ijmp"}) for t in o[2]) and not (o[3] & {"dyn", "addr", "ijmp"}) and not any(re.match(r"w\d", x) for x in o[3])      # a tail target reading no more slots than k (build_info_frame -> sound_streams_resume) keeps it exact
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
        self.gva_skip = set(); self.gva = {}                     # file extern name -> /*va*/ of its declaration
        self.hdr = header_decls(text, os.path.dirname(os.path.join(G, path)))
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
            if m:
                self.glob[m.group(2)] = base_type(m.group(1))
                va = re.search(r"/\*([0-9a-fA-F]{8})\*/", raw[i])
                if va: self.gva[m.group(2)] = va.group(1).lower()
                continue
            m = re.match(r"\s*extern\s+((?:(?:const|unsigned|signed|undefined\d?)\s+)*[A-Za-z_]\w*)\s+(\w+(?:\[[^\]]*\])*(?:\s*,\s*\w+(?:\[[^\]]*\])*)+)\s*;", l)      # `extern int a, b, c;`
            if m:
                for n in re.split(r"\s*,\s*", m.group(2)): self.glob[n.split("[")[0]] = base_type(m.group(1))

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
        # class 12 (T-weak2 2026-10-27): Ghidra's clamp idiom `(a <= b) && (y = b, c < b)`: `<=` is false for NaN so the clamp value wins; the original (`fcom / test ah,0x41 / jne`, `test ah,5 / jp`) lets NaN pass unchanged:
        # write `y = x; if (x > hi) y = hi; else if (x < lo) y = lo;` (car_air_control, camera_path_sample, player_stats_update)
        for m in re.finditer(r"\(\s*([\w.\[\]>-]+)\s*(<=|>=)\s*([\w.\[\]>-]+)\s*\)\s*&&\s*\(\s*[\w.]+\s*=\s*([\w.\[\]>-]+)\s*,", st):
            if m.group(4) in (m.group(1), m.group(3)): self.add(12, "high", line, fn, m.group(4), st, "clamp idiom `(%s %s %s) && (y = %s, ..)`: NaN takes the clamp value, the original passes NaN unchanged" % (m.group(1), m.group(2), m.group(3), m.group(4)))
        # class 11 (T-weak2 2026-10-27): an int / pointer object (typed local / global / parameter, `p[i]`, `*p`) compared with a float literal: the original is `cmp` / `test`, NaN or -0.0 bit patterns take the other branch (car_apply_forces in_contact)
        for m in re.finditer(r"(?<![<>\-=!])(<=|>=|==|!=|<|>)(?![<>=])", st):
            a, b = self.operand(st, m.start(), -1), self.operand(st, m.end(), 1)
            if not a or not b: continue
            ka, na = self.atom(a, env); kb, nb = self.atom(b, env)
            for kx, nx, ky, ny in ((ka, na, kb, nb), (kb, nb, ka, na)):
                if kx in "USuP" and ky == "F" and re.match(r"^-?(\d+\.\d*|\.\d+)([eE][-+]?\d+)?[fF]?$", ny): self.add(11, "high", line, fn, nx, st, "%s `%s` (int / pointer object) is compared with the float literal %s: the original compares integer bits (cmp / test)" % ("pointer" if kx == "P" else "int", nx, ny))
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
                fa = [a for a in fa if not re.search(r"\b(?:float|RwReal|double)\s+%s\s*\[" % re.escape(a.strip().split(".")[-1]), self.t)]      # an array decays to a pointer
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

    def check_regentry(self, fn, params, s):
        """class 20 (callee side of regcallee): a C definition installed AT the original address (cluster entry, tier 2 name) that is a plain cdecl function although the
        original reads register inputs at entry (regsigs / reginputs2 / reginputs_interior / clusters.txt column 4), or that still has a Ghidra `in_<reg>` / `unaff_<reg>` parameter:
        the C reads a stack slot the register callers never pushed (T-fix14 course_city_cb1_sub_455e60: ESI position read as [esp+4] = saved ESI 0x24, row 14 crash).  Fix: `<fn>_n` + naked entry thunk."""
        if fn in self.naked: return
        m = re.match(r"//\s*cluster-entries:\s*(.*)", self.raw.split("\n")[0])
        if not ((m and fn in re.split(r"[\s,]+", m.group(1).strip())) or ("/automatch_near/" in self.path and fn in T2NAMES)): return
        va = ALIASVA.get(fn) or NAMEVA.get(fn)
        regs = set(REGIN.get(va, ())); res = set(re.findall(r"\b(?:in|unaff)_(eax|ebx|ecx|edx|esi|edi|ebp)\b", params))
        conv = re.search(r"\b(__fastcall|__thiscall)\s+\**\s*%s\s*\(" % re.escape(fn), self.t)
        if conv: regs -= {"ecx", "edx"} if conv.group(1) == "__fastcall" else {"ecx"}
        if not (regs or res): return
        self.add(20, "high", s + 1, fn, va or fn, re.sub(r"\s+", " ", "%s(%s)" % (fn, params)), "the original %s takes %s in register(s) at entry, but the installed C definition is a plain cdecl function (stack parameters: `%s`): register callers' values never reach it; needs `%s_n` + a naked entry thunk (push reg; call; add esp; ret)" % (va or "?", "/".join(sorted(regs | res)), params.strip() or "void", fn))

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
            if pr and pr[1]: k += sum(1 for q in pr[1][:k] if re.match(r"(?:const\s+)?(?:long long|longlong|ulonglong|double|__int64)\b(?!\s*\*)", q.strip()))      # a 64-bit parameter is two stack slots
            if k < sl: self.add(10, "high", line, fn, f, st, "defarity: `%s` is called with %d argument(s), its original body reads %d stack argument slot(s)" % (f, k, sl))

    _NOCALL = KW | FLT_T | INT_T | UNS_T | {"defined", "asm", "__asm", "_asm", "__declspec", "float", "double", "void", "code", "main"} | {
        "sqrt", "sin", "cos", "tan", "atan", "atan2", "acos", "asin", "fabs", "floor", "ceil", "pow", "exp", "log", "log10", "fmod", "sinh", "cosh", "tanh", "ldexp", "frexp", "modf", "abs"}      # <math.h> functions take / return double anyway (no prototype needed for the promotion to be right)
    def declared_names(self):
        if getattr(self, "_decl", None) is None:
            d = set(self.protos) | set(self.hdr) | {n for n, _, _, _ in self.functions()} | set(self.glob)
            for l, r in zip(self.t.split("\n"), self.raw.split("\n")):
                if re.match(r"\s*extern\b", l): d.update(re.findall(r"\b([A-Za-z_]\w*)\s*\(", l)); continue      # also a block scope `extern int f(int, float);`
                if r[:1].isspace() or r.startswith("}"): continue
                if l.startswith("#"): d.update(re.findall(r"define\s+(\w+)\(", l))
                else: d.update(re.findall(r"\b([A-Za-z_]\w*)\s*\(", l))      # every column-0 line is a declaration / definition (bodies are indented), incl. one-line `static T f(..) { .. }`
            self._decl = d
        return self._decl

    def check_implicit(self, fn, line, st, env):
        """class 14: a call of a function with NO prototype in scope (implicit declaration, or a header `name()` K&R declaration) with a float / double argument: the argument is promoted to double (8 bytes pushed), the callee reads a float (T-H2 course_warzone_cb0: func_0x0048db30). Declared-`()` callees of the file itself are class 4."""
        if re.match(r"^\s*(extern|typedef)\b", st): return
        for m in re.finditer(r"(?<![\w.>])([A-Za-z_]\w*)\s*\(", st):
            f = m.group(1)
            if f in self._NOCALL or f in env or re.search(r"printf", f) or f in self.protos: continue
            kr = f in self.hdr and self.hdr[f] is None
            if f in self.declared_names() and not kr: continue
            if f in self.glob: continue
            j = balanced_after(st, m.end() - 1)
            if j < 0: continue
            fa = [a.strip() for a in split_args(st[m.end():j]) if not re.search(r"\b(?:float|RwReal|double)\s+%s\s*\[" % re.escape(a.strip().split(".")[-1]), self.t) and self.atom(a, env)[0] == "F" and self.atom(a, env)[1] != "(cast)" or re.match(r"^\s*\(\s*(float|double)\s*\)", a) or (not re.fullmatch(r"\s*[\w.]+\s*", a) and self.fexpr(a, env))]
            if fa: self.add(14, "high", line, fn, f, st, "float / double argument %s passed to `%s` (%s): promoted to double, the callee reads %s" % (fa[0], f, "`()` declaration in a header" if kr else "no prototype in scope", "a float"))

    def check_fpwidth(self, fn, s, e):
        """class 15: a float typed global (or a double typed one) used by this function whose ORIGINAL instructions in the same function access that address only as a qword (resp. dword): the C reads / writes the other half-width (k_f_0 at 0x5ddc28 is the double 90.0; read as float it is 0).  output/fp_const_uses.txt (gen_fpuse)"""
        fva = NAMEVA.get(fn) or NAMEVA.get(fn.removesuffix("_c"))
        fvs = {int(fva, 16)} if fva else set()
        if not fva:      # a cluster helper: the original functions of the whole cluster (first line `// cluster-entries: a b`)
            m = re.match(r"//\s*cluster-entries:\s*(.*)", self.raw.split("\n")[0])
            fvs = {int(NAMEVA[x], 16) for x in re.split(r"[\s,]+", m.group(1).strip()) if x in NAMEVA} if m else set()
            if not fvs: return
        body = "\n".join(self.lines[s:e + 1]); done = set()
        for m in re.finditer(r"(?<![\w.])([A-Za-z_]\w*)\b(?!\s*\()", body):
            n = m.group(1)
            if n in done: continue
            ty = self.glob.get(n)
            if ty not in ("float", "double", "RwReal") or n in self.gva_skip: continue
            done.add(n); cw = 8 if ty == "double" else 4
            vas = {int(self.gva[n], 16)} if n in self.gva else GVAS.get(n, set())
            ws = [FPUSE.get((fv, a)) for a in vas for fv in fvs]; ws = [w for w in ws if w]
            if not ws or any(cw in w for w in ws): continue
            self.add(15, "high", s + 1 + body[:m.start()].count("\n"), fn, n, n, "`%s` is declared %s but the original function reads its address 0x%s only as a %s (fld %s ptr): width mismatch" % (n, ty, "/".join("%x" % a for a in sorted(vas)), "qword double" if cw == 4 else "dword float", "qword" if cw == 4 else "dword"))

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
            n = max(ORIG.get(t, (0, 0))[0], ORIG.get(t, (0, 0, 0))[1] // 4, (0 if t in ORIG and exact_slots(t, ORIG[t][0]) else GT.get(t, 0)), *[int(f[1:]) for f in fl if re.fullmatch(r"w\d+", f)])      # exact original slots beat the callers' (batched) count
            if n: ev.append((n, "it tail-jumps to 0x%s which takes %d argument(s)" % (t, n)))
        if va not in REGVA:
            n = GT.get(va) if va in GT else PMALL[va][0] if va in PMALL and PMALL[va][1] >= 3 else 0
            if n and not exact_slots(va, cn): ev.append((n, "the original callers push %d argument(s)" % n))      # the original reads exactly cn slots: the callers' larger count is batched `add esp` cleanups
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
            if t in ORIG and exact_slots(t, ORIG[t][0]): n = ORIG[t][0]      # the target reads exactly these slots: neither the callers' (batched) count nor the w<N> rewrite of another tail target matters
            if not tn or not n: continue
            for ln, st in self.statements(s, e):
                for m in re.finditer(r"\b%s\s*\(" % re.escape(tn), st):
                    j = balanced_after(st, m.end() - 1)
                    k = len([a for a in split_args(st[m.end():j]) if a.strip()]) if j > 0 else 0
                    if k < n: self.add(10, "high", ln, fn, tn, st, "defarity: tail call `%s` passes %d argument(s), the original jumps there with %d (frame arguments)" % (tn, k, n))

    def check_nakedaddr(self, fn, line, st, env, body, params):
        """class 18 (T-P16 / T-P17): `&local` passed to a `__declspec(naked)` function DEFINED in this file: MSVC sees the naked body as not reading its parameters, so the store before the call / the read after it is dropped or the slot is shared (cl_menu_page_enter slot index -1). Fix: `volatile` local + cast at the call"""
        nk = self.naked
        if not nk or re.match(r"^\s*extern\b", st): return
        for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", st):
            if m.group(1) not in nk or m.group(1) == fn: continue
            j = balanced_after(st, m.end() - 1)
            seen = set()
            for a in split_args(st[m.end():j] if j > 0 else ""):
                for mm in re.finditer(r"&\s*([A-Za-z_]\w*)\b(?!\s*->)", a):
                    x = mm.group(1)
                    if x not in env or x in seen or re.search(r"\*\s*\([^()]*\*\s*\)\s*$", a[:mm.start()]): continue      # `*(int *)&f` passes the value; `&p->m` is not a local
                    seen.add(x)
                    if self.escapes(x, body): continue
                    vol =any(re.search(r"\b%s\b" % re.escape(x), re.sub(r"\{[^{}]*\}", "", d)) for d in re.findall(r"\bvolatile\b[^;{]*(?:\{[^{}]*\}[^;]*)?;", body + ";" + params.replace(",", ";")))
                    if not vol: self.add(18, "high", line, fn, x, st, "address of the non-volatile local `%s` goes to the naked wrapper `%s` defined in this file: MSVC may drop the store before / the read after the call (declare it volatile, cast at the call)" % (x, m.group(1)))

    def escapes(self, x, body):
        """`&x` (or `&x.m`, `&x[i]`) also goes to a function that is NOT defined in this file: MSVC then treats x as address-taken for the whole function and keeps its stores / reloads (fx_decal_stamp_matrix: RwMatrixTranslate(&matrix, ..))"""
        if getattr(self, "_defs", None) is None: self._defs = {n for n, _, _, _ in self.functions()}
        for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", body):
            f = m.group(1)
            if f in KW or f in self._defs or f in self.naked: continue
            j = balanced_after(body, m.end() - 1)
            if j > 0 and re.search(r"&\s*%s\b(?!\s*->)" % re.escape(x), body[m.end():j]): return True
        return False

    _OSITES = {}
    @staticmethod
    def orig_com_sites(va):
        """{disp: [[push operand of arg0 (last push), arg1, ..]]} of the `call [reg+disp]` sites of the ORIGINAL function at va (pushes collected backwards up to the previous call / jmp / ret)"""
        if va in Lint._OSITES: return Lint._OSITES[va]
        sz = FNSIZE.get(va, 0); out = {}
        if sz:
            a0 = int(va, 16)
            ins = [(m.group(1), m.group(2).strip()) for m in re.finditer(r"^\s*[0-9a-f]+:\t[0-9a-f ]+\t(\S+)\s*(.*)$", subprocess.run(["objdump", "-d", "-M", "intel", "--start-address=0x%x" % a0, "--stop-address=0x%x" % (a0 + sz), EXE], capture_output=True, text=True).stdout, flags=re.M)]
            for i, (op, r) in enumerate(ins):
                m = re.match(r"DWORD PTR \[e(?:[a-d]x|[sd]i|bp)(?:\+0x([0-9a-f]+))?\]$", r) if op == "call" else None
                if not m: continue
                ps = []
                for k in range(i - 1, -1, -1):
                    op2, r2 = ins[k]
                    if op2 in ("call", "jmp") or op2.startswith("ret"): break
                    if op2 != "push": continue
                    if re.fullmatch(r"e[a-d]x|e[sd]i|ebx|ebp", r2):      # `mov edx,[esi+4]; push edx`: the value of the last write of that register
                        for op3, r3 in reversed(ins[:k]):
                            if op3 in ("call", "jmp") or op3.startswith("ret"): break
                            q = r3.split(",", 1)
                            if q[0] != r2 or len(q) < 2: continue
                            r2 = q[1] if op3 == "mov" and not re.fullmatch(r"e[a-d]x|e[sd]i|ebx|ebp", q[1]) else "0x0" if op3 == "xor" and q[1] == r2 else r2
                            break
                    ps.append(r2)
                out.setdefault(int(m.group(1) or "0", 16), []).append(ps)
        Lint._OSITES[va] = out
        return out

    def check_comargs(self, fn, line, st):
        """class 19 (T-P18): indirect (COM vtable) call whose argument kinds contradict EVERY original `call [reg+disp]` site of the same displacement: a C literal where the original pushes a memory operand / another immediate, or a field where it pushes an immediate (DrawIndexedPrimitive with BaseVertexIndex / MinVertexIndex swapped)"""
        vas = self.fn_vas(fn)
        if not vas: return
        x = self.expand(st)
        for m in re.finditer(r"\)\s*\(", x):
            i = m.start(); d = 0; o = -1
            for j in range(i, -1, -1):
                if x[j] == ")": d += 1
                elif x[j] == "(":
                    d -= 1
                    if d == 0: o = j; break
            cal = x[o:i + 1] if o >= 0 else ""
            n = re.search(r"\[[\s(]*(0x[0-9a-fA-F]+|\d+)[\s)]*/\s*4\s*\]\s*\)*$", cal) or re.search(r"\+\s*(0x[0-9a-fA-F]+|\d+)\s*\)\s*\)$", cal)
            if not n or "*" not in cal: continue
            disp = int(n.group(1), 0); b = balanced_after(x, m.end() - 1)
            if b < 0: continue
            args = [self.argkind(a) for a in split_args(x[m.end():b])]
            sites = [ps for va in vas for ps in self.orig_com_sites(va).get(disp, []) if len(ps) >= len(args)]
            if not sites: continue
            bad = []
            for ps in sites:
                c = [k for k, (ca, op) in enumerate(zip(args, ps)) if self.conflict(ca, op)]
                if not c: break
                bad.append((ps, c))
            else:
                ps, c = bad[0]
                self.add(19, "high", line, fn, "+0x%x" % disp, st, "indirect call [+0x%x]: argument(s) %s contradict the original pushes (%s): swapped / wrong argument" % (disp, ",".join(str(k) for k in c), "; ".join("arg%d=%s" % (k, ps[k]) for k in c)))

    @staticmethod
    def conflict(ca, op):
        oi = re.fullmatch(r"(0x[0-9a-f]+)", op); om = "PTR [" in op or "PTR ds:" in op
        if ca[0] == "imm": return bool(oi) and int(oi.group(1), 16) & 0xffffffff != ca[1] & 0xffffffff or om
        return ca[0] == "mem" and bool(oi)

    def argkind(self, a):
        a = a.strip()
        while True:
            m = re.match(r"^\(\s*[A-Za-z_][\w\s]*\**\s*\)\s*(.*)$", a, flags=re.S)      # (cast)
            if m and m.group(1): a = m.group(1).strip(); continue
            if a.startswith("(") and balanced_after(a, 0) == len(a) - 1: a = a[1:-1].strip(); continue
            break
        if re.fullmatch(r"-?(0[xX][0-9a-fA-F]+|\d+)[uUlL]*", a): return ("imm", int(a.rstrip("uUlL"), 0))
        if a == "NULL": return ("imm", 0)
        if not a.startswith("&") and re.fullmatch(r"\*?[\w\s\.\[\]>-]+", a) and ("->" in a or "[" in a or a.startswith("*")) and not re.search(r"(?<!-)[+*/]|-(?!>)", a.lstrip("*")): return ("mem", a)
        return ("?", a)

    def expand(self, st):
        if getattr(self, "_mac", None) is None:
            self._mac = {}
            for m in re.finditer(r"^\s*#\s*define\s+(\w+)(\((\w+)\))?\s+(.+)$", self.raw, flags=re.M):
                self._mac[m.group(1)] = (m.group(3), strip_comments(m.group(4)).strip())
        for _ in range(3):
            def sub(m):
                p, v = self._mac[m.group(1)]
                if p is None: return "(" + v + ")"
                return m.group(0)
            st = re.sub(r"\b(%s)\b" % "|".join(map(re.escape, self._mac)) if self._mac else r"(?!)", sub, st)
            for nm, (p, v) in self._mac.items():
                if p is None: continue
                st = re.sub(r"\b%s\s*\(([^()]*(?:\([^()]*\))?[^()]*)\)" % re.escape(nm), lambda m: "(" + re.sub(r"\b%s\b" % re.escape(p), "(" + m.group(1) + ")", v) + ")", st)
        return st

    def fn_vas(self, fn):
        v = NAMEVA.get(fn) or NAMEVA.get(re.sub(r"_[cn]$", "", fn))
        if v: return [v]
        m = re.match(r"//\s*cluster-entries:\s*(.*)", self.raw.split("\n")[0])
        return [NAMEVA[x] for x in re.split(r"[\s,]+", m.group(1).strip()) if x in NAMEVA] if m else []

    def callee_va(self, f):
        if f in self.protos and self.protos[f][2]: return self.protos[f][2]
        h = re.fullmatch(r"(?:func_0x|FUN_)([0-9a-f]{8})", f)
        return h.group(1) if h else NAMEVA.get(f) or ALIASVA.get(f)

    def ret_type(self, f):
        """return type text of a prototype / definition of f in this file ('' = none; 'int' for an undeclared callee), conventions and storage classes removed"""
        m = re.search(r"^(?:/\*[0-9a-fA-F]{8}\*/\s*)?((?:__declspec\([^)]*\)\s*)?[A-Za-z_][\w \t\*]*?)\b%s\s*\(" % re.escape(f), self.raw, flags=re.M)      # column 0 only: declarations / definitions (bodies are indented)
        if not m: return "" if f in self.declared_names() or f in self.hdr else "int"
        r = re.sub(r"__declspec\([^)]*\)|\b(extern|static|inline|__inline|__forceinline|__cdecl|__stdcall|__fastcall|__thiscall|const|volatile|struct|enum|union)\b", " ", m.group(1))
        return " ".join(r.replace("*", " * ").split())

    def check_argkind(self, fn, line, st):
        """class 21 (argkind): a DIRECT call whose literal / memory arguments contradict EVERY original call site of the same callee in the original function (output/call_sites.txt pushes; class 19 does the same for
        indirect COM calls): a C literal where the original pushes a memory operand / another immediate, a field where it pushes an immediate (swapped arguments, T-P18 style)"""
        vas = self.fn_vas(fn)
        if not vas or re.match(r"^\s*(extern|typedef)\b", st): return
        if not any((va, self.callee_va(f)) in CSITES for f in re.findall(r"(?<![\w.>])([A-Za-z_]\w*)\s*\(", st) if f not in KW for va in vas): return      # macro expansion is the slow part
        x = self.expand(st)
        for m in re.finditer(r"(?<![\w.>])([A-Za-z_]\w*)\s*\(", x):
            f = m.group(1)
            if f in KW or f == fn: continue
            cva = self.callee_va(f)
            j = balanced_after(x, m.end() - 1)
            if not cva or j < 0: continue
            args = [self.argkind(a) for a in split_args(x[m.end():j]) if a.strip()]
            sites = [ps for va in vas for _, _, ps in CSITES.get((va, cva), [])]
            if not args or not sites or any(len(ps) < len(args) for ps in sites): continue      # a site whose pushes are not all known (hoisted before a branch target) could be the matching one
            bad = []
            for ps in sites:      # only literal-vs-literal and field-vs-literal count: a C literal where the original pushes a memory operand is often a constant kept in a stack temp / register / .rdata
                c = [k for k, (ca, op) in enumerate(zip(args, ps)) if re.fullmatch(r"0x[0-9a-f]+", op) and (ca[0] == "mem" and not 0x400000 <= int(op, 16) < 0xa00000 or ca[0] == "imm" and int(op, 16) & 0xffffffff != ca[1] & 0xffffffff)]      # `tab[i].name` (an array member) is an image address, not a memory read
                if not c: break
                bad.append((ps, c))
            else:
                ps, c = bad[0]
                self.add(21, "high", line, fn, f, st, "argkind: argument(s) %s of `%s` contradict every original call site in this function (%d; %s): swapped / wrong argument" % (",".join(map(str, c)), f, len(sites), "; ".join("arg%d=%s" % (k, ps[k]) for k in c)))

    def check_retwidth_call(self, fn, line, st):
        """class 22 (retwidth, call side): the C declares the callee returning <= 32 bits (or void) but the original function reads edx right after calling it (64-bit edx:eax result: T-P19 timer_ticks, __allmul / __alldiv declared int)"""
        vas = self.fn_vas(fn)
        if not vas or re.match(r"^\s*(extern|typedef)\b", st): return
        for m in re.finditer(r"(?<![\w.>])([A-Za-z_]\w*)\s*\(", st):
            f = m.group(1)
            if f in KW or f == fn: continue
            cva = self.callee_va(f)
            ss = [s_ for va in vas for s_ in CSITES.get((va, cva), [])] if cva else []
            k = sum(1 for s_ in ss if s_[1] == "r")
            if not k: continue
            rt = self.ret_type(f)
            if rt and not WIDE_T.search(rt): self.add(22, "high", line, fn, f, st, "retwidth: `%s` is declared returning `%s` but the original %s reads edx after %d of its %d call(s) of it (64-bit edx:eax result): declare it __int64" % (f, rt, fn, k, len(ss)))

    def check_retwidth_def(self, fn, s):
        """class 22 (retwidth, definition side): a C definition installed at an original address returns <= 32 bits while original callers read edx after calling it (edx:eax result), or returns void while they read eax"""
        if fn in self.naked or fn.endswith("_c"): return
        va = NAMEVA.get(fn) or ALIASVA.get(fn)
        sites = CSBYCALLEE.get(va or "", [])
        rt = self.ret_type(fn)
        if not sites or not rt: return
        ke = sum(1 for x in sites if x[2] == "r"); ka = sum(1 for x in sites if x[1] == "r")
        if ke and not WIDE_T.search(rt): self.add(22, "high", s + 1, fn, va, "%s %s(..)" % (rt, fn), "retwidth: returns `%s` but %d of the %d original call site(s) read edx after the call (edx:eax result, e.g. caller 0x%s): return __int64" % (rt, ke, len(sites), next(x[0] for x in sites if x[2] == "r")))
        elif ka * 2 >= len(sites) and rt == "void": self.add(22, "medium", s + 1, fn, va, "void %s(..)" % fn, "retwidth: returns void but %d of the %d original call site(s) read eax after the call (e.g. caller 0x%s): the original returns a value" % (ka, len(sites), next(x[0] for x in sites if x[1] == "r")))

    def run(self):
        self.naked = set(re.findall(r"__declspec\(\s*naked\s*\)[^;{(]*?\b(\w+)\s*\(", self.t))
        for fn, params, s, e in self.functions():
            self.check_regentry(fn, params, s)
            self.check_framelayout(fn, s, e); self.check_defarity(fn, params, s, e)
            env = self.local_env(params, s, e)
            vc = []; body = "\n".join(self.lines[s:e + 1])
            for line, st in self.statements(s, e):
                self.check_stmt(fn, line, st, env); self.check_regcallee(fn, line, st); self.check_krarity(fn, line, st); self.check_calleearity(fn, line, st); self.check_implicit(fn, line, st, env)
                self.check_nakedaddr(fn, line, st, env, body, params); self.check_comargs(fn, line, st); self.check_argkind(fn, line, st); self.check_retwidth_call(fn, line, st)
                vc += [(line, st) + c for c in self.vtable_calls(st)]
            self.check_vtable(fn, vc); self.check_fpwidth(fn, s, e); self.check_retwidth_def(fn, s)
            body = "\n".join(self.lines[s:e + 1])      # class 12, nested form: `if (x <= hi) { if (x < lo) x = lo; } else x = hi;` (also >= / else-first) on a float object
            for m in re.finditer(r"if \(\(?\s*([A-Za-z_][\w.\[\]>-]*) (<=|>=) ([\w.]+)\s*\)?\) \{\s*if \(\(?\s*\1 (<|>) ([\w.]+)\s*\)?\) \{\s*\1 = [\w.-]+;\s*\}\s*\}\s*else \{\s*\1 = [\w.-]+;\s*\}", body):
                k, n = self.atom(m.group(1), env)
                if k == "F" or (k == "?" and m.group(1) in self.float_members()) or self.atom(m.group(3), env)[0] == "F":      # T-weak3: a float bound (`k_f_0_95`, `0.95f`) makes x a float even when its member name is ambiguous (`frame.pos.x`)
                    self.add(12, "high", s + 1 + body[:m.start()].count("\n"), fn, m.group(1), " ".join(m.group(0).split()), "clamp `if (x %s k) { if (x %s k2) x = ..; } else x = ..;`: NaN takes the else value, the original passes NaN unchanged" % (m.group(2), m.group(4)))
            # class 13 (mixedwidth, T-weak3): the same array element is read / written as an int (`*(int *)&p[0x12]`) in this function but compared with a float literal elsewhere (`p[0x12] == 0.0`): an int flag word read as a float (-0.0 / NaN bit patterns differ; the original does cmp dword, 0)
            nrm = lambda x: re.sub(r"\s+", "", x)
            ints = {nrm(x) for x in re.findall(r"\*\(\s*(?:int|uint|unsigned int|undefined4|ulong|long)\s*\*\)\s*&\s*([A-Za-z_]\w*\[\s*(?:0x[0-9a-fA-F]+|\d+)\s*\])", body)}
            for m in re.finditer(r"(?<![\w&)\]])([A-Za-z_]\w*\[\s*(?:0x[0-9a-fA-F]+|\d+)\s*\])\s*(?:==|!=)\s*-?\d+\.\d*[fF]?(?![\w.])", body):
                if nrm(m.group(1)) in ints:
                    self.add(13, "high", s + 1 + body[:m.start()].count("\n"), fn, m.group(1), " ".join(m.group(0).split()), "`%s` is accessed as an int elsewhere in this function but compared with a float literal here: an int flag word (the original compares integer bits)" % m.group(1))
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
            sl = sum(2 if re.match(r"(?:const\s+)?(?:long long|longlong|double|__int64)\b(?!\s*\*)", q.strip()) else 1 for q in ps) if n else 0      # a 64-bit parameter is two stack slots
            if f in used and va and n is not None and va in GT and GT[va] != n and not exact_slots(va, sl):      # exact original slots = the declared ones: the callers' count is inflated by batched `add esp` cleanups
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


FPUSE = {}; GVAS = {}; CLENABLED = set()
FPUSE_TXT = os.path.join(G, "output", "fp_const_uses.txt")


def gen_fpuse():
    """output/fp_const_uses.txt `funcva addr widths`: for every original function, the absolute data addresses read / written by x87 memory instructions (fld / fst / fadd / fcom .. but not fild / fist) with the access widths (4 = dword float, 8 = qword double).  Used by class 15"""
    import bisect
    out = subprocess.run(["objdump", "-d", "-M", "intel", "--start-address=0x401000", "--stop-address=0x5d7000", EXE], capture_output=True, text=True).stdout.split("\n")
    starts = sorted(int(l.split()[0], 16) for l in open(os.path.join(G, "output", "fnsizes.txt")) if l.split()); use = {}
    for l in out:
        m = re.match(r"\s*([0-9a-f]+):\t[0-9a-f ]+\t(f[a-z0-9]+)\s+(DWORD|QWORD) PTR ds:0x([0-9a-f]+)", l)
        if m and not m.group(2).startswith("fi"):
            fa = starts[bisect.bisect_right(starts, int(m.group(1), 16)) - 1]
            use.setdefault((fa, int(m.group(4), 16)), set()).add(4 if m.group(3) == "DWORD" else 8)
    with open(FPUSE_TXT + ".tmp", "w") as f:
        for (fa, a), w in sorted(use.items()): f.write("%08x %08x %s\n" % (fa, a, ",".join(map(str, sorted(w)))))
    os.replace(FPUSE_TXT + ".tmp", FPUSE_TXT)


CALLSITES_TXT = os.path.join(G, "output", "call_sites.txt"); CSITES = {}; CSBYCALLEE = {}
_FAM = {"eax": ("eax", "ax", "al", "ah"), "edx": ("edx", "dx", "dl", "dh")}
def reg_rw(op, rest, reg):
    """'r' when the instruction reads `reg` (eax / edx family) before writing it, 'w' when it only writes it, None when it does not touch it"""
    fam = _FAM[reg]
    if op == "cdq": return "r" if reg == "eax" else "w"
    if op in ("div", "idiv"): return "r"
    if op == "mul" or op == "imul" and "," not in rest: return "r" if reg == "eax" else "w"
    if reg == "eax":
        if op in ("cwde", "cbw", "sahf") or re.match(r"(rep\w*\s+)?(stos|scas)", op + " " + rest): return "r"
        if op in ("fnstsw", "lahf"): return "w"
    if not set(re.findall(r"\w+", rest)) & set(fam): return None
    q = rest.split(",", 1); dst = q[0].strip(); src = q[1] if len(q) > 1 else ""
    if set(re.findall(r"\w+", src)) & set(fam) and not (op in ("xor", "sub", "sbb") and src.strip() == dst) or "[" in dst and set(re.findall(r"\w+", dst)) & set(fam): return "r"
    if op in ("xor", "sub", "sbb") and src.strip() == dst or op in ("mov", "movzx", "movsx", "lea", "pop") or op.startswith("set"): return "w"
    return "r"
def gen_callsites():
    """output/call_sites.txt `caller callee eax edx push0|push1|..` for every direct `call` of the original: eax / edx = what the caller does with the register after the call on the straight path
    (r read first, w written first, R reaches `ret` untouched, ? unknown: next call / jmp / 40 instructions); pushes = operands pushed since the previous call / jmp / ret (arg 0 first), a pushed register resolved
    to the operand of its last `mov` (classes 21 argkind, 22 retwidth)"""
    import bisect
    out = subprocess.run(["objdump", "-d", "-M", "intel", "--start-address=0x401000", "--stop-address=0x5d7000", EXE], capture_output=True, text=True).stdout.split("\n")
    ins = []
    for l in out:
        m = re.match(r"\s*([0-9a-f]+):\t[0-9a-f ]+\t(\S+)\s*(.*)$", l)
        if m: ins.append((int(m.group(1), 16), m.group(2), m.group(3).strip()))
    starts = sorted(int(l.split()[0], 16) for l in open(os.path.join(G, "output", "fnsizes.txt")) if l.split()); fs = set(starts)
    size = {int(l.split()[0], 16): int(l.split()[2]) for l in open(os.path.join(G, "output", "fnsizes.txt")) if len(l.split()) >= 3}
    clob = {}      # function -> registers its ORIGINAL body may write (a call / an outside jmp = both): a callee that never writes edx cannot return edx, the caller may keep a value in it across the call (screen_width)
    for a, op, rest in ins:
        fa = starts[bisect.bisect_right(starts, a) - 1]
        if fa + size.get(fa, 0) <= a: continue
        j = re.match(r"0x([0-9a-f]+)$", rest) if op == "jmp" else None
        if op == "call" or j and not fa <= int(j.group(1), 16) < fa + size.get(fa, 0): clob.setdefault(fa, set()).update(("eax", "edx")); continue
        if op in ("cdq", "mul", "div", "idiv") or op == "imul" and "," not in rest: clob.setdefault(fa, set()).add("edx")
        if op in ("mul", "div", "idiv", "fnstsw", "lahf") or op == "imul" and "," not in rest or op.startswith("lods"): clob.setdefault(fa, set()).add("eax")
        d = rest.split(",", 1)[0].strip()
        for reg, fam in _FAM.items():
            if d in fam: clob.setdefault(fa, set()).add(reg)
    rows = []; jt = {int(m.group(1), 16) for _, op, rest in ins if op.startswith("j") for m in [re.match(r"0x([0-9a-f]+)$", rest)] if m}
    for i, (a, op, rest) in enumerate(ins):
        t = re.match(r"0x([0-9a-f]+)$", rest) if op == "call" else None
        if not t: continue
        fa = starts[bisect.bisect_right(starts, a) - 1]; st = {}
        for reg in ("eax", "edx"):
            v = "?"
            for b, op2, r2 in ins[i + 1:i + 41]:
                if b in fs or b in jt: break      # a merge point: the register may come from another path
                if op2.startswith("ret"): v = "R"; break
                if op2 in ("call", "jmp", "leave"): break
                x = reg_rw(op2, r2, reg)
                if x: v = x; break
            if v in "rR" and int(t.group(1), 16) in size and reg not in clob.get(int(t.group(1), 16), ()): v = "k"      # kept across a callee that never writes it: not a result
            st[reg] = v
        ps = []
        for k in range(i - 1, -1, -1):      # pushes / register values are only followed back to the nearest branch target: before it they belong to one incoming path
            b, op2, r2 = ins[k]
            if b < fa or op2 in ("call", "jmp") or op2.startswith("ret"): break
            if op2 == "push":
                if re.fullmatch(r"e[a-d]x|e[sd]i|ebx|ebp", r2) and b not in jt:
                    for b3, op3, r3 in reversed(ins[max(0, k - 60):k]):
                        if b3 < fa or op3 in ("call", "jmp") or op3.startswith("ret") or op3.startswith(("rep", "movs", "stos", "lods", "cdq", "mul", "div", "idiv")) or op3 == "imul" and "," not in r3: break      # implicit register writers: unknown
                        q = r3.split(",", 1)
                        if q[0] == r2:      # the last write: a mov / xor zero gives the value, anything else (inc, add, pop, ..) leaves it unknown
                            if len(q) == 2: r2 = q[1] if op3 == "mov" and not re.fullmatch(r"e[a-d]x|e[sd]i|ebx|ebp", q[1]) else "0x0" if op3 == "xor" and q[1] == r2 else r2
                            break
                        if b3 in jt: break
                ps.append(r2)
            if b in jt: break
        rows.append("%08x\t%08x\t%s\t%s\t%s" % (fa, int(t.group(1), 16), st["eax"], st["edx"], "|".join(ps)))
    with open(CALLSITES_TXT + ".tmp", "w") as f: f.write("\n".join(rows) + "\n")
    os.replace(CALLSITES_TXT + ".tmp", CALLSITES_TXT)


WIDE_T = re.compile(r"\b(__int64|longlong|ulonglong|undefined8|int64_t|uint64_t|LONGLONG|ULONGLONG|long\s+long)\b")


def tier1_hygiene(rel, t):
    """class 16: missing description comment / unused extern in a tier 1 file"""
    out = []; body = strip_comments(t)
    if not re.search(r"^/\* (?!compile:)[A-Za-z].* \*/\s*$", t, re.M): out.append(dict(cls=16, sev="low", file=rel, line=1, function=os.path.basename(rel)[:-2], symbol="", text="", note="no one-line description comment before the function"))
    for i, ln in enumerate(body.split("\n")):
        if not ln.lstrip().startswith("extern"): continue
        m = re.search(r"\(\s*(?:__\w+\s*)?\*\s*(\w+)\s*\)", ln) or re.search(r"(\w+)\s*(?:\[[^\]]*\])?\s*(?:\([^)]*\))?\s*;", ln)
        if m and m.group(1) != "RwEngineInstance" and len(re.findall(r"\b%s\b" % m.group(1), body)) == 1:
            out.append(dict(cls=16, sev="low", file=rel, line=i + 1, function=os.path.basename(rel)[:-2], symbol=m.group(1), text=ln.strip(), note="extern `%s` is never used in this file" % m.group(1)))
    return out


def prologue_frame(b):
    """(bytes of locals reserved, aligned) from the first instructions of a function: [push ebp; mov ebp,esp] then sub esp,N / push ecx / mov eax,N+call __chkstk / and esp,-16; callee-saved pushes skipped"""
    i = 0; n = 0; al = False
    for _ in range(14):
        c = b[i:i + 6]
        if c[:1] == b"\x55" or c[:1] in (b"\x53", b"\x56", b"\x57") : i += 1
        elif c[:2] == b"\x8b\xec": i += 2
        elif c[:1] == b"\x51": n += 4; i += 1
        elif c[:2] == b"\x83\xec": n += c[2]; i += 3
        elif c[:2] == b"\x81\xec": n += int.from_bytes(c[2:6], "little"); i += 6
        elif c[:1] == b"\xb8" and b[i + 5:i + 6] == b"\xe8": n += int.from_bytes(b[i + 1:i + 5], "little"); i += 10      # mov eax,N; call __chkstk
        elif c[:2] == b"\x83\xe4": al = True; i += 3
        else: break
    return n, al


def framesize(a):
    """class 17 (--framesize): tier 2 function whose frame (prologue `sub esp,N` / push ecx / chkstk) is smaller than the ORIGINAL's by more than 8 bytes: a local array / struct is too small for what a callee writes through its address (585950: float res[16] for the 0xec bytes 586f20 stores).  Needs the diffemu venv + the compile cache (tools/diffemu/diffemu.py compile_obj)."""
    try: import capstone
    except ImportError:
        py = os.environ.get("DIFFEMU_PY") or os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python"); os.execv(py, [py] + sys.argv)
    sys.path[:0] = [os.path.join(H, "diffemu"), H]
    import diffemu, cmpfn
    rows = []; byfile = {}
    for l in open(os.path.join(G, "src", "automatch_near_names.txt")):
        q = l.split()
        if len(q) == 3 and "rwp" in q[1] or len(q) == 3 and "--all-tier2" in a: byfile.setdefault(q[2], []).append((q[1], int(q[0], 16)))
    for f, fns in sorted(byfile.items()):
        src = os.path.join(G, "src", "automatch_near", f)
        if not os.path.exists(src): f = "an_" + f; src = os.path.join(G, "src", "automatch_near", f)
        if not os.path.exists(src): continue
        try: obj, _ = diffemu.compile_obj(src)
        except Exception as e: print("framesize: %s: %s" % (f, e)); continue
        for nm, va in fns:
            try: ob = cmpfn.obj_function(obj, nm)[0]
            except SystemExit: continue
            ours, oal = prologue_frame(ob); orig, ral = prologue_frame(cmpfn.exe_bytes(va, 64))
            rows.append(dict(cls=17, sev="high" if ours < orig - 8 else "info", file="src/automatch_near/" + f, line=1, function=nm, symbol="%08x" % va, text="frame ours %d orig %d%s" % (ours, orig, " (aligned frame)" if ral else ""), note="our frame is %d bytes smaller than the original's: a local passed to a call is probably too small" % (orig - ours) if ours < orig - 8 else "ok"))
    try:      # callee store-extent audit (tools/frameaudit.py): every local address handed to a call, our room vs the original's room / the callee's decoded extent
        import frameaudit
        for r in frameaudit.main(["--all-tier2"] if "--all-tier2" in a else []):
            if r["status"] in ("CHECK", "OVERFLOW"):
                rows.append(dict(cls=17, sev="high", file="src/automatch_near/" + r["file"], line=1, function=r["function"], symbol="%s:%s" % (r["callee"], r["arg"]), text="callee %s arg %s: our room %s < original %s, required %s" % (r["callee"], r["arg"], r["room_ours"], r["room_orig"], r["required"]), note=r["note"]))
    except Exception as e: print("framesize: frameaudit: %s %s" % (type(e).__name__, e))
    return rows


def header_decls(text, here, seen=None):
    """names the included headers declare: name -> nparams (None = `()` K&R declaration, -1 = macro / function pointer / unknown shape), recursive over `#include "x.h"`"""
    seen = set() if seen is None else seen; out = {}
    for inc in re.findall(r'^\s*#\s*include\s+"([^"]+)"', text, flags=re.M):
        for d in (here, os.path.join(G, "src"), os.path.join(G, "src", "auto"), os.path.join(G, "output")):
            p = os.path.normpath(os.path.join(d, inc))
            if os.path.isfile(p):
                if p not in seen:
                    seen.add(p); ht = open(p, errors="replace").read(); out.update(header_decls(ht, os.path.dirname(p), seen))
                    for m in re.finditer(r"^\s*#\s*define\s+(\w+)\(", ht, flags=re.M): out[m.group(1)] = -1
                    for l in strip_comments(ht).split("\n"):
                        if l[:1].isspace() or l.startswith("#"): continue
                        for m in re.finditer(r"\b([A-Za-z_]\w*)\s*\(([^()]*)\)", l):
                            ps = m.group(2).strip(); out[m.group(1)] = None if ps == "" else 0 if ps == "void" else len(split_args(ps))
                        for m in re.finditer(r"\(\s*[\w\s]*\*\s*(\w+)\s*\)\s*\(", l): out[m.group(1)] = -1
                break
    return out


ORIG = {}; NAMEOF = {}
REGIN = {}; ALIASVA = {}; T2NAMES = set(); REGS_RE = re.compile(r"\b(eax|ebx|ecx|edx|esi|edi|ebp)\b")
FNSIZE = {}; ENABLED = set(); IND = {}; REGVA = {}; REGNAME = {}; GT = {}; GTCONV = {}; NAMEVA = {}; PM = {}; PMALL = {}; DPROTO = {}
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
        if len(q) >= 3: FNSIZE[q[0].lower()] = int(q[2])
    if not os.path.exists(ORIGARGS) or "--regen" in sys.argv: subprocess.run([sys.executable, os.path.join(H, "origargs.py")], check=True)
    for l in open(ORIGARGS):
        q = l.split()
        ORIG[q[0]] = (int(q[1]), int(q[2]), [] if q[3] == "-" else q[3].split(","), set(q[4].split(",")))
    NAMEOF.update({v: k for k, v in NAMEVA.items()})
    if not os.path.exists(FPUSE_TXT) or "--regen" in sys.argv: gen_fpuse()
    for l in open(FPUSE_TXT):
        q = l.split(); FPUSE[(int(q[0], 16), int(q[1], 16))] = {int(x) for x in q[2].split(",")}
    for l in open(os.path.join(G, "output", "decls.h"), errors="replace"):
        m = re.match(r"\s*/\*([0-9a-fA-F]{8})\*/\s*extern\s+\w+\s+(\w+)\s*;", l)
        if m: GVAS.setdefault(m.group(2), set()).add(int(m.group(1), 16))
    if not os.path.exists(CALLSITES_TXT) or "--regen" in sys.argv: gen_callsites()
    for l in open(CALLSITES_TXT):
        q = l.rstrip("\n").split("\t")
        if len(q) == 5:
            CSITES.setdefault((q[0], q[1]), []).append((q[2], q[3], q[4].split("|") if q[4] else []))
            CSBYCALLEE.setdefault(q[1], []).append((q[0], q[2], q[3]))
    if not os.path.exists(INDIRECT) or "--regen" in sys.argv: gen_indirect()
    for l in open(INDIRECT):
        q = l.split()
        if len(q) == 4: IND.setdefault(q[0], {}).setdefault(int(q[1]), []).append((int(q[2]), q[3]))
    for r in csv.DictReader(open(os.path.join(G, "output", "regsigs.csv"))):
        REGVA[r["addr"].lower()] = r["regs"]; REGNAME[r["name"]] = r["addr"].lower()
    # class 20: register inputs of the ORIGINAL at its entry, union of every source (regsigs.csv missed 455e60: its reginputs.csv row had a merged 608-byte body)
    for r in csv.DictReader(open(os.path.join(G, "output", "regsigs.csv"))): REGIN.setdefault(r["addr"].lower(), set()).update(REGS_RE.findall(r["regs"]))
    for f_, col in (("reginputs2.csv", "reg_inputs"), ("reginputs_interior.csv", "regs")):
        p = os.path.join(G, "output", f_)
        if os.path.exists(p):
            for r in csv.DictReader(open(p)):
                if r.get("entry", "1") == "1" and r[col]: REGIN.setdefault(r["addr"].lower(), set()).update(REGS_RE.findall(r[col]))
    for l in open(os.path.join(G, "src", "aliases.txt")):
        q = l.split()
        if len(q) == 2: ALIASVA.setdefault(q[0], q[1].lower())
    for l in open(os.path.join(G, "src", "automatch_near_names.txt")):
        q = l.split()
        if len(q) == 3: ALIASVA[q[1]] = q[0].lower(); T2NAMES.add(q[1])
    for p in glob.glob(os.path.join(G, "output", "cluster_pilot*", "clusters.txt")):      # the cluster harness's register model of each root (column 4)
        for l in open(p):
            q = [x.strip() for x in l.split("|")]
            if len(q) >= 4 and not l.startswith("#"):
                va = NAMEVA.get(q[0]) or ALIASVA.get(q[0])
                if va and REGS_RE.findall(q[3]): REGIN.setdefault(va, set()).update(REGS_RE.findall(q[3]))
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


def selftest():
    """negative fixtures tools/negtests/defectlint/*.c: `// expect: class N` must produce class N, `// expect: clean class N` must not; every lintable class needs both (5 x87 precision and 17 framesize are not part of it)"""
    load_gt(); gd = load_decls(); bad = 0; seen = set()
    for p in sorted(glob.glob(os.path.join(H, "negtests", "defectlint", "*.c"))):
        t = open(p).read(); x = re.search(r"//\s*expect:\s*(clean\s+)?class\s+(\d+)", t); pm = re.search(r"//\s*path:\s*(\S+)", t)
        if not x: print("FAIL %s: no `// expect:` line" % os.path.basename(p)); bad += 1; continue
        rel = pm.group(1) if pm else "src/automatch_near/" + os.path.basename(p)
        got = sorted({int(f["cls"]) for f in Lint(rel, t, gd).run() + (tier1_hygiene(rel, t) if "/tier1/" in rel else [])})
        c = int(x.group(2)); ok = (c in got) != bool(x.group(1)); bad += not ok; seen.add((c, bool(x.group(1))))
        print("%s %-26s expect %s class %d, got %s" % ("ok  " if ok else "FAIL", os.path.basename(p), "no" if x.group(1) else "a", c, got))
    miss = [c for c in range(1, 23) if c not in (5, 17) and not {(c, True), (c, False)} <= seen]
    if miss: print("FAIL classes without a fixture pair: %s" % miss); bad += 1
    print("defectlint selftest: %d fixture(s), %s" % (len(seen), "OK" if not bad else "%d FAILED" % bad)); return 1 if bad else 0


def main():
    a = sys.argv[1:]; ref = None
    out_csv = os.environ.get("DEFECTLINT_OUT") or os.path.join(G, "output", "defectlint.csv"); quiet = False
    if "--selftest" in a: return selftest()
    if "--ref" in a: i = a.index("--ref"); ref = a[i + 1]; del a[i:i + 2]; out_csv = os.path.join(os.environ.get("TMPDIR", "/tmp/claude-%d" % os.getuid()), "defectlint_ref.csv")
    a_all = "--all" in a
    if a_all: a.remove("--all")
    if "--regen" in a: a.remove("--regen")
    if "--framesize" in a:      # class 17: separate run (needs the diffemu venv + object cache); `--all-tier2` = every automatch_near function, default rwp_*.  Output: output/frameaudit.csv
        a.remove("--framesize"); allow17 = set(); allowc = set()      # baselines output/frameaudit_2026-11-*.csv: `frame` = a callee:arg key (call level, tools/frameaudit.py) or the frame text (function level); status justified / fixed
        for bl in sorted(glob.glob(os.path.join(G, "output", "frameaudit_2026-11-*.csv"))):
            for r in csv.DictReader(open(bl)):
                if r["status"] in ("justified", "fixed"): (allowc.add((r["function"], r["frame"])) if re.fullmatch(r"[0-9a-f]{8}:a?\w+", r["frame"]) else allow17.add(r["function"]))
        rows = [r for r in framesize(a) if r["sev"] == "info" or ((r["function"], r["symbol"]) not in allowc if ":" in r["symbol"] else r["function"] not in allow17)]
        with open(os.path.join(G, "output", "frameaudit.csv"), "w", newline="") as fh:
            w = csv.DictWriter(fh, ["cls", "sev", "file", "function", "symbol", "text", "note"], extrasaction="ignore"); w.writeheader(); w.writerows(rows)
        bad = [r for r in rows if r["sev"] == "high"]
        for r in bad: print("%s: class 17 (framesize) %s: %s" % (r["file"], r["function"], r["text"]))
        print("defectlint framesize: %d function(s) compared, %d flagged -> output/frameaudit.csv" % (len(rows), len(bad))); return 0
    if "--quiet" in a: a.remove("--quiet"); quiet = True
    load_gt(); gd = load_decls()
    ENABLED.update(l.strip() for l in open(os.path.join(G, "src", "bridges", "enabled.txt")))
    CLENABLED.update(l.strip() for l in open(os.path.join(G, "src", "clusters", "enabled.txt")))
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
        if "/tier1/" in rel: finds += tier1_hygiene(rel, t)
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
                if n < mx and not exact_slots(va, n):      # the smaller declaration is exact (the original reads n slots): the larger one over-declares, harmless in cdecl
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
        tier = "cluster" if "/clusters/" in f["file"] else "bridge" if "/bridges" in f["file"] or "usercall" in f["file"] else "tier1" if "/tier1/" in f["file"] else "agent" if "automatch_agent" in f["file"] else "tier2" if "automatch_near" in f["file"] else "candidate"
        f["tier"] = tier
        f["byte_identical"] = "yes" if tier in ("tier1", "agent") else "no"
        f["diffemu"] = verdict.get(f["function"], "")
        f["enabled"] = ("yes" if f["function"].removesuffix("_c") in ENABLED else "no") if f["file"].startswith("src/bridges/") else ("yes" if re.sub(r"^cl_|\.c$", "", os.path.basename(f["file"])) in CLENABLED else "no") if tier == "cluster" else "yes" if tier in ("tier2", "tier1", "agent") else "no"
        rows.append(f)
    rows.sort(key=lambda r: (r["cls"], r["file"], r["line"]))
    cols = ["cls", "sev", "tier", "file", "line", "function", "symbol", "diffemu", "enabled", "byte_identical", "text", "note"]
    with open(out_csv, "w", newline="") as fh:
        w = csv.DictWriter(fh, cols, extrasaction="ignore"); w.writeheader(); w.writerows(rows)
    if out_csv == os.path.join(G, "output", "defectlint.csv"):      # T-L3: class 15 over EVERY scanned file (tier1 / agent byte-identical ones too, report only; these never fail check_all)
        with open(os.path.join(G, "output", "fp_width_mismatch.txt"), "w") as fh:
            for r in rows:
                if str(r["cls"]) == "15": fh.write("%s:%s [%s%s] %s: %s\n" % (r["file"], r["line"], r["tier"], ", byte-identical" if r["byte_identical"] == "yes" else "", r["function"], r["note"]))
    names = {1: "signed", 2: "floatbits", 3: "ftol", 4: "proto", 6: "vtable_conv", 7: "regcallee", 8: "krarity", 9: "framelayout", 10: "defarity", 11: "intfloat", 12: "nanclamp", 13: "mixedwidth", 14: "implicit", 15: "fpwidth", 16: "t1hygiene", 17: "framesize", 18: "nakedaddr", 19: "comargs", 20: "regentry", 21: "argkind", 22: "retwidth"}
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
