#!/usr/bin/env python3
"""addrmove.py <delta hex>: stdin -> stdout, every 0x<hex> literal in the original data range 0x5dd000..0x9f9000 moved by +delta (play.sh scripts for a relinked exe).
Shared by delink/race_relinked.sh and wine-run/scenario.sh (SCEN_EXE mode)."""
import re, sys
d = int(sys.argv[1], 16)
mv = lambda m: '0x%x' % (int(m.group(0), 16) + d) if 0x5dd000 <= int(m.group(0), 16) < 0x9f9000 else m.group(0)
for l in sys.stdin:
    sys.stdout.write(re.sub(r'0x[0-9a-fA-F]+', mv, l))
