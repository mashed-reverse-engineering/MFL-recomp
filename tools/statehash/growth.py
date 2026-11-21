#!/usr/bin/env python3
"""growth.py A.bin B.bin [--ptr f] [--mask f] [--every N]: RPL-T C1 chaos growth of B (poke run) against A (control), same exe / same layout.
Per frame and class (cars / vehicles / aibots = statecmp DENY_RANGES, other = the rest; --ptr / --mask words are skipped in every class):
  nf = float words that differ, maxulp, maxrel = max |a-b| / max(|a|, |b|, 1.0), nd = discrete words that differ.
Float word = statecmp.floatclass(A) (same rule as --ftol).  A float pair without a ulp (exponent outside statecmp.ulp's range) counts in nf / maxrel only.
Summary: per class the first frame with any float diff, > 1 ulp, > 100 ulp, maxrel > 1e-3, first discrete diff, and the run maximum."""
import os, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import statecmp as S

CL = ["cars", "vehicles", "aibots", "other"]


def main(a):
    pos, opt, i = [], {}, 0
    while i < len(a):
        if a[i].startswith("--"):
            opt[a[i][2:]] = a[i + 1]; i += 2
        else:
            pos.append(a[i]); i += 1
    fa, fb = pos
    every = int(opt.get("every", 10))
    skip = set(S.read_words(opt.get("ptr"))) | set(S.read_words(opt.get("mask")))
    floatw = S.floatclass(fa)
    ga, gb = S.records(fa), S.records(fb)
    mem, cache, n = None, {}, 0
    first = {c: {} for c in CL}          # class -> {key: frame}
    top = {c: [0, 0.0] for c in CL}
    print("# frame phase racef | per class nf maxulp maxrel nd   (%s vs %s; float words %d)" % (fa, fb, len(floatw)))
    while True:
        try:
            (ha, _, pa), (hb, _, pb) = next(ga), next(gb)
        except StopIteration:
            break
        if mem is None:
            mem = [bytearray(S.PAGE * len(pa)), bytearray(S.PAGE * len(pa))]
            base = ha["base"]
        for m, pg in ((mem[0], pa), (mem[1], pb)):
            for k, d in pg.items():
                m[k * S.PAGE:(k + 1) * S.PAGE] = d
        for p in (range(len(mem[0]) // S.PAGE) if n == 0 else set(pa) | set(pb)):
            A, B = mem[0][p * S.PAGE:(p + 1) * S.PAGE], mem[1][p * S.PAGE:(p + 1) * S.PAGE]
            if A == B:
                cache.pop(p, None)
                continue
            r = {}
            for w, (c, o) in enumerate(zip(struct.unpack("<1024I", A), struct.unpack("<1024I", B))):
                if c == o:
                    continue
                va = base + p * S.PAGE + 4 * w
                cl = S.word_class(va)
                if va in skip:      # pointer words (heap addresses differ per run, also inside cars / vehicles) and masked words: statecmp judges those
                    continue
                e = r.setdefault(cl, [0, 0, 0.0, 0, None])      # nf, maxulp, maxrel, nd, first discrete va
                if va in floatw:
                    x, y = S.fl(c), S.fl(o)
                    e[0] += 1
                    e[1] = max(e[1], S.ulp(c, o) or 0)
                    e[2] = max(e[2], abs(x - y) / max(abs(x), abs(y), 1.0))
                else:
                    e[3] += 1
                    e[4] = e[4] or va
            cache[p] = r
        tot = {c: [0, 0, 0.0, 0, None] for c in CL}
        for r in cache.values():
            for c, e in r.items():
                t = tot[c]
                t[0] += e[0]; t[1] = max(t[1], e[1]); t[2] = max(t[2], e[2]); t[3] += e[3]; t[4] = t[4] or e[4]
        for c in CL:
            t, f = tot[c], first[c]
            for key, hit in (("any", t[0] > 0), (">1ulp", t[1] > 1), (">100ulp", t[1] > 100), ("rel>1e-3", t[2] > 1e-3)):
                if hit:
                    f.setdefault(key, n)
            if t[3] and "discrete" not in f:
                f["discrete"] = n
                f["discrete_va"] = "%08x" % t[4]
            top[c] = [max(top[c][0], t[1]), max(top[c][1], t[2])]
        if any(tot[c][0] or tot[c][3] for c in CL) and (n % every == 0 or n - min(v for c in CL for k, v in first[c].items() if k != "discrete_va") < 12):
            print("%4d ph%d rf%-4d | %s" % (n, ha["phase"], ha["racef"], " | ".join("%s %d %d %.3g %d" % (c, *tot[c][:4]) for c in CL)))
        n += 1
    print("SUMMARY frames %d (last phase %d racef %d / %d racef %d)" % (n, ha["phase"], ha["racef"], hb["phase"], hb["racef"]))
    for c in CL:
        f = first[c]
        print("CLASS %-8s first any=%s >1ulp=%s >100ulp=%s rel>1e-3=%s discrete=%s%s | run max ulp=%d rel=%.3g" % (
            c, *(f.get(k, "-") for k in ("any", ">1ulp", ">100ulp", "rel>1e-3", "discrete")),
            " (%s %s)" % (f["discrete_va"], S.Syms().name(int(f["discrete_va"], 16))) if "discrete_va" in f else "", top[c][0], top[c][1]))


if __name__ == "__main__":
    main(sys.argv[1:])
