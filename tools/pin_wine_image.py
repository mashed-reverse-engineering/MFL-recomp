#!/usr/bin/env python3
"""pin_wine_image.py <ref> [--dry-run] | --check | --selftest   (engine switch window 5, A1)

<ref> = the pinned image, e.g. tianon/wine@sha256:<64 hex> or the local tag tianon/wine:mfl-w5.  Replaces the floating tag in every text file under tools/
(run.py twice, vc7.sh, wine-run/mkpfx.sh + Dockerfile, asset_oracle/offline.sh, the delink/rw_*_check.sh ...).  Idempotent.
--check lists the files that still name the floating tag (exit 1); check_all.sh runs it as a FAIL line.  Run by M in slot A, never by a lane."""
import os, re, sys

T = os.path.dirname(os.path.abspath(__file__))
if os.path.basename(os.path.dirname(T)) == "agents":
    T = os.path.join(os.path.dirname(os.path.dirname(T)), "tools")
FLOAT = "tianon/wine" + ":latest"      # (split: this file must not name it itself)
REF = re.compile(r"tianon/wine(@sha256:[0-9a-f]{64}|:[A-Za-z0-9_.-]+)")


def files(root):
    for dp, dn, fn in os.walk(root):
        dn[:] = [d for d in dn if d not in ("__pycache__", ".git")]
        for f in fn:
            p = os.path.join(dp, f)
            if os.path.isfile(p) and not os.path.islink(p) and os.path.abspath(p) != os.path.abspath(__file__):
                try:
                    if FLOAT in open(p, encoding="utf-8").read():
                        yield p
                except (UnicodeDecodeError, OSError):
                    pass


def pin(root, ref, dry=False):
    n = 0
    for p in files(root):
        s = open(p, encoding="utf-8").read()
        n += s.count(FLOAT)
        if not dry:
            open(p, "w", encoding="utf-8", newline="").write(s.replace(FLOAT, ref))
        print("%s %s (%d)" % ("would pin" if dry else "pinned", os.path.relpath(p, root), s.count(FLOAT)))
    return n


def selftest():
    import tempfile
    d = tempfile.mkdtemp(prefix="pinwine")
    a, b = os.path.join(d, "a.sh"), os.path.join(d, "b.py")
    open(a, "w").write("docker run %s wine x\n" % FLOAT); open(b, "w").write("x = '%s'; y = '%s'\n" % (FLOAT, FLOAT))
    ref = "tianon/wine@sha256:" + "ab" * 32
    assert pin(d, ref) == 3 and not list(files(d)) and ref in open(a).read() and open(b).read().count(ref) == 2
    assert pin(d, ref) == 0      # idempotent
    os.remove(a); os.remove(b); os.rmdir(d)
    print("pin_wine_image selftest: PASS")


if __name__ == "__main__":
    a = sys.argv[1:]
    if a == ["--selftest"]:
        selftest()
    elif a == ["--check"]:
        bad = [os.path.relpath(p, T) for p in files(T)]
        for p in bad:
            print("floating Wine image tag in tools/" + p)
        sys.exit(1 if bad else 0)
    elif a and REF.fullmatch(a[0]) and a[0] != FLOAT and a[1:] in ([], ["--dry-run"]):
        print("%d occurrences" % pin(T, a[0], a[1:] == ["--dry-run"]))
    else:
        sys.exit(__doc__)
