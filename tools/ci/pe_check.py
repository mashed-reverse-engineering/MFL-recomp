#!/usr/bin/env python3
"""pe_check.py: structural check of a built exe (or --dll: mflrw.dll) before anything reaches Wine.
usage: pe_check.py <exe> [--rwdll <mflrw.dll>] [--manifest <data/link/MFLng.manifest>] [--icon <assets/icon/mflng.ico>] [--static-crt] | --dll <mflrw.dll>
       | --launcher <MFLng-launch.exe> [--manifest <file>] [--icon <file>] | --video <dll> | --selftest
--icon: our own application icon is allowed as well (RT_ICON / RT_GROUP_ICON): exactly the images of that .ico (byte-equal), one group, nothing else.
--static-crt (W16 MT, LIB_CRT=v143mt exes): ucrtbase / vcruntime140 / api-ms-win-crt-* leave the allow-list.
--launcher (W16 LAUNCH): its own list of Windows system DLLs (kernel32 user32 gdi32 shell32 advapi32 winhttp), static CRT (no CRT DLL),
     no mflrw.dll; resources and --manifest as for the game exes.  The game exes' list is unchanged.
exe: PE32 i386; imports only from the allow-list; every mflrw.dll import is exported by --rwdll; resources only RT_MANIFEST (24);
     --manifest: exactly one RT_MANIFEST, byte-equal to the file (embed it with rc `1 24 "MFLng.manifest"` + /manifest:no:
     link /manifest:embed /manifestinput rewrites the XML and fails this compare).
dll: PE32 i386; imports only kernel32/user32/gdi32/d3d9/msvcrt (mingw build, static libgcc/libstdc++).
--video (W16 VIDCI): mfl_video_ffmpeg.dll or one of the shared LGPL FFmpeg DLLs beside it: PE32 i386, no resources, imports only
     the system DLLs of VIDEO_OK plus each other (avformat/avcodec/avutil/swscale-NN.dll); no libgcc/libwinpthread, no GPL postproc."""
import fnmatch, os, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from rw_stub_guard import parse_pe, make_pe, icon_bad

# INV 5 item 3 (VC7 exe) + v143 /MD additions
CRT_DLLS = ["ucrtbase.dll", "vcruntime140.dll", "api-ms-win-crt-*.dll"]
EXE_OK = ["kernel32.dll", "user32.dll", "gdi32.dll", "advapi32.dll", "ole32.dll", "d3d9.dll", "dinput8.dll",
          "winmm.dll", "mflrw.dll"] + CRT_DLLS
DLL_OK = ["kernel32.dll", "user32.dll", "gdi32.dll", "d3d9.dll", "msvcrt.dll"]
LAUNCHER_OK = ["kernel32.dll", "user32.dll", "gdi32.dll", "shell32.dll", "advapi32.dll", "winhttp.dll"]      # system DLLs only, static CRT
VIDEO_OK = ["kernel32.dll", "user32.dll", "msvcrt.dll", "bcrypt.dll", "ole32.dll", "avformat-*.dll", "avcodec-*.dll", "avutil-*.dll", "swscale-*.dll"]


def check(data, dll=False, rwdata=None, manifest=None, static_crt=False, launcher=False, video=False, icon=None):
    p, bad = parse_pe(data), []
    if p["machine"] != 0x14C or p["magic"] != 0x10B:
        return ["not PE32 i386 (machine 0x%x magic 0x%x)" % (p["machine"], p["magic"])]
    if video and p["res_types"]:
        bad.append("resources in a video DLL: %s" % p["res_types"])
    ok = VIDEO_OK if video else DLL_OK if dll else LAUNCHER_OK if launcher else [a for a in EXE_OK if not (static_crt and a in CRT_DLLS)]      # W16 MT --static-crt: no redistributable DLL
    for d in p["imports"]:
        if not any(fnmatch.fnmatch(d, a) for a in ok):
            bad.append("import from %s not in allow-list" % d)
    if not (dll or video):
        if rwdata is not None and "mflrw.dll" in p["imports"]:
            have = set(parse_pe(rwdata)["exports"])
            miss = [n for n in p["imports"]["mflrw.dll"] if n not in have]
            if miss:
                bad.append("%d mflrw.dll imports not exported: %s" % (len(miss), " ".join(miss[:20])))
        if [r for r in p["res_types"] if r not in ((24, 3, 14) if icon is not None else (24,))]:      # --icon: our own RT_ICON (3) / RT_GROUP_ICON (14) too
            bad.append("resource types besides RT_MANIFEST%s: %s" % (" and the icon" if icon is not None else "", p["res_types"]))
        if icon is not None and icon_bad(p, icon):
            bad.append(icon_bad(p, icon))
        if manifest is not None and p["manifests"] != [manifest]:
            bad.append("RT_MANIFEST is not the committed manifest (%d resources, sizes %s, want one of %d bytes)" % (len(p["manifests"]), [len(m) for m in p["manifests"]], len(manifest)))
    return bad


def selftest():
    good = make_pe({"kernel32.dll": ["A"], "mflrw.dll": ["RwA"], "api-ms-win-crt-stdio-l1-1-0.dll": ["x"]}, res=[24])
    rw = make_pe({"kernel32.dll": ["A"]}, exports=["RwA", "RwB"])
    assert check(good, rwdata=rw) == []
    assert check(make_pe({"evil.dll": ["A"]})), "allow-list not enforced"
    assert check(make_pe({"mflrw.dll": ["RwZ"]}), rwdata=rw), "missing export not caught"
    assert check(make_pe({"kernel32.dll": ["A"]}, res=[24, 3])), "extra resource not caught"
    assert check(make_pe({"kernel32.dll": ["A"]}, machine=0x8664)), "arch not caught"
    m = b"<assembly/>"
    assert check(make_pe({"kernel32.dll": ["A"]}, res={24: m}), manifest=m) == []
    assert check(make_pe({"kernel32.dll": ["A"]}, res={24: m + b" "}), manifest=m), "changed manifest not caught"
    assert check(make_pe({"kernel32.dll": ["A"]}), manifest=m), "missing manifest not caught"
    assert check(good, rwdata=rw, static_crt=True), "--static-crt: CRT DLL import not caught"
    assert check(make_pe({"kernel32.dll": ["A"], "mflrw.dll": ["RwA"]}), rwdata=rw, static_crt=True) == []
    assert check(make_pe({"msvcrt.dll": ["a"], "d3d9.dll": ["b"]}), dll=True) == []
    assert check(make_pe({"libgcc_s_dw2-1.dll": ["a"]}), dll=True), "dll allow-list not enforced"
    la = {"kernel32.dll": ["A"], "user32.dll": ["B"], "shell32.dll": ["C"], "advapi32.dll": ["D"], "winhttp.dll": ["E"]}
    assert check(make_pe(la, res={24: m}), manifest=m, launcher=True) == []
    assert check(make_pe(dict(la, **{"vcruntime140.dll": ["x"]}), res={24: m}), launcher=True), "--launcher: CRT DLL not caught"
    assert check(make_pe(dict(la, **{"mflrw.dll": ["RwA"]}), res={24: m}), launcher=True), "--launcher: mflrw.dll not caught"
    assert check(make_pe(la, res={24: m, 3: b"i"}), launcher=True), "--launcher: extra resource not caught"
    assert check(make_pe({"winhttp.dll": ["E"]}), launcher=False), "game exe list must not take winhttp.dll"
    img = b"\x89PNG-fake"; ico = struct.pack("<HHH", 0, 1, 1) + struct.pack("<BBBBHHII", 16, 16, 0, 0, 1, 32, len(img), 22) + img
    ex = {"kernel32.dll": ["A"]}
    assert check(make_pe(ex, res={24: m, 3: img, 14: b"g"}), manifest=m, icon=ico) == []
    assert check(make_pe(ex, res={24: m, 3: img, 14: b"g"}), manifest=m), "icon resources without --icon not caught"
    assert check(make_pe(ex, res={24: m, 3: img + b"x", 14: b"g"}), manifest=m, icon=ico), "changed icon image not caught"
    assert check(make_pe(ex, res={24: m, 3: img}), manifest=m, icon=ico), "missing icon group not caught"
    assert check(make_pe(ex, res={24: m, 3: img, 14: b"g", 16: b"v"}), manifest=m, icon=ico), "extra resource besides the icon not caught"
    assert check(make_pe(ex, res={24: m}), manifest=m, icon=ico), "missing icon not caught"
    v = {"kernel32.dll": ["A"], "msvcrt.dll": ["B"], "bcrypt.dll": ["C"], "avcodec-61.dll": ["D"], "avutil-59.dll": ["E"]}
    assert check(make_pe(v), video=True) == []
    assert check(make_pe(dict(v, **{"libwinpthread-1.dll": ["x"]})), video=True), "--video: libwinpthread not caught"
    assert check(make_pe(dict(v, **{"postproc-58.dll": ["x"]})), video=True), "--video: GPL postproc not caught"
    assert check(make_pe(v, res=[16]), video=True), "--video: resource not caught"
    assert check(make_pe({"avcodec-61.dll": ["D"]})), "game exe list must not take FFmpeg DLLs"
    print("pe_check selftest OK")


def main(a):
    if a[:1] == ["--selftest"]:
        return selftest()
    dll = a[:1] == ["--dll"]
    video = a[:1] == ["--video"]
    if dll or video:
        a = a[1:]
    st = "--static-crt" in a
    la = "--launcher" in a
    a = [x for x in a if x not in ("--static-crt", "--launcher")]
    opt = {}
    for k in ("--rwdll", "--manifest", "--icon"):
        if k in a:
            i = a.index(k); opt[k] = open(a[i + 1], "rb").read(); a = a[:i] + a[i + 2:]
    if len(a) != 1:
        sys.exit(__doc__)
    bad = check(open(a[0], "rb").read(), dll, opt.get("--rwdll"), opt.get("--manifest"), st, la, video, opt.get("--icon"))
    for b in bad:
        print("FAIL: " + b)
    if bad:
        sys.exit(1)
    print("pe_check PASS: " + a[0])


if __name__ == "__main__":
    main(sys.argv[1:])
