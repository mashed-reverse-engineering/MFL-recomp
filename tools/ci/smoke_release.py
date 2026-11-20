#!/usr/bin/env python3
"""smoke_release.py <release dir> <maps dir> <game dir> <work dir> [--run ID] [--check-only]: Wine title smoke of a CI release.
<release dir>: the unpacked MFLng-<tag>.zip (MFLng.exe, MFLng_fixes.exe, mflrw.dll, mfl_audio_miniaudio.dll, BUILDINFO.txt);
<maps dir>: the unpacked MFLng-<tag>-maps.zip (MFLng.map, MFLng_fixes.map).
<game dir>: a prepared install, made once with `python3 tools/wine-run/prepare.py <install dir> <game dir>` (private tree: `run.py clone
<game dir>`): App_Executables/MFL_original.exe, videocfg.bin, data hardlinked to ~/.cache/mfl_base; must be on the work dir's filesystem. It is never written: each exe gets <work dir>/<exe stem>/game, a hardlink farm of it
with fresh copies of the exe (as MFL.exe) and the two DLLs, and delta.txt for tools/wine-run/release_smoke.sh.
The delta (exe address of _g_menu_current - 0x69cbd0) comes from the exe's OWN map (rule 41; the fixes exe's .data/.bss sit 0x1000 higher
than the off exe's): the map must be <exe stem>.map, carry the exe's PE TimeDateStamp, give one delta for both words release_smoke waits on,
and the exe must reference that address. Otherwise the exe is refused (FAIL, no Wine run).
Prints one line per exe and, when both reach the title, the line for data/link/smoke_ok.txt (`<cl version> <date> <run>`, release.yml);
--run = the GitHub run id of the release workflow that built the zip (`gh run list --workflow release`). Writes no tracked file.
--check-only: map checks only, no Wine run."""
import datetime, os, shutil, struct, subprocess, sys

R = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SMOKE = os.path.join(R, "tools", "wine-run", "release_smoke.sh")
WANT = {"_g_menu_current": 0x69cbd0, "_g_menu_fade_alpha": 0x69d1e4}
EXES = ("MFLng.exe", "MFLng_fixes.exe")
DLLS = ("mflrw.dll", "mfl_audio_miniaudio.dll")


def delta(exe_path, map_path):
    """the .bss delta of exe_path from map_path, or raise ValueError when the map is not the exe's own"""
    if os.path.splitext(os.path.basename(map_path))[0] != os.path.splitext(os.path.basename(exe_path))[0]:
        raise ValueError("map %s is not named after %s" % (os.path.basename(map_path), os.path.basename(exe_path)))
    exe = open(exe_path, "rb").read()
    ts = struct.unpack_from("<I", exe, struct.unpack_from("<I", exe, 0x3c)[0] + 8)[0]
    got, mts = {}, None
    for l in open(map_path, errors="replace"):
        p = l.split()
        if p[:2] == ["Timestamp", "is"] and mts is None: mts = int(p[2], 16)
        if len(p) >= 3 and p[1] in WANT and p[1] not in got: got[p[1]] = int(p[2], 16) - WANT[p[1]]
    if mts != ts: raise ValueError("map timestamp %s != exe TimeDateStamp %08x" % (mts and "%08x" % mts, ts))
    if len(got) != 2 or len(set(got.values())) != 1: raise ValueError("no single .bss delta: %s" % {k: hex(v) for k, v in got.items()})
    d = got["_g_menu_current"]
    refs = exe.count(struct.pack("<I", WANT["_g_menu_current"] + d))
    if not refs: raise ValueError("exe never references g_menu_current at %x" % (WANT["_g_menu_current"] + d))
    return d, refs


def main(a):
    run = a[a.index("--run") + 1] if "--run" in a else "<run>"
    check = "--check-only" in a
    rel, maps, game, work = [os.path.abspath(x) for x in a[:4]]
    if not os.path.exists(os.path.join(game, "App_Executables", "MFL_original.exe")):
        sys.exit("smoke_release: %s has no App_Executables/MFL_original.exe (run.py clone it first)" % game)
    ok = 0
    for x in EXES:
        stem = os.path.splitext(x)[0]
        try:
            d, refs = delta(os.path.join(rel, x), os.path.join(maps, stem + ".map"))
        except (ValueError, OSError) as e:
            print("%s: FAIL refused: %s" % (x, e)); continue
        tag = "%s: delta %x (%s.map, %d refs)" % (x, d, stem, refs)
        if check:
            print(tag, "map ok"); ok += 1; continue
        L = os.path.join(work, stem)
        if os.path.exists(L): shutil.rmtree(L)      # our own work dir; the game dir is only hardlinked from, never written
        shutil.copytree(game, os.path.join(L, "game"), copy_function=os.link)
        A = os.path.join(L, "game", "App_Executables")
        for src, dst in [(x, "MFL.exe")] + [(f, f) for f in DLLS]:
            p = os.path.join(A, dst)
            if os.path.lexists(p): os.unlink(p)      # unlink first: never write through a hardlink into the game dir
            shutil.copy2(os.path.join(rel, src), p)
        open(os.path.join(L, "delta.txt"), "w").write("%x\n" % d)
        r = subprocess.run(["bash", SMOKE, L, os.path.join(L, "out"), "title"], capture_output=True, text=True)
        last = (r.stdout.strip().splitlines() or ["release_smoke: no output (exit %d)" % r.returncode])[-1]
        print(tag, last, "(out %s/out)" % L)
        ok += r.returncode == 0 and last == "release_smoke title: ok"
    if ok == len(EXES) and not check:
        cl = [l.split()[1] for l in open(os.path.join(rel, "BUILDINFO.txt")) if l.startswith("cl ")][0]
        print("smoke_ok.txt line: %s %s %s" % (cl, datetime.date.today().isoformat(), run))
    return 0 if ok == len(EXES) else 1


if __name__ == "__main__":
    if len(sys.argv) < 5: sys.exit(__doc__)
    sys.exit(main(sys.argv[1:]))
