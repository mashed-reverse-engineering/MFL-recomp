#!/usr/bin/env python3
"""Variant search for the autocompiled functions that are not byte identical yet: apply ONE rewrite rule at ONE site (comparison constants, signedness of a variable,
`x != 0` forms, operand order, return type ...), compile all variants of a chunk in one cl call and keep every variant that is byte identical to the original.
Usage: variants.py <autocompile dir> <automatch.csv> <out dir> [jobs=6] [classes=STRUCTURE,OPERANDS]
Writes <out dir>/ok_NNN.c (the chunks with the winning variants substituted; use automatch.py / automatch_promote.py on that dir) and <out dir>/found.txt (function, rule)."""
import csv, glob, os, re, sys, collections
from concurrent.futures import ThreadPoolExecutor
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cmpfn
import subprocess
HERE = os.path.dirname(os.path.abspath(__file__))
ONLY = set(filter(None, os.environ.get("VARIANT_ONLY", "").split(",")))      # VARIANT_ONLY=cfirst,rmw_inline: run only these rules (a new rule over the near miss list)

def parse(lines):
    """-> list of (name, start, end): start = first signature line, end = closing brace line"""
    out, i = [], 0
    while i < len(lines):
        if lines[i] == "{":
            k = i - 1
            while k >= 0 and not lines[k].strip(): k -= 1
            s = k
            while s > 0 and lines[s - 1].strip() and not lines[s - 1].startswith(("/*", "#", "extern")): s -= 1
            j = i
            while j < len(lines) and lines[j] != "}": j += 1
            m = re.search(r"(\w+)\s*\(", " ".join(x.strip() for x in lines[s:k + 1]))
            if m: out.append((m.group(1), s, j))
            i = j + 1
        else:
            i += 1
    return out

SWAPOPS = ["==", "!=", "&&", "||", "+", "*", "&", "|"]
ATOM = r"[A-Za-z_]\w*(?:\[[^\]\[]*\])?(?:->\w+)?|0x[0-9a-fA-F]+|-?\d+"

def locals_rules(text):
    """locals as one struct / array (found by the matching agents: car_scale_velocity, clump_check_atomics_lookup, ai_route_target_for_car, clump_has_uv_anim): the stack layout of adjacent locals
    follows the declaration order of an aggregate.  The names stay valid through #define (undefined again after the function)"""
    L = text.split("\n")
    try: b = next(i for i, l in enumerate(L) if l == "{")
    except StopIteration: return
    decl = []
    for l in L[b + 1:]:
        m = re.match(r"^  ([A-Za-z_][\w ]*?\s*\**)\s*(\w+);$", l)
        if not m: break
        decl.append((m.group(1).strip(), m.group(2)))
    if len(decl) < 2 or len(decl) > 8: return
    rest = L[b + 1 + len(decl):]
    undef = "\n".join("#undef %s" % n for _, n in decl)
    types = {t for t, _ in decl}
    if len(types) == 1:
        t = decl[0][0]
        defs = ["  %s lcl_[%d];" % (t, len(decl))] + ["#define %s lcl_[%d]" % (n, i) for i, (_, n) in enumerate(decl)]
        yield "locals_array", "\n".join(L[:b + 1] + defs + rest) + "\n" + undef
    sd = ["  struct { %s } lcl_;" % " ".join("%s %s;" % (t, n) for t, n in decl)] + ["#define %s lcl_.%s" % (n, n) for _, n in decl]
    yield "locals_struct", "\n".join(L[:b + 1] + sd + rest) + "\n" + undef


def ifelse_rules(text):
    """if (c) { A } else { B }  ->  if (!c) { B } else { A } (Ghidra's layout: `}` / `else {` on separate lines).  Found by the triage: the jump polarity (je vs jne) of a whole if/else differs (triggers_update_dispatch)"""
    L = text.split("\n")
    for i, l in enumerate(L):
        m = re.match(r"^(\s*)if \((.*)\) \{$", l)
        if not m: continue
        ind, cond = m.group(1), m.group(2)
        j = next((k for k in range(i + 1, len(L)) if L[k] == ind + "}"), None)
        if j is None or j + 2 >= len(L) or L[j + 1] != ind + "else {": continue
        e = next((k for k in range(j + 2, len(L)) if L[k] == ind + "}"), None)
        if e is None: continue
        mc = re.match(r"^(.+?) (==|!=) (.+)$", cond)
        if mc and not re.search(r"&&|\|\|", cond): inv = "%s %s %s" % (mc.group(1), "!=" if mc.group(2) == "==" else "==", mc.group(3))
        elif re.fullmatch(r"!\w+", cond): inv = cond[1:]
        elif re.fullmatch(r"\w+", cond): inv = "!" + cond
        else: inv = "!(%s)" % cond
        yield "ifelse_swap", "\n".join(L[:i] + [ind + "if (" + inv + ") {"] + L[j + 2:e] + [ind + "}", ind + "else {"] + L[i + 1:j] + [ind + "}"] + L[e + 1:])


FLT = r"-?\d+\.\d+(?:e[-+]?\d+)?f?"

def lever_rules(text, pre):
    """levers found by a Haiku agent (README 'Matching toolbox'); pre = the chunk preamble (declarations of the globals / callees)"""
    import struct
    floatg = set(re.findall(r"extern float (\w+);", pre))
    dbl = set(re.findall(r"extern (?:double|float10) (?:__\w+ )?(\w+)\(", pre))
    # (1) retype_f: a float global stored with a constant: the original is `mov [g], imm32` (the global is undefined4 in the source).  The variant writes the store through an undefined4 view;
    #     install_levers.py turns the winner into the retyped declaration `extern undefined4 g;  g = 0x3f800000;`
    for m in re.finditer(r"^(\s*)(\w+) = (%s|0x[0-9a-fA-F]{8}|0);" % FLT, text, re.M):
        if m.group(2) not in floatg: continue
        v = m.group(3)
        bits = int(v, 16) if v.startswith("0x") else (0 if v == "0" else struct.unpack("<I", struct.pack("<f", float(v.rstrip("f"))))[0])
        yield "retype_f:" + m.group(2), text[:m.start()] + "%s*(undefined4 *)&%s = 0x%08x;" % (m.group(1), m.group(2), bits) + text[m.end():]
    def allbits(m):
        v = m.group(3)
        if m.group(2) not in floatg: return m.group(0)
        bits = int(v, 16) if v.startswith("0x") else (0 if v == "0" else struct.unpack("<I", struct.pack("<f", float(v.rstrip("f"))))[0])
        return "%s*(undefined4 *)&%s = 0x%08x;" % (m.group(1), m.group(2), bits)
    allv = re.sub(r"^(\s*)(\w+) = (%s|0x[0-9a-fA-F]{8}|0);" % FLT, allbits, text, flags=re.M)
    if allv != text: yield "retype_f:all", allv      # several stores (spring_grid_reset_constants): all float globals together
    # (3) fflip: float compare with the operands swapped (`k < g` -> `g > k`) for operands the generic flip does not see: float literals, casts, calls
    FA = r"%s|\(float\)\w+|\w+\([^()]*\)|[A-Za-z_]\w*(?:\[[^\]\[]*\])?(?:->\w+)?" % FLT
    for m in re.finditer(r"(?<![\w)])(%s) (<=|>=|<|>) (%s)(?=[ )&|;])" % (FA, FA), text):
        flip = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}[m.group(2)]
        yield "fflip", text[:m.start()] + "%s %s %s" % (m.group(3), flip, m.group(1)) + text[m.end():]
    # (4) icmp: signed compare of an undefined4 / uint global or variable: cast only the compared operand (gsign casts every use, which breaks `(int)g = x`)
    intv = set(re.findall(r"\b(?:undefined4|uint|int|ulong)\s+\**(\w+)\s*[;,=)]", pre + text))
    for m in re.finditer(r"(?<![\w.>)])([A-Za-z_]\w*) (<=|>=|<|>) (?=[\w(-])", text):
        if m.group(1) not in intv: continue
        for c in ("int", "uint"): yield "icmp", text[:m.start()] + "(%s)%s %s " % (c, m.group(1), m.group(2)) + text[m.end():]
    for m in re.finditer(r" (<=|>=|<|>) ([A-Za-z_]\w*)(?![\w(\[.>])", text):
        if m.group(2) not in intv: continue
        for c in ("int", "uint"): yield "icmp", text[:m.start()] + " %s (%s)%s" % (m.group(1), c, m.group(2)) + text[m.end():]
    # (5) basept: the base pointer of `(char *)&g + idx + off` in a local so the compiler cannot fold the constant offset into the addressing mode
    b = text.find("\n{\n")
    if b >= 0:
        for g in sorted(set(re.findall(r"\(char \*\)&(\w+)", text))):
            if len(re.findall(r"\(char \*\)&%s\b" % g, text)) < 2: continue
            body = re.sub(r"\(char \*\)&%s\b" % g, "bp_", text[b + 3:])
            yield "basept", text[:b + 3] + "  char *bp_ = (char *)&%s;\n" % g + body
    # (6) dblloc: a callee that returns double, compared with a float constant: take the result into a float local first
    for m in re.finditer(r"^(\s*)(if|while) \((\w+)(\([^()]*\)) (<=|>=|<|>|==|!=) ([^()&|;]+?)\)", text, re.M):
        if m.group(3) not in dbl or b < 0: continue
        new = text[:m.start()] + "%s%s = (float)%s%s;\n%s%s (fv_ %s %s)" % (m.group(1), "fv_", m.group(3), m.group(4), m.group(1), m.group(2), m.group(5), m.group(6)) + text[m.end():]
        yield "dblloc", new.replace("\n{\n", "\n{\n  float fv_;\n", 1)

    # (6b) dblloc: Ghidra's float10 local that holds a double callee's result: a plain float local, `(float)call`, no float10 casts on the other operands
    if "float10 " in text and re.search(r"\(float10\)\w+\(", text):
        n = re.sub(r"\(float10\)(\w+\()", r"(float)\1", text)
        n = re.sub(r"\(float10\)", "", n.replace("float10 ", "float "))
        yield "dblloc", n

def rules(text, pre=""):
    """yield (rule name, new text) for every single edit"""
    yield from lever_rules(text, pre)
    yield from ifelse_rules(text)
    # constant first comparisons (`0xf < x`): the constant adjusted forms `x >= 0x10`, `x > -1` -> `x >= 0` (the compiler picks cmp/jcc by the written form)
    for m in re.finditer(r"(?<![\w)])(-?\d+|0x[0-9a-fA-F]+) (<=|>=|<|>) ([A-Za-z_]\w*(?:\[[^\]\[]*\])?(?:->\w+)?)(?=[ )&|;])", text):
        v = int(m.group(1), 0)
        alt = {"<": (">=", v + 1), "<=": (">", v - 1), ">": ("<=", v - 1), ">=": ("<", v + 1)}[m.group(2)]
        yield "cfirst", text[:m.start()] + "%s %s %d" % (m.group(3), alt[0], alt[1]) + text[m.end():]
    # read-modify-write through a temporary pointer: `p = (uint *)(EXPR); *p = *p | C;` -> `*(uint *)(EXPR) |= C;` (or `*p |= C;`): the original does `or [reg+off], C`
    for m in re.finditer(r"^(\s*)(\w+) = (\((?:u?int|uint|undefined4|ushort|byte|u?short|float) \*\)\(.*\));\n\s*\*\2 = \*\2 (\||&|\+|-|\^) ([^;]+);", text, re.M):
        yield "rmw_inline", text[:m.start()] + "%s*%s %s= %s;" % (m.group(1), m.group(3), m.group(4), m.group(5)) + text[m.end():]
        yield "rmw_compound", text[:m.start()] + "%s%s = %s;\n%s*%s %s= %s;" % (m.group(1), m.group(2), m.group(3), m.group(1), m.group(2), m.group(4), m.group(5)) + text[m.end():]
    # agent_fixes: a bool return type -> int, return 0 / 1 (controller_present_004a70c0)
    mb = re.match(r"(\s*)bool (\w+\()", text)
    if mb: yield "bool_ret", re.sub(r"\breturn false;", "return 0;", re.sub(r"\breturn true;", "return 1;", text[:mb.start()] + "%sint %s" % (mb.group(1), mb.group(2)) + text[mb.end():]))
    if "locals" in os.environ.get("FIX_RULES", "widths,ret_wide,conv,locals,imports,idxscale").split(","): yield from locals_rules(text)      # rule "locals": measured by tools/rule_ab.sh
    for m in re.finditer(r"\b([A-Za-z_]\w*) (<=|>=|<|>) (-?\d+|0x[0-9a-fA-F]+)\b", text):
        v = int(m.group(3), 0)
        alts = {"<=": ("<", v + 1), "<": ("<=", v - 1), ">": (">=", v + 1), ">=": (">", v - 1)}[m.group(2)]
        yield "cmpconst", text[:m.start()] + "%s %s %d" % (m.group(1), alts[0], alts[1]) + text[m.end():]
    for m in re.finditer(r"\b(uint|int|undefined4|ulong|undefined2|ushort|short|char|uchar|byte)\s+(\**[A-Za-z_]\w*)\s*([;,)=])", text):
        for new in {"uint": ["int"], "int": ["uint"], "undefined4": ["int", "uint"], "ulong": ["int"], "undefined2": ["short", "ushort"], "ushort": ["short"], "short": ["ushort"],
                    "char": ["uchar"], "uchar": ["char"], "byte": ["char"]}[m.group(1)]:
            yield "sign", text[:m.start()] + "%s %s%s" % (new, m.group(2), m.group(3)) + text[m.end():]
    for m in re.finditer(r"\b([A-Za-z_]\w*(?:\[[^\]\[]*\])?(?:->\w+)?) != 0\b", text):
        yield "ne0", text[:m.start()] + m.group(1) + text[m.end():]
    for m in re.finditer(r"\b([A-Za-z_]\w*(?:\[[^\]\[]*\])?(?:->\w+)?) == 0\b", text):
        yield "eq0", text[:m.start()] + "!" + m.group(1) + text[m.end():]
    for m in re.finditer(r"\(!([A-Za-z_]\w*)\)", text):
        yield "not", text[:m.start()] + "(%s == 0)" % m.group(1) + text[m.end():]
    for m in re.finditer(r"(%s) (%s) (%s)" % (ATOM, "|".join(re.escape(o) for o in SWAPOPS), ATOM), text):
        yield "swap", text[:m.start()] + "%s %s %s" % (m.group(3), m.group(2), m.group(1)) + text[m.end():]
    for m in re.finditer(r"(%s) (<|>|<=|>=) (%s)" % (ATOM, ATOM), text):
        flip = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}[m.group(2)]
        yield "flip", text[:m.start()] + "%s %s %s" % (m.group(3), flip, m.group(1)) + text[m.end():]
    # signedness of a compared global / variable: cast the whole function's uses of it (a uint global compared with 0 gives jbe where the original has jle: the fix belongs in the type of the global)
    cmpvars = set(re.findall(r"\b([A-Za-z_]\w*) (?:<=|>=|<|>) ", text)) | set(re.findall(r" (?:<=|>=|<|>) ([A-Za-z_]\w*)\b", text))
    for v in sorted(cmpvars):
        if v in ("int", "uint", "float", "char"): continue
        for cast in ("int", "uint"):
            new = re.sub(r"(?<![\w.>])%s\b(?!\s*\()" % re.escape(v), "((%s)%s)" % (cast, v), text)
            if new != text: yield ("gsign:%s:%s" % (cast, v) if v.startswith("g_") else "csign"), new
    m = re.match(r"(\s*)undefined4 (\w+\()", text)
    if m:
        for new in ("int", "uint"):
            yield "rettype", text[:m.start()] + "%s%s %s" % (m.group(1), new, m.group(2)) + text[m.end():]
    for m in re.finditer(r"\bif \((\w+)\) \{\n(\s+)return ([^;]+);\n(\s+)\}\n(\s+)return ([^;]+);", text):    # if (c) { return a; } return b;  ->  return c ? a : b
        yield "ret_ternary", text[:m.start()] + "return %s ? %s : %s;" % (m.group(1), m.group(3), m.group(6)) + text[m.end():]

def work(args):
    chunk, cdir, want, outdir = args
    path = os.path.join(cdir, chunk)
    lines = open(path, errors="replace").read().split("\n")
    funcs = parse(lines)
    names = {f[0] for f in funcs}
    pre_end = funcs[0][1] if funcs else len(lines)
    def sig_of(f):
        k = next(k for k in range(f[1], f[2] + 1) if lines[k] == "{")
        return " ".join(x.strip() for x in lines[f[1]:k] if x.strip())
    sigs = {f[0]: sig_of(f) for f in funcs}
    pre_text = "\n".join(lines[:pre_end])
    tu = lines[:pre_end] + ["extern " + sg + ";" for sg in sigs.values()]
    items, src_of = [], {}
    n = 0
    for name, s, e in funcs:
        if name not in want: continue
        text = "\n".join(lines[s:e + 1])
        seen = {text}
        for rule, new in rules(text, pre_text):
            if ONLY and rule.split(":")[0] not in ONLY: continue
            if new in seen: continue
            seen.add(new)
            vn = "%s__v%d" % (name, n); n += 1
            new = re.sub(r"\b%s\s*\(" % re.escape(name), vn + "(", new, count=1)
            tu.append(new); items.append((vn, want[name])); src_of[vn] = (name, rule, "\n".join(lines[s:e + 1]), new.replace(vn + "(", name + "(", 1))
    if not items: return chunk, [], 0
    vp = os.path.join(cdir, "var_" + chunk)
    # a rewrite can make invalid C and one error stops the whole compile: syntax check, drop the variants that contain an error line, repeat
    vars_ = [(vn, t) for (vn, _), t in zip(items, tu[len(tu) - len(items):])]
    head = tu[:len(tu) - len(items)]
    for _ in range(8):
        text = "\n".join(head + [t for _, t in vars_])
        open(vp, "w").write(text + "\n")
        r = subprocess.run([os.path.join(HERE, "vc7.sh"), cdir, "cl.exe", "/nologo", "/c", "/Zs", "/W0", os.path.basename(vp)], capture_output=True, text=True, timeout=180)
        errs = sorted({int(m.group(1)) for m in re.finditer(r"\((\d+)\) : error", r.stdout + r.stderr)})
        if not errs: break
        starts, ln = [], len(head)
        for vn, t in vars_:
            starts.append((ln + 1, ln + t.count("\n") + 1, vn)); ln += t.count("\n") + 1
        bad = {vn for e in errs for (lo, hi, vn) in starts if lo <= e <= hi}
        if not bad: return chunk, [], 0                      # an error in the preamble: give up on this chunk
        vars_ = [(vn, t) for vn, t in vars_ if vn not in bad]
        if not vars_: os.unlink(vp); return chunk, [], 0
    keep = {vn for vn, _ in vars_}
    items = [(vn, va) for vn, va in items if vn in keep]
    res = cmpfn.check(vp, items, ["/O2"])
    os.unlink(vp)
    found = {}
    for vn, (ok, det) in res.items():
        if ok and src_of[vn][0] not in found: found[src_of[vn][0]] = src_of[vn]
    if found:
        text = "\n".join(lines)
        for name, (nm, rule, old, new) in found.items():
            text = text.replace(old, new, 1)
        open(os.path.join(outdir, chunk), "w").write(text)
    return chunk, [(v[0], v[1]) for v in found.values()], len(items)

def main():
    cdir, csvp, outdir = sys.argv[1:4]
    jobs = int(sys.argv[4]) if len(sys.argv) > 4 else 6
    classes = (sys.argv[5] if len(sys.argv) > 5 else "STRUCTURE,OPERANDS").split(",")
    os.makedirs(outdir, exist_ok=True)
    want = collections.defaultdict(dict)
    for r in csv.reader(open(csvp)):
        if len(r) >= 4 and r[3] in classes: want[r[0]][r[1]] = int(r[2], 16)
    for h in ("prelude.h", "ghidra_types.h"):
        if os.path.exists(os.path.join(cdir, h)):
            import shutil; shutil.copy(os.path.join(cdir, h), os.path.join(outdir, h))
    with ThreadPoolExecutor(jobs) as ex:
        results = list(ex.map(work, [(c, cdir, w, outdir) for c, w in sorted(want.items())]))
    tot = sum(len(r[1]) for r in results)
    with open(os.path.join(outdir, "found.txt"), "w") as o:
        for chunk, fs, n in results:
            for name, rule in fs: o.write("%s %s %s\n" % (chunk, name, rule))
    print("%d variants compiled, %d functions became byte identical" % (sum(r[2] for r in results), tot))
    print(collections.Counter(rule for _, fs, _ in results for _, rule in fs))

main()
