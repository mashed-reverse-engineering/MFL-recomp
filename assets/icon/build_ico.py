#!/usr/bin/env python3
"""build_ico.py [<mflng.svg> <mflng.ico>]: renders the SVG with rsvg-convert at 16 24 32 48 64 128 256 px and packs the PNGs into one .ico
(every image PNG-compressed: Windows Vista and later, rc.exe of VS2010+ / windres).  The .ico is committed so CI needs no SVG toolchain.
Run as `python3 -I assets/icon/build_ico.py`.  --check: render again and compare with the committed .ico (needs rsvg-convert)."""
import os, struct, subprocess, sys, tempfile

SIZES = (16, 24, 32, 48, 64, 128, 256)
H = os.path.dirname(os.path.abspath(__file__))


def build(svg):
    imgs = []
    with tempfile.TemporaryDirectory() as t:
        for n in SIZES:
            p = os.path.join(t, "%d.png" % n)
            subprocess.run(["rsvg-convert", "-w", str(n), "-h", str(n), "-o", p, svg], check=True)
            imgs.append(open(p, "rb").read())
    head = struct.pack("<HHH", 0, 1, len(imgs))
    off, ent, body = 6 + 16 * len(imgs), b"", b""
    for n, b in zip(SIZES, imgs):
        ent += struct.pack("<BBBBHHII", n % 256, n % 256, 0, 0, 1, 32, len(b), off + len(body))      # 256 is stored as 0
        body += b
    return head + ent + body


if __name__ == "__main__":
    a = [x for x in sys.argv[1:] if x != "--check"]
    svg, ico = (a + [H + "/mflng.svg", H + "/mflng.ico"][len(a):])[:2]
    new = build(svg)
    if "--check" in sys.argv:
        sys.exit(0 if new == open(ico, "rb").read() else "mflng.ico differs from a fresh render of the SVG (rsvg-convert versions may differ)")
    open(ico, "wb").write(new)
    print("wrote %s (%d bytes, sizes %s)" % (ico, len(new), SIZES))
