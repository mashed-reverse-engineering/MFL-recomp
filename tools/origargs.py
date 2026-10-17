#!/usr/bin/env python3
"""origargs.py -> output/orig_stackargs.txt `va slots retn_bytes tail_va|- flags name`: for every game code function of MFL.exe (output/delink_sdk/ownership_items.csv)
the number of STACK ARGUMENT SLOTS (dwords) the original reads or rewrites: path exploration over the CFG with the esp depth tracked (push / pop / sub / add esp, calls of
known stdcall callees pop their arguments, `push ebp; mov ebp,esp` frames: [ebp+8+4k]); `[esp+d+4+4k]` / `lea r,[esp+..]` (address taken) / QWORD PTR = 2 slots.
A `jmp` out of the function at stack depth 0 is a tail call: it passes the frame arguments on (`tail_va`).  flags: dyn = esp changed by something untracked (count is a lower bound),
addr = the address of an argument slot is taken (the extent is unknown).  Used by defectlint class 10 `defarity`."""
import csv, os, re, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import reginputs2 as R
G = R.G; OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(G, "output", "orig_stackargs.txt")
MEM = re.compile(r"(?:(byte|word|dword|qword|tbyte|xmmword) ptr )?(?:[a-z]s:)?\[(esp|ebp)(?:([+-])0x([0-9a-f]+))?\]")


def slots_of(a, d, ed, op):
    """-> (set of arg slots touched, addr_taken) for the esp / ebp operands of one instruction"""
    out, addr = set(), False
    for m in MEM.finditer(a):
        sz, base, sg, off = m.groups()
        o = (int(off, 16) if off else 0) * (-1 if sg == "-" else 1)
        if base == "esp": rel = o - d                      # offset from the entry esp
        elif ed is not None: rel = o - ed
        else: continue
        if rel < 4: continue                               # local / return address
        k = (rel - 4) // 4
        n = 2 if sz in ("qword",) else 1
        if k < 24: out.update(range(k, k + n))      # no game function takes more: a bigger index is a local whose esp depth was lost (alloca / __chkstk frames)
        if op == "lea": addr = True
    return out, addr


def analyse(lo, hi, ins, entries, retn_of, tbl):
    seen, work = set(), [(lo, 0, None)]
    slots, retn, tails, flags, steps, wmax = set(), 0, set(), set(), 0, 0
    while work and steps < 4000:
        pc, d, ed = work.pop(); ea = None
        while True:
            if (pc, d, ed) in seen or pc not in ins or (pc != lo and pc in entries) or steps >= 4000 or abs(d) > 0x10000: break      # falling / branching into another function ends the path
            seen.add((pc, d, ed)); steps += 1
            op, a = ins[pc][:2]; nxt = ins[pc][2]
            q = R.split_ops(a)
            if op == "mov" and q[:1] == ["eax"] and re.fullmatch(r"0x[0-9a-f]+", q[1]): ea = int(q[1], 16)
            elif q[:1] == ["eax"] and op not in ("cmp", "test", "push"): ea = None
            s, ad = slots_of(a, d, ed, op)
            slots |= s
            if op == "mov" and q and MEM.fullmatch(q[0]): wmax = max(wmax, max(slots_of(q[0], d, ed, op)[0], default=-1) + 1)      # a rewritten argument slot is passed on to a tail-jump target
            if ad: flags.add("addr")
            if op == "push": d += 4
            elif op == "pop":
                d -= 4
                if q[:1] == ["ebp"]: ed = None
            elif op in ("add", "sub") and q[:1] == ["esp"]:
                try: v = int(q[1], 16)
                except ValueError: flags.add("dyn"); break
                d += v if op == "sub" else -v
            elif op == "mov" and q == ["ebp", "esp"]: ed = d
            elif op == "mov" and q == ["esp", "ebp"]:
                if ed is None: flags.add("dyn"); break
                d = ed
            elif op == "leave":
                if ed is None: flags.add("dyn"); break
                d = ed - 4; ed = None
            elif op == "and" and q[:1] == ["esp"] and ed is not None:
                d = -0x8000      # `and esp,-8` behind `push ebp; mov ebp,esp`: esp-relative operands are locals from here on (the huge negative depth keeps them out of the argument slots), the [ebp+8+4k] arguments stay exact (ai_drive_a: 4 arguments were "dyn", 0 slots, and ai_bot_update called it with none)
            elif q[:1] == ["esp"] and op in ("mov", "lea", "and", "xchg"): flags.add("dyn"); break
            elif op in ("mov", "lea", "xchg") and q[:1] == ["ebp"]: ed = None
            elif op == "call":
                t = q[0] if q else ""
                if R.ADDR.match(t) and int(t, 16) == 0x4b3430:      # alloca_probe: esp -= eax (the frame size is the `mov eax, imm` before)
                    if ea is None: flags.add("dyn"); break
                    d += ea
                elif R.ADDR.match(t): d -= retn_of.get(int(t, 16), 0)
            elif op.startswith("ret"):
                retn = max(retn, int(q[0], 16) if q else 0); break
            elif op == "jmp":
                t = q[0] if q else ""
                if R.ADDR.match(t):
                    t = int(t, 16)
                    if t in entries and t != lo:
                        if d == 0: tails.add(t)
                        break
                    pc = t; continue
                if d == 0 and not tbl.get(pc) and re.match(r"(?:dword ptr )?(?:ds:)?\[|e[a-d]x$|e[sd]i$", t): flags.add("ijmp")      # `jmp [reg+N]`: a vtable thunk, the frame arguments pass through
                for t in tbl.get(pc, []): work.append((t, d, ed))
                break
            elif R.JCC.match(op):
                t = q[0] if q else ""
                if R.ADDR.match(t): work.append((int(t, 16), d, ed))
            elif op in ("hlt", "ud2", "int3"): break
            pc = nxt
    if wmax and tails: flags.add("w%d" % wmax)
    return (max(slots) + 1 if slots else 0), retn, tails, flags


def main():
    exe, secs = R.load_pe()
    items = sorted((int(r["lo"], 16), int(r["hi"], 16), r["name"]) for r in csv.DictReader(open(R.OWN)) if r["kind"] == "code")
    o = subprocess.run(["objdump", "-w", "-M", "intel", "-D", "--no-show-raw-insn", "--start-address=0x401000", "--stop-address=0x5d7000", R.EXE], capture_output=True, text=True).stdout
    ins0, order = R.parse_objdump(o)
    ins = {a: (ins0[a][0], ins0[a][1], order[i + 1] if i + 1 < len(order) else a + 1) for i, a in enumerate(order)}
    unnamed = lambda n: n.startswith(("gap_", "switchdata", "source match"))
    entries = {lo for lo, hi, n in items if not unnamed(n)}
    tbl = R.tables(0x401000, 0x5d7000, ins0, order, exe, secs)
    retn_of = {}
    for l in open(os.path.join(G, "output", "stdcall.txt")):
        q = l.split()
        if len(q) >= 2: retn_of[int(q[0], 16)] = int(q[1])
    for r in csv.DictReader(open(os.path.join(G, "output", "reginputs2.csv"))): retn_of.setdefault(int(r["addr"], 16), int(r["ret_n"]))
    rows = []
    for lo, hi, n in items:
        if unnamed(n) or lo not in ins: continue
        sl, rn, tl, fl = analyse(lo, hi, ins, entries, retn_of, tbl)
        rows.append("%08x %d %d %s %s %s" % (lo, sl, rn, ",".join("%08x" % t for t in sorted(tl)) or "-", ",".join(sorted(fl)) or "-", n))
    open(OUT, "w").write("\n".join(rows) + "\n")
    print("%d functions -> %s" % (len(rows), os.path.relpath(OUT, G)))


if __name__ == "__main__": main()
