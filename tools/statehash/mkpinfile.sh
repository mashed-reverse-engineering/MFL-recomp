#!/bin/bash
# mkpinfile.sh: rebuild data/state_pins.txt from hand rows (data/state_pins_head.txt), the loading-start residue (data/pins_load.txt, made by
# pinall.py from a cc_ph3 pair) and two reference control runs: $SH_WORK/runs/ph3_1 (phase-3 render-texture camera block) and
# $SH_WORK/runs/sp3_1 (phase-5 camera record region).  Course 0 only; other courses need their own reference runs.
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f "$T/../.."); W=${SH_WORK:-$R/output/statehash}
O=$T/data/state_pins.txt
P=$T/data/state_ptrwords.txt
{
cat "$T/data/state_pins_head.txt"
echo "# --- loading-start residue (pinall.py cc_ph3, clock words and RenderWare ring words excluded)"
cat "$T/data/pins_load.txt"
echo "# --- render-texture camera block"
python3 "$T/mkpins.py" "$W/runs/ph3_1/statehash.bin" 0 3 "$P" 948170-948240 | sed 's/$/ g_rt_948178..g_rt_9481f8+0x48 | render-texture camera matrices of the menu car preview: differ between control runs from frame 0, not written until race frame ~61 (frame 173), then equal; value = reference run ph3_1 at loading frame 0/'
echo "# --- camera record region, at countdown start"
python3 "$T/mkpins.py" "$W/runs/sp3_1/statehash.bin" 0 5 "$P" 8c5900-8c8000 | sed 's/$/ camera records 0x8c5900-0x8c8000 | per-car camera records + record ring: residue that the camera filter carries into the race; pinned at countdown start, value = reference run sp3_1 at frame 0/'
} > "$O"
grep -vc '^#' "$O"
