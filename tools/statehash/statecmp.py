#!/usr/bin/env python3
"""statecmp.py: N1 per-frame state compare of two statehash.bin files (d3d9 proxy, cfg statehash=1, run.sh STATEHASH=1).
The file holds, per Present, the changed 4 KB pages of the game's writable image (.data + .bss, original VA 0x5fb000-0x949d64, + sh_delta in the
standalone link) and the committed private memory ranges (heap).  Both sides are reconstructed frame by frame and compared word by word.

  statecmp.py A.bin B.bin [--map-b ours.map] [--ptr state_ptrwords.txt] [--mask state_mask.txt] [--frames N] [--show K] [--rwpad state_rwpad.txt]
      A = control.  B = control again (determinism) or ours (--map-b: the link map, pointers in B are translated to original VAs).
      A word counts as equal when the values are equal, or the word is listed in --ptr and both values are heap pointers of their own run
      or B's value translates (ours.map) to A's value, or the word is listed in --mask (reported as masked, never silently).
      Output: first divergent frame + its words (symbol, values, float view, ulp), per-word summary, VERDICT line.
      --rwpad (compare mode "rwpad", off by default; user decision 2026-11-16): rows `<base va> <count> <symbol> | <evidence>` name RwMatrix
      instances (count consecutive 0x40-byte records); only their pad1 / pad2 / pad3 words (+0x1c / +0x2c / +0x3c) are skipped, never flags or
      floats.  Allowed only for instances with a read check (static scan + gdb read watch, agents/rep_w12/notes.md).  Counted as "rwpad words".
      --sound (compare mode "sound", off by default; user decision 2026-11-16): rows `<va> <nwords> <symbol> | <evidence>` (state_sound.txt) name
      sound-timing words (voice queue, voice slots, sound frame records).  Their differences are printed on a SOUND line (shown, never a failure);
      every other word must still be identical.  Rows on car / physics / transform names or the car / vehicle / AI ranges are refused.
  statecmp.py --mkptr A.bin [A2.bin ...] > state_ptrwords.txt     words whose every non-zero value in every frame is a pointer (image or heap)
  statecmp.py --mkstale A.bin A2.bin state_ptrwords.txt <tag>     mask rows for stale menu residue (differs at frame 0, never written in the window)
  statecmp.py --peek A.bin <frame> <va> [n]                         n dwords at an original VA at that frame (e.g. a car's matrix_current)
  statecmp.py --selftest
Mask rows (state_mask.txt): `<va hex> <nwords> <symbol> | <evidence>`; rows on car / physics / world-transform / camera state are refused."""
import bisect, os, re, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
GH = os.environ.get("MFL_GHIDRA") or os.path.abspath(os.path.join(HERE, "..", ".."))   # repo root (ghidra/); needs src/symbols.txt
PAGE = 4096
# no mask may cover these (plan v2 item 5): car array, vehicles, AI bots (race_script.sh dump ranges) and anything named like physics / transforms
DENY_RANGES = [(0x8B06E0, 0x8B06E0 + 0xD040, "cars"), (0x659f00, 0x659f00 + 0xAB0, "vehicles"), (0x8c9ae0, 0x8c9ae0 + 0x1e0, "aibots")]
DENY_NAME = re.compile(r"car|vehicle|phys|rwp|matrix|xform|transform|camera|rwframe|ltm|world|pos|ai_|bot", re.I)


def records(path):
    """yield (hdr dict, regions [(base, end)], {page index: bytes})"""
    with open(path, "rb") as f:
        while True:
            h = f.read(40)
            if len(h) < 40:
                return
            mg, fr, ph, rf, nc, nr, ib, isz, base, delta = struct.unpack("<10I", h)
            if mg != 0x31464853:
                sys.exit("%s: bad record magic at %d" % (path, f.tell() - 40))
            rg = struct.unpack("<%dI" % (2 * nr), f.read(8 * nr))
            regs = sorted((rg[2 * i], rg[2 * i] + rg[2 * i + 1]) for i in range(nr))
            pages = {}
            for _ in range(nc):
                i = struct.unpack("<I", f.read(4))[0]
                pages[i] = f.read(PAGE)
            yield dict(frame=fr, phase=ph, racef=rf, img=(ib, ib + isz), base=base, delta=delta), regs, pages


def in_regs(v, regs):
    i = bisect.bisect_right(regs, (v, 0xFFFFFFFF)) - 1
    return i >= 0 and regs[i][0] <= v < regs[i][1]


def load_syms():
    out = []
    p = os.path.join(GH, "src", "symbols.txt")
    for l in open(p):
        s = l.split()
        if len(s) >= 2 and re.match(r"^[0-9a-fA-F]{8}$", s[1]):
            out.append((int(s[1], 16), s[0]))
    out.sort()
    return out


class Syms:
    def __init__(self):
        self.all = load_syms()
        self.byname = {n: v for v, n in self.all}
        self.data = [x for x in self.all if x[0] >= 0x5fb000]
        self.dva = [x[0] for x in self.data]

    def name(self, va):
        i = bisect.bisect_right(self.dva, va) - 1
        if i < 0:
            return "?"
        v, n = self.data[i]
        return n if v == va else "%s+0x%x" % (n, va - v)


class MapXlat:
    """ours address -> original VA from the link map: anchors = symbols whose original VA is known (name suffix _<va> or src/symbols.txt)"""
    def __init__(self, path, syms):
        an = {}
        for l in open(path, errors="replace"):
            s = l.split()
            if len(s) < 3 or not re.match(r"^[0-9a-f]{4}:[0-9a-f]{8}$", s[0]) or not re.match(r"^[0-9a-f]{8}$", s[2]):
                continue
            nm = re.sub(r"@\d+$", "", s[1].lstrip("_"))
            m = re.search(r"_([0-9a-f]{6,8})$", nm)
            orig = syms.byname.get(nm)
            if orig is None and m and 0x400000 <= int(m.group(1), 16) < 0x9f9000:
                orig = int(m.group(1), 16)
            if orig is not None:
                an.setdefault(int(s[2], 16), set()).add(orig)      # aliases: several original VAs can share one address in the link
        self.k = sorted(an)
        self.v = [an[x] for x in self.k]

    def __call__(self, a):
        i = bisect.bisect_right(self.k, a) - 1
        if i < 0 or (i + 1 < len(self.k) and a >= self.k[i + 1]):
            return None
        return {v + (a - self.k[i]) for v in self.v[i]} if a - self.k[i] < 0x100000 else None


def fl(v):
    return struct.unpack("<f", struct.pack("<I", v))[0]


def ulp(a, b):
    """ulp distance of two floats as bit patterns, None when either does not look like a normal float"""
    def ok(x):
        e = (x >> 23) & 0xFF
        return x == 0 or 0x60 <= e <= 0x9f      # 2^-31 .. 2^32: what game floats look like
    if not (ok(a) and ok(b)):
        return None
    k = lambda x: (x & 0x7FFFFFFF) if not x >> 31 else -(x & 0x7FFFFFFF)
    return abs(k(a) - k(b))


def read_words(path):
    """va set from a word list file (first column hex VA, optional second column count)"""
    out = {}
    if not path:
        return out
    for l in open(path):
        l = l.strip()
        if not l or l.startswith("#"):
            continue
        s = l.split()
        va = int(s[0], 16)
        n = int(s[1]) if len(s) > 1 and s[1].isdigit() else 1
        for i in range(n):
            out[va + 4 * i] = l
    return out


def read_mask(path, syms):
    m = read_words(path)
    bad = []
    for va, row in sorted(m.items()):
        nm = syms.name(va)
        hit = [d for lo, hi, d in DENY_RANGES if lo <= va < hi]
        if hit or DENY_NAME.search(nm) or DENY_NAME.search(row.split("|")[0]):
            bad.append("%08x %s (%s)" % (va, nm, hit[0] if hit else "name"))
        if "|" not in row or not row.split("|", 1)[1].strip():
            bad.append("%08x %s: no evidence column" % (va, nm))
    if bad:
        sys.exit("statecmp: REFUSED mask rows (car / physics / transform state, or no evidence):\n  " + "\n  ".join(bad))
    return m


RWPAD_OFFS = (0x1c, 0x2c, 0x3c)     # RwMatrix pad1 / pad2 / pad3; the only words the rwpad mode may skip


def read_rwpad(path):
    """--rwpad file -> {va: row}: the pad words of every listed RwMatrix instance; rows without evidence or with an unaligned base are refused"""
    out, bad = {}, []
    for l in open(path):
        if not l.strip() or l.startswith("#"):
            continue
        s = l.split()
        if len(s) < 3 or not s[1].isdigit() or "|" not in l or not l.split("|", 1)[1].strip() or int(s[0], 16) & 3:
            bad.append(l.strip())
            continue
        for k in range(int(s[1])):
            for o in RWPAD_OFFS:
                out[int(s[0], 16) + 0x40 * k + o] = l.strip()
    if bad:
        sys.exit("statecmp: REFUSED rwpad rows (need `<va> <count> <symbol> | <evidence>`, 4-aligned va):\n  " + "\n  ".join(bad))
    return out


def read_sound(path, syms):
    """--sound file (rows as --mask: `<va> <nwords> <symbol> | <evidence>`) -> {va: row}: sound-timing words (voice queue, voice slots, sound
    frame records) whose differences go on the SOUND line instead of the VERDICT. Same refusals as masks (car / physics / transform names and
    the car / vehicle / AI dump ranges, rows without evidence): gameplay state can never be listed here."""
    m = read_words(path)
    # `misnamed:` in the evidence skips only the symbols.txt name test (Ghidra names like g_weapon_ai_8be7f0 on voice-queue words); the row's own
    # name column, the deny ranges and the evidence test always apply, and the evidence must then say which code references the words
    bad = ["%08x %s" % (va, syms.name(va)) for va, row in sorted(m.items())
           if any(lo <= va < hi for lo, hi, _ in DENY_RANGES) or DENY_NAME.search(row.split("|")[0])
           or (DENY_NAME.search(syms.name(va)) and "misnamed:" not in row)
           or "|" not in row or not row.split("|", 1)[1].strip()]
    if bad:
        sys.exit("statecmp: REFUSED sound rows (gameplay names / ranges, or no evidence):\n  " + "\n  ".join(bad))
    return m


def dead_layer_fn(names):
    """--dead-layers Lua,qhull,...: data words owned by library layers that the standalone link REPLACES (tools/delink/layers.py, the link's
    LIB_* settings): ours never reads the original copies, so only the control writes them.  Counted and reported, never silently."""
    if not names:
        return None
    sys.path.insert(0, os.path.join(GH, "tools", "delink"))
    import layers
    want, cache = set(names.split(",")), {}
    def f(va):
        if va not in cache:
            it = layers.item_at(va)
            cache[va] = layers.layer(va, it[3], it[4]) if it else layers.other_layer(va, "data")
        return cache[va] in want
    return f


def compare(fa, fb, mapb=None, ptr=None, mask=None, frames=0, show=12, out=sys.stdout, dead=None, rwpad=None, sound=None):
    syms = Syms()
    padw = read_rwpad(rwpad) if rwpad else {}
    tot_pad = 0
    soundw = read_sound(sound, syms) if sound else {}
    snd = {}            # va -> [first frame, frames differing]: sound-timing class, reported on the SOUND line, never a divergence
    deadf = dead_layer_fn(dead)
    tot_dead = 0
    stale0 = set()
    if deadf:       # with --dead-layers also: words the link left zero (dropped data / pointers to dropped code): 0 in B at frame 0, A != 0, written in no frame of either run
        (h0, _, p0a), (_, _, p0b) = next(records(fa)), next(records(fb))
        cand = set()
        for p in p0a:
            if p0a[p] != p0b.get(p):
                for w, (x, y) in enumerate(zip(struct.unpack("<1024I", p0a[p]), struct.unpack("<1024I", p0b[p]))):
                    if y == 0 and x != 0:
                        cand.add(h0["base"] + p * PAGE + 4 * w)
        wa_, wb_ = writes(fa, cand, frames), writes(fb, cand, frames)
        stale0 = {v for v in cand if not wa_.get(v) and not wb_.get(v)}
    xl = MapXlat(mapb, syms) if mapb else None
    ptrw = read_words(ptr)
    maskw = read_mask(mask, syms) if mask else {}
    ga, gb = records(fa), records(fb)
    mem = [None, None]
    cache = {}          # page -> (diff list, masked list, ptr-normalised count)
    first = None
    per_word = {}       # va -> [first frame, frames differing]
    nfr = 0
    tot_mask = tot_ptr = 0
    counts = []
    misalign = 0
    while True:
        try:
            ra, rb = next(ga), next(gb)
        except StopIteration:
            break
        if frames and nfr >= frames:
            break
        (ha, rga, pa), (hb, rgb, pb) = ra, rb
        if mem[0] is None:
            n = len(pa)
            mem = [bytearray(PAGE * n), bytearray(PAGE * n)]
            base = ha["base"]
        for m, pg in ((mem[0], pa), (mem[1], pb)):
            for i, d in pg.items():
                m[i * PAGE:(i + 1) * PAGE] = d
        if (ha["phase"], ha["racef"]) != (hb["phase"], hb["racef"]):
            misalign += 1
        imga, imgb = ha["img"], hb["img"]
        touched = set(pa) | set(pb)
        if nfr == 0:
            touched = set(range(len(mem[0]) // PAGE))
        # heap pointer test depends on the frame's regions: re-check every cached page that has ptr-normalised words (cheap: few)
        for p in touched:
            A = mem[0][p * PAGE:(p + 1) * PAGE]
            B = mem[1][p * PAGE:(p + 1) * PAGE]
            if A == B:
                cache.pop(p, None)
                continue
            wa = struct.unpack("<1024I", A)
            wb = struct.unpack("<1024I", B)
            dl, ml, np_, nd, npad, sl = [], [], 0, 0, 0, []
            for w in range(1024):
                c, o = wa[w], wb[w]
                if c == o:
                    continue
                va = base + p * PAGE + 4 * w
                if va in ptrw:
                    if (in_regs(c, rga) and in_regs(o, rgb)) or (xl and c in (xl(o) or ())) or (not xl and imga[0] <= c < imga[1] and imgb[0] <= o < imgb[1] and c - imga[0] == o - imgb[0]):
                        np_ += 1
                        continue
                if va in maskw:
                    ml.append(va)
                    continue
                if va in padw:
                    npad += 1
                    continue
                if va in soundw:
                    sl.append(va)
                    continue
                if deadf and (deadf(va) or (o == 0 and va in stale0)):
                    nd += 1
                    continue
                dl.append((va, c, o))
            cache[p] = (dl, ml, np_, nd, npad, sl)
        fd = [x for p in sorted(cache) for x in cache[p][0]]
        for va in (x for p in cache for x in cache[p][5]):
            r = snd.setdefault(va, [nfr, 0])
            r[1] += 1
        fm = sum(len(cache[p][1]) for p in cache)
        tot_pad = max(tot_pad, sum(cache[p][4] for p in cache))
        tot_mask = max(tot_mask, fm)
        tot_dead = max(tot_dead, sum(cache[p][3] for p in cache))
        tot_ptr = max(tot_ptr, sum(cache[p][2] for p in cache))
        counts.append(len(fd))
        for va, c, o in fd:
            r = per_word.setdefault(va, [nfr, 0, c, o, 0])
            r[1] += 1
            u = ulp(c, o)
            r[4] = max(r[4], u if u is not None else 1 << 31)
        if fd and first is None:
            first = (nfr, ha, fd)
        nfr += 1
    print("compared %d frames (%s vs %s)%s" % (nfr, fa, fb, ", %d frames with a different phase/racef" % misalign if misalign else ""), file=out)
    if first:
        f, h, fd = first
        print("first divergent frame %d (phase %d racef %d): %d words" % (f, h["phase"], h["racef"], len(fd)), file=out)
        for va, c, o in fd[:show]:
            u = ulp(c, o)
            print("  %08x %-44s A %08x B %08x  f %.7g / %.7g%s" % (va, syms.name(va), c, o, fl(c), fl(o), "  ulp %d" % u if u is not None else ""), file=out)
        div = [(i, n) for i, n in enumerate(counts) if n]
        print("divergent frames %d of %d; words per frame (first 20): %s" % (len(div), nfr, " ".join("%d:%d" % x for x in div[:20])), file=out)
        wr = writes(fa, set(per_word), frames)
        print("words that ever differ: %d (va symbol first-frame frames-differing A B at first, writes = frames in which A's value changed)" % len(per_word), file=out)
        for va, (f0, k, c, o, mu) in sorted(per_word.items(), key=lambda x: (x[1][0], x[0]))[:max(show, 40)]:
            print("  %08x %-44s %4d %4d %08x %08x writes %d maxulp %s" % (va, syms.name(va), f0, k, c, o, wr.get(va, 0), mu if mu < 1 << 31 else "-"), file=out)
    if sound:       # shown, never a failure (user decision 2026-11-16); every other word still decides the VERDICT
        print("SOUND: %d sound-timing words differ%s (report only, never fails)" % (len(snd), ", first frame %d" % min(r[0] for r in snd.values()) if snd else ""), file=out)
        for va, (f0, k) in sorted(snd.items(), key=lambda x: (x[1][0], x[0]))[:show]:
            print("  sound %08x %-44s first %4d frames %4d" % (va, syms.name(va), f0, k), file=out)
    to = alock_timeouts(fa) + alock_timeouts(fb)
    for l in to[:6]:
        print("ALOCK: %s" % l, file=out)
    print("VERDICT: %s over %d frames (first divergent frame %s; masked words %d, pointer-normalised words %d, dead-layer words %d%s%s)" % (
        ("UNJUDGED (%d ALOCK TIMEOUT lines: lockstep lost), compare says %s" % (len(to), "DIVERGENT" if first else "IDENTICAL")) if to else
        "IDENTICAL" if not first else "DIVERGENT", nfr, first[0] if first else "-", tot_mask, tot_ptr, tot_dead, " [%s]" % dead if dead else "",
        "; mode rwpad: %d RwMatrix pad words skipped" % tot_pad if rwpad else ""), file=out)
    return first[0] if first else -1


def alock_timeouts(fpath):
    """W13 hx7 (harness review finding 17): `ALOCK TIMEOUT frame=N thread=i` lines of the run's trace.raw (d3d9.c logs each event): a worker
    missed lockstep in that run, so its compare carries no verdict (UNJUDGED, exit 3)"""
    t = os.path.join(os.path.dirname(os.path.abspath(fpath)), "trace.raw")
    if not os.path.exists(t):
        return []
    return ["%s: %s" % (t, l.strip()) for l in open(t, errors="replace")      # (+ the older proxy's 100-frame count line with a count > 0)
            if l.startswith("ALOCK TIMEOUT") or re.match(r"ALOCK threads=\d+ timeouts=[1-9]", l)]


def writes(fpath, vas, frames=0):
    """va -> number of frames (after frame 0) in which the value changed"""
    last, n = {}, {}
    for h, regs, pages in records(fpath):
        if frames and h["frame"] >= frames:
            break
        for va in vas:
            p = (va - h["base"]) // PAGE
            if p in pages:
                v = struct.unpack_from("<I", pages[p], (va - h["base"]) % PAGE)[0]
                if va in last and last[va] != v:
                    n[va] = n.get(va, 0) + 1
                last[va] = v
    return n


def mkstale(fa, fb, ptr, tag):
    """mask rows for stale residue: words that differ at frame 0 between two control runs and are written in no frame of either run"""
    syms = Syms()
    ptrw = read_words(ptr)
    (ha, _, pa), (hb, _, pb) = next(records(fa)), next(records(fb))
    base = ha["base"]
    d = {}
    for p in pa:
        if pa[p] != pb.get(p):
            wa, wb = struct.unpack("<1024I", pa[p]), struct.unpack("<1024I", pb[p])
            for w in range(1024):
                va = base + p * PAGE + 4 * w
                if wa[w] != wb[w] and va not in ptrw:
                    d[va] = (wa[w], wb[w])
    na, nb = writes(fa, set(d)), writes(fb, set(d))
    for va in sorted(d):
        if not na.get(va) and not nb.get(va):
            print("%08x 1 %s | stale menu residue: %08x vs %08x at frame 0, written in no frame of either control run (%s)" % (va, syms.name(va), d[va][0], d[va][1], tag))


def mkptr(files):
    syms = Syms()
    cand = None
    for fpath in files:
        seen, bad, mem, base = {}, set(), None, 0
        for h, regs, pages in records(fpath):
            base = h["base"]
            img = h["img"]
            for p, d in pages.items():
                for w, v in enumerate(struct.unpack("<1024I", d)):
                    if not v:
                        continue
                    va = base + p * PAGE + 4 * w
                    if va in bad:
                        continue
                    if img[0] <= v < img[1] or in_regs(v, regs):
                        seen[va] = "image" if img[0] <= v < img[1] else "heap"
                    else:
                        bad.add(va)
        ok = {va: k for va, k in seen.items() if va not in bad}
        cand = ok if cand is None else {va: k for va, k in cand.items() if va in ok}
    print("# state_ptrwords.txt (statecmp.py --mkptr %s): words whose every non-zero value in every frame of these control runs is an image or heap pointer;" % " ".join(os.path.basename(x) for x in files))
    print("# statecmp compares them as pointers (both heap / translated through the link map).  va kind symbol")
    for va in sorted(cand):
        print("%08x %s %s" % (va, cand[va], syms.name(va)))


def peek(fpath, frame, va, n):
    mem = None
    for h, regs, pages in records(fpath):
        if mem is None:
            mem = bytearray(PAGE * len(pages))
        for i, d in pages.items():
            mem[i * PAGE:(i + 1) * PAGE] = d
        if h["frame"] == frame:
            o = va - h["base"]
            w = struct.unpack_from("<%dI" % n, mem, o)
            print("frame %d phase %d racef %d %08x: %s" % (frame, h["phase"], h["racef"], va, " ".join("%08x(%.6g)" % (x, fl(x)) for x in w)))
            return


def selftest():
    import tempfile
    d = tempfile.mkdtemp()
    def wr(p, frames):
        with open(p, "wb") as f:
            for fr, pages in enumerate(frames):
                f.write(struct.pack("<10I", 0x31464853, fr, 6, fr, len(pages), 1, 0x400000, 0x600000, 0x5fb000, 0))
                f.write(struct.pack("<2I", 0x1000000, 0x100000))
                for i, b in pages.items():
                    f.write(struct.pack("<I", i) + b)
    z = bytes(PAGE)
    p1 = bytearray(PAGE); struct.pack_into("<I", p1, 8, 0x1000010)
    wr(d + "/a", [{0: z, 1: bytes(p1)}, {}, {}])
    p2 = bytearray(p1); struct.pack_into("<I", p2, 16, 0x3f800000)
    wr(d + "/b", [{0: z, 1: bytes(p1)}, {}, {1: bytes(p2)}])
    p3 = bytearray(p1); struct.pack_into("<I", p3, 8, 0x1000020)
    wr(d + "/c", [{0: z, 1: bytes(p3)}, {}, {}])
    open(d + "/ptr", "w").write("005fc008 heap x\n")
    null = open(os.devnull, "w")
    assert compare(d + "/a", d + "/a", out=null) == -1
    assert compare(d + "/a", d + "/b", out=null) == 2
    assert compare(d + "/a", d + "/c", out=null) == 0
    assert compare(d + "/a", d + "/c", ptr=d + "/ptr", out=null) == -1
    assert ulp(0x3f800000, 0x3f800001) == 1
    # rwpad mode: an RwMatrix at 0x5fc100; pad1/2/3 differences are skipped, a pos float or the flags word still DIFF
    open(d + "/rwpad", "w").write("005fc100 1 test_matrix | selftest\n")
    def one(off, v):
        q = bytearray(PAGE); struct.pack_into("<I", q, 0x100 + off, v)
        return bytes(q)
    wr(d + "/m0", [{0: z, 1: one(0x30, 0x3f800000)}])
    for off, want in ((0x1c, -1), (0x2c, -1), (0x3c, -1), (0x30, 0), (0x0c, 0), (0x40 + 0x1c, 0)):
        wr(d + "/m1", [{0: z, 1: bytes(a ^ b for a, b in zip(one(0x30, 0x3f800000), one(off, 0x614ca614)))}])
        assert compare(d + "/m0", d + "/m1", rwpad=d + "/rwpad", out=null) == want, hex(off)
        assert compare(d + "/m0", d + "/m1", out=null) == 0      # default mode: every difference counts
    # sound mode: a listed sound word differs -> IDENTICAL + SOUND line; an unlisted word next to it -> DIVERGENT; default mode -> DIVERGENT
    import io
    open(d + "/sound", "w").write("005fc200 2 test_voice_queue | selftest\n")
    wr(d + "/s1", [{0: z, 1: bytes(a ^ b for a, b in zip(one(0x30, 0x3f800000), one(0x104, 1)))}])
    buf = io.StringIO()
    assert compare(d + "/m0", d + "/s1", sound=d + "/sound", out=buf) == -1 and "SOUND: 1 sound-timing words differ" in buf.getvalue()
    assert compare(d + "/m0", d + "/s1", out=null) == 0
    wr(d + "/s2", [{0: z, 1: bytes(a ^ b for a, b in zip(one(0x30, 0x3f800000), one(0x108, 1)))}])
    assert compare(d + "/m0", d + "/s2", sound=d + "/sound", out=null) == 0
    # W13 hx7: an ALOCK TIMEOUT line in a run's trace.raw -> UNJUDGED (identical pages or not)
    os.makedirs(d + "/to"); wr(d + "/to/statehash.bin", [{0: z, 1: one(0x30, 0x3f800000)}])
    open(d + "/to/trace.raw", "w").write("TRACE d3dtrace 1\nALOCK TIMEOUT frame=17 thread=2\nPRESENT 0\n")
    buf = io.StringIO(); compare(d + "/m0", d + "/to/statehash.bin", out=buf)
    assert "VERDICT: UNJUDGED (1 ALOCK TIMEOUT" in buf.getvalue() and alock_timeouts(d + "/m0") == [], buf.getvalue()
    open(d + "/sound_bad", "w").write("008b1038 1 player_car_0__matrices_12_ | must be refused\n")
    try:
        read_sound(d + "/sound_bad", Syms())
        assert False, "car row accepted"
    except SystemExit:
        pass
    print("selftest ok")


if __name__ == "__main__":
    a = sys.argv[1:]
    if a[:1] == ["--selftest"]:
        selftest()
    elif a[:1] == ["--mkptr"]:
        mkptr(a[1:])
    elif a[:1] == ["--mkstale"]:
        mkstale(a[1], a[2], a[3], a[4])
    elif a[:1] == ["--peek"]:
        peek(a[1], int(a[2]), int(a[3], 16), int(a[4]) if len(a) > 4 else 1)
    else:
        opt = {}
        pos = []
        i = 0
        while i < len(a):
            if a[i].startswith("--"):
                opt[a[i][2:]] = a[i + 1]
                i += 2
            else:
                pos.append(a[i])
                i += 1
        if len(pos) != 2:
            sys.exit(__doc__)
        r = compare(pos[0], pos[1], mapb=opt.get("map-b"), ptr=opt.get("ptr"), mask=opt.get("mask"), frames=int(opt.get("frames", 0)), show=int(opt.get("show", 12)), dead=opt.get("dead-layers"), rwpad=opt.get("rwpad"), sound=opt.get("sound"))
        sys.exit(3 if alock_timeouts(pos[0]) + alock_timeouts(pos[1]) else 0 if r < 0 else 1)
