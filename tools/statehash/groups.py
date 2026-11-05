#!/usr/bin/env python3
"""group the per-word summary of a statecmp output by symbol base: va symbol words first-frame frames-max writes-max"""
import re, sys
g = {}
sec = False
for l in open(sys.argv[1]):
    if l.startswith("words that ever differ"):
        sec = True
        continue
    if not sec or not l.startswith("  "):
        continue
    s = l.split()
    if len(s) < 6:
        continue
    base = re.sub(r"\+0x[0-9a-f]+$", "", s[1])
    r = g.setdefault(base, [0, 10 ** 9, 0, s[0], 0])
    r[0] += 1
    r[1] = min(r[1], int(s[2]))
    r[2] = max(r[2], int(s[3]))
    if len(s) >= 8:
        r[4] = max(r[4], int(s[7]))
for k, (n, f0, mx, va, w) in sorted(g.items(), key=lambda x: (x[1][1], x[1][3])):
    print("%s %-48s words %4d first %4d frames<=%4d writes<=%4d" % (va, k, n, f0, mx, w))
