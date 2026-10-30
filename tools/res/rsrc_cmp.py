#!/usr/bin/env python3
"""rsrc_cmp.py <a.exe|a.res> <b.exe|b.res>: compare resource trees (type/name/lang/bytes). Exit 0 = equal."""
import sys
from rsrc import entries
a, b = entries(sys.argv[1]), entries(sys.argv[2])
bad = [k for k in sorted(set(a) | set(b), key=str) if a.get(k) != b.get(k)]
for k in bad[:30]: print("DIFF", k, len(a.get(k, b"")) if k in a else "missing", len(b.get(k, b"")) if k in b else "missing")
print("%d entries vs %d, %d differ" % (len(a), len(b), len(bad)))
sys.exit(bool(bad))
