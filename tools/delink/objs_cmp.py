#!/usr/bin/env python3
"""objs_cmp.py <link a>/objs.txt <link b>/objs.txt: which sources of two links compiled to different objects. COFF objects are compared with
the file-header TimeDateStamp (bytes 4..8) zeroed, so a comment-only edit shows as `same`. Prints per source: same / DIFF / only-a / only-b."""
import sys
def objs(p): return dict(l.split(None, 1) for l in open(p) if len(l.split()) == 2)
def norm(p):
    d = bytearray(open(p.strip(), "rb").read()); d[4:8] = b"\0\0\0\0"; return bytes(d)
a, b = objs(sys.argv[1]), objs(sys.argv[2]); n = {"same": 0, "DIFF": 0, "only-a": 0, "only-b": 0}
for s in sorted(set(a) | set(b)):
    if s not in b: k = "only-a"
    elif s not in a: k = "only-b"
    elif a[s] == b[s]: continue                    # same cache object: unchanged source
    else: k = "same" if norm(a[s]) == norm(b[s]) else "DIFF"
    n[k] += 1; print(k, s)
print("changed objects: %(same)d same after stamp mask, %(DIFF)d DIFF, %(only-a)d only in a, %(only-b)d only in b" % n)
sys.exit(1 if n["DIFF"] else 0)
