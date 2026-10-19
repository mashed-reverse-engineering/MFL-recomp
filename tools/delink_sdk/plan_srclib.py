#!/usr/bin/env python3
"""plan_srclib.py <lib> <objdir with the lib's _o/*.obj> <out dir> [items csv to extend]: replace a whole open source library of the exe (libs.cfg `variant = source`, keys
region = lo-hi (exe code range of the lib), ns = libmatch2 csv with the byte identical functions "U,name,file,size,va,") by objects built from upstream
source (output/libmatch2/build_libs.sh).  Every code item of ownership_items.csv inside the region becomes a drop item of the pseudo member
"<lib>:src" (named by the ns csv / names csv where known, so live references to it become symbol references); the data stays in the image (dead).
Writes <out>/items.csv (ownership_items.csv + these rows, for mkobj --drop-items), members.txt, extra_objs.txt (lines "lib:<lib>:src <obj>")."""
import configparser, csv, glob, os, sys
H = os.path.dirname(os.path.abspath(__file__)); G = os.path.normpath(os.path.join(H, "..", "..")); O = os.path.join(G, "output")
LIB, OBJD, OUT = sys.argv[1:4]; IN = sys.argv[4] if len(sys.argv) > 4 else os.path.join(O, "delink_sdk", "ownership_items.csv")      # IN: items.csv of a previous lib (chain)
cfg = configparser.ConfigParser(); cfg.read(os.path.join(H, "libs.cfg")); c = cfg[LIB]
lo, hi = (int(x, 16) for x in c["region"].split("-")); KEY = LIB + ":src"
name = {}
for f in c.get("ns", "").split():
    for r in csv.reader(open(os.path.join(G, f))):
        if r[0] == "U": name[int(r[4], 16)] = r[1]
for f in c.get("idnames", "").split():          # hand / callee-set identified (not byte identical) functions: addr,name,why
    for r in csv.reader(open(os.path.join(G, f))):
        if r and not r[0].startswith("#"): name[int(r[0], 16)] = r[1]
for pat in c.get("names", "").split():          # names csv "addr,name,...": only where the object defines it (checked by mkobj through the member's symbols)
    for f in glob.glob(os.path.join(G, pat)):
        for r in csv.reader(open(f)):
            try: name.setdefault(int(r[0], 16), r[1])
            except (ValueError, IndexError): pass
os.makedirs(OUT, exist_ok=True)
rows = list(csv.DictReader(open(IN)))
mine = []
for r in rows:
    a = int(r["lo"], 16)
    if r["kind"] == "code" and lo <= a < hi:
        mine.append({"lo": r["lo"], "hi": r["hi"], "kind": "code", "owner": LIB, "member": KEY, "name": name.get(a, r["name"])})
with open(os.path.join(OUT, "items.csv"), "w") as f:
    w = csv.DictWriter(f, ["lo", "hi", "kind", "owner", "member", "name"]); w.writeheader(); w.writerows(rows); w.writerows(mine)
open(os.path.join(OUT, "members.txt"), "w").write(KEY + "\n")
objs = sorted(glob.glob(os.path.join(OBJD, "*.obj")))
import struct      # libs.cfg `alias = name=addr`: a COMMON variable of the lib that live game code reads / writes by its exe address (qh_qh) becomes an undefined external,
fixed = {"_" + a.split("=")[0] for a in c.get("alias", "").split()}      # mkobj defines it at that address (lib_names.txt), so game and lib share the bytes
nfix = 0; os.makedirs(os.path.join(OUT, "o"), exist_ok=True)      # the build dir belongs to docker (root): patched copies
for o in objs:
    d = bytearray(open(o, "rb").read()); so, ns_ = struct.unpack_from("<II", d, 8); st = so + ns_ * 18; i = 0
    while i < ns_:
        q = so + i * 18; raw = bytes(d[q:q + 8])
        nm = d[st + struct.unpack_from("<I", raw, 4)[0]:d.index(b"\0", st + struct.unpack_from("<I", raw, 4)[0])].decode("latin1") if raw[:4] == b"\0\0\0\0" else raw.rstrip(b"\0").decode("latin1")
        val, sec, typ, cls, aux = struct.unpack_from("<IhHBB", d, q + 8)
        if nm in fixed and cls == 2 and sec == 0 and val > 0: struct.pack_into("<I", d, q + 8, 0); nfix += 1
        i += 1 + aux
    open(os.path.join(OUT, "o", os.path.basename(o)), "wb").write(d)
open(os.path.join(OUT, "extra_objs.txt"), "w").write("".join("lib:%s %s\n" % (KEY, os.path.join(OUT, "o", os.path.basename(o))) for o in objs))
print("%s: %d COMMON symbols moved to their exe address; %d code items (%d bytes) dropped, %d named, %d objects" % (LIB, nfix, len(mine), sum(int(m["hi"], 16) - int(m["lo"], 16) for m in mine), sum(1 for m in mine if int(m["lo"], 16) in name), len(objs)))
