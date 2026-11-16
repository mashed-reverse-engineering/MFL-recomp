#!/bin/bash
# scen_pin.sh <App_Executables dir> <delta hex, 0 = control> <srand va hex>: install the d3d9 proxy (tools/d3dtrace) with the determinism pins of
# lane V2 (W11: course 0 control runs IDENTICAL over a full race) into a prepared game dir, for scenario.sh SCEN_PIN=1 (lane TS W11).
# Same cfg as d3dtrace/run.sh STATEHASH=1 ALOCK=1 SNDPIN=1, minus the trace and the state hash: alock (worker threads in lockstep), vds/apoll/aio/vst +
# vdssnap (virtual dsound cursor snapped at race-phase changes), vsnap=2 (virtual clock on a 50 ms frame grid), srand pinned at phase 3, the
# state pins of tools/statehash/data/state_pins.txt (set at loading start, va + delta).  The trace stops after 1 frame (max_frames=1, geom=0).
# Ours: the import of MFL.exe AND of mflrw.dll (librw-mfl imports Direct3DCreate9 itself) is renamed d3d9.dll -> d3dp.dll.
# Env: SCEN_PIN_DLL (proxy build, default tools/d3dtrace/d3d9.dll; the V2 test build agents/v2_w11/d3d9_p1.dll until batch b8snd lands).
AE=${1:?App_Executables dir}; D=${2:?delta hex}; SR=${3:?srand va}
T=$(dirname "$(readlink -f "$0")"); D3=$(readlink -f $T/../d3dtrace); DLL=${SCEN_PIN_DLL:-$D3/d3d9.dll}
[ -f $DLL ] || make -C $D3 >/dev/null || { echo "scen_pin: no proxy dll"; exit 1; }
python3 $D3/patch_import.py $AE/MFL.exe d3dp.dll || exit 1
[ -f $AE/mflrw.dll ] && { python3 $D3/patch_import.py $AE/mflrw.dll d3dp.dll || exit 1; }
cp $DLL $AE/d3dp.dll
{ echo "out=d3dtrace.log"; echo "max_frames=1"; echo "geom=0"; echo "alock=1"; printf 'vds=1\napoll=1\naio=1\nvst=1\nvdssnap=1\n'; echo "vsnap=2"
  echo "sh_delta=$D"; printf 'phase_addr=0x%x\n' $((0x652f68 + 0x$D)); echo "srand=3:$SR:1"
  grep -v '^#' $T/../statehash/data/state_pins.txt | cut -d' ' -f1 | sed 's/^/pin=/'; } > $AE/d3dtrace.cfg
