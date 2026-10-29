"""/GS stack-cookie functions: Ghidra shows `uint unaff_retaddr; ... local_N = __security_cookie ^ unaff_retaddr;` (the check is hidden in an
injection comment).  The compiler regenerates both with /GS, so the C text drops them and carries the marker GS_MARK, which
build_overlay.py / diffemu.compile_obj turn into cl /GS for that file."""
import re
GS_MARK = "/* compile: /GS */"
def strip(body):
    """-> body without the cookie statements + marker, or unchanged when the function does not have exactly the plain cookie pattern"""
    m = re.search(r"^[ \t]*(\w+) = __security_cookie \^ unaff_retaddr;[ \t]*\n", body, re.M)
    if not m or body.count("unaff_retaddr") != 2: return body
    v = m.group(1)
    if len(re.findall(r"\b%s\b" % v, body)) != 2: return body      # declaration + the assignment only (a use of the cookie slot elsewhere is not the plain pattern)
    b = body[:m.start()] + body[m.end():]
    b, k = re.subn(r"^[ \t]*\w+ unaff_retaddr;[ \t]*\n", "", b, flags=re.M)
    b, j = re.subn(r"^[ \t]*[\w ]+ %s;[ \t]*\n" % v, "", b, flags=re.M)
    if k != 1 or j != 1: return body
    return GS_MARK + "\n" + b
WIN32 = {"GetDlgItem": 2, "GetCurrentDirectoryA": 2, "DialogBoxParamA": 5, "LoadBitmapA": 2, "GetVersionExA": 1, "lstrcmpiA": 2, "OutputDebugStringA": 1, "SetWindowTextA": 2, "ShowWindow": 2, "SetWindowLongA": 3,
         "SetWindowPos": 7, "RegOpenKeyExA": 5, "RegCloseKey": 1, "CharUpperA": 1, "InterlockedExchange": 2, "SendMessageA": 4, "SendDlgItemMessageA": 5, "EndDialog": 2, "GetWindowTextA": 3, "SetDlgItemTextA": 3,
         "CheckDlgButton": 3, "IsDlgButtonChecked": 2, "EnableWindow": 2, "GetWindowLongA": 2, "LoadStringA": 4, "MessageBoxA": 4, "GetModuleFileNameA": 3, "CloseHandle": 1, "SetFocus": 1, "GetDC": 1, "ReleaseDC": 2}      # Win32 API arities (names/MFL_import_protos.txt votes the pushes in front of the IAT call and is wrong when a push belongs to an enclosing call)
ARITY = {"RwStreamClose": 2, "RwStreamRead": 3, "SUB_0045dc10": 1, "gamesave_cb_dispatch_b": 0}      # library callees whose Ghidra signature swallowed the caller's frame (unaff_ESI, unaff_retaddr, param_1.. as arguments)
JUNK = re.compile(r"^(?:unaff_\w+|in_stack_\w+|extraout_\w+)$")
def _call_args(b, i):
    """b[i] == '(' -> list of (start, end) of the top level arguments, index of the closing ')'"""
    d = 0; st = i + 1; out = []
    for k in range(i, len(b)):
        c = b[k]
        if c in "([{": d += 1
        elif c in ")]}":
            d -= 1
            if d == 0: out.append((st, k)); return out, k
        elif c == "," and d == 1: out.append((st, k)); st = k + 1
    return None, -1
def junkargs(body):
    """Ghidra gives a callee with a wrong (huge) signature the frame of its caller as arguments: `RwStreamClose(stream, 0, in_stack_fffffff8, uVar3, unaff_retaddr, param_1, .. param_14)`.  The call keeps its real
    arity (ARITY) or loses everything from the first unaff_ / in_stack_ / extraout_ argument; then declarations nobody uses any more (unaff_ESI, in_stack_..) and trailing unused `undefined4 param_N` parameters of the function go."""
    changed = False
    for m in list(re.finditer(r"(?<![\w.>])(\w+)\s*\(", body))[::-1]:
        if m.group(1) in WIN32: continue
        al, close = _call_args(body, m.end() - 1)
        if not al or m.group(1) in ("if", "while", "for", "switch", "return", "sizeof"): continue
        a = [body[s:e].strip() for s, e in al]; k = ARITY.get(m.group(1))
        if k is None or len(a) <= k:
            k = next((i for i, x in enumerate(a) if JUNK.match(x)), None)
            if k is None or k == 0 or len(a) < 3: continue
        if len(a) <= k: continue
        body = (body[:al[k - 1][1]] if k else body[:al[0][0]]) + body[close:]; changed = True
    if not changed: return body
    for v in set(re.findall(r"^[ \t]+[\w ]+ ((?:unaff_|in_stack_|extraout_)\w+);[ \t]*\n", body, re.M)):
        if len(re.findall(r"\b%s\b" % v, body)) == 1: body = re.sub(r"^[ \t]+[\w ]+ %s;[ \t]*\n" % v, "", body, flags=re.M)
    h = re.search(r"\(([^(){};]*\bparam_\d+)\)\s*\{", body)
    if h:
        ps = [" ".join(x.split()) for x in h.group(1).split(",")]
        while ps and re.fullmatch(r"[\w ]+ (param_\d+)", ps[-1]) and len(re.findall(r"\b%s\b" % ps[-1].split()[-1], body)) == 1: ps.pop()
        body = body[:h.start()] + "(" + (",".join(ps) if ps else "void") + ")\n{" + body[h.end():]
    return body
def clean(body): return strip(junkargs(body))
if __name__ == "__main__":
    import sys
    t = open("output/named/MFL_app_named.c", errors="replace").read(); ok = 0; n = 0
    for p in re.split(r"^// ==== ", t, flags=re.M):
        if "unaff_retaddr" in p:
            n += 1; ok += strip(p) != p
    print(ok, "of", n)

# ---------------------------------------------------------------------------------------------------------------- stack buffers Ghidra split into scalars
_SZ = {"char": 1, "undefined1": 1, "undefined": 1, "byte": 1, "uchar": 1, "RwChar": 1, "bool": 1, "short": 2, "ushort": 2, "undefined2": 2, "word": 2, "int": 4, "uint": 4, "undefined4": 4, "float": 4, "dword": 4, "long": 4, "ulong": 4}
_LOC = re.compile(r"^  ((?:unsigned )?\w+) (\w*?(?:local_|Stack_)([0-9a-f]+))(?: ?\[(\d+)\])?;[ \t]*$", re.M)
def gsbuf(t):
    """/GS protects arrays (>= 5 bytes) only.  Ghidra prints a char buffer as scalars (`char local_44; undefined4 local_43;`, no array) or as a `frame` struct of small members: cl then emits no cookie check
    (orig calls __security_check_cookie, ours does not).  Contiguous scalar locals become one `char name[N]` (N up to the next buffer / the cookie at local_4; value uses go through a cast into the array),
    a `frame` struct becomes `union { char gs_buf[N]; struct {..} m; } frame` (members `frame.m.x`).  -> text (unchanged when nothing applies)"""
    h = re.search(r"\n\{\n", t)
    if not h: return t
    head, body = t[:h.end()], t[h.end():]
    fr = re.search(r"^  struct \{\n((?:    .*\n)+?)  \} frame;\n", body, re.M)
    if fr:
        xs = [int(m.group(3), 16) for m in re.finditer(r"^    (?:unsigned )?\w+ (\w*?(?:local_|Stack_)([0-9a-f]+)) ?(?:\[\d+\])?;", fr.group(1), re.M) for m in [re.match(r"(.*)", m.group(0)) and re.search(r"((?:local_|Stack_)([0-9a-f]+))", m.group(0)) and re.search(r"(\w*?)((?:local_|Stack_)([0-9a-f]+))", m.group(0))] if False] or \
             [int(x, 16) for x in re.findall(r"^    (?:unsigned )?\w+ \w*?(?:local_|Stack_)([0-9a-f]+) ?(?:\[\d+\])?;", fr.group(1), re.M)]
        if not xs or max(xs) < 8: return t
        n = max(xs) - 4
        body = body[:fr.start()] + "  union {\n    char gs_buf[%d];\n    struct {\n%s    } m;\n  } frame;\n" % (n, "".join("  " + l + "\n" for l in fr.group(1).splitlines())) + body[fr.end():]
        body = re.sub(r"\bframe\.(?!m\.|gs_buf)", "frame.m.", body)
        return head + body
    decl = [(int(m.group(3), 16), m) for m in _LOC.finditer(body) if m.group(1) in _SZ]
    if not decl: return t
    decl.sort(key=lambda d: -d[0]); groups = []
    for X, m in decl:
        sz = (int(m.group(4)) if m.group(4) else 1) * _SZ[m.group(1)]
        if groups and groups[-1][-1][0] - groups[-1][-1][1] == X: groups[-1].append((X, sz, m))
        else: groups.append([(X, sz, m)])
    groups = [[(x, s, m) for x, s, m in g] for g in groups]
    # (X, size, match) -> contiguity above compares previous X - previous size == X
    edits = []; subs = []
    for gi, g in enumerate(groups):
        if len(g) < 2 and not g[0][2].group(4) and not re.search(r"&\s*%s\b" % g[0][2].group(2), body): continue      # a lone scalar only when its address is taken (sprintf(&local_c, ..))
        X0 = g[0][0]; nxt = groups[gi + 1][0][0] if gi + 1 < len(groups) else 4
        if nxt >= X0 or X0 - nxt < 5: continue
        name = g[0][2].group(2)
        for x, s, m in g: subs.append((m.group(2), X0 - x, m.group(1), name, bool(m.group(4))))
        edits.append((g, "  char %s[%d];" % (name, X0 - nxt)))
    if not edits: return t
    for g, new in sorted(edits, key=lambda e: -e[0][0][2].start()):
        for k, (x, s, m) in reversed(list(enumerate(g))):
            body = body[:m.start()] + (new if k == 0 else "\0DEL\0") + body[m.end():]
    body = re.sub(r"[ \t]*\0DEL\0\n?", "", body)
    ph = []
    for nm, d, ty, base, isarr in subs:
        where = base if d == 0 else "(%s + %d)" % (base, d); ph.append(where); k = "\x01%d\x01" % (len(ph) - 1)
        if isarr and nm == base: continue
        body = re.sub(r"&\s*%s\b" % nm, k if _SZ[ty] == 1 or isarr else "((%s *)%s)" % (ty, k), body)
        if isarr: body = re.sub(r"(?<![\w.\x01])%s\b" % nm, k, body)
        else: body = re.sub(r"(?<![\w.\x01])%s\b(?!\s*\[)" % nm, "(*(%s *)%s)" % (ty, k), body)
    body = re.sub("\x01(\\d+)\x01", lambda m: ph[int(m.group(1))], body)
    return head + body
