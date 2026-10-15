#!/usr/bin/env python3
"""Reduce the big Ghidra type header to the declarations a translation unit needs (closure over identifiers).
Usage: slim_types.py <ghidra_types.h (fixed)> <tu.c> <out.h>"""
import re, sys

ID = re.compile(r"[A-Za-z_]\w*")


def blocks(text):
    out, cur, depth = [], [], 0
    for l in text.split("\n"):
        if not l.strip() and depth == 0:
            if cur:
                out.append("\n".join(cur))
                cur = []
            continue
        cur.append(l)
        depth += l.count("{") - l.count("}")
    if cur:
        out.append("\n".join(cur))
    return out


def provides(b):
    first = b.split("\n")[0]
    if b.startswith("typedef") or b.startswith("#define"):
        return set(ID.findall(b))
    m = re.match(r"(struct|union|enum)\s+(\w+)", first)
    p = set()
    if m:
        p.add(m.group(2))
        if m.group(1) == "enum":
            p |= set(ID.findall(b))
    else:
        p |= set(ID.findall(first))
    return p


def slim(types_text, tu_text):
    bl = blocks(types_text)
    prov = [provides(b) for b in bl]
    def dependencies(b):
        if b.startswith("typedef") or b.startswith("#define") or b.startswith("enum"):
            return set(ID.findall(b))
        d = set()
        for l in b.split("\n")[1:]:
            ids = ID.findall(re.sub(r":\s*\d+", "", l))
            if l.rstrip().endswith(";") and ids:
                ids = ids[:-1]
            d |= set(ids)
        return d
    deps = [dependencies(b) for b in bl]
    need = set(ID.findall(tu_text))
    take = [False] * len(bl)
    changed = True
    while changed:
        changed = False
        for i, p in enumerate(prov):
            if not take[i] and p & need:
                take[i] = True
                need |= deps[i]
                changed = True
    return "\n\n".join(b for b, t in zip(bl, take) if t) + "\n"


if __name__ == "__main__":
    open(sys.argv[3], "w").write(slim(open(sys.argv[1], errors="replace").read(), open(sys.argv[2], errors="replace").read()))
