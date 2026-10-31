#!/usr/bin/env python3
"""Prepare a runnable copy of the MFL install for ghidra/tools/wine-run/run.sh (stdlib only).
Usage: prepare.py <install dir (the one with App_Executables)> <target dir> [exe to use instead of the install's MFL.exe, e.g. an overlay.py result [link map of a linked exe]]
- copies the install (the game writes mashed.log / videocfg.bin / saves next to the exe, so never run the original),
- patches the copy of MFL.exe so the DirectShow movie code is skipped (Wine has no mpeg decoder in the image):
    0x4A44A0 video_play_loop -> ret, 0x4A3A20 movie_renderer_create -> return 0x80004005, 0x4A3930 sound_volume_to_directsound_db -> ret, 0x4A39A0 movie_stop -> ret, 0x4A3860 movie_renderer_release -> ret
  An overlay exe (overlay.py) has `jmp <C replacement>` at a replaced entry: the stub is then written at the jump target as well (followed through the
  --count counter thunk), so the C callers that call the C function directly hit the stub too (movie_stop_b -> C movie_stop crashed on the NULL graph g_app_76e8ac),
- VIDEO_QUALITY=low|high (env) sets the quality checkboxes of videocfg.bin (default: the dialog defaults),
- writes videocfg.bin for window mode 640x480x32 (mode 0, fullscreen=1/mode 2 also works in Xvfb 640x480x24).
The VA -> file offset mapping of MFL.exe .text is VA - 0x400000."""
import os, shutil, struct, sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "MashedFormats"))
import mashed_config as mc

def recomp_sites(data, va):
    """the C replacement of the function at va in an overlay exe: the entry is `jmp` into the .recomp section, possibly via a --count thunk
    `pushfd; inc [cnt]; popfd; jmp new` (13 bytes); [] for an original function"""
    e = struct.unpack_from("<I", data, 0x3C)[0]; ns, osz = struct.unpack_from("<H", data, e + 6)[0], struct.unpack_from("<H", data, e + 20)[0]
    sec = {}
    for k in range(ns):
        h = e + 24 + osz + k * 40
        vs, rva, rs, rp = struct.unpack_from("<IIII", data, h + 8)
        sec[bytes(data[h:h + 8]).rstrip(b"\0")] = (0x400000 + rva, rs, rp)
    if b".recomp" not in sec or data[va - 0x400000] != 0xE9: return []
    lo, rs, rp = sec[b".recomp"]
    t = (va + 5 + struct.unpack_from("<i", data, va - 0x400000 + 1)[0]) & 0xFFFFFFFF
    if not lo <= t < lo + rs: return []
    o = rp + t - lo
    if data[o:o + 3] == b"\x9c\xff\x05" and data[o + 8] == 0xE9:      # coverage counter thunk: the function is behind it
        t = (t + 13 + struct.unpack_from("<i", data, o + 9)[0]) & 0xFFFFFFFF
        if not lo <= t < lo + rs: return []
    return [rp + t - lo]


PATCHES = [(0x4A44A0, b"\xC3"), (0x4A3A20, bytes.fromhex("B805400080C3")), (0x4A3930, b"\xC3"), (0x4A39A0, b"\xC3"), (0x4A3860, b"\xC3")]


MOVIE_NAMES = {"video_play_loop": 0x4A44A0, "movie_renderer_create": 0x4A3A20, "sound_volume_to_directsound_db": 0x4A3930, "movie_stop": 0x4A39A0, "movie_renderer_release": 0x4A3860}


def map_sites(map_path):
    """file offsets of our C copies of the movie functions in a linked exe (link.exe /map: `0001:00001234 _name 00401234 f obj`); the entry jmp
    of the original address leads there, so the stub has to be written at the map address as well"""
    import re
    code = dict(PATCHES); offs = []; orig = set(); ib = 0x400000
    for l in open(map_path, errors="replace"):
        q = l.split()
        if len(q) >= 5 and q[:3] == ["Preferred", "load", "address"]: ib = int(q[4], 16)      # a relinked exe may sit at another base (file offset = VA - base)
        if len(q) >= 4 and re.fullmatch(r"[0-9a-f]{8}", q[2]) and q[-1].endswith(".obj"):
            n = q[1].lstrip("_").split("@")[0]
            if n in MOVIE_NAMES: offs.append((int(q[2], 16) - ib, code[MOVIE_NAMES[n]]))
            elif n.endswith("_orig") and n[:-5] in MOVIE_NAMES: offs.append((int(q[2], 16) - ib, code[MOVIE_NAMES[n[:-5]]])); orig.add(MOVIE_NAMES[n[:-5]])      # the original entry, wherever the layout put it (layer swaps)
    return offs, orig


def main(src, dst, exe_src=None, map_path=None):
    # base image: ONE pristine copy of the install (327 MB), every prepared game directory is a hardlink farm of it (no disk space, no copy time);
    # only MFL.exe and videocfg.bin are written as new files (unlinked first: a write into a hardlinked file would change the base and every other directory)
    base = os.environ.get("MFL_BASE", os.path.expanduser("~/.cache/mfl_base"))
    if not os.path.isdir(os.path.join(base, "App_Executables")):
        shutil.rmtree(base + ".tmp", ignore_errors=True)
        shutil.copytree(src, base + ".tmp")
        os.rename(base + ".tmp", base)
        for root, dirs, files in os.walk(base):      # the shared files are read-only: a write by a host tool fails instead of silently changing every game directory
            for f in files:
                p = os.path.join(root, f)
                os.chmod(p, os.stat(p).st_mode & ~0o222)
        open(os.path.join(base, ".stamp"), "w").close()
    if os.path.exists(dst):
        shutil.rmtree(dst)
    shutil.copytree(base, dst, copy_function=os.link)
    exe = os.path.join(dst, "App_Executables", "MFL.exe")
    data = bytearray(open(exe_src or exe, "rb").read())
    msites, morig = map_sites(map_path) if map_path else ([], set())
    stubs = bool(os.environ.get("MOVIE_STUBS"))      # default: only functions that are still the ORIGINAL (control exe, pristine MFL.exe) are stubbed; a replaced one (entry jmp to our C) runs the mfl_video null backend (src/video, A7 2026-10-31); MOVIE_STUBS=1 = the old behaviour
    for va, code in PATCHES:
        if not stubs and data[va - 0x400000] == 0xE9: continue
        for off in recomp_sites(data, va) + ([] if va in morig else [va - 0x400000]):      # the C replacement first (found through the entry jmp), then the entry itself (a link map names it `_<fn>_orig`: moved layers)
            data[off:off + len(code)] = code
    for off, code in (msites if stubs else []):      # linked exe (tools/delink/link_ours.sh): no .recomp section, our C functions are found through the link map
        data[off:off + len(code)] = code
    os.unlink(exe)
    open(exe, "wb").write(data)
    os.chmod(exe, 0o644)
    for f in ("videocfg.bin", "mashed.log"):
        p = os.path.join(dst, "App_Executables", f)
        if os.path.exists(p):
            os.unlink(p)
    v = mc.VideoCfg(bytearray(0x200))
    v.version = 5; v.width = 640; v.height = 480; v.depth = 32; v.fullscreen = 1; v.mode_index = 2
    v.subsystem = "NVIDIA GeForce GTX 470"; v.mode_name = "640 x 480 x 32"
    if os.environ.get("VIDEO_QUALITY") == "low":     # the four quality checkboxes of the video dialog: multitexture, dynamic shadows, environment effects off
        v.no_multitexture = 1; v.no_dynamic_shadows = 1; v.no_env_effects = 1; v.trilinear = 0
    elif os.environ.get("VIDEO_QUALITY") == "high":
        v.no_multitexture = 0; v.no_dynamic_shadows = 0; v.no_env_effects = 0; v.trilinear = 1
    open(os.path.join(dst, "App_Executables", "videocfg.bin"), "wb").write(v.build())   # a new file (unlinked above)
    print("prepared", dst)


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2], sys.argv[3] if len(sys.argv) > 3 else None, sys.argv[4] if len(sys.argv) > 4 else None)
