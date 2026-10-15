#!/usr/bin/env python3
"""ptrcast.py [files...]: portable pointer arithmetic for Ghidra output (same x86 code, no int<->pointer round trip; rule `ptrcast` of tools/autocompile.py, AC_PORT=1).
  (T *)((int)&x + N) / (T *)((int)p + N)  ->  (T *)((char *)&x + N) / (T *)((char *)p + N)     (p a declared pointer)
  (int)p - (int)q                          ->  (p - q)   for char / byte / undefined1 pointers (same subtraction)
No file arguments: src/automatch_near/an_hk_*.c.  `rewrite(text)` is imported by autocompile."""
import glob, os, re, sys
PTR = r"(?:\w+ )*(?:char|byte|undefined1|undefined|uchar)"
ITYPE = r"(?:int|uint|undefined4)"
def retype(body):
    """`int X` whose every use is `(T *)(X + N)` (or `X = *(int *)(..)` loads) is a byte pointer: `char *X`, the loads `*(char **)(..)`"""
    for x in sorted(set(re.findall(r"\b%s ((?:param_\d+|[iu]Var\d+))\b\s*[;,)]" % ITYPE, body))):
        uses = [m for m in re.finditer(r"\b%s\b" % x, body)]
        ok, loads = True, []
        for m in uses:
            pre, post = body[:m.start()], body[m.end():]
            if re.search(r"\b%s $" % ITYPE, pre) and re.match(r"\s*[;,)]", post): continue      # the declaration
            if re.search(r"\*\)\($", pre) and re.match(r" \+ ", post): continue      # (T *)(x + N)
            r = re.match(r" = \*\(int \*\)\(", post)
            if r and re.search(r"(^|[;{}\n])\s*$", pre): loads.append(m.end() + 3); continue      # x = *(int *)(..)
            ok = False; break
        if not ok or not uses: continue
        for o in sorted(loads, reverse=True): body = body[:o] + body[o:].replace("*(int *)(", "*(char **)(", 1)
        body = re.sub(r"\b%s (%s)\b(\s*[;,)])" % (ITYPE, re.escape(x)), r"char *\1\2", body)
    return body
def rewrite(t):
    t = re.sub(r"(\*\)\()\*\(int \*\)(\((?:[^()]|\([^()]*\))*\)) \+ ", r"\1*(char **)\2 + ", t)
    parts = re.split(r"(?m)^(?=/\* \w+ \*/$)", t); t = parts[0] + "".join(retype(q) for q in parts[1:])
    ptrs = set(re.findall(r"\b(?:\w+ )+\*+\s*(\w+)\s*[;,)=]", t)); cptrs = set(re.findall(r"\b%s \*\s*(\w+)\s*[;,)=]" % PTR, t))
    ptype = {n: re.sub(r"^extern ", "", ty.strip()) + " *" for ty, n in re.findall(r"\b((?:\w+ )*\w+) \*\s*(\w+)[;,)]", t)}; u4p = {n for n, ty in ptype.items() if ty == "undefined4 *"}
    def a(m):
        x = m.group(2)
        return "%s((char *)%s + %s)" % (m.group(1), x, m.group(3)) if x.startswith("&") or (x.split("->")[0] in ptrs or "->" in x) else m.group(0)
    t = re.sub(r"(\(\s*[\w ]+\*+\s*\)\s*)\(\(int\)(&?\w+(?:->\w+)*)\s*\+\s*([^()]+?)\)", a, t)
    t = re.sub(r"\(int\)(\w+) < \(\(int\)&(\w+)\)", lambda m: "%s < (%s)&%s" % (m.group(1), ptype.get(m.group(1), "char *"), m.group(2)), t)      # loop bound `p < &end`
    t = re.sub(r"\(void \*\)(\**)\(undefined4 (\**)\)", lambda m: "%s(void %s)" % (m.group(1), "*" * (len(m.group(2)) + 1)) if len(m.group(1)) == len(m.group(2)) else m.group(0), t)      # (void *)**(undefined4 **)E -> **(void ***)E
    t = re.sub(r"\(void \*\)\*(\w+)\b", lambda m: "*(void **)%s" % m.group(1) if m.group(1) in u4p else m.group(0), t)      # (void *)*p, p an undefined4 *
    return re.sub(r"\(int\)(\w+) - \(int\)(\w+)", lambda m: "(%s - %s)" % m.groups() if m.group(1) in cptrs and m.group(2) in cptrs else m.group(0), t)
if __name__ == "__main__":
    H = os.path.dirname(os.path.abspath(__file__))
    for f in sys.argv[1:] or sorted(glob.glob(os.path.join(H, "..", "src", "automatch_near", "an_hk_*.c"))):
        s = open(f).read(); n = rewrite(s)
        if n != s: open(f, "w").write(n); print("rewrote", f)
