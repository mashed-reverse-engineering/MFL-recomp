#!/usr/bin/env python3
"""callsite_floats.py : output/callsite_floats.txt, one line per direct callee: `<va> <sites> <j><kind>,..` (kind f = dword float store, d = qword) = the stack argument positions (dword index, 0 = first argument) where at least one call site
anywhere in MFL.exe stores a float into the argument slot (`fstp / fst dword [esp+k]`; a qword store marks two positions with `d`).  Used by t2_postfix `floatproto`: a K&R `extern f();` called with float
expressions gets a prototype whose float parameters are real `float` (C promotes a float argument of an unprototyped call to double: 2 dwords, wrong stack layout).
Straight-line stack simulation per function (push / pop / add esp / sub esp / mov, fst to [esp+k]), a cdecl callee's arguments are the top n dwords at the call."""
import os, re, sys, collections
py = os.path.expanduser("~/.cache/mfl_diffemu_venv/bin/python")
try: import unicorn
except ImportError: os.execv(py, [py] + sys.argv)
HERE = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(HERE, ".."))
sys.path[:0] = [HERE, os.path.join(HERE, "diffemu")]
os.environ.setdefault("DIFFEMU_OUT", "/tmp/callsite_floats_dx")
import core
CS, tb = core.CS, core.T()
def run(va, sz, votes):
    st = []; a = va; end = va + sz
    while a < end:
        ins = next(CS.disasm(core.img_read(a, 16), a, 1), None)
        if ins is None: a += 1; continue
        mn, o = ins.mnemonic, ins.op_str
        if mn == "push": st.append(" ")
        elif mn == "pop":
            if st: st.pop()
        elif mn == "add" and re.fullmatch(r"esp, (?:0x[0-9a-f]+|\d+)", o):
            n = int(o[5:], 0) // 4; st[:] = st[:len(st) - n] if n <= len(st) else []
        elif mn == "sub" and re.fullmatch(r"esp, (?:0x[0-9a-f]+|\d+)", o): st += [" "] * (int(o[5:], 0) // 4)
        elif mn.startswith("fst") and (m := re.fullmatch(r"(dword|qword) ptr \[esp(?: \+ (0x[0-9a-f]+|\d+))?\]", o)):
            k = int(m.group(2) or "0", 0) // 4
            for j in range(k, k + (2 if m.group(1) == "qword" else 1)):
                if j < len(st): st[len(st) - 1 - j] = "d" if m.group(1) == "qword" else "f"
        elif mn == "call":
            if re.fullmatch(r"0x[0-9a-f]+", o):
                t = int(o, 16)
                if core.BASE <= t < core.IMGEND:
                    v = votes[t]; v["n"] += 1
                    for j, c in enumerate(reversed(st[-12:])):
                        if c != " ": v["p"][j].add(c)
                    pop = core.callee_info(t).pop // 4
                    for _ in range(pop):
                        if st: st.pop()
        elif mn == "ret": st = []
        a += ins.size
def main():
    votes = collections.defaultdict(lambda: {"n": 0, "p": collections.defaultdict(set)})
    for va, sz in sorted(tb.size.items()):
        if core.BASE <= va < core.IMGEND and sz: run(va, sz, votes)
    out = []
    for t, v in sorted(votes.items()):
        k = core.callee_info(t).nargs
        ps = sorted((j, "".join(sorted(c))) for j, c in v["p"].items() if j < k)
        if ps: out.append("%08x %d %s" % (t, v["n"], ",".join("%d%s" % (j, "".join(s)) for j, s in ps)))
    open(os.path.join(G, "output", "callsite_floats.txt"), "w").write("\n".join(out) + "\n"); print(len(out), "callees")
main()
