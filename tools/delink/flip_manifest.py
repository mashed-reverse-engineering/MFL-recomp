#!/usr/bin/env python3
"""flip_manifest.py [--gate <gate dir>] [--out <csv>] [--engine <txt>]: the engine-switch flip manifest (plan_engine_switch_2026-11-07 P3).
Every still-original RenderWare byte range of the gate's zero_original_ranges.csv (default: the newest gate dir that has one) is split into the
ownership items of output/delink_sdk/ownership_items.csv; each function row gets: va, bytes still original, SDK member object, group (= member
object basename), scenario coverage (output/scenario/rwcov_*.csv, newest), the route and the blocker:
  KES    D3D9 driver module (state = file statics + the device): a DLL kernel on exe state, proven natively against a fake device
  flip   object layout / lifecycle shared with game C (frame, clump, atomic, geometry, world, camera, light, Rx pipelines, plugins on those objects)
  tier2  device independent core without shared layout (math, stream, image, plugin registry, ...): codfh tier 2 (user decision 1) or a KES leaf
  trap   0 hits in the coverage union and not a device path: candidate only, the three proofs are still needed
Rows already in a tools/delink/rw_<group>.txt list get that list's name in `planned` (e.g. d3drs from lane D).
Also writes the rw_engine.txt draft: the flip rows whose exe name has a librw facade export (librw-mfl capi/rwmfl.def), exe_name=export.
codfh_file / codfh: the codfh-win32 C file named after the group (rwref/codfh-win32/src/rw/**/<group>.c, --codfh <dir>) and y/n whether `<name>(`
appears in it (name scan only, body not checked: an upper bound for tier-2 candidates; the md sums it per route, tier2_codfh = the tier2 rows with y).
Outputs default to output/engine_flip_manifest_<date>.csv / .md and output/rw_engine_draft.txt. Rerunnable; reads only."""
import collections, csv, datetime, glob, ntpath, os, re, sys
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
A = sys.argv[1:]
def opt(k, d=None): return A[A.index(k) + 1] if k in A else d
DEF = opt("--def", os.path.join(G, "..", "librw-mfl", "capi", "rwmfl.def"))
TODAY = datetime.date.today().isoformat()
CODFH = opt("--codfh", os.path.join(G, "..", "rwref", "codfh-win32"))

def codfh_files():
    fs = {}
    for p in sorted(glob.glob(os.path.join(CODFH, "src", "rw", "**", "*.c"), recursive=True)):
        fs.setdefault(os.path.splitext(os.path.basename(p))[0].lower(), p)
    return fs

def newest_gate():
    c = [p for p in glob.glob(os.path.join(G, "output", "gate", "*", "zero_original_ranges.csv"))]
    return os.path.dirname(max(c, key=os.path.getmtime))

# member object basename -> route; first matching rule wins (regex on the basename)
ROUTES = [
    (r"^(baframe|baclump|baworld|bacamera|bacamval|baatomic|bageomet|bamateri|bamatlst|balight|bamesh|bameshop|wrldsect|baworobj|basector|nodeworld|wrldpipe|bapipe|bapipew|"
     r"p2|nodes|nodeatom|nodewsec|nodeinstance|nodeimmi|rpskin|skin|rphanim|hanim|rpmatfx|matfx|rpuvanim|rppvs|rpptank|ptank|rplodatm|rpcollis|ctbsp|rpdmorph|rpsplit|rtanim|anim|"
     r"effectpipes|noded3d9|patch|rppatch|rpmorph|rpusrdat|babinwor|coll|ctquery|ctdata|im3dpipe|rpspline|stdkey|linear|param|ssematbl|bsplit|rpnormmap)", "flip"),
    (r"^(d3d9|d3d8|devd3d|d3dx)", "KES"),
    (r"^(baraster|batextur|babintex|rtpitexd|baimage|babinras|native|baresamp|palquant|rtbmp|rtpng|gzio)$", "KES"),   # exe-layout objects mirrored in capi/rw37.h (as g3)
]
FLIP_PREFIX = re.compile(r"^_?_?(Rp|Rx|_rx|_rp|rx|rp)")       # Rx / Rp names are object-layout code even in mixed objects
def route_of(group, name):
    for rx, r in ROUTES:
        if re.match(rx, group): return r
    if FLIP_PREFIX.match(name or ""): return "flip"
    return "tier2"

BLOCK = {
    "KES": "DLL kernel on exe state: driver rows need fake device slots for the methods they call, core rows (raster / texture / image) a g3-style native check; never trapped (device paths)",
    "flip": "librw object layout = RW 3.7 (layout_check.cpp TODO rows: Raster, Texture, Atomic, Light, Camera, Stream) and lifecycle owned by librw",
    "tier2": "user decision 1 (codfh tier 2) or a KES leaf port",
    "trap": "three trap proofs (coverage union, assets, call-graph closure); traphit 0 on the gate",
}
GROUP_BLOCK = {"bacamera": "Camera.frustumCorners 0x124 / recipViewWindow / viewOffset not at RW 3.7 offsets in librw",
               "baraster": "Raster byte fields 0x20..0x23 and size 0x34 differ in librw", "batextur": "Texture size 0x58 differs in librw",
               "baatomic": "Atomic size 0x70 differs in librw", "balight": "Light size 0x40 differs in librw", "rpptank": "no PTank Data in the exe-shared layout (capi/rw37.h RpPTankExt37)",
               "bastream": "Stream: RtFile / memory position at +0xc (librw Stream is a C++ object)", "bainit": "RwEngineInstance shadow (RwGlobals37, dOpenDevice 0x10)",
               "baengine": "RwEngineInstance shadow (RwGlobals37, dOpenDevice 0x10)",
               "d3d9device": "device create / reset / loss / caps paths: only a native sequence covers them (fake device needs Reset, caps from the Wine dump)",
               "d3d9rendst": "lane D1 (d3drs): 6 device methods, fake device ready", "d3d9convrt": "lane X1 (d3dconv): fake LockRect / UnlockRect arenas",
               "d3d92drend": "lane D2 (d3dim2d): VB Lock / Unlock arena, needs V2 normalised drawcmp", "d3d9vertexbuffer": "lane D2 (d3dim2d): ring state, VB Lock arena",
               "d3d9raster": "X2 slice (d3dras): palettised depth (_rwD3D9RasterCreate port), texture create / lock through the device",
               "d3d9vertexshader": "vertex shader creation on the device; MFL draws fixed function (check coverage before porting)"}

def main():
    gate = opt("--gate") or newest_gate()
    zr = [(int(r["lo"], 16), int(r["hi"], 16)) for r in csv.DictReader(open(os.path.join(gate, "zero_original_ranges.csv"))) if r["layer"] == "RenderWare"]
    items = [r for r in csv.DictReader(open(os.path.join(G, "output", "delink_sdk", "ownership_items.csv"))) if r["kind"] == "code"]
    for r in items: r["lo"], r["hi"] = int(r["lo"], 16), int(r["hi"], 16)
    items.sort(key=lambda r: r["lo"])
    covf = max(glob.glob(os.path.join(G, "output", "scenario", "rwcov_*.csv")), key=os.path.getmtime)
    cov = {int(r["va"], 16): (int(r["hit"]), len([j for j in r["jobs"].split(";") if j])) for r in csv.DictReader(open(covf))}
    planned = {}
    for f in glob.glob(os.path.join(G, "tools", "delink", "rw_*.txt")):
        g = os.path.basename(f)[3:-4]
        if g in ("dead", "trap", "g0_candidates"): continue
        for l in open(f):
            l = l.split("#")[0].strip()
            if l: planned.setdefault(l.split("=")[0].split()[0], g)
    exports = {l.split()[0].split("=")[0] for l in open(DEF) if l.strip() and not l.lstrip().startswith((";", "EXPORTS", "LIBRARY"))} if os.path.exists(DEF) else set()
    cfs, ctext = codfh_files(), {}
    rows, last_member = [], ""
    for it in items:
        member = it["member"] or ""
        if member: last_member = member
        n = sum(max(0, min(hi, it["hi"]) - max(lo, it["lo"])) for lo, hi in zr if lo < it["hi"] and hi > it["lo"])
        if not n: continue
        m = member or last_member                         # unnamed lib_ items sit inside the previous object
        grp = os.path.splitext(ntpath.basename(m.split(":", 1)[-1]))[0].lower() if m else "unknown"
        lib = m.split(":", 1)[0] if ":" in m else ""
        name = it["name"]; hit, jobs = cov.get(it["lo"], (None, 0))
        rt = route_of(grp, name)
        if hit == 0 and rt != "KES" and not grp.startswith("d3d9"): rt_final = "trap"
        else: rt_final = rt
        blk = GROUP_BLOCK[grp] if rt_final != "trap" and grp in GROUP_BLOCK else BLOCK[rt_final]
        rows.append({"va": "%08x" % it["lo"], "name": name, "bytes": n, "lib": lib, "group": grp, "member_guess": "" if member else "1",
                     "hit": "" if hit is None else hit, "jobs": jobs, "route": rt_final, "route_if_hit": rt, "planned": planned.get(name, ""), "blocker": blk})
        cf = cfs.get(grp)
        if cf and cf not in ctext: ctext[cf] = open(cf, errors="replace").read()
        found = bool(cf) and re.search(r"(?<![A-Za-z0-9])_*%s\s*\(" % re.escape(name.lstrip("_")), ctext[cf]) is not None
        rows[-1]["codfh_file"] = os.path.relpath(cf, CODFH) if cf else ""
        rows[-1]["codfh"] = "y" if found else "n"
    out = opt("--out", os.path.join(G, "output", "engine_flip_manifest_%s.csv" % TODAY))
    with open(out, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0])); w.writeheader(); w.writerows(rows)
    # summary
    by = collections.defaultdict(lambda: [0, 0]); byr = collections.Counter()
    for r in rows: by[(r["route"], r["group"])][0] += r["bytes"]; by[(r["route"], r["group"])][1] += 1; byr[r["route"]] += r["bytes"]
    tot = sum(byr.values()); ztot = sum(hi - lo for lo, hi in zr)
    md = ["# Engine flip manifest %s" % TODAY, "", "Source: %s (RenderWare %d B still original, %d B of it inside code items = the rows; the rest is padding / non-code items), %s, %s. Script: tools/delink/flip_manifest.py." % (os.path.relpath(gate, G), ztot, tot, "output/delink_sdk/ownership_items.csv", os.path.relpath(covf, G)), "",
          "trap = 0 hits in the coverage union (KES rows are never trap candidates); route_if_hit in the csv keeps the route it would have otherwise.", "",
          "| route | KB | rows |", "|---|---:|---:|"]
    for k in ("KES", "flip", "tier2", "trap"): md.append("| %s | %.1f | %d |" % (k, byr[k] / 1024, sum(1 for r in rows if r["route"] == k)))
    cy = [r for r in rows if r["codfh"] == "y"]
    md += ["", "codfh name found in the group's codfh-win32 file (codfh = y, name scan, body not checked; upper bound): " +
           ", ".join("%s %.1f KB / %d" % (k, sum(r["bytes"] for r in cy if r["route"] == k) / 1024, sum(1 for r in cy if r["route"] == k)) for k in ("tier2", "flip", "KES", "trap")) +
           "; tier2_codfh = %.1f KB." % (sum(r["bytes"] for r in cy if r["route"] == "tier2") / 1024)]
    md += ["", "Routes: " + "; ".join("%s = %s" % (k, v) for k, v in BLOCK.items()), "", "| route | group | KB | rows | planned |", "|---|---|---:|---:|---|"]
    for (rt, g), (b, n) in sorted(by.items(), key=lambda x: (x[0][0], -x[1][0])):
        pl = sorted({r["planned"] for r in rows if r["route"] == rt and r["group"] == g and r["planned"]})
        md.append("| %s | %s | %.1f | %d | %s |" % (rt, g, b / 1024, n, ",".join(pl)))
    open(os.path.splitext(out)[0] + ".md", "w").write("\n".join(md) + "\n")
    eng = opt("--engine", os.path.join(G, "output", "rw_engine_draft.txt"))
    auto = re.compile(r"^(FUN_|lib_|LAB_)|_[0-9a-f]{6,8}$")         # auto names in rwmfl.def are api_used.csv mislabels (not RW API)
    fst = {r["api"]: r["status"] for r in csv.DictReader(open(os.path.join(G, "output", "librw_facade_coverage.csv")))}
    fl = [r for r in rows if r["route_if_hit"] == "flip" and r["name"].lstrip("_") in exports and not r["planned"] and not auto.search(r["name"])]
    with open(eng, "w") as f:
        f.write("# DRAFT group \"engine\" (flip, plan_engine_switch_2026-11-07 P3), generated by tools/delink/flip_manifest.py: still-original flip-route RenderWare\n"
                "# functions whose name is a librw facade export of capi/rwmfl.def (the librw object, not the exe's). NOT installable as is: the flip needs\n"
                "# the layout blockers of the manifest closed first. Rows whose facade is not implemented (output/librw_facade_coverage.csv status != Y)\n"
                "# are commented out with their status. Format: exe_name=dll_export.\n")
        for r in fl:
            s = fst.get(r["name"].lstrip("_"), "?")
            f.write("%s%s=%s\n" % ("" if s == "Y" else "#%s " % s, r["name"], r["name"].lstrip("_")))
    print("%d rows, %d B: %s; engine draft %d rows -> %s" % (len(rows), tot, ", ".join("%s %d" % (k, byr[k]) for k in ("KES", "flip", "tier2", "trap")), len(fl), out))

if __name__ == "__main__": main()
