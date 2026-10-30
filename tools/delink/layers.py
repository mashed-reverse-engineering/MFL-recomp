"""Code layer of an address (shared by origbytes.py and mkrange.py): ownership_items.csv owner/member -> layer name."""
import bisect, csv, os
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
H = lambda s: int(s, 16)
LAYERS = ["app", "CRT", "RenderWare", "rwp", "Lua", "qhull", "zlib/PNG/TIFF", "DirectShow", "D3DX", "other"]
ZLIB = ("deflate", "inflate", "inftrees", "infblock", "infcodes", "inffast", "infutil", "adler32", "crc32", "trees", "zutil", "compress", "uncompr", "gzio", "tif", "fax3", "lzw", "jpeg")
H = lambda s: int(s, 16)

# 0x4ab230-0x4ac4f0 (4,800 bytes, ownership item gap_4ab230 "code"): no code, DIOBJECTDATAFORMAT tables of the DirectInput library (c_rgodfDIJoy / DIKeyboard / DIMouse style entries {guid ptr, ofs, type, flags}
# = 300 x 16 bytes) that the linker of the game put in .text; library data, not application code
LIBDATA_IN_TEXT = [(0x4ab230, 0x4ac4f0, "other")]
def layer(va, owner, member):
    for lo, hi, L in LIBDATA_IN_TEXT:
        if lo <= va < hi: return L
    if 0x569b30 <= va < 0x58ede0: return "rwp"
    if 0x5b5eb0 <= va < 0x5d09d0: return "D3DX"
    if (0x5d2b40 <= va < 0x5dc451 or 0x4ac600 <= va < 0x4b0b70) and owner == "game": return "DirectShow"      # strmbase + the movie filters (ROADMAP l.820)
    o = owner.rstrip("?~")
    if o == "game": return "app" if va < 0x4ac600 else "other"
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
def item_at(va):
    i = bisect.bisect_right(ITL, va) - 1
    return items[i] if i >= 0 and va < items[i][1] else None

