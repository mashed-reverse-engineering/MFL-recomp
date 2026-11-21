#!/usr/bin/env python3
"""gate_inherit.py: output-keyed inheritance of the Wine steps of `gate.sh full` (W16 GATE, user decision B 2026-11-20).

  step <gate dir> <step id> <parts>   parts = link,tools[,release].  Computes the missing parts of <gate dir>/inherit_key.txt (always, so that this
        gate can be a donor later), then prints ONE line:
          INHERITED from <donor dir> (key <hash>)             exit 0: gate.sh skips the step (only with GATE_INHERIT=1)
          WOULD INHERIT from <donor dir> (key <hash>; ...)    exit 1: shadow mode (the default): the step runs
          inherit: run (<why>)                                exit 1
        donor = the NEWEST sibling gate dir whose gate.log says "GATE full PASSED" and whose inherit_ran.txt lists <step id> (gate.sh writes the id
        only after the step really ran and passed: no inheritance chains); not older than 7 days (gate.log mtime); its key parts must equal ours.
        GATE_INHERIT=1 only takes effect after 5 clean shadow gates (passed full gates whose inherit_ran.txt lists all 7 step ids: sweep15 minisweep state render scen
        training v7); before that the line says WOULD INHERIT (... only n of 5 clean shadow gates so far) and the step runs.
        Never at the freeze (GATE_FREEZE=1); GATE_NOINHERIT=1 forces the real run.
  --selftest
Key parts (sha256 each):
  link     <gate>/link: ours.exe with the PE TimeDateStamp zeroed (linkrepro.exe_masked) + ours.map without its timestamp line + <gate>/delta.txt
           + every file of link/game/App_Executables (mflrw.dll ...; .exe masked the same way, .map likewise; run artefacts skipped)
  release  the same over <gate>/v7/rel (+ v7/rel/delta.txt)
  tools    tools/wine-run/ and tools/d3dtrace/ (whole dirs), tools/gate.sh, the tools/delink files the steps run (+ the delink files they name),
           tools/audio/pcm_check2.py, the control build (build_overlay.py, overlay.py, vc7.sh, src/harness/, output/scenario/_build/ctrl.exe),
           the `state` rows of output/zero_original_baseline.txt, the game install (path + size; game/MFL.exe content), the Wine image references
           named in those scripts, and the value of every environment variable those scripts name (gate-control variables excluded: VOLATILE)."""
import glob, hashlib, os, re, shutil, struct, sys, tempfile, time

T = os.path.dirname(os.path.abspath(__file__)); G = os.path.dirname(T)
sys.path.insert(0, os.path.join(T, "delink"))
from linkrepro import exe_masked, map_masked      # one owner of the link normalisation (W11 B1)

GAME = os.path.normpath(os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed"))
DELINK = ["sweep.sh", "race_relinked.sh", "colorcheck.py", "sa_oracles.sh", "state_compare.sh"]
RUNFILES = {"mashed.log", "gamesave.bin", "mfl_trap.log"}      # written by a game run in the dir
VOLATILE = {"D", "LPID", "CHILD", "GATE_DIR", "GATE_INHERIT", "GATE_NOINHERIT", "GATE_FREEZE", "FORCE_NEGTEST", "FORCE_STRICT", "PWD", "OLDPWD", "SHLVL"}
MAXAGE = 7 * 86400


def rd(p):
    with open(p, "rb") as f:
        return f.read()


def add(h, label, data):
    h.update(("%s\0%d\0" % (label, len(data))).encode() + data)


def norm(p):
    """file bytes with the link timestamps masked (.exe header + debug copies, .map 'Timestamp is' line)"""
    if p.lower().endswith(".exe"):
        try:
            return exe_masked(p)
        except (struct.error, IndexError):      # not a PE image: as is
            return rd(p)
    if p.lower().endswith(".map"):
        return "\n".join(map_masked(p)).encode()
    return rd(p)


def files_key(L, delta):
    h = hashlib.sha256()
    for f in ("ours.exe", "ours.map"):
        add(h, f, norm(os.path.join(L, f)))
    add(h, "delta", rd(delta).strip())
    A = os.path.join(L, "game", "App_Executables")
    for f in sorted(os.listdir(A)):
        p = os.path.join(A, f)
        if f not in RUNFILES and os.path.isfile(p):
            add(h, "app/" + f, norm(p))
    return h.hexdigest()


def tree(root):
    for dp, dn, fn in os.walk(root):
        dn[:] = sorted(d for d in dn if d not in ("__pycache__", "work"))
        for f in sorted(fn):
            yield os.path.join(dp, f)


def part_tools(env=os.environ):
    fs = list(tree(os.path.join(T, "wine-run"))) + list(tree(os.path.join(T, "d3dtrace"))) + list(tree(os.path.join(G, "src", "harness")))
    dl = [os.path.join(T, "delink", x) for x in DELINK]
    for p in list(dl):      # the delink files these scripts name ($T/x, python3 x.py ...): derived, not a fixed subset
        for m in re.findall(r"\b[A-Za-z0-9_]+\.(?:sh|py|pl|txt)\b", rd(p).decode("latin-1")):
            q = os.path.join(T, "delink", m)
            if os.path.isfile(q) and q not in dl:
                dl.append(q)
    fs += dl + [os.path.join(T, x) for x in ("gate.sh", "audio/pcm_check2.py", "build_overlay.py", "overlay.py", "vc7.sh")]
    fs.append(os.path.join(G, "output", "scenario", "_build", "ctrl.exe"))
    h = hashlib.sha256(); txt = []
    for p in fs:
        b = rd(p) if os.path.isfile(p) else b"<missing>"
        add(h, os.path.relpath(p, G), b)
        if p.endswith((".sh", ".py", ".pl")):
            txt.append(b.decode("latin-1"))
    bf = os.path.join(G, "output", "zero_original_baseline.txt")
    add(h, "baseline state rows", b"".join(l for l in open(bf, "rb") if l.startswith(b"state ")) if os.path.exists(bf) else b"<missing>")
    for p in tree(GAME):
        add(h, "game/" + os.path.relpath(p, GAME), str(os.path.getsize(p)).encode())
    add(h, "game/MFL.exe", rd(os.path.join(G, "game", "MFL.exe")))
    txt = "\n".join(txt)
    add(h, "image", "\n".join(sorted(set(re.findall(r"tianon/wine(?:@sha256:[0-9a-f]{64}|:[A-Za-z0-9_.-]+)", txt)))).encode())
    names = sorted(set(re.findall(r"\$\{?([A-Z][A-Z0-9_]*)", txt)) - VOLATILE)
    add(h, "env", "\n".join("%s=%s" % (n, env.get(n)) for n in names).encode())
    return h.hexdigest()


PARTS = {"link": lambda gd: files_key(os.path.join(gd, "link"), os.path.join(gd, "delta.txt")),
         "release": lambda gd: files_key(os.path.join(gd, "v7", "rel"), os.path.join(gd, "v7", "rel", "delta.txt")),
         "tools": lambda gd: part_tools()}


def keys(gd):
    try:
        return dict(l.split()[:2] for l in open(os.path.join(gd, "inherit_key.txt")) if len(l.split()) >= 2)
    except OSError:
        return {}


def ran(gd):
    try:
        return set(open(os.path.join(gd, "inherit_ran.txt")).read().split())
    except OSError:
        return set()


def passed(gd):
    try:
        return re.search(r"^GATE full PASSED", open(os.path.join(gd, "gate.log"), errors="replace").read(), re.M) is not None
    except OSError:
        return False


IDS = {"sweep15", "minisweep", "state", "render", "scen", "training", "v7"}
MINCLEAN = 5


def clean(gd):
    """a clean shadow gate: a passed full gate that keyed and really ran every inheritable step (all 7 ids in inherit_ran.txt = made by this code)"""
    return passed(gd) and IDS <= ran(gd)


def step(gd, sid, parts, env=os.environ, now=None, parts_fn=PARTS, minclean=MINCLEAN):
    """-> (exit code, line)"""
    gd = os.path.normpath(os.path.abspath(gd)); have = keys(gd)
    for p in parts:
        if p not in have:
            try:
                have[p] = parts_fn[p](gd)
            except Exception as e:      # fail-safe: the step runs
                return 1, "inherit: run (key part %s not computable: %s: %s)" % (p, type(e).__name__, e)
            with open(os.path.join(gd, "inherit_key.txt"), "a") as f:
                f.write("%s %s\n" % (p, have[p]))
    key = hashlib.sha256("|".join(have[p] for p in parts).encode()).hexdigest()[:16]
    if env.get("GATE_FREEZE") == "1":
        return 1, "inherit: run (freeze gate, GATE_FREEZE=1; key %s)" % key
    if env.get("GATE_NOINHERIT") == "1":
        return 1, "inherit: run (GATE_NOINHERIT=1; key %s)" % key
    donors = []
    for lg in glob.glob(os.path.join(os.path.dirname(gd), "*", "gate.log")):
        dd = os.path.dirname(lg)
        if os.path.realpath(dd) != os.path.realpath(gd) and sid in ran(dd) and passed(dd):
            donors.append((os.path.getmtime(lg), dd))
    if not donors:
        return 1, "inherit: run (no passed full gate that really ran step %s; key %s)" % (sid, key)
    m, dd = max(donors)
    age = ((now or time.time()) - m) / 86400
    if age * 86400 > MAXAGE:
        return 1, "inherit: run (newest donor %s is %.1f days old, > 7; key %s)" % (dd, age, key)
    dk = keys(dd); diff = [p for p in parts if dk.get(p) != have[p]]
    if diff:
        return 1, "inherit: run (key part %s differs from %s; key %s)" % ("+".join(diff), dd, key)
    if env.get("GATE_INHERIT") == "1":
        n = sum(clean(os.path.dirname(lg)) for lg in glob.glob(os.path.join(os.path.dirname(gd), "*", "gate.log")) if os.path.realpath(os.path.dirname(lg)) != os.path.realpath(gd))
        if n < minclean:      # W17 GATEFAST: shadow mode must have run clean for 5 full gates before a step may be skipped
            return 1, "WOULD INHERIT from %s (key %s; GATE_INHERIT=1 but only %d of %d clean shadow gates so far: the step runs)" % (dd, key, n, minclean)
        return 0, "INHERITED from %s (key %s)" % (dd, key)
    return 1, "WOULD INHERIT from %s (key %s; shadow mode: the step runs, GATE_INHERIT=1 enables)" % (dd, key)


def selftest():
    w = tempfile.mkdtemp(); bad = 0
    def pe(ts, code):
        b = bytearray(0x200); b[0:2] = b"MZ"; struct.pack_into("<I", b, 0x3c, 0x80); b[0x80:0x84] = b"PE\0\0"; struct.pack_into("<I", b, 0x88, ts); b[0x100] = code
        return bytes(b)
    def mk(name, ts=1, code=0x90, dll=b"dll", ran_ids="sweep15 v7", ok=True, age=0):
        gd = os.path.join(w, name); A = os.path.join(gd, "link", "game", "App_Executables"); os.makedirs(A)
        open(os.path.join(gd, "link", "ours.exe"), "wb").write(pe(ts, code)); open(os.path.join(A, "MFL.exe"), "wb").write(pe(ts, code))
        open(os.path.join(gd, "link", "ours.map"), "w").write(" ours\n Timestamp is %08x (x)\n  0001:00000000 _f\n" % ts)
        open(os.path.join(A, "mflrw.dll"), "wb").write(dll); open(os.path.join(A, "mashed.log"), "w").write("run %d" % ts)
        open(os.path.join(gd, "delta.txt"), "w").write("18e000\n")
        if ran_ids is not None:
            open(os.path.join(gd, "inherit_ran.txt"), "w").write(ran_ids.replace(" ", "\n") + "\n")
            open(os.path.join(gd, "gate.log"), "w").write("x\nGATE full %s (14 steps)\n" % ("PASSED" if ok else "FAILED"))
            t = time.time() - age * 86400; os.utime(os.path.join(gd, "gate.log"), (t, t))
        return gd
    fn = dict(PARTS, tools=lambda gd: "t" * 64)      # the tools part is the live tree in both gates; the cases vary the link part
    def case2(what, prefix, donor_kw, ours_kw=None, env=None, rc_want=1, mc=0, extra=0):      # prefix: the line carries the temp donor path; mc = clean gates needed; extra = further clean gates beside the donor
        nonlocal bad
        sub = tempfile.mkdtemp(dir=w)
        if donor_kw is not None:      # the donor's own key, written during its own run
            d = mk(os.path.join(sub, "donor"), **donor_kw); step(d, "sweep15", ["link", "tools"], env={}, parts_fn=fn)
        ok_ = dict(ours_kw or {}); nolink = ok_.pop("nolink", False)
        for k in range(extra): mk(os.path.join(sub, "more%d" % k), ran_ids=" ".join(sorted(IDS)), age=0.01)      # older than the donor: the donor stays the newest
        o = mk(os.path.join(sub, "ours"), ran_ids=None, **ok_)
        if nolink: os.remove(os.path.join(o, "link", "ours.exe"))
        rc, line = step(o, "sweep15", ["link", "tools"], env=env if env is not None else {"GATE_INHERIT": "1"}, parts_fn=fn, minclean=mc)
        ok = line.startswith(prefix) and rc == rc_want
        print("%-4s %-52s -> %s" % ("ok" if ok else "FAIL", what, line[:140])); bad += not ok
    case2("equal link (only timestamps / run log differ), enabled", "INHERITED from", {"ts": 1}, {"ts": 2}, rc_want=0)
    case2("equal link, default env = shadow mode", "WOULD INHERIT from", {"ts": 1}, {"ts": 2}, env={})
    all7 = " ".join(sorted(IDS))
    case2("enabled but only 1 clean gate of 5 (the donor)", "WOULD INHERIT from", {"ts": 1, "ran_ids": all7}, {"ts": 2}, mc=5)
    case2("enabled, 4 clean gates of 5", "WOULD INHERIT from", {"ts": 1, "ran_ids": all7}, {"ts": 2}, mc=5, extra=3)
    case2("enabled, 5 clean gates", "INHERITED from", {"ts": 1, "ran_ids": all7}, {"ts": 2}, mc=5, extra=4, rc_want=0)
    case2("one code byte differs", "inherit: run (key part link differs", {"code": 0x90}, {"code": 0xcc})
    case2("mflrw.dll differs", "inherit: run (key part link differs", {"dll": b"a"}, {"dll": b"b"})
    case2("donor gate FAILED", "inherit: run (no passed full gate", {"ok": False})
    case2("donor inherited the step (not in inherit_ran)", "inherit: run (no passed full gate", {"ran_ids": "v7"})
    case2("donor 8 days old", "inherit: run (newest donor", {"age": 8})
    case2("no donor at all", "inherit: run (no passed full gate", None)
    case2("freeze gate", "inherit: run (freeze gate", {}, env={"GATE_INHERIT": "1", "GATE_FREEZE": "1"})
    case2("GATE_NOINHERIT=1", "inherit: run (GATE_NOINHERIT=1", {}, env={"GATE_INHERIT": "1", "GATE_NOINHERIT": "1"})
    case2("ours has no link (part not computable)", "inherit: run (key part link not computable", {}, {"nolink": True})
    # the live tools part: stable between two calls, and an env knob the scripts name changes it
    a, b = part_tools({}), part_tools({})
    c = part_tools({"GAME_SLOTS": "3"}); v = part_tools({"GATE_INHERIT": "1", "LPID": "123"})
    for what, ok in (("tools part stable", a == b), ("env knob GAME_SLOTS changes the tools part", a != c), ("gate-control env (GATE_INHERIT, LPID) does not", a == v)):
        print("%-4s %s" % ("ok" if ok else "FAIL", what)); bad += not ok
    shutil.rmtree(w)
    print("gate_inherit selftest: %s" % ("PASS" if not bad else "FAIL (%d)" % bad))
    return 1 if bad else 0


if __name__ == "__main__":
    a = sys.argv[1:]
    if a == ["--selftest"]:
        sys.exit(selftest())
    if len(a) == 4 and a[0] == "step" and set(a[3].split(",")) <= set(PARTS):
        rc, line = step(a[1], a[2], a[3].split(","))
        print(line)
        sys.exit(rc)
    sys.exit(__doc__)
