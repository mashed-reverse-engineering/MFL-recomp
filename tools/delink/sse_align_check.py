#!/usr/bin/env python3
"""sse_align_check.py [dll] | --selftest: aligned SSE memory operands in a function that does not realign the stack.

The exe calls the library DLL with a 4 byte aligned stack.  gcc assumes 16 and uses aligned operands (movaps / mulps / addps / ... xmmword ptr [esp+x]) on stack
temporaries: #GP, which Wine reports as a read fault at FFFFFFFF (librw-mfl rwmath sseMany, gate 1105_0841: every sweep row crashed).
Scans the executable sections of the pinned DLL (default output/librw/mflrw.dll) with capstone (diffemu venv: run.py venv, or ~/.cache/mfl_diffemu_venv/bin/python).
Every legacy SSE instruction with a 128 bit memory operand (not movups / movupd / movdqu / lddqu; VEX forms only vmova* / vmovdqa / vmovnt*), esp relative or not, must sit in a function whose
code before it contains `and esp, -16` (force_align_arg_pointer).  Verified exceptions: tools/delink/sse_align_allow.txt ("<export name or sub_<rva hex>> <reason>").
Function starts (ponytail: heuristic, no symbols for statics; a missed start can only hide a violation behind the realign of the function before it): exports, direct call targets,
16 aligned addresses behind `ret` + padding.  Exit 0 clean, 1 violations, 2 setup problem."""
import bisect, hashlib, os, re, sys

HERE = os.path.dirname(os.path.abspath(__file__))
DLL = os.path.join(HERE, "..", "..", "output", "librw", "mflrw.dll")
ALLOW = os.path.join(HERE, "sse_align_allow.txt")
UNALIGNED_OK = {"movups", "movupd", "movdqu", "lddqu"}
LEA_PAD = re.compile(r"(\w+), (?:dword ptr )?\[(\w+)(?: \+ (?:0x[0-9a-f]+|eiz\*1(?: \+ 0x[0-9a-f]+)?))?\]$")


def needs_align(i):
    if "xmmword ptr" not in i.op_str: return False
    m = i.mnemonic
    if m.startswith("v"): return m.startswith(("vmova", "vmovdqa", "vmovnt"))
    return m not in UNALIGNED_OK


def is_realign(i):
    if i.mnemonic != "and" or not i.op_str.startswith("esp, 0x"): return False
    v = int(i.op_str[5:], 16)
    return v >= 0xffffff00 and v & 0xf == 0


def is_pad(i):
    if i.mnemonic in ("nop", "int3"): return True
    m = LEA_PAD.match(i.op_str) if i.mnemonic == "lea" else None
    return bool(m) and m.group(1) == m.group(2)


def scan(insns, exports):
    """insns: capstone instructions of one section in address order, exports: {addr: name} -> (violations [(addr, func, text)], nops checked)"""
    st = set(exports)
    if insns: st.add(insns[0].address)
    after_ret = False
    for i in insns:
        if i.mnemonic == "call" and i.op_str.startswith("0x"): st.add(int(i.op_str, 16))
        if is_pad(i):
            continue
        if after_ret and i.address % 16 == 0: st.add(i.address)
        after_ret = i.mnemonic == "ret"
    starts = sorted(st)
    bad, nchk, cur, ok = [], 0, None, False
    for i in insns:
        k = bisect.bisect_right(starts, i.address) - 1
        if k >= 0 and starts[k] != cur: cur, ok = starts[k], False
        if is_realign(i): ok = True
        if needs_align(i):
            nchk += 1
            if not ok: bad.append((i.address, exports.get(cur, "sub_%x" % (cur or 0)), "%s %s" % (i.mnemonic, i.op_str)))
    return bad, nchk


def disasm(code, base):
    import capstone
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.skipdata = True
    return list(md.disasm(code, base))


def selftest():
    mulps = b"\x0f\x59\x44\x24\x20"      # mulps xmm0, xmmword ptr [esp+0x20]
    movups = b"\x0f\x10\x44\x24\x20"     # movups xmm0, xmmword ptr [esp+0x20]: fine
    bad_fn = mulps + b"\xc3"
    good_fn = b"\x83\xe4\xf0" + mulps + b"\xc3"      # and esp, -16
    ok = True
    for name, code, want in (("unaligned", bad_fn, 1), ("realigned", good_fn, 0), ("movups", movups + b"\xc3", 0),
                             ("two functions, only the first realigns", good_fn + b"\xcc" * 7 + bad_fn, 1)):
        bad, _ = scan(disasm(code, 0x1000), {})
        print("sse_align selftest %s: %s %d violation(s), want %d" % (name, "ok  " if len(bad) == want else "FAIL", len(bad), want))
        ok &= len(bad) == want
    return 0 if ok else 1


def main():
    a = [x for x in sys.argv[1:] if not x.startswith("--")]
    if "--selftest" in sys.argv: sys.exit(selftest())
    dll = a[0] if a else DLL
    try:
        import pefile, capstone  # noqa: F401
    except ImportError as e:
        sys.exit("sse_align_check: %s (run it with the diffemu venv: tools/run.py venv)" % e)
    if not os.path.exists(dll): sys.exit("sse_align_check: %s not found" % dll)
    allow = {}
    for l in open(ALLOW) if os.path.exists(ALLOW) else []:
        p = l.split(None, 1)
        if p and not l.startswith("#"):
            if len(p) < 2: sys.exit("sse_align_check: allow entry %r has no reason" % p[0])
            allow[p[0]] = p[1].strip()
    pe = pefile.PE(dll)
    ex = {e.address:(e.name or b"?").decode() for e in pe.DIRECTORY_ENTRY_EXPORT.symbols}
    bad, nchk, used = [], 0, set()
    for s in pe.sections:
        if not s.Characteristics & 0x20000000: continue
        b, n = scan(disasm(s.get_data(), s.VirtualAddress), ex)
        bad += b; nchk += n
    left = []
    for addr, fn, txt in bad:
        if fn in allow: used.add(fn)
        else: left.append((addr, fn, txt))
    print("sse_align: %s sha256 %s: %d aligned SSE memory operands, %d in functions without `and esp,-16` (%d allowed: %s)" % (
        os.path.basename(dll), hashlib.sha256(open(dll, "rb").read()).hexdigest()[:12], nchk, len(bad), len(bad) - len(left), ",".join(sorted(used)) or "-"))
    for addr, fn, txt in left: print("VIOLATION rva 0x%x in %s: %s" % (addr, fn, txt))
    for k in sorted(set(allow) - used): print("note: allow entry %s matches nothing (stale?)" % k)
    sys.exit(1 if left else 0)


if __name__ == "__main__":
    main()
