#!/usr/bin/env python3
"""pcm_check2.py <audio.pcm> [--scenario-seconds S] [--wav OUT.wav] [--csv OUT.csv] [--rate 48000] [--channels 2] [--selftest]
s16le dump of MFL_AUDIO_DUMP (every frame the release exe hands to the audio device).  = agents/b1_w5/pcm_check.py (sample count,
seconds, max |x|, share non-zero) plus three numbers (plan window 6, 2.6 item 3):
  cover    dump seconds / scenario seconds (--scenario-seconds: wall time from "title screen reached" to the last script step, as
           relrun.py / tools/wine-run/release_v7.sh pass it; the device opens shortly before the title, after the intro movie; without it the number is not judged)
  silence  longest silent stretch after the first sound, in seconds (silent = every sample of a 50 ms block below SIL_ABS)
  clip     share of samples at +-32767 / -32768
Verdict PASS needs: the old pass (max |x| > 0 over >= 1 s) and the three thresholds below.  --csv: per second rms and peak;
--wav: the dump as a WAV file.  Exit 0 PASS, 1 FAIL.  Stdlib only.  Copied from agents/pc_w6/wpl/ (lane PC, W6) for V7 of gate.sh full.
RATE: the dump is at the rate the game opens the device with, 48000 Hz (audio_cmp2.py against the original's 48 kHz mix: spectrum
scale 1.000 at 48000, 0.917 when read as 44100).  The 44100 of agents/b1_w5/pcm_check.py and of the BUILDING text is wrong.
THRESHOLDS: set from the first dump of lane PC (2026-11-12, release link of acc1af09, title -> Quick Battle -> 20 s drive):
39.92 s of audio over 39.1 s from the title to the end (cover 1.02), longest silence 1.7 s, clip 0, max 21709."""
import struct, sys, wave
from array import array

COVER_MIN = 0.85      # dump seconds / scenario seconds
COVER_MAX = 1.25      # more audio than wall time = the mixer runs too fast (the 9039-vs-141 periods failure of 2026-11-05)
SILENCE_MAX_S = 8.0   # longest silent stretch after the first sound
CLIP_MAX = 0.001      # share of samples at full scale
SIL_ABS = 8           # |x| below this is silence
BLOCK_S = 0.05

def analyse(a, rate, ch):
    n = len(a); mx = max((abs(x) for x in a), default=0); nz = sum(1 for x in a if x) / n if n else 0.0
    clip = sum(1 for x in a if x >= 32767 or x <= -32767) / n if n else 0.0
    blk = max(1, int(rate * BLOCK_S)) * ch; first = None; run = longest = 0
    for i in range(0, n, blk):
        loud = max((abs(x) for x in a[i:i + blk]), default=0) >= SIL_ABS
        if loud:
            if first is None: first = i
            run = 0
        elif first is not None:
            run += 1; longest = max(longest, run)
    per = []
    for i in range(0, n, rate * ch):
        c = a[i:i + rate * ch]; per.append(((sum(x * x for x in c) / len(c)) ** 0.5, max(abs(x) for x in c)))
    return {"samples": n, "seconds": n / ch / rate, "max": mx, "nonzero": nz, "clip": clip, "silence": longest * BLOCK_S,
            "first_sound": None if first is None else first / ch / rate, "per": per}

def verdict(r, scen):
    bad = []
    if not (r["max"] > 0 and r["seconds"] >= 1): bad.append("old pass (max>0 over >=1 s)")
    if scen:
        c = r["seconds"] / scen
        if not COVER_MIN <= c <= COVER_MAX: bad.append("cover %.2f outside %.2f..%.2f" % (c, COVER_MIN, COVER_MAX))
    if r["silence"] > SILENCE_MAX_S: bad.append("silence %.1f s > %.1f" % (r["silence"], SILENCE_MAX_S))
    if r["clip"] > CLIP_MAX: bad.append("clip %.5f > %.5f" % (r["clip"], CLIP_MAX))
    return bad

def opt(k, d=None): return sys.argv[sys.argv.index(k) + 1] if k in sys.argv else d

def selftest():
    import math
    rate, ch = 48000, 2
    tone = array("h", [int(8000 * math.sin(i * 0.05)) for i in range(rate * ch * 4)])
    gap = array("h", tone[:rate * ch]) + array("h", [0] * (rate * ch * 40)) + array("h", tone[:rate * ch])
    clipd = array("h", [32767 if i % 3 == 0 else x for i, x in enumerate(tone)])
    ok = True
    for name, a, scen, want in (("tone", tone, 4.0, 0), ("all zero", array("h", [0] * (rate * ch * 4)), None, 1), ("40 s hole", gap, None, 1),
                                ("clipped", clipd, None, 1), ("too short for the run", tone, 60.0, 1), ("too long for the run", tone, 2.0, 1)):
        got = 1 if verdict(analyse(a, rate, ch), scen) else 0
        print("selftest %-22s %d (want %d) %s" % (name, got, want, "ok" if got == want else "WRONG")); ok &= got == want
    sys.exit(0 if ok else 1)

def main():
    if "--selftest" in sys.argv: selftest()
    rate, ch = int(opt("--rate", 48000)), int(opt("--channels", 2)); scen = float(opt("--scenario-seconds", 0)) or None
    raw = open(sys.argv[1], "rb").read(); raw = raw[:len(raw) // 2 * 2]
    a = array("h"); a.frombytes(raw)
    if sys.byteorder == "big": a.byteswap()
    r = analyse(a, rate, ch); bad = verdict(r, scen)
    if opt("--csv"):
        open(opt("--csv"), "w").write("second,rms,peak\n" + "".join("%d,%.1f,%d\n" % (i, e, p) for i, (e, p) in enumerate(r["per"])))
    if opt("--wav") and r["samples"]:
        with wave.open(opt("--wav"), "wb") as w: w.setnchannels(ch); w.setsampwidth(2); w.setframerate(rate); w.writeframes(raw)
    print("pcm_check2: samples=%d seconds=%.2f max=%d nonzero=%.3f first_sound=%s cover=%s silence=%.2f clip=%.6f %s" % (
        r["samples"], r["seconds"], r["max"], r["nonzero"], "-" if r["first_sound"] is None else "%.2f" % r["first_sound"],
        "%.3f" % (r["seconds"] / scen) if scen else "-", r["silence"], r["clip"], "PASS" if not bad else "FAIL (%s)" % "; ".join(bad)))
    sys.exit(1 if bad else 0)

if __name__ == "__main__":
    main()
