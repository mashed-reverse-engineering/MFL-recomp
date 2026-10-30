#!/usr/bin/env python3
"""S6 metric: how many bytes of the linked exe still come from the original image, per layer.
Usage: origbytes.py <work dir of link_ours.sh GEN_DATA=1 IMPLIBS=1> [--date YYYY-MM-DD] [--out output]
Reads <work>/mfl.obj.dropapp.csv (mkobj --drop-app: replaced functions, dropped or kept), <work>/gen/globals_layout.csv (globgen: every .rdata/.data/.bss row,
kind app|gap|lib|pe), output/delink_sdk/ownership_items.csv (code / data owners), output/fnsizes.txt.  Writes <out>/zero_original_bytes_<date>.md and
<out>/zero_original_bytes.json (read by tools/progress.py).  Categories per section:
  original   bytes of the original image in the exe (code: not filled; data: library rows, which the generator copies as bytes + pointers)
  stub       5 byte entry jmp to our C (generated, no original content)
  C-gen      app data rows generated as C (initial values were extracted from the image, no original bytes are linked)
  linker     linker generated (IAT, import tables)
Code layers: app (VA < 0x4b0000), CRT, RenderWare, rwp, Lua, qhull, zlib/PNG/TIFF, DirectShow, D3DX, other (game owned islands >= 0x4b0000)."""
import collections, csv, json, os, sys, datetime
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
W = os.path.abspath(sys.argv[1]); a = sys.argv[2:]
DATE = a[a.index("--date") + 1] if "--date" in a else datetime.date.today().isoformat()
OUT = a[a.index("--out") + 1] if "--out" in a else os.path.join(G, "output")
LAYERS = ["app", "CRT", "RenderWare", "rwp", "Lua", "qhull", "zlib/PNG/TIFF", "DirectShow", "D3DX", "other"]
ZLIB = ("deflate", "inflate", "inftrees", "infblock", "infcodes", "inffast", "infutil", "adler32", "crc32", "trees", "zutil", "compress", "uncompr", "gzio", "tif", "fax3", "lzw", "jpeg")
H = lambda s: int(s, 16)

def layer(va, owner, member):
    if 0x569b30 <= va < 0x58ede0: return "rwp"
    if 0x5b5eb0 <= va < 0x5d09d0: return "D3DX"
    if 0x5d2b40 <= va < 0x5dc451 and owner == "game": return "DirectShow"
    o = owner.rstrip("?~")
    if o == "game": return "app" if va < 0x4b0000 else "other"
    if o == "crt": return "CRT"
    if o in ("lua", "qhull"): return "Lua" if o == "lua" else "qhull"
    if o in ("rw", "shared"):
        lib = member.split(":")[0].lower(); base = member.replace("\\", "/").split("/")[-1].lower().replace(".obj", "")
        if lib in ("rtpng.lib", "rttiff.lib") or base in ZLIB: return "zlib/PNG/TIFF"
        return "RenderWare" if o == "rw" else "other"
    return "other"

items = [(H(r["lo"]), H(r["hi"]), r["kind"], r["owner"], r["member"]) for r in csv.DictReader(open(os.path.join(G, "output", "delink_sdk", "ownership_items.csv")))]
items.sort()
TEXT, TSZ = 0x401000, 0x1dab20; RWC, RSZ = 0x5dc000, 0x451
ITL = [x[0] for x in items]
import bisect
def item_at(va):
    i = bisect.bisect_right(ITL, va) - 1
    return items[i] if i >= 0 and va < items[i][1] else None

# ---- code ----
drop = list(csv.DictReader(open(os.path.join(W, "mfl.obj.dropapp.csv"))))
status = bytearray(TSZ + RSZ)          # 0 original, 1 filled (int3), 2 entry stub
def sidx(va): return va - TEXT if va < RWC else TSZ + va - RWC
for r in drop:
    va, sz = H(r["va"]), int(r["size"])
    for k in range(5): status[sidx(va) + k] = 2
    if r["status"] == "dropped":
        for k in range(5, sz): status[sidx(va) + k] = 1
code = collections.defaultdict(collections.Counter)         # layer -> {original, filled, stub}
total_code = collections.Counter()
for base, n in ((TEXT, TSZ), (RWC, RSZ)):
    va = base
    while va < base + n:
        it = item_at(va); end = min(it[1] if it else va + 1, base + n)
        L = layer(va, it[3], it[4]) if it else "other"
        for v in range(va, end):
            code[L][("original", "filled", "stub")[status[sidx(v)]]] += 1
        va = end
fns = [(H(l.split()[0]), l.split()[1], int(l.split()[2])) for l in open(os.path.join(G, "output", "fnsizes.txt"))]
fsz = {va: sz for va, _, sz in fns}
done = {H(r["va"]) for r in drop}
legacy = [r for r in drop if r["status"] == "legacy"]
app_unreplaced = [(va, n, sz) for va, n, sz in fns if va < 0x4b0000 and va not in done]
# ---- data ----
lay = list(csv.DictReader(open(os.path.join(W, "gen", "globals_layout.csv"))))
data = collections.defaultdict(collections.Counter)         # (section) -> {kind: bytes}
dlib = collections.defaultdict(collections.Counter)         # layer -> section -> bytes of lib rows
for r in lay:
    va, sz, sec, kind = H(r["va"]), int(r["size"]), r["section"], r["kind"]
    data[sec][kind] += sz
    if kind == "lib":
        # a lib row can straddle owners: split by ownership items, qhull block by address
        v = va
        while v < va + sz:
            it = item_at(v); e = min(it[1], va + sz) if it else va + sz
            L = "qhull" if 0x63a526 <= v < 0x64c190 else (layer(v, it[3], it[4]) if it and it[3] in ("crt", "rw", "crt?", "rw?") else "other")
            dlib[L][sec] += e - v; v = e
RSRC = 0xad7a8
res = {"rsrc_original_bytes": RSRC}
tot = lambda c: sum(c.values())
js = {"date": DATE, "code": {L: dict(code[L]) for L in LAYERS}, "data_by_section": {s: dict(c) for s, c in data.items()},
      "data_lib_by_layer": {L: dict(dlib[L]) for L in LAYERS if L in dlib}, "legacy_functions": len(legacy), "app_unreplaced_functions": len(app_unreplaced),
      "resources": res}
corig = sum(c["original"] for c in code.values()); cfill = sum(c["filled"] for c in code.values()); cstub = sum(c["stub"] for c in code.values())
dorig = sum(sum(c.values()) for c in dlib.values())
dapp = sum(c["app"] + c["gap"] for c in data.values()); dpe = sum(c["pe"] for c in data.values())
js["totals"] = {"code_original": corig, "code_filled": cfill, "code_stub": cstub, "data_original_lib": dorig, "data_generated_c": dapp, "data_linker": dpe,
                "app_code_original": code["app"]["original"], "all_original": corig + dorig + RSRC}
os.makedirs(OUT, exist_ok=True)
json.dump(js, open(os.path.join(OUT, "zero_original_bytes.json"), "w"), indent=1, sort_keys=True)
md = ["# Original bytes still in the linked exe (%s)" % DATE, "",
      "Generated by `tools/delink/origbytes.py` from a `link_ours.sh GEN_DATA=1 IMPLIBS=1` build: the image object keeps code that is not replaced, our objects and the generated data (`tools/globals`) supply the rest.",
      "Original = bytes of the original MFL.exe that are linked unchanged. Stub = 5 byte entry `jmp` into our C (generated). Filled = int3 over a replaced body. C-gen = data rows generated as C source (initial values were extracted from the image; no image bytes are linked).", "",
      "## Headline", "",
      "| | Bytes |", "|---|---:|",
      "| .text + _rwcseg total | %s |" % format(TSZ + RSZ, ","),
      "| ...original code bytes still linked | %s (%.1f %%) |" % (format(corig, ","), 100.0 * corig / (TSZ + RSZ)),
      "| ...original **app** code bytes (VA < 0x4b0000) | %s |" % format(code["app"]["original"], ","),
      "| ...filled (replaced function bodies) | %s |" % format(cfill, ","),
      "| ...entry stubs to our C | %s |" % format(cstub, ","),
      "| data (.rdata/.data/.bss) total | %s |" % format(sum(sum(c.values()) for c in data.values()), ","),
      "| ...library data rows, bytes from the original image (layers below) | %s |" % format(dorig, ","),
      "| ...app data generated as C (`gen`: app + gap rows) | %s |" % format(dapp, ","),
      "| ...linker generated (IAT, imports) | %s |" % format(dpe, ","),
      "| resources (.rsrc appended from the original by pefix, S3 not done) | %s |" % format(RSRC, ","), "",
      "## Code per layer", "", "| Layer | Original | Filled | Stub | Total |", "|---|---:|---:|---:|---:|"]
for L in LAYERS:
    c = code[L]; md.append("| %s | %s | %s | %s | %s |" % (L, format(c["original"], ","), format(c["filled"], ","), format(c["stub"], ","), format(tot(c), ",")))
md += ["", "## Data per section", "", "| Section | app | gap | lib | pe |", "|---|---:|---:|---:|---:|"]
for s in ("rdata", "data", "bss"):
    c = data[s]; md.append("| .%s | %s | %s | %s | %s |" % (s, format(c["app"], ","), format(c["gap"], ","), format(c["lib"], ","), format(c["pe"], ",")))
md += ["", "## Library data (original bytes) per layer", "", "| Layer | .rdata | .data | .bss |", "|---|---:|---:|---:|"]
for L in LAYERS:
    if L in dlib: md.append("| %s | %s | %s | %s |" % (L, *(format(dlib[L].get(s, 0), ",") for s in ("rdata", "data", "bss"))))
md += ["", "## Application code that is still original", "",
       "- Replaced by C, but body kept (live reference into the interior), all layers: %d functions (bodies counted under their layer above); app only: %d functions, %s bytes." % (len(legacy), sum(1 for r in legacy if H(r["va"]) < 0x4b0000), format(sum(fsz[H(r["va"])] for r in legacy if H(r["va"]) < 0x4b0000), ",")),
       "- Never replaced (no C): %d functions below 0x4b0000, %s bytes." % (len(app_unreplaced), format(sum(s for _, _, s in app_unreplaced), ",")), "",
       "- Rest of the original app code bytes (%s): padding, jump tables and unlisted code behind sized bodies that a raw pointer or a symbol still reaches (the extension of a dropped body stops there)." % format(code["app"]["original"] - sum(fsz[H(r["va"])] for r in legacy if H(r["va"]) < 0x4b0000) - sum(s_ for _, _, s_ in app_unreplaced), ","),
       "- Honesty notes: data rows generated as C are not counted as original, but their initial values were extracted from the image by tools/globals; the library rows (kind lib) are the original bytes re-emitted as C; the resources are the original .rsrc (S3 not done); the 5 byte stubs are generated but each replaces original code bytes.", "",
       "### Legacy list (replaced by C, body kept; all layers)", "", "| VA | Size | Function | Reason |", "|---|---:|---|---|"]
for r in sorted(legacy, key=lambda r: H(r["va"])):
    it = item_at(H(r["va"])); md.append("| %s | %s | %s | %s (%s) |" % (r["va"], r["size"], r["name"], r["reason"], layer(H(r["va"]), it[3], it[4]) if it else "other"))
md += [""]
open(os.path.join(OUT, "zero_original_bytes_%s.md" % DATE), "w").write("\n".join(md))
print("\n".join(md[:30]))
