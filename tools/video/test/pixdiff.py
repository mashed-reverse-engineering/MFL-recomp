"""pixdiff.py a.bgra b.bgra: mean / max absolute byte difference of two equal-size raw frames (sanity, not bit-exactness)."""
import sys
a = open(sys.argv[1], "rb").read()
b = open(sys.argv[2], "rb").read()
assert len(a) == len(b) and a, (len(a), len(b))
d = [abs(x - y) for x, y in zip(a, b)]
print("pixdiff bytes %d mean %.3f max %d %s" % (len(a), sum(d) / len(d), max(d), "OK" if sum(d) / len(d) < 2 else "BAD"))
