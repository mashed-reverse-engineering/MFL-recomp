#!/usr/bin/env python3
"""x87ord.py: x87 operation-order compare, ours (linked exe, by map) vs the original (MFL.exe, by output/fnsizes.txt). Plan W14 2.2 step 2.

  x87ord.py fn <ours.exe> <ours.map> <delta hex|auto> <name> [-v]    one function: rows + both expression lists (auto: delta from the
                                                                map address of g_race_load_752364)
  x87ord.py all <ours.exe> <ours.map> <delta hex> <out.csv>     every function of our C (map `f` symbols, obj != app.obj) with an original
  x87ord.py selftest                                            negative fixture built at run time (swapped three-term sum) + basics

Method: each function is walked linearly with a symbolic x87 stack (opcode-decoded, so fsub/fsubr and the DC/DE register forms follow the
Intel tables, not a disassembler's mnemonic). fld pushes a leaf, arithmetic builds a tree, every fst/fstp/fist to memory and every compare
emits an event "dest <- tree". Leaves: K<float> (read-only constant, by value), G<orig va> (global, ours minus delta), M<disp> (field of a
pointer, base register ignored), S (stack slot not written in the function: argument or local), i: prefix for integer loads, ret (st0 after
a call). Stores to a stack slot are forwarded to later loads of the same slot (spills), so temporaries vanish. add/mul children are sorted
(a swap of the two operands of one commutative op is ignored); a different association ((a+b)+c vs a+(b+c)) stays a difference.
The two event multisets are compared: a row = an event present on one side only.
ponytail: linear walk, the stack is reset at jmp/ret and underflow gives U leaves; esp tracking ignores stdcall callee cleanup; base
registers of field loads are not tracked (a.x*b.x == a.x*a.x). X87ORD_BB=1 (W15b CLS-X87 r2): per basic block walk over forward
edges instead (events(bb=True), comment there); default stays linear so classsweep / known counts do not move without a decision.
Needs capstone: run.py venv."""
import collections, csv, os, re, struct, sys
import capstone
from capstone import x86

G = os.environ.get('MFL_GHIDRA') or os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))   # tools/delink/x87ord.py -> ghidra/ (MFL_GHIDRA: a copy run from elsewhere)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
BB, CAP = os.environ.get('X87ORD_BB') == '1', 16   # per basic block walk (events bb=); states walked per block at most


class Img:
    def __init__(self, path, delta=0):
        exe = open(path, 'rb').read()
        pe = struct.unpack_from('<I', exe, 0x3c)[0]
        nsec, optsz = struct.unpack_from('<H', exe, pe + 6)[0], struct.unpack_from('<H', exe, pe + 20)[0]
        self.base = struct.unpack_from('<I', exe, pe + 52)[0]
        self.secs = []
        for i in range(nsec):
            o = pe + 24 + optsz + 40 * i
            vsz, v, rsz, raw = struct.unpack_from('<IIII', exe, o + 8)
            ch = struct.unpack_from('<I', exe, o + 36)[0]
            self.secs.append((self.base + v, max(vsz, rsz), raw, rsz, bool(ch & 0x80000000)))
        self.exe, self.delta = exe, delta

    def rd(self, va, n):
        for s, sz, raw, rsz, w in self.secs:
            if s <= va < s + rsz:
                return self.exe[raw + va - s: raw + va - s + n]
        return b''

    def writable(self, va):
        for s, sz, raw, rsz, w in self.secs:
            if s <= va < s + sz:
                return w
        return True


R32 = {}
for _r in ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp'):
    R32[_r] = _r
    R32[_r[1:]] = _r
    if _r in ('eax', 'ebx', 'ecx', 'edx'):
        R32[_r[1] + 'l'] = R32[_r[1] + 'h'] = _r
R32.update(si='esi', di='edi', bp='ebp', sp='esp', sil='esi', dil='edi')

OPS = {0: 'add', 1: 'mul', 4: 'sub', 5: 'subr', 6: 'div', 7: 'divr'}


def node(op, a, b):
    if op == 'subr':
        op, a, b = 'sub', b, a
    elif op == 'divr':
        op, a, b = 'div', b, a
    if op in ('add', 'mul') and b < a:
        a, b = b, a
    return '%s(%s,%s)' % (op, a, b)


class Sites(list):
    """bb mode: every event is kept with the address (site) that emits it"""
    def __init__(self, cur):
        list.__init__(self)
        self.cur = cur

    def append(self, e):
        list.append(self, (self.cur[0], e))


def events(img, va, n, own=False, bb=None):
    """list of event strings of the function [va, va+n); bb (default: env X87ORD_BB=1) = per basic block walk"""
    bb = BB if bb is None else bb
    cur = [None]
    st, ev, slots, esp = [], Sites(cur) if bb else [], {}, 0

    def fill(i):
        """bb: an access below the stack bottom reads the last call's st0 once ('ret', then it is on the stack), deeper ones U"""
        while bb and len(st) <= i:
            st.insert(0, 'ret' if called[0] else 'U')
            called[0] = False

    def pop():
        fill(0)
        return st.pop() if st else ('ret' if called[0] else 'U')

    def top():
        fill(0)
        return st[-1] if st else ('ret' if called[0] else 'U')

    def sti(i):
        fill(i)
        k = len(st) - 1 - i
        return st[k] if k >= 0 else 'U'

    def setsti(i, v):
        fill(i)
        k = len(st) - 1 - i
        if k >= 0:
            st[k] = v

    called = [False]
    regs, islots, fresh, unread = {}, {}, {}, {}   # unread: float stack-slot stores no fld reads back (local results, emitted)   # integer registers: (path, offset) or absent; integer stack slots by entry-relative offset

    def rv(r):
        if r == 'esp':
            return ('SP', esp)
        return regs.get(r)

    def gnorm(a):
        if own and img.delta and 0x5dd000 + img.delta <= a < 0x9f9000 + img.delta:
            a -= img.delta
        return a

    def addr(m):
        """symbolic address of a memory operand: ('SP', off) | ('G', va) | (path, off) | None"""
        d = m.disp & 0xffffffff
        if not m.base and not m.index:
            return ('G', gnorm(d))
        bv = rv(R32.get(md.reg_name(m.base), '')) if m.base else None
        if m.index or (bv is None and d >= 0x400000):
            if bv is None or bv[0] == 'G':
                return ('G[i]', gnorm(d + (bv[1] if bv else 0)) & 0xffffffff)
            return ('%s[i]' % bv[0], bv[1] + m.disp)
        if bv is None:
            return None
        return (bv[0], (bv[1] + m.disp) & 0xffffffff if bv[0] == 'G' else bv[1] + m.disp)

    def leafname(a, m):
        if a is None:
            d = m.disp & 0xffffffff
            return leafname(('G[i]', gnorm(d)), m) if 0x400000 <= d else 'M?%x' % d   # [reg + image va]: an indexed global
        if a[0] == 'SP':
            return 'A%x' % a[1] if a[1] >= 4 else 'S'
        if a[0] == 'G[i]' and 0x8b06e0 <= a[1] < 0x8b06e0 + 0xd040:
            return 'M?%x' % ((a[1] - 0x8b06e0) % 0xd04)   # player_car[i].field (ours indexes the array, the original uses a car pointer)
        if a[0] == 'G[i]' and a[1] < 0x10000:
            return 'M?%x' % a[1]   # [ptr + index + small disp]: a field, not a global
        if a[0] == 'G':
            return 'G%x' % a[1]
        return '%s@%x' % (a[0], a[1] & 0xffffffff)

    def mem(op, size, ints=False):
        m = op.mem
        if not m.base and not m.index:
            a = m.disp & 0xffffffff
            if not img.writable(a):
                b = img.rd(a, size)
                if size == 4 and len(b) == 4:
                    v = struct.unpack('<i' if ints else '<f', b)[0]
                elif size == 8 and len(b) == 8:
                    v = struct.unpack('<q' if ints else '<d', b)[0]
                else:
                    v = b.hex()
                return 'K%.9g' % v if isinstance(v, float) else 'K%s' % v
            return 'G%x' % gnorm(a)
        a = addr(m)
        if a and a[0] == 'SP':
            return ('s', a[1])
        return leafname(a, m)

    def load(op, size, ints=False):
        k = mem(op, size, ints)
        if isinstance(k, tuple):
            if k in slots:
                unread.pop(k, None)
                return slots[k]
            iv = islots.get(k[1])   # a word moved into the slot with integer moves: its source leaf
            if size == 4 and iv and iv[1] == 0 and iv[0].startswith('*'):
                return iv[0][1:]
            return 'A%x' % k[1] if k[1] >= 4 else 'S'
        return ('i:' if ints else '') + k + ('/%d' % size if ints else '')

    def intop(i):
        """integer register tracking for pointer paths (mov / lea / add / sub imm, push / pop, spills)"""
        m, ops = i.mnemonic, i.operands
        dst = R32.get(md.reg_name(ops[0].reg), '') if ops and ops[0].type == x86.X86_OP_REG else ''
        if m == 'mov' and dst and len(ops) == 2:
            s = ops[1]
            if s.type == x86.X86_OP_REG:
                v = rv(R32.get(md.reg_name(s.reg), ''))
            elif s.type == x86.X86_OP_IMM:
                v = ('G', gnorm(s.imm & 0xffffffff))
            else:
                a = addr(s.mem)
                v = islots.get(a[1]) if a and a[0] == 'SP' and a[1] < 4 else ('*' + (mem(s, 4) if not (a and a[0] == 'SP') else 'A%x' % a[1]), 0)
                if s.size != 4:
                    v = None
            setr(dst, v, ops[0].size)
            return
        if m == 'mov' and len(ops) == 2 and ops[0].type == x86.X86_OP_MEM and ops[1].type == x86.X86_OP_REG:
            a = addr(ops[0].mem)
            if a and a[0] == 'SP':
                islots[a[1]] = rv(R32.get(md.reg_name(ops[1].reg), ''))
            return
        if m == 'lea' and dst:
            setr(dst, addr(ops[1].mem), 4)
            return
        if m in ('add', 'sub') and dst and dst != 'esp' and ops[1].type == x86.X86_OP_IMM and rv(dst):
            p, o = rv(dst)
            setr(dst, (p, o + (ops[1].imm if m == 'add' else -ops[1].imm)), 4)
            return
        if m == 'push' and ops:
            islots[esp - 4] = rv(R32.get(md.reg_name(ops[0].reg), '')) if ops[0].type == x86.X86_OP_REG else None
            return
        if m == 'pop' and dst:
            setr(dst, islots.get(esp), 4)
            return
        try:
            w = i.regs_access()[1]
        except capstone.CsError:
            w = []
        for r in w:
            r = R32.get(md.reg_name(r), '')
            if r and r != 'esp':
                regs.pop(r, None)

    def setr(r, v, size):
        if r == 'esp':
            return
        if v is None or size != 4:
            regs.pop(r, None)
        else:
            regs[r] = v

    def store(op, size, v, ints=False):
        k = mem(op, size, ints)
        if isinstance(k, tuple):
            if k in unread:
                ev.append('local <- %s' % unread.pop(k))
            slots[k] = ('int(%s)' % v) if ints else (v if size >= 4 else 'w(%s)' % v)
            unread[k] = slots[k]
            islots.pop(k[1], None)
            fresh[k[1]] = slots[k]
            return
        ev.append('%s <- %s%s' % (k, 'int ' if ints else '', v))

    def step(i):
        nonlocal st, esp
        m, b = i.mnemonic, i.bytes
        if not m.startswith('f') and m not in ('call', 'jmp', 'ret', 'retn'):
            intop(i)
        if m == 'push':
            esp -= 4
        elif m == 'pop':
            esp += 4
        elif m in ('sub', 'add') and i.op_str.startswith('esp,') and i.operands[1].type == x86.X86_OP_IMM:
            esp += -i.operands[1].imm if m == 'sub' else i.operands[1].imm
        elif m == 'call':
            if 'eax' in regs and regs['eax'][0] == 'G' and 0x1000 <= regs['eax'][1] < 0x100000:
                esp -= regs['eax'][1]   # mov eax, size; call __chkstk / __alloca_probe (frames >= 4 KB)
            for o in sorted(fresh):   # floats stored to the outgoing argument area since the last call = call arguments
                if esp <= o < esp + 0x40:
                    ev.append('arg <- %s' % fresh[o])   # no index: push order around the stores differs between compilers
                    unread.pop(('s', o), None)
            fresh.clear()
            st, called[0] = [], True
            for r in ('eax', 'ecx', 'edx'):
                regs.pop(r, None)
            return
        elif m in ('jmp', 'ret', 'retn'):
            if m != 'jmp' and st:
                ev.append('return <- %s' % st[-1])
            if not bb or m != 'jmp':   # bb: the stack at a jmp flows to its target
                st = []
            return
        k = 0
        while k < len(b) and b[k] in (0x66, 0x67, 0xf2, 0xf3, 0x2e, 0x3e, 0x26, 0x36, 0x64, 0x65, 0x9b):
            k += 1
        if k >= len(b) or not 0xd8 <= b[k] <= 0xdf or k + 1 >= len(b):
            return
        op, mr = b[k], b[k + 1]
        reg, ri = (mr >> 3) & 7, mr & 7
        memop = next((o for o in i.operands if o.type == x86.X86_OP_MEM), None)
        if mr < 0xc0 and memop is not None:   # memory forms
            if op in (0xd8, 0xdc, 0xda, 0xde):
                ints, size = op in (0xda, 0xde), {0xd8: 4, 0xdc: 8, 0xda: 4, 0xde: 2}[op]
                x = load(memop, size, ints)
                if reg in OPS:
                    st.append(node(OPS[reg], pop(), x))
                else:   # fcom / fcomp
                    t = top() if reg == 2 else pop()
                    ev.append('cmp ' + node('add', t, x).replace('add', 'cmp', 1))
            elif op == 0xd9:
                if reg == 0:
                    st.append(load(memop, 4))
                elif reg in (2, 3):
                    store(memop, 4, top() if reg == 2 else pop())
            elif op == 0xdd:
                if reg == 0:
                    st.append(load(memop, 8))
                elif reg in (2, 3):
                    store(memop, 8, top() if reg == 2 else pop())
            elif op == 0xdb:
                if reg == 0:
                    st.append(load(memop, 4, True))
                elif reg in (2, 3):
                    store(memop, 4, top() if reg == 2 else pop(), True)
                elif reg == 5:
                    st.append(load(memop, 10))
                elif reg == 7:
                    store(memop, 10, pop())
            elif op == 0xdf:
                if reg in (0, 5):
                    st.append(load(memop, 2 if reg == 0 else 8, True))
                elif reg in (2, 3, 7):
                    store(memop, 8 if reg == 7 else 2, top() if reg == 2 else pop(), True)
            return
        if op == 0xd8:   # st0 = st0 op st(i)
            if reg in OPS:
                setsti(0, node(OPS[reg], top(), sti(ri)))
            else:
                ev.append('cmp ' + node('add', top(), sti(ri)).replace('add', 'cmp', 1))
                if reg == 3:
                    pop()
        elif op in (0xdc, 0xde):   # st(i) = st(i) op st0 (Intel: E0 = subr, E8 = sub, F0 = divr, F8 = div)
            if reg in (0, 1, 4, 5, 6, 7):
                o = {0: 'add', 1: 'mul', 4: 'subr', 5: 'sub', 6: 'divr', 7: 'div'}[reg]
                setsti(ri, node(o, sti(ri), top()))
                if op == 0xde:
                    pop()
            elif op == 0xde and mr == 0xd9:
                a = pop()
                ev.append('cmp ' + node('add', a, pop()).replace('add', 'cmp', 1))
        elif op == 0xd9:
            if 0xc0 <= mr <= 0xc7:
                st.append(sti(ri))
            elif 0xc8 <= mr <= 0xcf:
                x, y = top(), sti(ri)
                setsti(0, y)
                setsti(ri, x)
            elif mr in (0xe0, 0xe1, 0xfa, 0xfc, 0xfe, 0xff):
                setsti(0, '%s(%s)' % ({0xe0: 'chs', 0xe1: 'abs', 0xfa: 'sqrt', 0xfc: 'rnd', 0xfe: 'sin', 0xff: 'cos'}[mr], top()))
            elif mr in (0xe8, 0xee, 0xeb):
                st.append({0xe8: 'K1', 0xee: 'K0', 0xeb: 'Kpi'}[mr])
            elif mr in (0xf1, 0xf3, 0xfd):
                a = pop()
                setsti(0, '%s(%s,%s)' % ({0xf1: 'yl2x', 0xf3: 'patan', 0xfd: 'scale'}[mr], top(), a))
            elif mr == 0xe4:
                ev.append('cmp tst(%s)' % top())
        elif op == 0xdd:
            if 0xd0 <= mr <= 0xdf:   # fst / fstp st(i)
                setsti(ri, top())
                if mr >= 0xd8:
                    pop()
            elif 0xe0 <= mr <= 0xef:
                ev.append('cmp ' + node('add', top(), sti(ri)).replace('add', 'cmp', 1))
                if mr >= 0xe8:
                    pop()
        elif op in (0xdb, 0xdf) and 0xe8 <= mr <= 0xf7:   # fucomi / fcomi (+p)
            ev.append('cmp ' + node('add', top(), sti(ri)).replace('add', 'cmp', 1))
            if op == 0xdf:
                pop()
        elif op == 0xda and mr == 0xe9:
            a = pop()
            ev.append('cmp ' + node('add', a, pop()).replace('add', 'cmp', 1))
    insns = list(md.disasm(img.rd(va, n), va))
    if not bb:
        for i in insns:
            step(i)
        ev += ['local <- %s' % v for v in unread.values() if not v.startswith('int(')]
        return ev

    # bb: per basic block walk.  Blocks in address order; a block is walked once per distinct state that reaches it over a forward
    # edge (fall-through, jcc / jmp to a higher address); back edges are dropped.  A block no forward edge reaches (rotated loop
    # body, switch case) gets the out-state of the block before it with an empty x87 stack (the linear walk's state there).
    # Events are a set of (site, event): a site that sees one value on two paths counts once.
    # ponytail: at most CAP states per block (later paths dropped), no loop-carried x87 values; a real fixpoint if rows need it.
    def snap():
        return (tuple(st), dict(slots), dict(regs), dict(islots), dict(fresh), dict(unread), esp, called[0])

    def restore(s):
        nonlocal st, esp
        st = list(s[0])
        for d, src in zip((slots, regs, islots, fresh, unread), s[1:6]):
            d.clear()
            d.update(src)
        esp, called[0] = s[6], s[7]

    def key(s):
        return repr((s[0],) + tuple(sorted(d.items(), key=repr) for d in s[1:6]) + s[6:])

    def leave():
        for k, v in unread.items():
            if not v.startswith('int('):
                cur[0] = ('end', k)
                ev.append('local <- %s' % v)

    end = va + n
    lead, tgt = {va}, {}
    for i in insns:
        if i.group(x86.X86_GRP_JUMP) or i.mnemonic in ('ret', 'retn'):
            lead.add(i.address + i.size)
            o = i.operands
            if o and o[0].type == x86.X86_OP_IMM:
                tgt[i.address] = o[0].imm
                if va <= o[0].imm < end:
                    lead.add(o[0].imm)
    blocks = []
    for i in insns:
        if i.address in lead or not blocks:
            blocks.append([])
        blocks[-1].append(i)
    inc, prev = collections.defaultdict(dict), snap()
    for bi, blk in enumerate(blocks):
        start, last = blk[0].address, blk[-1]
        nxt = blocks[bi + 1][0].address if bi + 1 < len(blocks) else None
        states = list(inc.pop(start, {}).values())[:CAP] or [((),) + prev[1:]]
        for s in states:
            restore(s)
            for i in blk:
                cur[0] = i.address
                step(i)
            m, t = last.mnemonic, tgt.get(last.address)
            if m in ('ret', 'retn'):
                leave()
            elif last.group(x86.X86_GRP_JUMP):
                if t is not None and start < t < end:
                    inc[t].setdefault(key(snap()), snap())
                elif t is None or not va <= t < end:
                    leave()      # indirect jump (switch table) or tail jump out of the function
                if m != 'jmp' and nxt is not None:
                    inc[nxt].setdefault(key(snap()), snap())
            elif nxt is not None:
                inc[nxt].setdefault(key(snap()), snap())
            else:
                leave()
            prev = snap()
    return [e for s, e in sorted(set(ev), key=repr)]


def diff(a, b):
    """events whose value is a bare leaf are copies, not operation order (fpucopy.py covers copies): left out"""
    ca = collections.Counter(e for e in a if '(' in e.split(' <- ')[-1])
    cb = collections.Counter(e for e in b if '(' in e.split(' <- ')[-1])
    return sorted((ca - cb).elements()), sorted((cb - ca).elements())


def orig_table():
    t = {}
    for l in open(G + '/output/fnsizes.txt'):
        f = l.split()
        if len(f) >= 3 and f[2].isdigit():
            t[f[1]] = (int(f[0], 16), int(f[2]))
    return t


def ours_table(path):
    syms = []
    for l in open(path):
        f = l.split()
        if len(f) >= 4 and f[0].startswith('0001:') and re.match(r'^[0-9a-f]{8}$', f[2]):
            syms.append((int(f[2], 16), f[1], f[-1], 'f' in f[3:-1]))
    syms.sort()
    t = {}
    for k, (va, name, obj, isf) in enumerate(syms):
        if not isf or obj == 'app.obj' or k + 1 >= len(syms):
            continue
        nm = re.sub(r'@\d+$', '', name[1:] if name.startswith('_') else name)
        t[nm] = (va, min(syms[k + 1][0] - va, 0x4000), obj)
    return t


def selftest():
    """three-term sum a+b+c: (fld a; fadd b; fadd c; fstp g) vs (fld a; fadd c; fadd b; fstp g) must be a row; a commutative swap must not."""
    class Fake(Img):
        def __init__(self, code):
            self.code, self.delta = code, 0

        def rd(self, va, n):
            return self.code if va == 0x1000 and n else b''

        def writable(self, va):
            return True
    A = bytes.fromhex('d90510000000' 'd80514000000' 'd80518000000' 'd91d20000000')   # (x10 + x14) + x18 -> g20
    B = bytes.fromhex('d90510000000' 'd80518000000' 'd80514000000' 'd91d20000000')   # (x10 + x18) + x14
    C = bytes.fromhex('d90514000000' 'd80510000000' 'd80518000000' 'd91d20000000')   # (x14 + x10) + x18: swap only
    D = bytes.fromhex('d90510000000' 'd90514000000' 'dee9' 'd91d20000000')           # x10 - x14 (Intel DE E8+i = fsubp st1,st0: st1-st0)
    E = bytes.fromhex('d90514000000' 'd90510000000' 'dee1' 'd91d20000000')           # DE E0+i = fsubrp: st1 = st0 - st1 = x10 - x14
    ev = {k: events(Fake(v), 0x1000, len(v)) for k, v in dict(A=A, B=B, C=C, D=D, E=E).items()}
    assert ev['A'] == ['G20 <- add(G18,add(G10,G14))'], ev['A']
    assert diff(ev['A'], ev['B']) != ([], []), 'fixture: swapped three-term sum must be a row'
    assert diff(ev['A'], ev['C']) == ([], []), 'commutative swap must not be a row'
    assert ev['D'] == ev['E'] == ['G20 <- sub(G10,G14)'], (ev['D'], ev['E'])
    # bb: g20 = (x10 < x14 ? x18 : x1c) + x24, the x87 stack carried over the jmp; the linear walk loses the x18 path
    F = bytes.fromhex('d90510000000' 'd81d14000000' 'dfe0' 'f6c401' '7508' 'd90518000000' 'eb06' 'd9051c000000'
                      'd80524000000' 'd91d20000000')
    ev['F'] = events(Fake(F), 0x1000, len(F), bb=True)
    assert sorted(ev['F']) == ['G20 <- add(G18,G24)', 'G20 <- add(G1c,G24)', 'cmp cmp(G10,G14)'], ev['F']
    assert events(Fake(F), 0x1000, len(F), bb=False) == ['cmp cmp(G10,G14)', 'G20 <- add(G1c,G24)']
    assert all(events(Fake(v), 0x1000, len(v), bb=True) == ev[k] for k, v in dict(A=A, B=B, C=C, D=D).items())
    print('selftest PASS', ev)


def main():
    a = sys.argv
    if a[1] == 'selftest':
        return selftest()
    delta = int(a[4], 16) if a[4] != 'auto' else next(int(l.split()[2], 16) - 0x752364 for l in open(a[3]) if ' _g_race_load_752364 ' in l)
    ours = Img(a[2], delta)
    orig, ot, mt = Img(G + '/game/MFL.exe'), orig_table(), ours_table(a[3])
    if a[1] == 'fn':
        nm = a[5]
        ev_o, ev_m = events(orig, *ot[nm]), events(ours, mt[nm][0], mt[nm][1], True)
        x, y = diff(ev_o, ev_m)
        print('%s orig %d events, ours %d events, rows: orig-only %d ours-only %d' % (nm, len(ev_o), len(ev_m), len(x), len(y)))
        for e in x:
            print('  ORIG-ONLY', e)
        for e in y:
            print('  OURS-ONLY', e)
        if '-v' in a:
            for lab, im, (v0, n0) in (('orig', orig, ot[nm]), ('ours', ours, mt[nm][:2])):
                last = list(md.disasm_lite(im.rd(v0, n0), v0))[-1]
                print('%s %08x+%x decoded to %08x' % (lab, v0, n0, last[0] + last[1]))
            print('ORIG', *ev_o, sep='\n  ')
            print('OURS', *ev_m, sep='\n  ')
        return
    rows, per = [], collections.Counter()
    for nm, (va, n, obj) in sorted(mt.items()):
        if nm not in ot:
            continue
        x, y = diff(events(orig, *ot[nm]), events(ours, va, n, True))
        if x or y:
            rows.append((nm, obj, len(x), len(y), ' | '.join(x[:3]), ' | '.join(y[:3])))
            per[obj] += 1
    with open(a[5], 'w', newline='') as f:
        w = csv.writer(f)
        w.writerow(('function', 'obj', 'orig_only', 'ours_only', 'orig_first', 'ours_first'))
        w.writerows(rows)
    print('functions with rows', len(rows), 'of', sum(1 for nm in mt if nm in ot), 'files', len(per))


if __name__ == '__main__':
    main()
