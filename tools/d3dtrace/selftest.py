#!/usr/bin/env python3
"""selftest.py: normalise + diff on a synthetic raw trace (no game needed): full records, dynamic buffer ids dropped, ulp tolerance, mask."""
import os, subprocess, sys, tempfile
T = os.path.dirname(os.path.abspath(__file__))
raw = lambda x: """TRACE d3dtrace 1
VBDEF 0 1024 usage=208 fvf=0 pool=0
VBDEF 1 1024 usage=8 fvf=0 pool=1
TEXDEF 0 64 64 1 fmt=21 usage=0 pool=1
TEXH 0 00000000deadbeef
STATE
RS 7 1
RS 36 3f800000
TEX 0 0
XF 256 %s
SRC 0 %d 0 32
ENDSTATE
SCENE 5 0
D 0 DIP 4 0 0 4 0 2 vh=0000000000000001 ih=0000000000000002
PRESENT 0 phase=5 racef=0 draws=1
""" % (" ".join(x), 0)
def run(*a):
    return subprocess.run([sys.executable, *a], capture_output=True, text=True)
with tempfile.TemporaryDirectory() as d:
    one = ["1", "0", "0", "0", "0", "1", "0", "0", "0", "0", "1", "0", "0", "0", "0", "1"]
    two = one[:]; two[0] = "1.00000012"          # 1 ulp off
    three = one[:]; three[0] = "2"
    for n, m in (("a", one), ("b", two), ("c", three)):
        open(d + "/" + n + ".raw", "w").write(raw(m))
        assert run(T + "/normalise.py", d + "/" + n + ".raw", d + "/" + n + ".d3d").returncode == 0
    rec = open(d + "/a.d3d").read()
    assert "rs.36=~1" in rec and "src.0=0|32|#dyn" in rec and "tex.0=00000000deadbeef|21|64x64|1|#0" in rec, rec
    assert run(T + "/diff.py", d + "/a.d3d", d + "/a.d3d").returncode == 0
    assert run(T + "/diff.py", d + "/a.d3d", d + "/b.d3d").returncode == 1
    r = run(T + "/diff.py", d + "/a.d3d", d + "/b.d3d", "--ulp", "1"); assert r.returncode == 0, r.stdout + r.stderr
    assert run(T + "/diff.py", d + "/a.d3d", d + "/c.d3d", "--ulp", "4").returncode == 1
    assert run(T + "/diff.py", d + "/a.d3d", d + "/c.d3d", "--ulp", "4", "--mask", d + "/c.d3d").returncode == 0   # noise mask: A vs mask differ at xf.256, so that key is ignored
print("selftest ok")
