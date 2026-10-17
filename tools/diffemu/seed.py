"""seed.py: coverage-guided, struct-shape-aware input synthesis for diffemu (evaluate() hook: Guide).
Inputs are only inputs (patches / windows / args are applied to BOTH sides), so the comparison stays real.
Hints mined from the ORIGINAL bytes of the function:
  imms      dword immediates it compares / tests / subtracts (loop bounds, mode values, flag masks, +-1)
  fields    [reg+disp] (0 <= disp < 0x400, size) of every structure access behind a register, with the immediates compared against that field
  ptrfields disps whose loaded value is used as a base register within a few instructions (pointer members)
  globals   absolute data addresses (writable sections) it reads
Guide: a case that grows the coverage of the original joins a corpus; later cases are mutations of corpus members (window fields set to imms /
small counts / pointers to other windows / list terminators, globals patched, scalar arguments replaced) while the coverage is below LIMIT."""
import random
import core
from core import CS, CX, WSEC, ARGBUF, u32, img_read

LIMIT = 0.8      # guide only while the original's coverage is below this
import os
OFF = not os.environ.get("SEED_ON")   # opt-in: no measurable coverage gain on the LOWCOV set (2026-10-17), kept for experiments
SMALL = (0, 0, 1, 1, 2, 3, 4, 5, 8, 16, 0xffffffff)

def hints(fn):
    if hasattr(fn, "_seedh"): return fn._seedh
    imms, fields, ptrf, glob = set(), {}, set(), set()
    a = fn.va; n = 0; last = {}
    isg = lambda d: any(x <= d < z for x, z in WSEC)
    while a < fn.hi and n < 4000:
        ins = next(CS.disasm(img_read(a, 16), a, 1), None)
        if ins is None: a += 1; continue
        a += ins.size; n += 1; ops = ins.operands
        for k, o in enumerate(ops):
            if o.type != CX.X86_OP_MEM: continue
            if o.mem.base == 0 and o.mem.index == 0 and o.size == 4 and isg(o.mem.disp): glob.add(o.mem.disp)
            elif o.mem.base not in (0, CX.X86_REG_ESP, CX.X86_REG_EBP) and 0 <= o.mem.disp < 0x400 and o.size in (1, 2, 4):
                fields.setdefault((o.mem.disp, o.size), set())
                if o.mem.index == 0 and k == 1 and ins.mnemonic == "mov" and ops[0].type == CX.X86_OP_REG and o.size == 4: last[ops[0].reg] = (o.mem.disp, n)
            if o.mem.base in last and n - last[o.mem.base][1] <= 6: ptrf.add(last[o.mem.base][0])      # mov esi,[edi+0x10] ... [esi+x]
        if ins.mnemonic in ("cmp", "test", "sub", "and") and len(ops) == 2 and ops[1].type == CX.X86_OP_IMM and abs(ops[1].imm) < 1 << 31:
            v = ops[1].imm
            if ins.mnemonic != "and": imms |= {u32(v), u32(v + 1), u32(v - 1)}
            else: imms.add(u32(v))
            if ops[0].type == CX.X86_OP_MEM and (ops[0].mem.disp, ops[0].size) in fields: fields[(ops[0].mem.disp, ops[0].size)] |= {u32(v), u32(v + 1)}
    fn._seedh = (sorted(imms)[:80], {k: sorted(v) for k, v in fields.items()}, sorted(ptrf), sorted(glob)[:120])
    return fn._seedh

class Guide:
    def __init__(g, fn, seed0=1):
        g.fn, g.r, g.corpus = fn, random.Random(0x5eed ^ seed0), []
        g.imms, g.fields, g.ptrf, g.glob = hints(fn)
        g.sent = core.sentinels(fn)

    def active(g): return not OFF and bool(g.fn.leaders_o) and len(g.fn.cov_o) / len(g.fn.leaders_o) < LIMIT

    def val(g, size=4):
        r = g.r.random(); pool = g.imms if r < 0.45 and g.imms else g.sent if r < 0.5 and g.sent else SMALL
        return g.r.choice(pool) & ((1 << 8 * size) - 1)

    def mutate(g, c):
        """a mutated copy of case c"""
        r = g.r; c2 = core.Case(c.seed, c.mode, c.fill, list(c.args), dict(c.regs), list(c.wins)); c2.patch = dict(c.patch)
        if c2.wins:
            ws = [[a, bytearray(b)] for a, b in c2.wins]
            for _ in range(r.randint(1, 4)):
                w = r.choice(ws); keys = [k for k in g.fields if k[0] + k[1] <= len(w[1])]
                if not keys: break
                d, sz = r.choice(keys)
                if d in g.ptrf and sz == 4 and r.random() < 0.7: v = r.choice(ws)[0] + r.choice((0, 0, 4 * r.randrange(0, 16)))      # pointer member: another window (or this one)
                elif g.fields[(d, sz)] and r.random() < 0.6: v = r.choice(g.fields[(d, sz)]) & ((1 << 8 * sz) - 1)
                else: v = g.val(sz)
                w[1][d:d + sz] = v.to_bytes(sz, "little")
            c2.wins = [(a, bytes(b)) for a, b in ws]
        if g.glob and r.random() < 0.6:
            for a in r.sample(g.glob, min(len(g.glob), r.randint(1, 3))): c2.patch[a] = g.val()
        sc = [i for i, v in enumerate(c2.args) if not (ARGBUF <= v < ARGBUF + 0x40000) and not core.isstack(v)]
        if sc and r.random() < 0.4: c2.args[r.choice(sc)] = g.val()
        return c2

    def prep(g, c):
        """the case to run instead of c (c itself while inactive / for some cases)"""
        if not g.active(): return c
        x = g.r.random()      # half of the cases stay fresh (coverage of a pointer-only function often comes from rare page seeds)
        if x < 0.5: return c
        return g.mutate(g.r.choice(g.corpus)) if g.corpus and x < 0.75 else g.mutate(c)

    def feed(g, c, grew):
        """after the run: a case that reached new blocks of the original is kept"""
        if grew and len(g.corpus) < 200: g.corpus.append(c)


# ---------------------------------------------------------------------------------------------------------- game-state snapshots
# Real memory of the running game (tools/wine-run/race_script.sh dumps: cars, vehicles, aibots, statsrace, powerups) lays over the image pages of some seeds,
# so the car / bot / stats structures hold plausible floats, indices and counts instead of noise.  `python3 seed.py --collect` copies up to 10 distinct dumps per region
# from ~/.cache/mfl_ctrl/*/out_ctrl_*/ into output/diffemu_snaps/<region>_<k>.bin (so verdicts stay reproducible).
import glob, hashlib, os
REGIONS = {"cars": 0x8B06E0, "vehicles": 0x659f00, "aibots": 0x8c9ae0, "statsrace": 0x8c8ae0, "powerups": 0x6a8bc0, "counters": None}
SNAPS = os.path.join(core.ROOT, "output", "diffemu_snaps")      # output/ is not exported (game memory data)
_snaps = []

def collect():
    os.makedirs(SNAPS, exist_ok=True)
    for reg, va in REGIONS.items():
        if va is None: continue
        seen = {}
        for f in sorted(glob.glob(os.path.expanduser("~/.cache/mfl_ctrl/*/out_ctrl_*/%s.bin" % reg))):
            b = open(f, "rb").read()
            if len(b) > 64 and b.count(0) < len(b) * 0.97: seen.setdefault(hashlib.sha1(b).hexdigest(), b)
            if len(seen) >= 10: break
        for k, b in enumerate(seen.values()): open(os.path.join(SNAPS, "%s_%d.bin" % (reg, k)), "wb").write(b)
        print(reg, len(seen))

def load():
    if not _snaps:
        for f in sorted(glob.glob(os.path.join(SNAPS, "*.bin"))):
            reg, k = os.path.basename(f)[:-4].rsplit("_", 1)
            if REGIONS.get(reg): _snaps.append((reg, int(k), REGIONS[reg], open(f, "rb").read()))
        _snaps.append(None)      # loaded marker
    return _snaps

def snap(p, data, seed):
    """page p (bytes `data`) of a Machine with this seed: half of the seeds lay one variant of every region over it"""
    if OFF or seed % 2: return data
    out = None
    for e in load():
        if e is None: continue
        reg, k, va, b = e
        if k != (seed // 2) % 10 or not (va < p + 0x1000 and p < va + len(b)): continue
        out = out or bytearray(data); a = max(va, p); z = min(va + len(b), p + 0x1000); out[a - p:z - p] = b[a - va:z - va]
    return bytes(out) if out else data

core.SNAP = snap

if __name__ == "__main__": collect()
