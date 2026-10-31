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
Code layers: app (VA < 0x4ac600; 0x4ac600-0x4b0b70 is DirectShow), CRT, RenderWare, rwp, Lua, qhull, zlib/PNG/TIFF, DirectShow, D3DX, other (game owned islands >= 0x4b0000)."""
import collections, csv, json, os, sys, datetime
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
W = os.path.abspath(sys.argv[1]); a = sys.argv[2:]
DATE = a[a.index("--date") + 1] if "--date" in a else datetime.date.today().isoformat()
OUT = a[a.index("--out") + 1] if "--out" in a else os.path.join(G, "output")
from layers import *

# ---- code ----
drop = list(csv.DictReader(open(os.path.join(W, "mfl.obj.dropapp.csv"))))
status = bytearray(TSZ + RSZ)          # 0 original, 1 filled (int3), 2 entry stub
def sidx(va): return va - TEXT if va < RWC else TSZ + va - RWC
for r in drop:
    va, sz = H(r["va"]), int(r["size"])
    for k in range(5): status[sidx(va) + k] = 2
    if r["status"] in ("dropped", "partial"):          # partial: the body in front of a kept tail is filled, sz = bytes up to the tail
        for k in range(5, sz): status[sidx(va) + k] = 1
import glob      # A4 LIB_LUA=src / LIB_QHULL=src (link_ours.sh): the code items of the source-built libs (member "<lib>:src") are filled, the objects supply the code
for _f in glob.glob(os.path.join(W, "lib", "*", "items.csv")):
    for r in csv.DictReader(open(_f)):
        if r["member"].endswith(":src") and r["kind"] == "code":
            for v in range(H(r["lo"]), H(r["hi"])):
                if status[sidx(v)] == 0: status[sidx(v)] = 1
_dp_ = os.path.join(W, "mfl.obj.dead.csv")
for r in (csv.DictReader(open(_dp_)) if os.path.exists(_dp_) else []):      # dead jump tables (mkobj dead_tables.txt): filled
    for k in range(int(r["size"])): status[sidx(H(r["va"]) + k)] = 1
# T-N18: 5 byte `E9 rel32` thunks that stayed in the app block (the linker relocates the jmp, no game content) and the 0x90 alignment fill behind them / between functions
# (app layer only): the thunk counts as stub, the fill as filled.  A thunk is a 16-aligned E9 followed by 0x90 up to the next 16-aligned boundary.
_img = open(os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe"), "rb").read()
_b = lambda v: _img[v - 0x400000]
_app = lambda v: (lambda it: layer(v, it[3], it[4]) if it else other_layer(v))(item_at(v)) == "app"
for va in range(TEXT, TEXT + TSZ):
    if status[sidx(va)] != 0 or _b(va) not in (0xE9, 0xFF, 0x90) or not _app(va): continue
    tl = 5 if _b(va) == 0xE9 else 6 if _b(va) == 0xFF and _b(va + 1) == 0x25 else 0      # jmp rel32 / jmp [IAT slot]
    if va % 16 == 0 and tl and all(_b(va + k) in (0x90, 0xCC) for k in range(tl, 16)) and all(status[sidx(va + k)] == 0 for k in range(tl, 16)):
        for k in range(tl): status[sidx(va + k)] = 2
        for k in range(tl, 16): status[sidx(va + k)] = 1
    elif _b(va) == 0x90 and va % 16 and all(status[sidx(va + k)] == 0 and _b(va + k) == 0x90 for k in range(0, 16 - va % 16)) and _b(va - 1) in (0x90, 0xC3, 0xCC):
        for k in range(0, 16 - va % 16): status[sidx(va + k)] = 1
code = collections.defaultdict(collections.Counter)         # layer -> {original, filled, stub}
total_code = collections.Counter()
for base, n in ((TEXT, TSZ), (RWC, RSZ)):
    va = base
    while va < base + n:
        it = item_at(va); end = min(it[1] if it else va + 1, base + n)
        L = layer(va, it[3], it[4]) if it else other_layer(va)
        for v in range(va, end):
            code[L][("original", "filled", "stub")[status[sidx(v)]]] += 1
        va = end
fns = [(H(l.split()[0]), l.split()[1], int(l.split()[2])) for l in open(os.path.join(G, "output", "fnsizes.txt"))]
fsz = {va: sz for va, _, sz in fns}
done = {H(r["va"]) for r in drop}
legacy = [r for r in drop if r["status"] in ("legacy", "partial")]
import re
for r in legacy:      # bytes still original: the whole extent (legacy) or the tail (partial)
    m = re.search(r"\((\d+) bytes\)", r["reason"]); r["kept"] = int(m.group(1)) if r["status"] == "partial" else int(r["size"])
app_unreplaced = [(va, n, sz) for va, n, sz in fns if va < 0x4ac600 and va not in done]
ds_unreplaced = [(va, n, sz) for va, n, sz in fns if 0x4ac600 <= va < 0x4b0b70 and va not in done]
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
            L = "qhull" if 0x63a610 <= v < 0x649138 else (layer(v, it[3], it[4]) if it and it[3] in ("crt", "rw", "crt?", "rw?") else other_layer(v, "data"))
            dlib[L][sec] += e - v; v = e
RSRC = 0xad7a8
res = {"rsrc_original_bytes": RSRC}
tot = lambda c: sum(c.values())
js = {"date": DATE, "code": {L: dict(code[L]) for L in LAYERS}, "data_by_section": {s: dict(c) for s, c in data.items()},
      "data_lib_by_layer": {L: dict(dlib[L]) for L in LAYERS if L in dlib}, "legacy_functions": len(legacy), "app_unreplaced_functions": len(app_unreplaced), "directshow_unreplaced_functions": len(ds_unreplaced),
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
      "| ...original **app** code bytes (VA < 0x4ac600; 0x4ac600-0x4b0b70 is DirectShow) | %s |" % format(code["app"]["original"], ","),
      "| ...filled (replaced function bodies) | %s |" % format(cfill, ","),
      "| ...entry stubs to our C | %s |" % format(cstub, ","),
      "| data (.rdata/.data/.bss) total | %s |" % format(sum(sum(c.values()) for c in data.values()), ","),
      "| ...library data rows, bytes from the original image (layers below) | %s |" % format(dorig, ","),
      "| ...app data generated as C (`gen`: app + gap rows) | %s |" % format(dapp, ","),
      "| ...linker generated (IAT, imports) | %s |" % format(dpe, ","),
      "| resources (.rsrc linked from output/res/mfl.res = rc.exe output; entries byte-identical to the original, they are game data) | %s |" % format(RSRC, ","), "",
      "## Code per layer", "", "| Layer | Original | Filled | Stub | Total |", "|---|---:|---:|---:|---:|"]
for L in LAYERS:
    c = code[L]; md.append("| %s | %s | %s | %s | %s |" % (L, format(c["original"], ","), format(c["filled"], ","), format(c["stub"], ","), format(tot(c), ",")))
md += ["", "## Data per section", "", "| Section | app | gap | lib | pe |", "|---|---:|---:|---:|---:|"]
for s in ("rdata", "data", "bss"):
    c = data[s]; md.append("| .%s | %s | %s | %s | %s |" % (s, format(c["app"], ","), format(c["gap"], ","), format(c["lib"], ","), format(c["pe"], ",")))
md += ["", "## Library data (original bytes) per layer", "", "| Layer | .rdata | .data | .bss |", "|---|---:|---:|---:|"]
for L in LAYERS:
    if L in dlib: md.append("| %s | %s | %s | %s |" % (L, *(format(dlib[L].get(s, 0), ",") for s in ("rdata", "data", "bss"))))
lkept = lambda L: sum(r["kept"] for r in legacy if layer(H(r["va"]), *(lambda it: (it[3], it[4]) if it else ("", ""))(item_at(H(r["va"])))) == L)
md += ["", "## Application code that is still original", "",
       "- Replaced by C, but a tail kept (live reference into the interior: a function Ghidra did not split off, a callback label): %d functions; app only: %d functions, %s bytes kept (the bodies in front of the tails are filled)." % (len(legacy), sum(1 for r in legacy if layer(H(r["va"]), "game", "") == "app"), format(lkept("app"), ",")),
       "- Never replaced (no C): %d functions below 0x4ac600 (app), %s bytes; %d functions in the DirectShow range 0x4ac600-0x4b0b70, %s bytes (list: output/zero_original_unreplaced.csv)." % (len(app_unreplaced), format(sum(s for _, _, s in app_unreplaced), ","), len(ds_unreplaced), format(sum(s for _, _, s in ds_unreplaced), ",")), "",
       "- Rest of the original app code bytes (%s): padding, jump tables and unlisted code behind sized bodies that a raw pointer or a symbol still reaches (the extension of a dropped body stops there)." % format(code["app"]["original"] - lkept("app") - sum(s_ for _, _, s_ in app_unreplaced), ","),
       "- Honesty notes: data rows generated as C are not counted as original, but their initial values were extracted from the image by tools/globals; the library rows (kind lib) are the original bytes re-emitted as C; the resources are game data compiled by rc.exe from the extracted tree; the 5 byte stubs are generated but each replaces original code bytes.", "",
       "### Kept tails and bodies (replaced by C; all layers)", "", "| VA | Kept bytes | Function | Reason |", "|---|---:|---|---|"]
for r in sorted(legacy, key=lambda r: H(r["va"])):
    it = item_at(H(r["va"])); md.append("| %s | %d | %s | %s (%s) |" % (r["va"], r["kept"], r["name"], r["reason"], layer(H(r["va"]), it[3], it[4]) if it else "other"))
# the no-C list: name, size, why
def why(va, n, sz):
    b = n.lower()
    if sz <= 6: return "5 byte thunk or stub (jmp rel32 / ret): bytes relocated by the linker, no C yet"
    if 0x4ac600 <= va < 0x4b0b70: return "DirectShow / movie filter code (strmbase, Criterion filters): library, not app"
    if n.startswith("gap_"): return "unnamed gap code (no Ghidra function), not decompiled"
    return "app function blocked in an earlier round (see ROADMAP): " + n
with open(os.path.join(OUT, "zero_original_unreplaced.csv"), "w") as f:
    f.write("va,size,name,region,why\n")
    for va, n, sz in app_unreplaced + ds_unreplaced:
        f.write("%08x,%d,%s,%s,%s\n" % (va, sz, n, "app" if va < 0x4ac600 else "DirectShow", why(va, n, sz)))
# per layer address ranges of the code that is still original (Brief G: one range list per layer, the input of a per-layer object)
with open(os.path.join(OUT, "zero_original_ranges.csv"), "w") as f:
    f.write("layer,lo,hi,bytes\n")
    for base, n in ((TEXT, TSZ), (RWC, RSZ)):
        va = base; cur = None
        while va < base + n:
            it = item_at(va); end = min(it[1] if it else va + 1, base + n)
            L = layer(va, it[3], it[4]) if it else other_layer(va)
            for v in range(va, end):
                if status[sidx(v)] != 0: continue
                if cur and cur[0] == L and cur[2] == v: cur[2] = v + 1
                else:
                    if cur: f.write("%s,%08x,%08x,%d\n" % (cur[0], cur[1], cur[2], cur[2] - cur[1]))
                    cur = [L, v, v + 1]
            va = end
        if cur: f.write("%s,%08x,%08x,%d\n" % (cur[0], cur[1], cur[2], cur[2] - cur[1]))
md += [""]
open(os.path.join(OUT, "zero_original_bytes_%s.md" % DATE), "w").write("\n".join(md))
print("\n".join(md[:30]))
