#!/usr/bin/env python3
"""Writes output/librw_asset_parity_<date>.csv from the librw rows and the oracle scenes.  Usage: report.py <librw.csv> <scenes dir (menu, c0..c14 each with oracle.csv)> <out.csv>
Sections: scene (rows per scene and family), field (per family / field over all 16 scenes: compared, equal, differing, status, note)."""
import os, sys, csv, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import compare
NOTE = {   # (family, field) -> (status, note); fields not listed are OK when they have no differences
 ("F", "*"): ("RUNTIME", "non-root frames of an instance differ only when the game animates / moves them (wheels, hanim, MTS placement); the root frame is masked"),
 ("A", "frame"): ("RUNTIME", "depth of the atomic frame: the game re-parents atomics (vehicle debris: oracle -1 = frame outside the root tree)"),
 ("C", "nframes"): ("RUNTIME", "game rebuilds frame trees (debris clumps 8 -> 2 frames, one 20 atomic HUD clump 21 -> 87); geometry identical"),
 ("WM", "tex"): ("RUNTIME", "world material texture replaced by the game (WetRoad removed, offices/GlassBuilding -> sky) on 20 of 1876 materials, 3 courses; not a stream-read difference (names equal in the other 1856)"),
 ("G", "flags"): ("RUNTIME", "game sets / clears geometry flag bits 0x20 (lighting) and 0x40 (modulate material colour) after loading, not derived from material colours; masked"),
 ("G", "h_colors"): ("RUNTIME", "pre-lit colours of particle sprite geometries (no triangles) are rewritten by the game"),
 ("M", "rgba"): ("RUNTIME", "material alpha / colour set by the game (0x80 alpha on translucent props, shadow materials)"),
 ("M", "tex"): ("AMBIG", "geometries with identical vertex hash (quads) but different textures share a key"),
 ("T", "filteraddr"): ("RUNTIME", "Sun / smoke / lensflare-fs8: file says MIPLINEAR (0x1104), the game sets LINEAR (0x1102) after loading"),
 ("TD", "h_names"): ("RUNTIME", "single-texture dictionaries the game builds itself; textures it creates from PNG (keyboard / joypad / LoadIcon) have no file counterpart"),
 ("A", "interp_flags"): ("FIXED", "was 0 in librw, original 1 for every atomic with a geometry (create writes 3): clump.cpp"),
}
def note(f, n):
    for k in ((f, n), (f, n.split(".")[0] + ".*"), (f, "*")):
        if k in NOTE: return NOTE[k]
    return None
def main(lib, scenes, out):
    names = ["menu"] + ["c%d" % i for i in range(15)]
    rows = []; allst = collections.defaultdict(lambda: [0, 0, 0, 0]); fd = collections.Counter(); fc = collections.Counter(); sm = {}
    for sc in names:
        fn = os.path.join(scenes, sc, "oracle.csv")
        if not os.path.exists(fn): continue
        st, d, c, s = compare.main(lib, [fn])
        for f, v in sorted(st.items()):
            rows.append(["scene", sc, f, "", v[0], v[1], v[2], v[3], "", ""])
            for i in range(4): allst[f][i] += v[i]
        fd.update(d); fc.update(c)
        for k, v in s.items(): sm.setdefault(k, v)
    for f, v in sorted(allst.items()): rows.append(["family", "all", f, "", v[0], v[1], v[2], v[3], "", ""])
    for (f, n), c in sorted(fc.items()):
        d = fd.get((f, n), 0); nt = note(f, n)
        status = nt[0] if d and nt else ("OK" if not d else "OPEN"); text = nt[1] if nt and d else ""
        rows.append(["field", "all", f, n, c, c - d, d, "", status, text])
    with open(out, "w", newline="") as o:
        w = csv.writer(o); w.writerow(["kind", "scene", "family", "field", "compared_or_oracle_rows", "equal_or_matched", "differ_or_fielddiff", "no_counterpart", "status", "note"])
        w.writerows(rows)
    print("wrote", out, len(rows), "rows")
if __name__ == "__main__": main(*sys.argv[1:4])
