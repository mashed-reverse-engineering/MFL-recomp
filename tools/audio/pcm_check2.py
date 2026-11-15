#!/usr/bin/env python3
"""pcm_check2.py <audio.pcm> [--scenario-seconds S] [--wav OUT.wav] [--csv OUT.csv] [--rate 48000] [--channels 2] [--selftest]
s16le dump of MFL_AUDIO_DUMP (every frame the release exe hands to the audio device).  = agents/b1_w5/pcm_check.py (sample count,
seconds, max |x|, share non-zero) plus three numbers (plan window 6, 2.6 item 3):
  cover    dump seconds / scenario seconds (--scenario-seconds: wall time from "title screen reached" to the last script step, as
           relrun.py / tools/wine-run/release_v7.sh pass it; the device opens shortly before the title, after the intro movie; without it the number is not judged)
  silence  longest silent stretch after the onset, in seconds (silent = every sample of a 50 ms block below SIL_ABS)
  dropout  share of silent 50 ms blocks after the onset
  lead     seconds before the onset (onset = first ONSET_S of uninterrupted sound); lead / dump seconds is judged
  clip     share of samples at +-32767 / -32768
V7A (W9): the frontend before the menu music is silent apart from UI blips (title screen waits for input), and that phase grows with
host load (4.0 .. 12.9 s in 6 good runs); the old "longest silence after the first sound" measured exactly it.  After the onset the
6 good runs have 0 silent blocks.  So: skip the lead, judge silence + dropout share after it, cap the lead as a share of the dump.
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
SILENCE_MAX_S = 3.0   # longest silent stretch after the onset (6 good runs: 0.00)
DROPOUT_MAX = 0.05    # share of silent blocks after the onset (6 good runs: 0.000)
LEAD_MAX = 0.6        # lead / dump seconds (6 good runs: 0.23 .. 0.38)
ONSET_S = 1.0         # uninterrupted sound that marks the onset (UI blips before it are shorter)
CLIP_MAX = 0.001      # share of samples at full scale
SIL_ABS = 8           # |x| below this is silence
BLOCK_S = 0.05

def analyse(a, rate, ch):
    n = len(a); mx = max((abs(x) for x in a), default=0); nz = sum(1 for x in a if x) / n if n else 0.0
    clip = sum(1 for x in a if x >= 32767 or x <= -32767) / n if n else 0.0
    blk = max(1, int(rate * BLOCK_S)) * ch; first = None; run = longest = 0
    loud = [max(a[i:i + blk], default=0) >= SIL_ABS or -min(a[i:i + blk], default=0) >= SIL_ABS for i in range(0, n, blk)]
    if True in loud: first = loud.index(True) * blk
    k = round(ONSET_S / BLOCK_S)
    onset = next((j for j in range(len(loud) - k + 1) if all(loud[j:j + k])), None)
    rest = loud[onset:] if onset is not None else []
    for l in rest:
        run = 0 if l else run + 1; longest = max(longest, run)
    per = []
    for i in range(0, n, rate * ch):
        c = a[i:i + rate * ch]; per.append(((sum(x * x for x in c) / len(c)) ** 0.5, max(abs(x) for x in c)))
    return {"samples": n, "seconds": n / ch / rate, "max": mx, "nonzero": nz, "clip": clip, "silence": longest * BLOCK_S,
            "first_sound": None if first is None else first / ch / rate, "per": per,
            "lead": None if onset is None else onset * BLOCK_S, "dropout": rest.count(False) / len(rest) if rest else 1.0}

def verdict(r, scen):
    bad = []
    if not (r["max"] > 0 and r["seconds"] >= 1): bad.append("old pass (max>0 over >=1 s)")
    if scen:
        c = r["seconds"] / scen
        if not COVER_MIN <= c <= COVER_MAX: bad.append("cover %.2f outside %.2f..%.2f" % (c, COVER_MIN, COVER_MAX))
    if r["lead"] is None: bad.append("no %.1f s of uninterrupted sound" % ONSET_S)
    elif r["lead"] > LEAD_MAX * r["seconds"]: bad.append("lead %.1f s > %.2f of %.1f s" % (r["lead"], LEAD_MAX, r["seconds"]))
    if r["silence"] > SILENCE_MAX_S: bad.append("silence %.1f s > %.1f" % (r["silence"], SILENCE_MAX_S))
    if r["dropout"] > DROPOUT_MAX: bad.append("dropout %.3f > %.3f" % (r["dropout"], DROPOUT_MAX))
    if r["clip"] > CLIP_MAX: bad.append("clip %.5f > %.5f" % (r["clip"], CLIP_MAX))
    return bad

def opt(k, d=None): return sys.argv[sys.argv.index(k) + 1] if k in sys.argv else d

def selftest():
    import math
    rate, ch = 48000, 2
    tone = array("h", [int(8000 * math.sin(i * 0.05)) for i in range(rate * ch * 4)])
    gap = array("h", tone[:rate * ch]) + array("h", [0] * (rate * ch * 40)) + array("h", tone[:rate * ch])
    clipd = array("h", [32767 if i % 3 == 0 else x for i, x in enumerate(tone)])
    z = lambda s: array("h", [0] * int(rate * ch * s))
    blip = array("h", tone[:rate * ch // 10])
    long_ = tone * 5                                  # 20 s
    cases = [("tone", tone, 4.0, 0), ("all zero", z(4), None, 1), ("40 s hole", gap, None, 1),
             ("clipped", clipd, None, 1), ("too short for the run", tone, 60.0, 1), ("too long for the run", tone, 2.0, 1),
             ("silent title, blips, music", blip + z(13) + blip + z(2) + long_, None, 0),
             ("blips only", (blip + z(2)) * 10, None, 1), ("lead too long", blip + z(40) + long_, None, 1),
             ("muted after 10 s", long_[:rate * ch * 10] + z(10), None, 1),
             ("50% dropouts", mute_half(long_, rate * ch), None, 1),
             ("25% dropouts 1.5 s on/0.5 s off", mute_half(long_, rate * ch, 1.5, 0.5), None, 1)]
    for p in sys.argv[sys.argv.index("--selftest") + 1:]:   # real dumps: must PASS, broken copies must FAIL
        d = load(p); s = len(d) // (rate * ch); cases += [(p[-40:], d, None, 0),
            (p[-28:] + " muted after onset+3 s", muted_after(d, rate, ch), None, 1), (p[-28:] + " 50% dropouts", mute_half(d, rate * ch), None, 1)]
    ok = True
    for name, a, scen, want in cases:
        r = analyse(a, rate, ch); bad = verdict(r, scen); got = 1 if bad else 0
        print("selftest %-50s %d (want %d) %s  %s" % (name, got, want, "ok" if got == want else "WRONG", "; ".join(bad))); ok &= got == want
    sys.exit(0 if ok else 1)

def mute_half(a, sec, on=0.25, off=0.25):            # default: zero every other 0.25 s
    q, r = int(sec * on), int(sec * off); b = array("h", a)
    for i in range(q, len(b), q + r): b[i:i + r] = array("h", [0] * len(b[i:i + r]))
    return b

def muted_after(a, rate, ch):                         # zero everything from 3 s after the onset
    cut = int((analyse(a, rate, ch)["lead"] + 3) * rate) * ch; return a[:cut] + array("h", [0] * (len(a) - cut))

def load(p):
    raw = open(p, "rb").read(); a = array("h"); a.frombytes(raw[:len(raw) // 2 * 2])
    if sys.byteorder == "big": a.byteswap()
    return a

def main():
    if "--selftest" in sys.argv: selftest()
    rate, ch = int(opt("--rate", 48000)), int(opt("--channels", 2)); scen = float(opt("--scenario-seconds", 0)) or None
    a = load(sys.argv[1]); r = analyse(a, rate, ch); bad = verdict(r, scen)
    if opt("--csv"):
        open(opt("--csv"), "w").write("second,rms,peak\n" + "".join("%d,%.1f,%d\n" % (i, e, p) for i, (e, p) in enumerate(r["per"])))
    if opt("--wav") and r["samples"]:
        with wave.open(opt("--wav"), "wb") as w: w.setnchannels(ch); w.setsampwidth(2); w.setframerate(rate); w.writeframes(open(sys.argv[1], "rb").read()[:r["samples"] * 2])
    print("pcm_check2: samples=%d seconds=%.2f max=%d nonzero=%.3f first_sound=%s lead=%s cover=%s silence=%.2f dropout=%.3f clip=%.6f %s" % (
        r["samples"], r["seconds"], r["max"], r["nonzero"], "-" if r["first_sound"] is None else "%.2f" % r["first_sound"],
        "-" if r["lead"] is None else "%.2f" % r["lead"],
        "%.3f" % (r["seconds"] / scen) if scen else "-", r["silence"], r["dropout"], r["clip"], "PASS" if not bad else "FAIL (%s)" % "; ".join(bad)))
    sys.exit(1 if bad else 0)

if __name__ == "__main__":
    main()
