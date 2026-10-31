"""Code layer of an address (shared by origbytes.py and mkrange.py): ownership_items.csv owner/member -> layer name."""
import bisect, csv, os
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
H = lambda s: int(s, 16)
LAYERS = ["app", "CRT", "RenderWare", "rwp", "Lua", "qhull", "zlib/PNG/TIFF", "DirectShow", "D3DX", "other"]
ZLIB = ("deflate", "inflate", "inftrees", "infblock", "infcodes", "inffast", "infutil", "adler32", "crc32", "trees", "zutil", "compress", "uncompr", "gzio", "tif", "fax3", "lzw", "jpeg")
H = lambda s: int(s, 16)

# A8: layer "other" is classified by output/other_layer_2026-10-31.csv (tools/delink/other_layer_classify.py): code regions (e.g. 0x4ab230-0x4ac4f0 DirectInput DIOBJECTDATAFORMAT tables in .text)
# and library data runs without ownership item, each with a layer and evidence.  other_layer(va) = that layer, "other" if not covered.
OTHER = sorted((H(r["lo"]), H(r["hi"]), r["layer"], r["kind"]) for r in csv.DictReader(open(os.path.join(G, "output", "other_layer_2026-10-31.csv"))))
_OL = {k: [x for x in OTHER if x[3] == k] for k in ("code", "data")}
def other_layer(va, kind="code"):
    for lo, hi, L, _ in _OL[kind]:
        if lo <= va < hi: return L
    return "other"
_DS = sorted((int(l.split()[0], 16), int(l.split()[0], 16) + int(l.split()[1])) for l in open(os.path.join(G, "tools", "delink", "dshow_dead.txt")) if l.strip() and not l.startswith("#"))
_DSL = [x[0] for x in _DS]
def _ds(va):
    i = bisect.bisect_right(_DSL, va) - 1
    return i >= 0 and va < _DS[i][1]
def layer(va, owner, member):
    if va == 0x4c9c30: return "Lua"            # A5: the map's "gzputs" is a Lua function (strlen + lua call, called from the Lua range); ownership says rtpng gzio
    if va == 0x5021c0: return "RenderWare"     # A5: the map's "gztell" is RpBuildMeshGenerateDefaultTriStrip
    if va < 0x5dc451 and other_layer(va) != "other" and (owner == "game" and va >= 0x4ac600 or not owner): return other_layer(va)
    if 0x569b30 <= va < 0x58ede0: return "rwp"
    if 0x5b5eb0 <= va < 0x5d09d0: return "D3DX"
    if _ds(va): return "DirectShow"      # the dead movie code (strmbase + RwaDS + the renderer class, tools/delink/dshow_dead.txt = dshow_drop.py closure); the rest of the old range 0x5d2b40-0x5dc451 is not DirectShow (called by RenderWare / D3DX / CRT)
    o = owner.rstrip("?~")
    if o == "game": return "app" if va < 0x4ac600 else other_layer(va)
    if o == "crt": return "CRT"
    if va in (0x52f2b0, 0x52f790): return "zlib/PNG/TIFF"      # A5: png_write_oFFs / png_write_pHYs (pngwutil), no member in the ownership
    if o in ("lua", "qhull"): return "Lua" if o == "lua" else "qhull"
    if o in ("rw", "shared"):
        lib = member.split(":")[0].lower(); base = member.replace("\\", "/").split("/")[-1].lower().replace(".obj", "")
        if base == "rtpng": return "RenderWare"      # A8: rtpng.obj = RW wrapper (RtPNGImageRead/Write, PNGMalloc/Free), zlib_png_map.csv class "RW wrapper"
        if lib in ("rtpng.lib", "rttiff.lib") or base in ZLIB: return "zlib/PNG/TIFF"
        return "RenderWare" if o == "rw" else other_layer(va, "data" if va >= 0x5dc000 else "code")
    return other_layer(va, "data" if va >= 0x5dc000 else "code")

items = [(H(r["lo"]), H(r["hi"]), r["kind"], r["owner"], r["member"]) for r in csv.DictReader(open(os.path.join(G, "output", "delink_sdk", "ownership_items.csv")))]
items.sort()
TEXT, TSZ = 0x401000, 0x1dab20; RWC, RSZ = 0x5dc000, 0x451
ITL = [x[0] for x in items]
def item_at(va):
    i = bisect.bisect_right(ITL, va) - 1
    return items[i] if i >= 0 and va < items[i][1] else None

