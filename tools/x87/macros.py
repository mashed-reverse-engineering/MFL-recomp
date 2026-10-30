#!/usr/bin/env python3
"""macros.py (T-X91, RWP-X28): rewrite lifted C (tools/x87/lift.py text) with the RenderWare SDK vector macros of src/mfl_tier2.h.
  1. `float *aN` parameters that are only indexed 0..2 become `RwV3d *aN` (aN[1] -> aN->y);
  2. three consecutive statements that form a sub / add / scale / cross product (either operand role) become RwV3dSubMacro / AddMacro / ScaleMacro / CrossProductMacro;
     the three scalar targets (locals fN or fv[k..k+2]) are grouped into one `RwV3d` local (all other uses -> v.x/.y/.z);
  3. a sum of three products over two vectors becomes RwV3dDotProductMacro(a, b) (operand roles tried as written).
macroize(src) returns the new text (or src when nothing matched). Used as an extra variant by run.py; the caller keeps it only when cl says it is byte identical."""
import re, sys
XYZ = "xyz"
ID = r"[A-Za-z_]\w*"
# a vector operand after step 1/2: `p->x`, `v.x`
def comp(k): return r"(%s)(?:->|\.)%s" % (ID, XYZ[k])

def retype_params(src):
    m = re.search(r"^(\w[\w ]*?)\s+(\w+)\(([^)]*)\)\s*\n\{", src, re.M)
    if not m: return src
    params = m.group(3)
    for p in re.findall(r"float \*(%s)" % ID, params):
        idx = re.findall(r"\b%s\[(\d+)\]" % p, src)
        uses = len(re.findall(r"\b%s\b" % p, src)) - 1       # minus the declaration
        if idx and all(int(i) < 3 for i in idx) and len(idx) <= uses:
            src = re.sub(r"\b%s\[(\d)\]" % p, lambda mm: "%s->%s" % (p, XYZ[int(mm.group(1))]), src)
            src = re.sub(r"float \*%s\b" % p, "RwV3d *%s" % p, src, count=1)
    return src

def split_fn(src):
    i = src.index("{\n") + 2; j = src.rindex("}")
    return src[:i], src[i:j], src[j:]

def stmt_lines(body):
    return body.split("\n")

SCAL = r"(%s(?:\[\d+\])?)"      # scalar target: fN or fv[k]

def operand(txt, k):
    """('p', k) for `p->x`/`v.x` at component k"""
    m = re.fullmatch(comp(k), txt.strip())
    return m.group(1) if m else None

def parse_triplet(L3):
    """L3: three lines `T = rhs;`  -> (kind, a, b/s, targets) or None"""
    T = []; R = []
    for l in L3:
        m = re.fullmatch(r"\s*(%s(?:\[\d+\])?) = (.*);" % ID, l)
        if not m: return None
        T.append(m.group(1)); R.append(m.group(2))
    if len(set(T)) != 3: return None
    for op, kind in (("-", "sub"), ("+", "add")):
        P = []
        for k in range(3):
            m = re.fullmatch(r"(%s)(?:->|\.)%s %s (%s)(?:->|\.)%s" % (ID, XYZ[k], re.escape(op), ID, XYZ[k]), R[k])
            if not m: break
            P.append((m.group(1), m.group(2)))
        if len(P) == 3 and len({p for p in P}) == 1: return (kind, P[0][0], P[0][1], T)
    P = []
    for k in range(3):
        m = re.fullmatch(r"(%s)(?:->|\.)%s \* (.+)" % (ID, XYZ[k]), R[k])
        if not m: break
        P.append((m.group(1), m.group(2)))
    if len(P) == 3 and len({p for p in P}) == 1 and re.fullmatch(r"[\w.>\-\[\]]+|[0-9.]+f", P[0][1]): return ("scale", P[0][0], P[0][1], T)
    # cross: x = a.y*b.z - a.z*b.y ; y = a.z*b.x - a.x*b.z ; z = a.x*b.y - a.y*b.x   (a, b in either role: the products of the lifted text may be swapped)
    c = [(1, 2), (2, 0), (0, 1)]; PR = []
    for k in range(3):
        i, j = c[k]
        m = re.fullmatch(r"\(?(%s)(?:->|\.)(\w) \* (%s)(?:->|\.)(\w)\)? - \(?(%s)(?:->|\.)(\w) \* (%s)(?:->|\.)(\w)\)?" % (ID, ID, ID, ID), R[k])
        if not m: return None
        PR.append(m.groups())
    for ab in (0, 1):
        ok = True; names = None
        for k in range(3):
            i, j = c[k]; g = PR[k]
            # term1 = a.i * b.j, term2 = a.j * b.i  (first factor from a)
            t1 = (g[0], g[1], g[2], g[3]); t2 = (g[4], g[5], g[6], g[7])
            if ab: t1 = (t1[2], t1[3], t1[0], t1[1]); t2 = (t2[2], t2[3], t2[0], t2[1])
            if not (t1[1] == XYZ[i] and t1[3] == XYZ[j] and t2[1] == XYZ[j] and t2[3] == XYZ[i] and t1[0] == t2[0] and t1[2] == t2[2]): ok = False; break
            if names is None: names = (t1[0], t1[2])
            elif names != (t1[0], t1[2]): ok = False; break
        if ok: return ("cross", names[0], names[1], T)
    return None

def vref(trip):
    """three component strings -> C pointer expression of one RwV3d (`p->x,p->y,p->z` -> p, `v.x,v.y,v.z` -> &v) or None"""
    m = [re.fullmatch(r"(%s)(->|\.)%s" % (ID, XYZ[k]), t.strip()) for k, t in enumerate(trip)]
    if not all(m) or len({(x.group(1), x.group(2)) for x in m}) != 1: return None
    return m[0].group(1) if m[0].group(2) == "->" else "&" + m[0].group(1)

def comp_of(t):
    m = re.fullmatch(r"(%s)(->|\.)([xyz])" % ID, t.strip())
    return (m.group(1), m.group(2), m.group(3)) if m else None

def dot_macro(rhs):
    """`A.y * B.y + A.x * B.x + A.z * B.z` (any term order) -> RwV3dDotProductMacro(&A, &B) or None"""
    terms = rhs.strip().split(" + ")
    if len(terms) != 3: return None
    terms = [t.strip("()") if t.count("(") != t.count(")") else t for t in terms]
    P = []
    for t in terms:
        m = re.fullmatch(r"(\S+) \* (\S+)", t)
        if not m: return None
        a, b = comp_of(m.group(1)), comp_of(m.group(2))
        if not a or not b or a[2] != b[2]: return None
        P.append((a, b))
    if sorted(p[0][2] for p in P) != list("xyz") or len({(p[0][0], p[0][1]) for p in P}) != 1 or len({(p[1][0], p[1][1]) for p in P}) != 1: return None
    A, B = P[0][0], P[0][1]
    f = lambda v: v[0] if v[1] == "->" else "&" + v[0]
    return "RwV3dDotProductMacro(%s, %s)" % (f(A), f(B))

def copies_to_dot(body):
    """`fA = P->y; fB = P->x; fC = P->z; ... f4 = fB * fB + fA * fA + fC * fC;` (the lifter keeps the loaded components of a vector on the x87 stack) -> f4 = RwV3dDotProductMacro(P, P);
    the copy statements that become unused are dropped (the macro form re-reads the memory; cl removes the dead loads, RWP-X28)"""
    lines = body.split("\n"); cp = {}
    for l in lines:
        m = re.fullmatch(r"\s*(f\d+) = (%s(?:->|\.)[xyz]);" % ID, l)
        if m: cp[m.group(1)] = m.group(2)
    if len(cp) < 3: return body
    O = r"[\w.\[\]>\-]+"
    changed = False
    for i, l in enumerate(lines):
        def sub(m):
            inner = re.sub(r"\bf\d+\b", lambda q: cp.get(q.group(0), q.group(0)), m.group(0))
            return dot_macro(inner) or m.group(0)
        n = re.sub(r"%s \* %s \+ %s \* %s \+ %s \* %s" % ((O,) * 6), sub, l)
        if n != l: lines[i] = n; changed = True
    if not changed: return body
    text = "\n".join(lines)
    keep = [l for l in lines if not (re.fullmatch(r"\s*(f\d+) = %s(?:->|\.)[xyz];" % ID, l) and len(re.findall(r"\b%s\b" % re.match(r"\s*(f\d+)", l).group(1), text)) <= 2)]
    dead = {re.match(r"\s*(f\d+)", l).group(1) for l in lines if l not in keep}
    return "\n".join(l for l in keep if not any(re.fullmatch(r"  (?:float|double) %s;" % d, l) for d in dead))

def macroize(src):
    src = retype_params(src)
    hd, body, tl = split_fn(src)
    body = copies_to_dot(body)
    r = vectorize(hd, body, tl)
    if "RwV3d" in r and "mfl_tier2.h" not in r: r = '#include "mfl_tier2.h"\n' + r
    return r

def vectorize(hd, body, tl):
    lines = body.split("\n")
    names = {}; decls = {}; nv = [0]
    def sc(t): return re.fullmatch(ID + r"(?:\[\d+\])?", t.strip()) is not None
    def newvec(trip):
        if all(t in names for t in trip):
            vs = {names[t][0] for t in trip}
            return list(vs)[0] if len(vs) == 1 and [names[t][1] for t in trip] == [0, 1, 2] else None
        if any(t in names for t in trip): return None
        nv[0] += 1; v = "v%d" % nv[0]
        for k, t in enumerate(trip): names[t] = (v, k)
        decls[v] = trip
        return v
    def read(l):
        m = re.fullmatch(r"\s*(%s(?:\[\d+\]|->[xyz]|\.[xyz])?) = (.*);" % ID, l)
        return m.groups() if m else None
    def ref(trip):
        r = vref(trip)
        if r: return r
        if all(sc(t) for t in trip):
            v = newvec(trip); return "&" + v if v else None
        return None
    out = []; i = 0; hit = 0
    while i < len(lines):
        L3 = lines[i:i + 3]; done = False
        if len(L3) == 3 and all(read(l) for l in L3):
            T = [read(l)[0] for l in L3]; R = [read(l)[1] for l in L3]
            if len(set(T)) == 3:
                for op, kind in (("-", "Sub"), ("+", "Add")):
                    P = [re.fullmatch(r"(\S+) %s (\S+)" % re.escape(op), r) for r in R]
                    if all(P):
                        ra, rb, rt = ref([p.group(1) for p in P]), ref([p.group(2) for p in P]), ref(T)
                        if ra and rb and rt:
                            out.append("  RwV3d%sMacro(%s, %s, %s);" % (kind, rt, ra, rb)); i += 3; hit += 1; done = True; break
                if not done:
                    P = [re.fullmatch(r"(\S+) \* (\S+)", r) for r in R]
                    if all(P):
                        for sidx in (1, 0):      # the scalar is the second or the first factor (the lifted text mixes them per component)
                            sc_ = {p.group(sidx + 1) for p in P}
                            if len(sc_) == 1 and not comp_of(next(iter(sc_))):
                                pass
                            else:
                                # mixed: component factor is whichever matches p->c / v.c
                                pass
                        sv = None; vec = []
                        for k, p_ in enumerate(P):
                            c1, c2 = comp_of(p_.group(1)), comp_of(p_.group(2))
                            if c1 and not c2 and c1[2] == XYZ[k]: vec.append(p_.group(1)); sv = sv or p_.group(2); ok_ = sv == p_.group(2)
                            elif c2 and not c1 and c2[2] == XYZ[k]: vec.append(p_.group(2)); sv = sv or p_.group(1); ok_ = sv == p_.group(1)
                            else: ok_ = False
                            if not ok_: vec = None; break
                        ra, rt = (ref(vec), ref(T)) if vec else (None, None)
                        if ra and rt: out.append("  RwV3dScaleMacro(%s, %s, %s);" % (rt, ra, sv)); i += 3; hit += 1; done = True
                if not done:
                    # cross: x = a1 * b2 - a2 * b1 ; y = a2 * b0 - a0 * b2 ; z = a0 * b1 - a1 * b0 (the lifted text may have the factors of a product swapped: try both roles)
                    c = [(1, 2), (2, 0), (0, 1)]
                    M = [re.fullmatch(r"\(?(\S+) \* (\S+)\)? - \(?(\S+) \* (\S+)\)?", r) for r in R]
                    if all(M):
                        for sw in (0, 1):
                            a = [None] * 3; b = [None] * 3; ok = True
                            for k, (i1, j1) in enumerate(c):
                                t1 = M[k].group(1, 2); t2 = M[k].group(3, 4)
                                if sw: t1 = t1[::-1]; t2 = t2[::-1]
                                for (idx, slot, val) in ((i1, "a", t1[0]), (j1, "b", t1[1]), (j1, "a", t2[0]), (i1, "b", t2[1])):
                                    arr = a if slot == "a" else b
                                    if arr[idx] not in (None, val): ok = False
                                    arr[idx] = val
                            if ok and None not in a + b:
                                ra, rb, rt = ref(a), ref(b), ref(T)
                                if ra and rb and rt:
                                    out.append("  RwV3dCrossProductMacro(%s, %s, %s);" % (rt, ra, rb)); i += 3; hit += 1; done = True; break
        if not done and i + 1 < len(lines) and read(lines[i]) and read(lines[i + 1]):
            # cross product whose z component was inlined by the lifter into a later expression: x, y statements + the z text found below
            (T0, R0), (T1, R1) = read(lines[i]), read(lines[i + 1])
            M0 = re.fullmatch(r"\(?(\S+) \* (\S+)\)? - \(?(\S+) \* (\S+)\)?", R0); M1 = re.fullmatch(r"\(?(\S+) \* (\S+)\)? - \(?(\S+) \* (\S+)\)?", R1)
            if M0 and M1 and T0 != T1:
                for sw in (0, 1):
                    a = [None] * 3; b = [None] * 3; ok = True
                    for (i1, j1), M in (((1, 2), M0), ((2, 0), M1)):
                        t1 = M.group(1, 2); t2 = M.group(3, 4)
                        if sw: t1 = t1[::-1]; t2 = t2[::-1]
                        for (idx, slot, val) in ((i1, "a", t1[0]), (j1, "b", t1[1]), (j1, "a", t2[0]), (i1, "b", t2[1])):
                            arr = a if slot == "a" else b
                            if arr[idx] not in (None, val): ok = False
                            arr[idx] = val
                    if not ok or None in a + b: continue
                    zt = [("(%s - %s)" % (p1, p2)) for p1 in ("%s * %s" % (a[0], b[1]), "%s * %s" % (b[1], a[0])) for p2 in ("%s * %s" % (a[1], b[0]), "%s * %s" % (b[0], a[1]))]
                    rest = "\n".join(lines[i + 2:])
                    zz = next((z for z in zt if z in rest), None)
                    if not zz: continue
                    T2 = "zc%d" % (len(decls) + len(names) + 1)
                    ra, rb, rt = ref(a), ref(b), ref([T0, T1, T2])
                    if ra and rb and rt:
                        out.append("  RwV3dCrossProductMacro(%s, %s, %s);" % (rt, ra, rb)); hit += 1
                        for k in range(i + 2, len(lines)): lines[k] = lines[k].replace(zz, T2)
                        i += 2; done = True; break
        if not done:
            r = read(lines[i])
            d = dot_macro(r[1]) if r else None
            if d: out.append("  %s = %s;" % (r[0], d)); hit += 1
            else: out.append(lines[i])
            i += 1
    if not hit and hd + body + tl == hd + "\n".join(out) + tl: return hd + body + tl
    out = [l for l in out if not any(re.fullmatch(r"  (?:float|double) %s;" % re.escape(t), l) for t in names)]
    nb = "\n".join(out)
    for t, (v, k) in names.items():
        nb = re.sub(r"(?<![\w.>\[])%s(?![\w\[])" % re.escape(t), "%s.%s" % (v, XYZ[k]), nb) if "[" not in t else nb.replace(t, "%s.%s" % (v, XYZ[k]))
    for t in names: hd = re.sub(r"^  (?:float|double) %s;\n" % re.escape(t), "", hd, flags=re.M)
    if "mfl_tier2.h" not in hd: hd = '#include "mfl_tier2.h"\n' + hd
    O = r"[\w.\[\]]+"
    nb = re.sub(r"%s \* %s \+ %s \* %s \+ %s \* %s" % ((O,) * 6), lambda m: dot_macro(m.group(0)) or m.group(0), nb)
    return hd + "".join("  RwV3d %s;\n" % v for v in decls) + nb + tl

if __name__ == "__main__":
    sys.stdout.write(macroize(open(sys.argv[1]).read()))
