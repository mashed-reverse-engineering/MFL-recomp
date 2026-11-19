#!/usr/bin/env python3
"""link_tables.py: the committed inputs of the self-contained CC=v143 NOEXE link (tools/delink/link_ci.sh <work>, no ref dir), SK2 W16.
  link_tables.py emit  <link dir> <data/link dir>   write the tables from a NORMAL CC=v143 LIB_CRT=v143md link_release.sh dir of this HEAD
  link_tables.py check <link dir> <data/link dir>   FAIL (exit 1, lists the files) when the committed tables differ from that link (drift)
  link_tables.py selftest <link dir>                emit to a temp dir, check passes, a changed byte / a missing file FAILS (negative fixture)
Tables (text, numbers and names; all HEAD-specific like data/link/gen, so refreshed at each freeze from the normal v143 link):
  mfl_image_v143.txt  objtable.py emit of <link dir>/mfl.obj (the image object mkobj built from the exe; mkobj --table)
  imports_v143.txt    <link dir>/imp/*.def without mflrw.def (the exe's import list + argument bytes; mk_importlibs.py --table)
  crt_xcu.txt         the 12 __xc_a..__xc_z slot values of the exe (crt_xcu.py reads a .txt instead of the exe)
  gen_v143/           <link dir>/ds = dsform.py split of the v143md globgen output, without held.h (joined with the held rows zero)"""
import os, sys, tempfile
G = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
sys.path[:0] = [G + "/tools/delink", G + "/tools/msvcw"]
import objtable, crt_xcu


def tables(L):
    t = {"mfl_image_v143.txt": objtable.emit(L + "/mfl.obj"),
         "imports_v143.txt": "".join(open(L + "/imp/" + f).read() for f in sorted(os.listdir(L + "/imp")) if f.endswith(".def") and f != "mflrw.def"),
         "crt_xcu.txt": "# __xc_a..__xc_z slots of MFL.exe without zero / __security_init_cookie (tools/ci/link_tables.py)\n"
                        + "".join("%08x\n" % v for v in crt_xcu.table(L + "/mfl_patched.exe"))}
    for f in sorted(os.listdir(L + "/ds")):
        if f != "held.h": t["gen_v143/" + f] = open(L + "/ds/" + f).read()
    return t


def drift(t, D):
    bad = [k for k, v in t.items() if not os.path.exists(D + "/" + k) or open(D + "/" + k).read() != v]
    gd = D + "/gen_v143"
    return bad + sorted("gen_v143/" + f + " (stale)" for f in (os.listdir(gd) if os.path.isdir(gd) else []) if "gen_v143/" + f not in t)


def emit(t, D):
    os.makedirs(D + "/gen_v143", exist_ok=True)
    for k in drift(t, D):
        if k.endswith(" (stale)"): os.remove(D + "/" + k[:-8])      # exact file names only
    for k, v in t.items(): open(D + "/" + k, "w").write(v)
    print("link_tables: %d files -> %s (image table %d B)" % (len(t), D, len(t["mfl_image_v143.txt"])))


def check(t, D):
    bad = drift(t, D)
    for k in bad: print("DRIFT " + k)
    print("link_tables check %s: %s" % (D, "FAIL: %d files differ (refresh: link_tables.py emit <normal v143 link> data/link)" % len(bad) if bad else "SAME"))
    return 1 if bad else 0


def selftest(L):
    t = tables(L); D = tempfile.mkdtemp()
    emit(t, D); assert check(t, D) == 0, "fresh emit does not check"
    p = D + "/crt_xcu.txt"; s = open(p).read(); open(p, "w").write(s.replace("0", "1", 1))
    assert check(t, D) == 1, "a changed byte is not caught"
    emit(t, D); os.remove(D + "/" + next(k for k in t if k.startswith("gen_v143/")))
    assert check(t, D) == 1, "a missing file is not caught"
    open(D + "/gen_v143/zz_stale.inc", "w").write(""); emit(t, D)
    assert check(t, D) == 0 and not os.path.exists(D + "/gen_v143/zz_stale.inc"), "a stale file survives emit"
    print("link_tables selftest ok")


if __name__ == "__main__":
    a = sys.argv[1:]
    if a[:1] == ["emit"] and len(a) == 3: emit(tables(os.path.abspath(a[1])), os.path.abspath(a[2]))
    elif a[:1] == ["check"] and len(a) == 3: sys.exit(check(tables(os.path.abspath(a[1])), os.path.abspath(a[2])))
    elif a[:1] == ["selftest"] and len(a) == 2: selftest(os.path.abspath(a[1]))
    else: sys.exit(__doc__)
