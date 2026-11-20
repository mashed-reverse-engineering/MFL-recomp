#!/usr/bin/env python3
"""v143mt_dups.py <link work dir>   |   --check <link work dir>   |   --selftest
W16 MT (CC=v143 LIB_CRT=v143mt only; tools/delink/link_ours.sh runs it after the first link, then links again).
With the static CRT, libucrt.lib / libvcruntime.lib members are real objects, and some define a name that our objects define as well: the
VC7 CRT data of the generated data (gen/data.obj: _iob, __pioinfo, _nhandle, __lc_time_c, _mbctype ...) or an image label.  /force:multiple
keeps OUR definition (first on the line) and the static CRT's own code then works on VC7 data of another layout (LNK4006 "second definition
ignored" in link.log).  Under /MD the DLL CRT keeps its own data and only an import stub loses; this step gives the static CRT the same:
every such name is renamed <name>_vc7 in each of our objects that defines or references it (objcopy --redefine-sym), so our code and the
image keep binding to the VC7 copy and the CRT to its own.  libcmt.lib collisions (start-up members: __xp_a, __security_cookie) are the same
members msvcrt.lib has under /MD and stay as they are.
Objects: the link.rsp lines, layers/*.obj, mfl.obj, gen/*.obj.  Prints the count; writes <dir>/v143/mt_renamed.txt."""
import os, re, subprocess, sys

LIBS = ("libucrt.lib", "libvcruntime.lib")
PAT = re.compile(r"^(\S+?)\((\S+)\) : warning LNK4006: (\S+) already defined in (\S+); second definition ignored")


def names(log_lines):
    """-> sorted names whose ignored second definition is a libucrt / libvcruntime member"""
    return sorted({m.group(3) for m in map(PAT.match, log_lines) if m and re.split(r"[\\/]", m.group(1))[-1].lower() in LIBS})


def objects(w):
    objs = [l.strip() for l in open(os.path.join(w, "link.rsp")) if l.strip()]
    for d in ("layers", "gen"):
        if os.path.isdir(os.path.join(w, d)):
            objs += sorted(d + "/" + f for f in os.listdir(os.path.join(w, d)) if f.endswith(".obj"))
    if os.path.exists(os.path.join(w, "mfl.obj")): objs.append("mfl.obj")
    return objs


def main(w):
    ns = names(open(os.path.join(w, "link.log"), errors="replace").read().splitlines())
    want, hit = set(ns), set()
    objs = objects(w)
    for i in range(0, len(objs), 300):      # one nm per 300 objects (msys2: a process start is expensive)
        out = subprocess.run(["nm", "-A"] + objs[i:i + 300], cwd=w, capture_output=True, text=True, errors="replace").stdout
        for l in out.splitlines():
            p = l.rsplit(" ", 1)
            if len(p) == 2 and p[1] in want: hit.add(l.split(":")[0])
    for o in sorted(hit):
        subprocess.run(["objcopy"] + ["--redefine-sym=%s=%s_vc7" % (n, n) for n in ns] + [o], cwd=w, check=True)
    os.makedirs(os.path.join(w, "v143"), exist_ok=True)
    open(os.path.join(w, "v143", "mt_renamed.txt"), "w").write("".join(n + "\n" for n in ns))
    print("v143mt_dups: %d names the static CRT defines as well renamed _vc7 in %d of our objects" % (len(ns), len(hit)))


def selftest():
    log = ["libucrt.lib(_file.obj) : warning LNK4006: __iob already defined in data.obj; second definition ignored",
           "libvcruntime.lib(exsup.obj) : warning LNK4006: __NLG_Destination already defined in data.obj; second definition ignored",
           "libcmt.lib(gs_cookie.obj) : warning LNK4006: ___security_cookie already defined in data.obj; second definition ignored",
           "C:\\x\\libucrt.lib(ldexp.obj) : warning LNK4006: __d_inf already defined in data.obj; second definition ignored",
           "zlib__png.obj : warning LNK4006: _png_IHDR already defined in rdata.obj; second definition ignored"]
    assert names(log) == ["__NLG_Destination", "__d_inf", "__iob"], names(log)
    print("v143mt_dups selftest: ok")


def check(w):
    """after the second link: no static CRT definition may lose any more"""
    ns = names(open(os.path.join(w, "link.log"), errors="replace").read().splitlines())
    if ns: sys.exit("v143mt_dups --check: still %d static CRT definitions ignored: %s" % (len(ns), " ".join(ns[:12])))
    print("v143mt_dups --check: no static CRT definition ignored")


if __name__ == "__main__":
    a = sys.argv[1:]
    selftest() if a == ["--selftest"] else check(a[1]) if a[:1] == ["--check"] else main(a[0])
