#!/usr/bin/env python3
"""Delinker step 1: find every absolute pointer in MFL.exe (the exe has no base relocations).
Candidates: (1) 32-bit immediates / displacements of every instruction inside a Ghidra code run whose value lies in the image, (2) every 4-aligned word of
.rdata / .data / _rwdseg and of the non-instruction gaps of .text / _rwcseg (jump tables) with a value in the image, (3) every Ghidra reference whose
source word holds the target (also unaligned).  Evidence: Ghidra references (output/delink/ghidra_refs.csv from scripts/ExportRefs.java), function /
label starts, string shape of the target.
Output: output/delink_relocs.csv  site,target,kind,confidence,src,tsec,why   (site/target hex VAs; src = code | data; kind = code | jumptable |
code_interior | fntable | import | string | data | rva | header | const (round arithmetic immediate, not relocated); confidence high = Ghidra has this exact reference, medium = strong shape, low = value only)
Usage: classify.py [--refs output/delink/ghidra_refs.csv]"""
import bisect, collections, csv, os, re, struct, subprocess, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, "..", ".."))
EXE = os.path.normpath(os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe"))
REFS = sys.argv[sys.argv.index("--refs") + 1] if "--refs" in sys.argv else os.path.join(G, "output", "delink", "ghidra_refs.csv")
OUT = os.path.join(G, "output", "delink_relocs.csv")

def pe(path):
    d = open(path, "rb").read()
    e = struct.unpack_from("<I", d, 0x3C)[0]; ns = struct.unpack_from("<H", d, e + 6)[0]; osz = struct.unpack_from("<H", d, e + 20)[0]
    base, = struct.unpack_from("<I", d, e + 24 + 28); size, = struct.unpack_from("<I", d, e + 24 + 56)
    dirs = [struct.unpack_from("<II", d, e + 24 + 96 + 8 * i) for i in range(16)]
    secs = []
    for i in range(ns):
        o = e + 24 + osz + i * 40
        nm = d[o:o + 8].rstrip(b"\0").decode(); vs, va, rs, rp = struct.unpack_from("<IIII", d, o + 8); fl, = struct.unpack_from("<I", d, o + 36)
        secs.append(dict(name=nm, va=base + va, vs=vs, raw=d[rp:rp + min(rs, vs)], fl=fl))
    return d, base, size, dirs, secs

D, BASE, SIZE, DIRS, SECS = pe(EXE)
LO, HI = BASE, BASE + SIZE
def sec_of(v):
    for s in SECS:
        if s["va"] <= v < s["va"] + max(s["vs"], len(s["raw"])): return s
    return None
def word(v):
    s = sec_of(v)
    if not s or v - s["va"] + 4 > len(s["raw"]): return None
    return struct.unpack_from("<I", s["raw"], v - s["va"])[0]
IAT = (BASE + DIRS[12][0], BASE + DIRS[12][0] + DIRS[12][1])

# --- Ghidra data
refs, runs, labels, fnstart = collections.defaultdict(set), [], {}, set()
reftypes = {}
for r in csv.reader(open(REFS)):
    if r[0] == "C": runs.append((int(r[1], 16), int(r[2], 16))); continue
    if r[0] == "L":
        a = int(r[1], 16); labels[a] = r[2]
        if not r[2].startswith(("switch", "caseD")): fnstart.add(a)
        continue
    f, t = int(r[0], 16), int(r[1], 16); refs[f].add(t); reftypes[(f, t)] = (r[2], r[5])
for l in open(os.path.join(G, "output", "fnsizes.txt")): fnstart.add(int(l.split()[0], 16))
# Ghidra's own pointer guesses in data ("undefined *") are weak evidence: the CRT ctype table (16-bit 0x0084,0x0084) is full of them
def strong(site, v): return v in refs.get(site, ()) and reftypes[(site, v)][1] != "D:undefined *"
reftargets = {t for f, ts in refs.items() for t in ts if reftypes[(f, t)][1] != "D:undefined *"}
tsrc = collections.defaultdict(set)
for f, ts in refs.items():
    for t in ts: tsrc[t].add(f)
runs.sort(); rstarts = [a for a, b in runs]
fns = sorted(fnstart)
def in_run(a):
    i = bisect.bisect_right(rstarts, a) - 1
    return i >= 0 and a < runs[i][1]

# --- (1) instruction fields
code_secs = [s for s in SECS if s["fl"] & 0x20]
LINE = re.compile(r"\s*([0-9a-f]+):\t([0-9a-f ]+)\t(.*)$")
def objdump(a, b):
    out = subprocess.run(["objdump", "-d", "-z", "-M", "intel", "--insn-width=16", "--start-address=0x%x" % a, "--stop-address=0x%x" % b, EXE],
                         capture_output=True, text=True).stdout
    ins = {}
    for l in out.splitlines():
        m = LINE.match(l)
        if m: ins[int(m.group(1), 16)] = (bytes.fromhex(m.group(2).replace(" ", "")), m.group(3))
    return ins
ins = objdump(code_secs[0]["va"], code_secs[-1]["va"] + code_secs[-1]["vs"])
resync = 0
for a, b in runs:
    if a not in ins: ins.update(objdump(a, b)); resync += 1
BRANCH = re.compile(r"^(call|jmp|j[a-z]+|loop[a-z]*|jecxz)\s+0x[0-9a-f]+\s*(<.*>)?$")
# non-padding gaps of the code sections = code Ghidra never disassembled, or tables: decoded linearly as "code?" (instructions that overlap a word
# Ghidra references as data, e.g. a jump table entry, are skipped; those words come in through step 3)
dref = {f + k for f in refs if not in_run(f) for k in range(4)}
gaps, p = [], code_secs[0]["va"]
for a, b in runs + [(code_secs[-1]["va"] + len(code_secs[-1]["raw"]),) * 2]:
    if a > p:
        by = D[p - BASE:a - BASE]
        if len(by) - sum(1 for x in by if x in (0xCC, 0x90, 0)) > 2: gaps.append((p, a))
    p = max(p, b)
for a, b in gaps: ins.update(objdump(a, b))
# a gap that decodes as garbage is a data table (DirectInput object formats at 0x4ab4f0: in al,dx / push ds / add [eax],al): its aligned words go to step 2
ODD = re.compile(r"^(in|out|ins[bdw]?|outs[bdw]?|push +[cdefgs]s|pop +[cdefgs]s|arpl|les|lds|hlt|int|int3|into|bound|aa[adms]|da[as]|sahf|lahf|\(bad\)|\.byte|fwait|salc|xlat)\b|BYTE PTR \[eax\],al$")
def gap_is_code(a, b):
    n = odd = 0; p = a
    while p < b and p in ins:
        bs, txt = ins[p]; p += len(bs)
        if bs.strip(b"\x90\xcc") == b"" or bs == b"\x8d\xa4\x24\0\0\0\0" or bs == b"\x8d\x49\x00": continue      # alignment filler
        n += 1; odd += bool(ODD.search(txt.strip()))
    return n > 0 and odd <= 0.1 * n
data_gaps = [g for g in gaps if not gap_is_code(*g)]
gaps = [g for g in gaps if g not in data_gaps]
cand = {}          # site -> (value, src, insn addr)
rel32 = {}         # relative branch fields that cross sections: site -> (target, src)
nins = 0
for a, b, src in [r + ("code",) for r in runs] + [g + ("code?",) for g in gaps]:
    p = a
    while p < b:
        if p not in ins:              # objdump went out of step with Ghidra's instruction boundaries: decode again from here
            ins.update(objdump(p, b)); resync += 1
            if p not in ins: break
        bs, txt = ins[p]; nins += 1
        m_ = BRANCH.match(txt.strip())
        if m_ and len(bs) >= 5 and (bs[0] in (0xE8, 0xE9) or bs[0] == 0x0F and 0x80 <= bs[1] <= 0x8F):
            t_ = p + len(bs) + struct.unpack_from("<i", bs, len(bs) - 4)[0]
            if sec_of(t_) is not None and sec_of(t_) is not sec_of(p): rel32[p + len(bs) - 4] = (t_, src)     # .text <-> _rwcseg: must move with the target section
        if not m_ and not (src == "code?" and any(p + k in dref for k in range(len(bs)))):
            for h in set(re.findall(r"0x([0-9a-f]+)", txt)):
                v = int(h, 16)
                if LO <= v < HI:
                    k = bs.find(struct.pack("<I", v), 1)
                    while k >= 0:
                        cand[p + k] = (v, src, p); k = bs.find(struct.pack("<I", v), k + 1)
        p += len(bs)

# --- (2) data words (aligned) in data sections and in the non-instruction gaps of code sections
for s in SECS:
    if s["name"] == ".rsrc": continue
    raw = s["raw"]
    for o in range(0, len(raw) - 3, 4):
        a = s["va"] + o
        if s["fl"] & 0x20 and (in_run(a) or in_run(a + 3) or any(g[0] <= a < g[1] for g in gaps)): continue
        v = struct.unpack_from("<I", raw, o)[0]
        if LO <= v < HI: cand.setdefault(a, (v, "data", None))
# jump tables inside gaps decoded as code (Ghidra missed the whole function, e.g. 0x40fe50 with its table at 0x40fee0): runs of >= 3 aligned words
# pointing into code within 64 KB replace the garbage instruction fields decoded over them
cs0, cs1 = code_secs[0]["va"], code_secs[-1]["va"] + len(code_secs[-1]["raw"])
jtw = lambda a: (lambda w: w is not None and cs0 <= w < cs1 and abs(w - a) < 0x10000)(word(a))
for a, b in gaps:
    p = (a + 3) & ~3
    while p + 4 <= b:
        q = p
        while q + 4 <= b and jtw(q): q += 4
        if q - p >= 12:
            for k in range(p - 3, q):
                if k in cand and cand[k][1] == "code?": del cand[k]
            for k in range(p, q, 4): cand[k] = (word(k), "data", None)
        p = max(q, p + 4)
# --- (3) Ghidra data references whose word holds the target (unaligned too); RVA fields
rva = {}
for f, ts in refs.items():
    if in_run(f) or not sec_of(f): continue
    w = word(f)
    for t in ts:
        if w == t: cand.setdefault(f, (t, "data", None))
        elif w is not None and w + BASE == t and "ImageBaseOffset" in reftypes[(f, t)][1]: rva[f] = t

# targets used as an address somewhere: a memory operand or a data word
def is_mem(txt, v):
    """v is (part of) a memory operand: [..0x..] or ds:0x.. in objdump syntax"""
    return "[" in txt and ("0x%x" % v) in txt[txt.index("["):txt.index("]")] or ("ds:0x%x" % v) in txt
memtargets = {v for site, (v, src, ia) in cand.items() if src == "data" and v in refs.get(site, ()) or src != "data" and is_mem(ins[ia][1], v)}
# --- classify
def is_string(t):
    s = sec_of(t)
    if not s: return False
    raw, o = s["raw"], t - s["va"]
    n = 0
    while o + n < len(raw) and 32 <= raw[o + n] < 127 or (o + n < len(raw) and raw[o + n] in (9, 10, 13)): n += 1
    return n >= 2 and o + n < len(raw) and raw[o + n] == 0
def containing_fn(t):
    i = bisect.bisect_right(fns, t) - 1
    return fns[i] if i >= 0 else None
def looks_text(site):
    """the word is part of ASCII text (a printable run of >= 6 bytes covering it) or UTF-16 text (x 00 y 00)"""
    s = sec_of(site); raw, o = s["raw"], site - s["va"]
    b = raw[o:o + 4]
    if b[1] == 0 and b[3] == 0 and 32 <= b[0] < 127 and 32 <= b[2] < 127: return True
    if b[3] == 0 and all(x < 128 and chr(x).isalnum() for x in b[:3]): return True        # 3 letter codes ("ENU\0") of the CRT locale tables
    pr = lambda x: 32 <= x < 127 or x in (9, 10, 13)
    l = o
    while l > 0 and pr(raw[l - 1]): l -= 1
    r = o
    while r < len(raw) and pr(raw[r]): r += 1
    return r - l >= 6 and r >= o + 3
def short_string(site):
    """3 printable chars + NUL between text, not between pointers: 0x5de97c "U:\\" (the CD drive prefix, Ghidra typed it a pointer)"""
    b = word(site).to_bytes(4, "little")
    if b[3] or not all(32 <= x < 127 for x in b[:3]): return False
    nb = [word(site + k) for k in (-4, 4)]
    if any(w is not None and LO <= w < HI for w in nb): return False
    return any(w is not None and all(32 <= x < 127 or x == 0 for x in w.to_bytes(4, "little")) and sum(32 < x < 127 for x in w.to_bytes(4, "little")) >= 3 for w in nb)
def pair_run(site):
    """a word of two small 16-bit values (< 0x200) between two more such words: a ctype style table row (0x5e6db0 = 0x0020, 0x0048 = "pointer" 0x480020, the address of a real function, which
    made the rule below keep it: relocated by the delta, the space entry 0x48 became 0x48 + (delta >> 16), 'digit' for some deltas = the parked delta dependent crashes of 2026-10-10)"""
    small = lambda w: w is not None and (w & 0xFFFF) < 0x200 and (w >> 16) < 0x200
    return small(word(site)) and small(word(site - 4)) and small(word(site + 4))
def u16_array(site):
    """inside an array of small 16-bit values (< 0x200; the bison tables of the D3DX shader assembler at 0x631000-0x636300: 0x0044010f, 0x00630164 ...)"""
    small = lambda w: w is not None and (w >> 9) & 0x7F == 0 and w >> 25 == 0      # both halves < 0x200
    nb = [word(site + k) for k in (-8, -4, 4, 8)]
    return small(word(site) & 0xFF00FFFF | 0) and (word(site) >> 16) < 0x200 and sum(small(w) for w in nb) >= 3
rows, rejected, u16rej = [], collections.Counter(), {}
for site, (v, src, ia) in sorted(cand.items()):
    jt = sec_of(site)["fl"] & 0x20 and sec_of(v) and sec_of(v)["fl"] & 0x20 and abs(v - site) < 0x10000     # jump table shape: never text
    if src == "data" and not jt and v in fnstart and word(site).to_bytes(4, "little")[1::2] == b"\0\0" and pair_run(site):
        rejected["u16 pair (function start by chance)"] += 1; u16rej[site] = v; continue      # 0x5e6db0 / 0x5e6fb8 / 0x5e70b8: ctype rows 0x0020, 0x0048 = 0x480020 = a function start
    if src == "data" and not jt and v not in fnstart:   # even Ghidra "pointer" typed words: 0x5e0be4 "png\0" was typed pointer     # a word naming a function start stays (0x62e7cc -> 0x4e5070 = "pPN\0" is the RW D3D9 render state getter)
        if looks_text(site): rejected["text"] += 1; continue
        if short_string(site): rejected["short string"] += 1; continue
        if u16_array(site) and v not in fnstart and v not in labels: rejected["u16 array"] += 1; u16rej[site] = v; continue
        b = word(site).to_bytes(4, "little")
        if b[1] == 0 and b[3] == 0 and (v not in fnstart and v not in labels and v not in reftargets or pair_run(site)):
            rejected["u16 pair"] += 1; u16rej[site] = v; continue      # two 16-bit values (the CRT ctype table: 0x0081,0x0081 = "pointer" 0x810081 broke Lua's lexer)
        if sec_of(v) and sec_of(v)["name"] == ".rsrc": rejected["data->rsrc"] += 1; continue     # resources are reached by RVA through the resource directory, never by a stored VA
    ts = sec_of(v)
    ghidra = strong(site, v) if src == "data" else v in refs.get(ia, ())
    auto = src == "data" and not ghidra and v in refs.get(site, ())
    if v < SECS[0]["va"]: kind = "header"
    elif ts is None: kind = "unknown"
    elif ts["name"] == ".rsrc": kind = "rsrc"
    elif ts["fl"] & 0x20:
        if v in fnstart: kind = "code"
        elif src == "data" and sec_of(site)["fl"] & 0x20 and site > containing_fn(v) and site - containing_fn(v) < 0x10000: kind = "jumptable"
        else: kind = "code_interior"
    elif IAT[0] <= v < IAT[1]: kind = "import"
    elif is_string(v): kind = "string"
    else: kind = "data"
    mask = bin(v).count("1") <= 2 or (v + 1) & v == 0 or v % 10000 == 0 and v not in fnstart and v not in memtargets   # 0x800000, 0x7fffff masks; 5000000 = 0x4c4b40 (mov [esi+0xf8],5000000 is a time, not code)
    if src != "data" and not is_mem(ins[ia][1], v) and v not in labels and (mask or ins[ia][1].split()[0] in ("test", "and", "or", "xor")):   # bit operations never take an address     # (round arithmetic immediates are NOT constants in general: add edx,0x8cb500 = array base, cmp edi,0x5e9d00 = array end)
        conf, why, kind = "low", "mask-like immediate (<= 2 bits set or 2^n-1); not an address", "const"      # cmp eax,0x800000 / mov ecx,0x989680: constants Ghidra took for addresses
    elif ghidra: conf, why = "high", "ghidra"
    elif src != "data":
        # value used as an address operand ([0x...]) or pointing at a known start / referenced address: medium
        bs, txt = ins[ia]
        mem = is_mem(txt, v)
        conf, why = ("medium", "mem operand" if mem else "known target") if mem or v in fnstart or v in labels or v in reftargets else ("low", "imm only")
        if src == "code?": conf, why = "low", "undisassembled gap; " + why
    else:
        conf, why = ("medium", "known target") if v in fnstart or v in labels or v in reftargets or kind in ("string", "jumptable") else ("low", "value only")
        if auto: conf, why = "medium", "Ghidra auto pointer (undefined *)"
    rows.append([site, v, kind, conf, src, ts["name"] if ts else "-", why])
# a rejected 16-bit-pair word next to an accepted code pointer is a code pointer of the same table (0x64be08: 0x5d0040, 0x5d0070, 0x5d00a0, ...)
iscode = lambda v: sec_of(v) is not None and sec_of(v)["fl"] & 0x20
codesites = {r[0] for r in rows if iscode(r[1])}
changed = True
while changed:
    changed = False
    for site, v in list(u16rej.items()):
        if iscode(v) and (site - 4 in codesites or site + 4 in codesites):
            rows.append([site, v, "code" if v in fnstart else "code_interior", "low", "data", sec_of(v)["name"], "16-bit-pair shape; neighbour of a code pointer"])
            codesites.add(site); del u16rej[site]; rejected["recovered from u16 rules"] += 1; changed = True
rows.sort()
# function tables: runs of >= 3 consecutive data words that are code pointers
by = {r[0]: r for r in rows}
for r in rows:
    if r[4] == "data" and r[2] == "code":
        n = sum(1 for d in (-8, -4, 4, 8) if by.get(r[0] + d, [0, 0, ""])[2] == "code")
        if n >= 2: r[2] = "fntable"
for f, (t, src) in rel32.items(): rows.append([f, t, "rel32", "high" if src == "code" else "low", "code", sec_of(t)["name"], "relative branch into another section"])
for f, t in rva.items(): rows.append([f, t, "rva", "high", "data", sec_of(t)["name"] if sec_of(t) else "-", "ghidra ImageBaseOffset32"])
rows.sort()
with open(OUT, "w") as o:
    o.write("site,target,kind,confidence,src,tsec,why\n")
    for r in rows: o.write("%08x,%08x,%s,%s,%s,%s,%s\n" % tuple(r))

# --- report
c = collections.Counter((r[4], r[2], r[3]) for r in rows)
print("instructions decoded %d in %d Ghidra code runs (%d resynced) + %d undisassembled gaps decoded as code (%d bytes; %d gaps look like data); candidates %d -> %s; rejected %s" % (
    nins, len(runs), resync, len(gaps), sum(b - a for a, b in gaps), len(data_gaps), len(rows), OUT, dict(rejected)))
for src in ("code", "code?", "data"):
    for kind in sorted({k for s, k, _ in c if s == src}):
        print("  %-4s %-14s high %6d  medium %6d  low %6d" % (src, kind, c[(src, kind, "high")], c[(src, kind, "medium")], c[(src, kind, "low")]))
# Ghidra references to memory that are not explained by a site (value not stored in the source word / instruction)
found = {(r[4] != "data" and cand[r[0]][2] or r[0], r[1]) for r in rows if r[0] in cand}
miss = collections.Counter()
for f, ts in refs.items():
    for t in ts:
        if (f, t) in found or f in rva: continue
        rt, k = reftypes[(f, t)]
        if rt.endswith(("JUMP", "CALL")) and k.startswith("I") and rt != "COMPUTED_JUMP" and rt != "COMPUTED_CALL": continue      # relative branches
        miss[(rt, "insn" if k.startswith("I") else "data")] += 1
print("Ghidra refs without a site (register / propagated refs, computed jumps: no absolute word to relocate):", dict(miss))
