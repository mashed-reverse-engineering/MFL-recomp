#!/usr/bin/env python3
"""Overlay recompiled C onto MFL.exe: add a new PE section holding the code/data of COFF objects (cl 13.00 output),
resolve their external symbols to the original addresses and put `jmp new` at the entry of every function they
replace.  The result is a hybrid exe that runs the C version of those functions in the original program.
Usage: overlay.py <orig.exe> <out.exe> <symbols.txt> [--no-patch] [--replace a,b,c] [--hook-call <VA>=<fn>] [--patch-dword <VA>=<symbol>] <obj> [<obj> ...]   (also writes <out.exe>.map with the new symbol addresses)
symbols.txt: "<name> <hex VA>" per line (ghidra/scripts/ExportSymbols.java).  Without --replace every public function
defined by the objects that also exists in symbols.txt is patched."""
import os
import re, struct, sys, hashlib

BASE = 0x400000


def undecorate(n):
    if n.startswith("@"):                       # @name@N fastcall
        return n[1:].split("@")[0]
    if n.startswith("_"):
        n = n[1:]
    return n.split("@")[0] if re.match(r"^\w+@\d+$", n) else n


class Obj:
    def const_secs(self):
        if not hasattr(self, "_cs"):
            self._cs = {x["sec"] - 1 for x in self.syms if x and x["sec"] > 0 and x["name"].startswith(("??_C@", "_real@", "__real@", "__xmm@"))}
        return self._cs

    def __init__(self, path):
        d = self.d = open(path, "rb").read()
        self.path = path
        _ed = os.environ.get("OVERLAY_EXTDATA")      # directory of SDK library objects (build_overlay LIBOBJ_DIR): their writable data (.data / .bss / commons) is NOT copied, the symbols resolve to the original addresses (tools/libsyms.py map)
        self.extdata = bool(_ed) and os.path.dirname(os.path.abspath(path)) == os.path.abspath(_ed)
        machine, nsec, ts, symoff, nsym, opt, ch = struct.unpack_from("<HHIIIHH", d, 0)
        self.nsym, self.symoff = nsym, symoff
        self.strtab = symoff + nsym * 18
        self.secs = []
        for i in range(nsec):
            o = 20 + opt + i * 40
            name, vs, va, rawsz, rawptr, relptr, lnptr, nrel, nln, flags = struct.unpack_from("<8sIIIIIIHHI", d, o)
            self.secs.append(dict(name=name.rstrip(b"\0").decode(), size=rawsz, ptr=rawptr, relptr=relptr, nrel=nrel, flags=flags))
        self.syms = []
        i = 0
        while i < nsym:
            o = symoff + i * 18
            raw = d[o:o + 8]
            if raw[:4] == b"\0\0\0\0":
                off = struct.unpack_from("<I", raw, 4)[0]
                nm = d[self.strtab + off:d.index(b"\0", self.strtab + off)].decode()
            else:
                nm = raw.rstrip(b"\0").decode()
            value, sec, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
            self.syms.append(dict(name=nm, value=value, sec=sec, type=typ, cls=cls))
            for _ in range(aux):
                self.syms.append(None)
            i += 1 + aux


def main():
    args = sys.argv[1:]
    patch = True
    count = False
    pad = 0
    hooks = []
    dwords = []
    only = None
    skip = set()
    objs = []
    i = 0
    pos = []
    while i < len(args):
        if args[i] == "--no-patch":
            patch = False
        elif args[i] == "--pad":        # round the virtual size of the new section up to a multiple of this (same heap layout for builds of different size)
            pad = int(args[i + 1], 0)
            i += 1
        elif args[i] == "--count":      # coverage: the entry jmp goes to a thunk "inc [counter]; jmp new"; counters listed in <out>.map as "count:<fn> <va>"
            count = True
        elif args[i] == "--hook-call":
            hooks.append(args[i + 1])
            i += 1
        elif args[i] == "--patch-dword":
            dwords.append(args[i + 1])
            i += 1
        elif args[i] == "--skip":       # do not patch these functions (disabled bridge stubs)
            skip |= set(args[i + 1].split(","))      # several --skip options add up
            i += 1
        elif args[i] == "--replace":
            only = set(args[i + 1].split(","))
            i += 1
        else:
            pos.append(args[i])
        i += 1
    exe_path, out_path, sym_path = pos[:3]
    objs = [Obj(p) for p in pos[3:]]
    symmap = {}
    multi = {}          # name -> all addresses (a name at several addresses: the first one wins in symmap)
    ambiguous = set()
    for l in open(sym_path):
        p = l.split()
        if len(p) == 2:
            symmap.setdefault(p[0], int(p[1], 16))
            multi.setdefault(p[0], set()).add(int(p[1], 16))
    exe = bytearray(open(exe_path, "rb").read())
    orig_exe = bytes(exe)       # pristine copy for the relocation target check
    e = struct.unpack_from("<I", exe, 0x3C)[0]
    nsec, = struct.unpack_from("<H", exe, e + 6)
    osz, = struct.unpack_from("<H", exe, e + 20)
    falign, = struct.unpack_from("<I", exe, e + 24 + 36)
    salign, = struct.unpack_from("<I", exe, e + 24 + 32)
    imgsize, = struct.unpack_from("<I", exe, e + 24 + 56)
    sect_tbl = e + 24 + osz
    secs = []
    for k in range(nsec):
        o = sect_tbl + k * 40
        vs, va, rs, rp = struct.unpack_from("<IIII", exe, o + 8)
        secs.append((va, max(vs, rs), rp))

    def va_to_off(va):
        rva = va - BASE
        for sva, sz, rp in secs:
            if sva <= rva < sva + sz:
                return rp + rva - sva
        raise SystemExit("VA %x outside the image" % va)

    # import address table: function name -> IAT slot VA
    imports = {}
    imp_rva, imp_size = struct.unpack_from("<II", exe, e + 24 + 96 + 8)
    def rva_off(rva):
        for sva, sz, rp in secs:
            if sva <= rva < sva + sz:
                return rp + rva - sva
        return None
    d_off = rva_off(imp_rva) if imp_rva else None
    while d_off is not None:
        oft, ts, fc, name_rva, ft = struct.unpack_from("<IIIII", exe, d_off)
        if not name_rva:
            break
        lt = rva_off(oft or ft)
        k = 0
        while lt is not None:
            v, = struct.unpack_from("<I", exe, lt + 4 * k)
            if v == 0:
                break
            if not v & 0x80000000:
                no = rva_off(v)
                fn = bytes(exe[no + 2:exe.index(b"\0", no + 2)]).decode()
                imports.setdefault(fn, BASE + ft + 4 * k)
            k += 1
        d_off += 20
    new_rva = (imgsize + salign - 1) // salign * salign
    new_va = BASE + new_rva
    # layout: code, then rdata/data (initialised), then bss
    layout = []   # (obj, sec index, offset in new section)
    blob = bytearray()
    bss = 0
    bss_items = []
    def align(n):
        while len(blob) % n:
            blob.append(0xCC)
    for ob in objs:
        for si, s in enumerate(ob.secs):
            nm, fl = s["name"], s["flags"]
            if nm.startswith(".debug") or nm == ".drectve" or (fl & 0x200) or (fl & 0x800):
                continue
            if ob.extdata and (fl & 0x80000000) and not (fl & 0x20) and si not in ob.const_secs():      # writable data of a library object: lives at its original address (string literals and float constants of VC6 objects sit in writable COMDAT sections but are constants: copied)
                continue
            if fl & 0x80:   # uninitialised
                bss_items.append((ob, si, s["size"]))
                continue
            align(16)
            layout.append((ob, si, len(blob)))
            blob += ob.d[s["ptr"]:s["ptr"] + s["size"]]
    CAP = 4096
    thunk_off = None
    align(16)
    bss_start = len(blob)
    off_in_bss = 0
    placed = {}
    for ob, si, size in bss_items:
        off_in_bss = (off_in_bss + 15) // 16 * 16
        placed[(id(ob), si)] = bss_start + off_in_bss
        off_in_bss += size
    for ob, si, off in layout:
        placed[(id(ob), si)] = off
    # common symbols (class 2, section 0, value > 0): allocate in bss
    commons = {}
    for ob in objs:
        for s in ob.syms:
            if s and s["cls"] == 2 and s["sec"] == 0 and s["value"] > 0 and s["name"] not in commons and not ob.extdata:
                off_in_bss = (off_in_bss + 15) // 16 * 16
                commons[s["name"]] = bss_start + off_in_bss
                off_in_bss += s["value"]
    cnt_off = None
    total_vsize = bss_start + off_in_bss

    # stubs `jmp [IAT slot]` for imported functions the objects call directly (_Name@N)
    thunks = {}
    wanted = set()
    for ob in objs:
        for sy in ob.syms:
            if sy and sy["cls"] == 2 and sy["sec"] == 0 and sy["value"] == 0:
                nm = undecorate(sy["name"])
                if nm in imports and nm not in symmap and not sy["name"].startswith("__imp_"):
                    wanted.add(nm)
    # blob is complete up to bss_start; thunks must live inside the file part, so append them before bss_start was fixed:
    # rebuild layout offsets by inserting the stubs at the end of the initialised data (bss follows)
    stub_off = bss_start
    blob_extra = bytearray()
    for nm in sorted(wanted):
        thunks[nm] = stub_off + len(blob_extra)
        blob_extra += b"\xFF\x25" + struct.pack("<I", imports[nm]) + b"\xCC\xCC"
    blob += blob_extra
    shift = len(blob_extra)
    bss_start += shift
    for k2 in list(placed.keys()):
        if placed[k2] >= stub_off and (k2[1] is not None):
            pass
    for k2, v2 in list(placed.items()):
        if v2 >= stub_off:
            placed[k2] = v2 + shift
    for nm2, v2 in list(commons.items()):
        commons[nm2] = v2 + shift
    total_vsize += shift
    if count:      # the counter thunks and the table go BEHIND the bss (which becomes file backed zeros): every real function and variable keeps the address it has in a build without --count.
        # (Before, the thunks sat in front of the bss and moved the data of the replaced code by 80 KB: a count build had another layout than the same build without --count.)
        blob += b"\0" * (total_vsize - len(blob))
        align(16)
        thunk_off = len(blob)
        blob += b"\xCC" * (16 * CAP)
        cnt_off = len(blob)
        blob += b"\0" * (4 * CAP)
        total_vsize = len(blob)
    # defined external symbols -> new VA
    defined = {}
    defsec = {}
    dup_defs = []
    for ob in objs:
        for s in ob.syms:
            if s and s["cls"] == 2 and s["sec"] > 0:
                key = (id(ob), s["sec"] - 1)
                if key in placed:
                    nm_ = undecorate(s["name"])
                    if nm_ in defined and not ob.extdata and not nm_.startswith(("?", "__real", "__xmm", "_real")):      # library objects: identical inline / COMDAT copies in several members (IsEqualGUID, operator==) are normal   # a symbol defined twice: the later object silently won before
                        dup_defs.append(nm_ + "(" + os.path.basename(ob.path) + " + " + os.path.basename(defsec[nm_][0].path) + ")")
                    defined[nm_] = new_va + placed[key] + s["value"]
                    defsec[nm_] = (ob, s["sec"] - 1, s["value"], placed[key])

    def resolve(ob, s):
        if s["sec"] > 0:
            if (id(ob), s["sec"] - 1) not in placed:      # data section of a library object that was not copied (extdata)
                nm0 = undecorate(s["name"])
                if nm0 in symmap: return symmap[nm0]
                raise KeyError(nm0)
            return new_va + placed[(id(ob), s["sec"] - 1)] + s["value"]
        nm = undecorate(s["name"])
        if nm in defined:
            if nm in defsec and (id(defsec[nm][0]), defsec[nm][1]) in bad_secs and nm in symmap:      # a library function whose new copy is unusable (unplaced symbols): call the original
                return symmap[nm]
            return defined[nm]
        if s["name"] in commons:
            return new_va + commons[s["name"]]
        if s["name"].startswith("__imp_"):
            fn = undecorate(s["name"][5:])
            if fn in imports:
                return imports[fn]
        if nm in thunks:
            return new_va + thunks[nm]
        if nm in symmap:
            if len(multi.get(nm, ())) > 1: ambiguous.add(nm)
            return symmap[nm]
        if ob.extdata and "_" + nm in symmap:      # C runtime names in the Ghidra export keep their underscore (`_calloc`)
            return symmap["_" + nm]
        raise KeyError(nm)

    missing = set()
    missing_from = {}
    unplaced = {}
    unplaced_idx = {}
    bad_secs = set()      # (object, section) of library functions that refer to a symbol we cannot place: never patched in
    changed = True      # pre-pass (fixpoint): library function sections that refer to symbols that cannot be placed, or call such a function that has no original address to fall back to
    while changed:
        changed = False
        for ob, si, off in layout:
            if not ob.extdata or (id(ob), si) in bad_secs: continue
            s = ob.secs[si]
            for r in range(s["nrel"]):
                va, symidx, rtype = struct.unpack_from("<IIH", ob.d, s["relptr"] + r * 10)
                try: resolve(ob, ob.syms[symidx])
                except KeyError:
                    bad_secs.add((id(ob), si)); changed = True; break
                sy_ = ob.syms[symidx]
                nm_ = undecorate(sy_["name"])
                if sy_["sec"] == 0 and nm_ in defsec and (id(defsec[nm_][0]), defsec[nm_][1]) in bad_secs and nm_ not in symmap:
                    bad_secs.add((id(ob), si)); changed = True; break
                if sy_["sec"] > 0 and sy_["sec"] - 1 != si and (id(ob), sy_["sec"] - 1) in bad_secs:      # a static (local) function / data section of the same object that is bad: no original address to fall back to
                    bad_secs.add((id(ob), si)); changed = True; break
    if os.environ.get("DEBUGFN"):
        for k_ in os.environ["DEBUGFN"].split(","):
            print("DEBUGFN", k_, "defsec" if k_ in defsec else "NOT in defsec", (id(defsec[k_][0]), defsec[k_][1]) in bad_secs if k_ in defsec else None, "extdata" if k_ in defsec and defsec[k_][0].extdata else "", file=sys.stderr)
            if k_ in defsec:
                ob_, si_ = defsec[k_][0], defsec[k_][1]
                for r_ in range(ob_.secs[si_]["nrel"]):
                    va_, symidx_, rt_ = struct.unpack_from("<IIH", ob_.d, ob_.secs[si_]["relptr"] + r_ * 10)
                    sy_ = ob_.syms[symidx_]
                    try: t_ = hex(resolve(ob_, sy_))
                    except KeyError as e_: t_ = "KeyError %s" % e_
                    print("   reloc", hex(va_), sy_["name"], "sec", sy_["sec"], "cls", sy_["cls"], "->", t_, file=sys.stderr)
    for ob, si, off in layout:
        s = ob.secs[si]
        for r in range(s["nrel"]):
            va, symidx, rtype = struct.unpack_from("<IIH", ob.d, s["relptr"] + r * 10)
            sym = ob.syms[symidx]
            try:
                target = resolve(ob, sym)
            except KeyError as ex:
                if ob.extdata:      # a library object function that refers to something we cannot place: it is only a problem when that function is executed (it is not patched in when it does not exist in the exe)
                    unplaced.setdefault(os.path.basename(ob.path), set()).add(str(ex)); target = 0xDEAD0000 + 16 * unplaced_idx.setdefault(str(ex), len(unplaced_idx)); bad_secs.add((id(ob), si))
                else:
                    missing.add(str(ex)); missing_from.setdefault(str(ex), os.path.basename(ob.path))
                    continue
            at = off + va
            cur, = struct.unpack_from("<I", blob, at)
            if rtype == 6:       # DIR32
                struct.pack_into("<I", blob, at, (target + cur) & 0xFFFFFFFF)
            elif rtype == 0x14:  # REL32
                P = new_va + at
                struct.pack_into("<I", blob, at, (target + cur - (P + 4)) & 0xFFFFFFFF)
            elif rtype == 7:     # DIR32NB
                struct.pack_into("<I", blob, at, (target - BASE + cur) & 0xFFFFFFFF)
            else:
                raise SystemExit("unsupported relocation type %d in %s" % (rtype, ob.path))
    def _jmp_final(va):
        for _ in range(4):
            try: o = va_to_off(va)
            except SystemExit: return va
            if orig_exe[o:o + 1] != b"\xe9": break
            va = (va + 5 + struct.unpack_from("<i", orig_exe, o + 1)[0]) & 0xFFFFFFFF
        return va
    def _same_const(n):
        sz = 8 if re.match(r"(k_d_|double_)", n) else 4 if re.match(r"(k_f_|float_)", n) else 0
        return bool(sz) and len({orig_exe[va_to_off(a):va_to_off(a) + sz] for a in multi[n]}) == 1
    bad_amb = sorted(n for n in ambiguous if len({_jmp_final(a) for a in multi[n]}) > 1 and not _same_const(n))
    if bad_amb and os.environ.get("RELCHECK", "strict") != "off":
        raise SystemExit("names that exist at several addresses (not thunks of one function): the overlay would pick the first one: " + " ".join("%s(%s)" % (n, ",".join("%08x" % a for a in sorted(multi[n]))) for n in bad_amb))
    if unplaced_idx:      # a crash at 0xDEAD0000 + 16 * n names the symbol n that could not be placed
        open(sys.argv[2] + ".unplaced.txt", "w").write("\n".join("%08x %s" % (0xDEAD0000 + 16 * i, n) for n, i in sorted(unplaced_idx.items(), key=lambda x: x[1])) + "\n")
    for k_, v_ in sorted(unplaced.items()): print("library object %s: %d symbols not placed (only a problem if a function that uses them is patched in): %s" % (k_, len(v_), " ".join(sorted(v_))[:300]), file=sys.stderr)
    if missing:
        print("unresolved symbols:", " ".join("%s(%s)" % (m, missing_from.get(m, "?")) for m in sorted(missing)), file=sys.stderr)
        sys.exit(2)

    patched = []
    counters = []
    wiped = []
    wiped_bytes = [0]
    wipe_sizes = None
    if os.environ.get("OVERLAY_WIPE"):
        wipe_sizes = {}
        wipe_keep = set()      # functions that other code enters in the middle (output/interior_refs.csv, tools/interior_refs.py): their bodies stay
        _ip = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "output", "interior_refs.csv")
        if os.path.exists(_ip):
            for l_ in list(open(_ip))[1:]:
                q_ = l_.strip().split(",")
                if len(q_) >= 4: wipe_keep.add(q_[3])
        for l_ in open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "output", "fnsizes.txt")):
            q_ = l_.split()
            if len(q_) == 3: wipe_sizes[int(q_[0], 16)] = int(q_[2])
    if patch:
        for nm, nva in defined.items():
            if nm in symmap and (only is None or nm in only) and nm not in skip and not (nm in defsec and (id(defsec[nm][0]), defsec[nm][1]) in bad_secs):
                # only code symbols: the target must be inside the original .text
                ova = symmap[nm]
                if not (BASE + secs[0][0] <= ova < BASE + secs[0][0] + secs[0][1]):
                    continue
                off = va_to_off(ova)
                target = nva
                if count:
                    k = len(patched)
                    if k >= CAP:
                        raise SystemExit("--count: more than %d patched functions" % CAP)
                    tva = new_va + thunk_off + 16 * k
                    cva = new_va + cnt_off + 4 * k
                    blob[thunk_off + 16 * k:thunk_off + 16 * k + 13] = b"\x9C\xFF\x05" + struct.pack("<I", cva) + b"\x9D\xE9" + struct.pack("<i", nva - (tva + 13))   # pushfd; inc [counter]; popfd; jmp (inc changes flags a caller may still test)
                    counters.append((nm, cva))
                    target = tva
                exe[off] = 0xE9
                struct.pack_into("<i", exe, off + 1, target - (ova + 5))
                patched.append(nm)
                if wipe_sizes is not None and wipe_sizes.get(ova, 0) > 5 and nm not in wipe_keep:      # OVERLAY_WIPE=1: the rest of the replaced original function becomes int3 (anything that still enters its middle crashes visibly)
                    for k_ in range(off + 5, off + wipe_sizes[ova]): exe[k_] = 0xCC
                    wiped.append(nm); wiped_bytes[0] += wipe_sizes[ova]

    if wiped: print("OVERLAY_WIPE: %d original function bodies (%d bytes) filled with int3 after the entry jmp" % (len(wiped), wiped_bytes[0]), file=sys.stderr)
    # ---- relocation target check (what a byte compare with masked relocation fields cannot see) ----
    # for every patched function that is byte identical to the original apart from its relocated fields, each field must lead to the same place as the original's: the same
    # address, the replacement we link for it, an equal literal (string / float constant), or a one instruction jmp thunk to the other.  RELCHECK=warn prints instead of failing, RELCHECK=off skips.
    relmode = os.environ.get("RELCHECK", "strict")
    if relmode != "off" and patch:
        new_of_name = {}
        for nm_, v in defined.items(): new_of_name[v] = nm_
        for nm_, off_ in commons.items(): new_of_name[new_va + off_] = nm_.lstrip("_")
        def orig_u32(va):
            o = va_to_off(va); return struct.unpack_from("<I", orig_exe, o)[0]
        def bytes_at(va, n):
            if new_va <= va < new_va + len(blob): return bytes(blob[va - new_va:va - new_va + n])
            try: o = va_to_off(va)
            except SystemExit: return b""
            return orig_exe[o:o + n]
        def thunk_target(va):          # jmp rel32 -> target, jmp [mem] -> ("ind", mem)
            b = bytes_at(va, 6)
            if len(b) >= 5 and b[0] == 0xE9: return (va + 5 + struct.unpack_from("<i", b, 1)[0], None)
            if len(b) >= 6 and b[:2] == b"\xFF\x25": return (None, struct.unpack_from("<I", b, 2)[0])
            return (None, None)
        def same_literal(a, b, name=""):
            x, y = bytes_at(a, 40), bytes_at(b, 40)
            if not x or not y: return False
            if x[:1] == b"\0" and y[:1] == b"\0": return True          # the empty string
            if name.startswith("??_C@") or (b"\0" in x and 0 < x.index(b"\0") < 39 and x[:x.index(b"\0")].isascii() and all(32 <= c < 127 or c in (9, 10, 13) for c in x[:x.index(b"\0")])):
                if b"\0" in x: n = x.index(b"\0") + 1; return x[:n] == y[:n]     # a string: compare it completely
            n = 8 if re.match(r"_?_real@8@", name) else 4                         # a float / double constant (pooled constants live at other addresses)
            return x[:n] == y[:n]
        def same_table(ours, orig, o_fn_lo, o_fn_hi, v_fn_lo, v_fn_hi):
            """a switch jump table or index byte table: equal bytes, or dword entries that point to the same offset inside the function
            (the reference often points a few bytes in front of the table because of the alignment padding: try the shifts 0..3)"""
            x, y = bytes_at(ours, 72), bytes_at(orig, 72)
            if not (o_fn_lo <= ours < o_fn_hi + 256 and v_fn_lo <= orig < v_fn_hi + 256): return False      # tables sit right behind their function; any other equal looking data is no proof
            if x and y and x[:16] == y[:16]: return True
            for sh in range(4):
                ok = 0
                for k in range(sh, 64, 4):
                    if len(x) < k + 4 or len(y) < k + 4: break
                    xe, ye = struct.unpack_from("<I", x, k)[0], struct.unpack_from("<I", y, k)[0]
                    if xe == ye: ok += 1; continue
                    if o_fn_lo <= xe < o_fn_hi and v_fn_lo <= ye < v_fn_hi and xe - o_fn_lo == ye - v_fn_lo: ok += 1; continue
                    break
                if ok >= 2: return True
            return False
        const_at = {}
        for n_, ads in multi.items():
            sz_ = 8 if re.match(r"(k_d_|double_)", n_) else 4 if re.match(r"(k_f_|float_)", n_) else 0
            if sz_:
                for a_ in ads: const_at[a_] = sz_
        def accepted(ours, orig, ctx):
            if ours == orig: return True
            nm_ = new_of_name.get(ours)
            if nm_ is not None and symmap.get(nm_) == orig: return True                 # the replacement we link for that original function / variable
            if nm_ is not None and symmap.get(nm_.lstrip("_")) == orig: return True
            for a, b in ((ours, orig), (orig, ours)):
                t = thunk_target(a)
                if t[0] == b: return True
            ta, tb = thunk_target(ours), thunk_target(orig)
            if ta[1] is not None and ta[1] == tb[1]: return True                         # both are `jmp [same IAT slot]`
            if ta[0] is not None and ta[0] == tb[0]: return True
            if ours in const_at and orig in const_at and bytes_at(ours, const_at[ours]) == bytes_at(orig, const_at[orig]) and const_at[ours] == const_at[orig]: return True      # the same float / double constant stored twice in the original data
            if new_va <= ours < new_va + len(blob) and same_literal(ours, orig, new_of_name.get(ours, "")): return True      # a literal WE placed (string / float pool); two original addresses must be equal
            return same_table(ours, orig, *ctx)
        reorder_ok = set()
        wl = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src", "relcheck_whitelist.txt")      # functions whose fields use the same addresses in another order, verified by hand
        if os.path.exists(wl):
            reorder_ok = {l_.split()[0]: (l_.split()[1] if len(l_.split()) > 1 else None) for l_ in open(wl) if l_.strip() and not l_.startswith("#")}      # name [sha1 prefix of our code bytes]
        sizes = {}
        fsz = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "output", "fnsizes.txt")      # Ghidra function sizes (scripts/FnIndex.java): our code must be as long as the original function
        if os.path.exists(fsz):
            for l_ in open(fsz):
                p_ = l_.split()
                if len(p_) == 3: sizes[int(p_[0], 16)] = int(p_[2])
        starts = sorted(sizes)
        import bisect
        nviol, nchecked, nskip, nreorder, nsize, ncomm = 0, 0, 0, 0, 0, 0
        for nm_ in sorted(patched):
            ob, si, val, base_off = defsec[nm_]
            sec = ob.secs[si]
            ova = symmap[nm_]
            length = sec["size"] - val
            ours = bytes(blob[base_off + val:base_off + val + length])
            ours = ours.rstrip(b"\x90\xcc") or ours
            length = len(ours)
            masked = set()
            for r in range(sec["nrel"]):
                va_, symidx, rtype = struct.unpack_from("<IIH", ob.d, sec["relptr"] + r * 10)
                if val <= va_ < val + length: masked.update(range(va_ - val, va_ - val + 4))
            ob_bytes = orig_exe[va_to_off(ova):va_to_off(ova) + length]
            if any(a != b for i_, (a, b) in enumerate(zip(ours, ob_bytes)) if i_ not in masked):
                nskip += 1; continue                       # not byte identical (NEAR / EFFECTIVE): the offsets do not line up
            gsz = sizes.get(ova)
            nxt = starts[bisect.bisect_right(starts, ova)] if starts and bisect.bisect_right(starts, ova) < len(starts) else None
            if length < 5 and (nxt is None or nxt - ova < 5):
                print("RELCHECK-SIZE %s: only %d bytes, the 5 byte entry jmp overwrites the next function (at %08x)" % (nm_, length, nxt or 0), file=sys.stderr); nsize += 1
            elif gsz is not None and gsz - length > 3 and not os.environ.get("RELCHECK_NOSIZE"):      # shorter than the original function (longer is normal: jump tables / padding after the code)
                print("RELCHECK-SIZE %s: our code %d bytes, Ghidra function %d bytes" % (nm_, length, gsz), file=sys.stderr); nsize += 1
            nchecked += 1
            bad_fields = []
            for r in range(sec["nrel"]):
                va_, symidx, rtype = struct.unpack_from("<IIH", ob.d, sec["relptr"] + r * 10)
                if not (val <= va_ < val + length): continue
                o = va_ - val
                F = struct.unpack_from("<I", ours, o)[0]
                O = struct.unpack_from("<I", ob_bytes, o)[0]
                if rtype == 6:   our_t, orig_t = F, O
                elif rtype == 0x14:
                    our_t = (new_va + base_off + va_ + 4 + struct.unpack_from("<i", ours, o)[0]) & 0xFFFFFFFF
                    orig_t = (ova + o + 4 + struct.unpack_from("<i", ob_bytes, o)[0]) & 0xFFFFFFFF
                else: continue
                if not accepted(our_t, orig_t, (new_va + base_off + val, new_va + base_off + val + length, ova, ova + length)):
                    bad_fields.append((o, our_t, orig_t))
            if bad_fields:
                # `a + b` compiled as `fld [b]; fadd [a]` instead of `fld [a]; fadd [b]` (fld dword = D9 05, fadd dword = D8 05, fmul dword = D8 0D): the two operands of a commutative operation are swapped, the result is bit identical
                remaining = list(bad_fields)
                for i1 in range(len(remaining)):
                    for i2 in range(i1 + 1, len(remaining)):
                        f1, f2 = remaining[i1], remaining[i2]
                        if f1 is None or f2 is None: continue
                        if f1[1] == f2[2] and f2[1] == f1[2]:
                            op1, op2 = bytes(ours[f1[0] - 2:f1[0]]), bytes(ours[f2[0] - 2:f2[0]])
                            lo_, hi_ = (f1, f2) if f1[0] < f2[0] else (f2, f1)
                            if 0 <= hi_[0] - lo_[0] - 6 <= 2 and all(0x50 <= c_ <= 0x5f for c_ in ours[lo_[0] + 4:hi_[0] - 2]) and bytes(ours[lo_[0] - 2:lo_[0]]) == b"\xd9\x05" and bytes(ours[hi_[0] - 2:hi_[0]]) in (b"\xd8\x05", b"\xd8\x0d"):      # `fld [a]` followed by `fadd/fmul [b]` (at most a push/pop between)
                                remaining[i1] = remaining[i2] = None; ncomm += 1
                bad_fields = [f for f in remaining if f is not None]
            if bad_fields:
                if sorted(f[1] for f in bad_fields) == sorted(f[2] for f in bad_fields) and nm_ in reorder_ok and reorder_ok[nm_] == hashlib.sha1(ours).hexdigest()[:12]:      # same set of addresses, other order, and checked by hand (src/relcheck_whitelist.txt)
                    nreorder += 1
                    print("RELCHECK-NOTE %s: %d fields use the same addresses in another order (needs a data flow look: conformance / shadow test)" % (nm_, len(bad_fields)), file=sys.stderr)
                else:
                    if nm_ in reorder_ok: print("RELCHECK-HASH %s: whitelist hash is %s, our code now hashes to %s" % (nm_, reorder_ok[nm_], hashlib.sha1(ours).hexdigest()[:12]), file=sys.stderr)
                    for o, our_t, orig_t in bad_fields:
                        nviol += 1
                        print("RELCHECK %s+0x%x: ours -> %08x (%s), original -> %08x" % (nm_, o, our_t, new_of_name.get(our_t, "?"), orig_t), file=sys.stderr)
        print("relocation check: %d functions checked, %d not byte identical (skipped), %d reordered, %d commutative float operand swaps, %d suspicious fields, %d size problems" % (nchecked, nskip, nreorder, ncomm, nviol, nsize), file=sys.stderr)
        if (nviol or nsize) and relmode == "strict":
            sys.exit("relocation check failed (RELCHECK=warn to continue, RELCHECK=off to skip)")
    # a C file that DEFINES a variable whose name exists in symbols.txt creates a second copy of it (resolve() prefers it over the original address): the code and the original would use different variables
    shadow = []
    for nm_, (ob_, si_, val_, bo_) in defsec.items():
        if not (ob_.secs[si_]["flags"] & 0x20) and nm_ in symmap: shadow.append(nm_)
    for nm_ in commons:
        if nm_.lstrip("_") in symmap: shadow.append(nm_.lstrip("_"))
    if shadow and os.environ.get("RELCHECK", "strict") != "off":
        raise SystemExit("variables defined in C that already exist in the original (a second copy would be used): " + " ".join(sorted(set(shadow))))
    if dup_defs:
        raise SystemExit("symbols defined in more than one object (the overlay would use an arbitrary one): " + " ".join(sorted(set(dup_defs))))
    for h in hooks:      # <call site VA hex>=<function defined by the objects>: retarget a 5 byte E8 call
        va_s, nm = h.split("=")
        cva = int(va_s, 16)
        off = va_to_off(cva)
        if exe[off] != 0xE8:
            raise SystemExit("hook-call %s: no call instruction there (byte %02x)" % (va_s, exe[off]))
        struct.pack_into("<i", exe, off + 1, defined[nm] - (cva + 5))
    for dw in dwords:    # <VA hex>=<symbol defined by the objects>: store that symbol's address in the 4 byte operand at VA
        va_s, nm = dw.split("=")
        struct.pack_into("<I", exe, va_to_off(int(va_s, 16)), defined[nm])
    # append section
    if pad:
        total_vsize = (total_vsize + pad - 1) // pad * pad
    raw = bytes(blob)
    fsz = (len(raw) + falign - 1) // falign * falign
    foff = (len(exe) + falign - 1) // falign * falign
    exe += b"\0" * (foff - len(exe))
    exe += raw + b"\0" * (fsz - len(raw))
    o = sect_tbl + nsec * 40
    exe[o:o + 40] = struct.pack("<8sIIIIIIHHI", b".recomp", total_vsize, new_rva, fsz, foff, 0, 0, 0, 0, 0xE0000060)
    struct.pack_into("<H", exe, e + 6, nsec + 1)
    struct.pack_into("<I", exe, e + 24 + 56, new_rva + (total_vsize + salign - 1) // salign * salign)
    open(out_path, "wb").write(exe)
    with open(out_path + ".map", "w") as mf:
        for nm, v in sorted(defined.items(), key=lambda x: x[1]):
            mf.write("%s %08x\n" % (nm, v))
        for nm, off in commons.items():          # zero initialised globals (common symbols) are placed in the bss part of the section
            mf.write("%s %08x\n" % (nm.lstrip("_"), new_va + off))
        for nm, v in counters:
            mf.write("count:%s %08x\n" % (nm, v))
        if count:
            mf.write("count_table %08x\ncount_bytes %x\n" % (new_va + cnt_off, 4 * CAP))
    print("section .recomp at %08x size %x, patched %d functions: %s" % (new_va, total_vsize, len(patched), " ".join(sorted(patched))))


main()
