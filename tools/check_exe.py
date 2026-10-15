#!/usr/bin/env python3
"""check_exe.py [path]: verify that MFL.exe is the supported build (default $MFL_EXE or game/MFL.exe)."""
import hashlib, os, sys
SHA1 = "d52ca4a5995875b6d052707188e2c6d065f41766"   # Mashed: Fully Loaded, installed App_Executables/MFL.exe, 3129344 bytes
p = sys.argv[1] if len(sys.argv) > 1 else os.environ.get("MFL_EXE", os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "game", "MFL.exe"))
h = hashlib.sha1(open(p, "rb").read()).hexdigest()
print(("OK  " if h == SHA1 else "MISMATCH ") + h + "  " + p)
sys.exit(h != SHA1)
