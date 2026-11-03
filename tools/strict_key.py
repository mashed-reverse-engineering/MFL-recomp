#!/usr/bin/env python3
"""strict_key.py [flag strings...]: sha1 over every input of check_all.sh's strict builds (src/**, build_overlay.py, overlay.py, vc7.sh, MFL.exe,
output/fnsizes.txt + cluster_pilot*/fnsizes_extra.txt + *_libsyms.csv, the env vars build_overlay reads, the build flag strings).  check_all stores it in
output/gate/strict_ok.key after the builds passed and skips them while the key is unchanged (FORCE_STRICT=1 rebuilds)."""
import glob, hashlib, os, sys
T = os.path.dirname(os.path.abspath(__file__)); G = os.path.dirname(T)
if sys.argv[1:2] == ["--objs-exist"]:      # --objs-exist <objs.txt>: exit 0 when every cached object of the stored list is still there
    sys.exit(0 if all(os.path.exists(l.split()[1]) for l in open(sys.argv[2]) if l.strip()) else 1)
h = hashlib.sha1()
fs = [os.path.join(dp, f) for dp, dn, fn in os.walk(os.path.join(G, "src")) for f in fn]
fs += [os.path.join(T, f) for f in ("build_overlay.py", "overlay.py", "vc7.sh")] + [os.path.join(G, "output", "fnsizes.txt")]
fs += glob.glob(os.path.join(G, "output", "cluster_pilot*", "fnsizes_extra.txt")) + glob.glob(os.path.join(G, "output", "*_libsyms.csv"))
fs.append(os.path.join(G, "..", "MashedGame", "FullyLoaded", "installed", "App_Executables", "MFL.exe"))
for f in sorted(fs):
    h.update(os.path.relpath(f, G).encode() + b"\0")
    if os.path.isfile(f): h.update(hashlib.sha1(open(f, "rb").read()).digest())
for v in ("SKIP_FILES", "T2_SHADOW_DIR", "CLUSTER_ENABLED", "BRIDGE_ENABLED", "FIX_RULES", "LIBOBJ_DIR", "SECTION_PAD", "OBJCACHE", "VC7_CONTAINER"):
    h.update(("%s=%s\0" % (v, os.environ.get(v))).encode())
h.update("\0".join(sys.argv[1:]).encode())
print(h.hexdigest())
