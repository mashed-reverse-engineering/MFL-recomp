#!/bin/bash
# mkpinfile.sh [course]: rebuild a pin file.
#  course 0 (default): data/state_pins.txt from hand rows (data/state_pins_head.txt), the loading-start residue (data/pins_load.txt, made by
#    pinall.py from a cc_ph3 pair) and two reference control runs: $SH_WORK/runs/ph3_1 (phase-3 render-texture camera block) and
#    $SH_WORK/runs/sp3_1 (phase-5 camera record region).
#  course N (W12): data/state_pins_c<N>.txt from the same hand rows plus reference control runs of that course made WITHOUT pins
#    (PINFILE=/dev/null): c<N>ph3_1 / c<N>ph3_2 logged from loading start (PHASE_MIN=3, like ph3_1) and c<N>np_1 from countdown start (default,
#    like sp3_1).  The ph3 pair's statecmp (pointer list + mask) names the words that differ at loading frame 0; pinall.py pins them to c<N>ph3_1's
#    value, except the hand-row words and data/pins_never.txt (window handles, the camera region); the render-texture block comes from c<N>ph3_1,
#    the camera record block from c<N>np_1.  one.sh then uses this file for course N automatically.
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f "$T/../.."); W=${SH_WORK:-$R/output/statehash}
C=${1:-0}; P=$T/data/state_ptrwords.txt
if [ "$C" = 0 ]; then O=$T/data/state_pins.txt; REF3=$W/runs/ph3_1; REF5=$W/runs/sp3_1; LOAD=$T/data/pins_load.txt
else
  O=$T/data/state_pins_c$C.txt; REF3=$W/runs/c${C}ph3_1; REF5=$W/runs/c${C}np_1; LOAD=$W/pins_load_c$C.txt
  for r in c${C}ph3_1 c${C}ph3_2 c${C}np_1; do [ -s "$W/runs/$r/statehash.bin" ] || { echo "mkpinfile: missing $W/runs/$r/statehash.bin (one.sh $r $C, PINFILE=/dev/null, ph3 runs with PHASE_MIN=3)"; exit 1; }; done
  python3 "$T/statecmp.py" "$REF3/statehash.bin" "$W/runs/c${C}ph3_2/statehash.bin" --ptr "$P" --mask "$T/data/state_mask.txt" --show 100000 > "$W/cc_c${C}ph3.txt"
  EX=; for v in $(grep -v '^#' "$T/data/state_pins_head.txt" | cut -d: -f2); do EX="$EX $v-$(printf %x $((0x$v + 4)))"; done      # hand-row words stay hand rows
  EX="$EX $(grep -v '^#' "$T/data/pins_never.txt" | cut -d' ' -f1 | tr '\n' ' ')"
  python3 "$T/pinall.py" "$W/cc_c${C}ph3.txt" "$REF3/statehash.bin" 0 3 "$LOAD" $EX || exit 1
fi
{
cat "$T/data/state_pins_head.txt"
echo "# --- loading-start residue (pinall.py $([ "$C" = 0 ] && echo cc_ph3 || echo "cc_c${C}ph3, course $C"), clock words and RenderWare ring words excluded)"
cat "$LOAD"
echo "# --- render-texture camera block"
python3 "$T/mkpins.py" "$REF3/statehash.bin" 0 3 "$P" 948170-948240 | sed "s/\$/ g_rt_948178..g_rt_9481f8+0x48 | render-texture camera matrices of the menu car preview: differ between control runs from frame 0, not written until race frame ~61 (frame 173), then equal; value = reference run $(basename $REF3) at loading frame 0/"
if [ "$C" != 0 ] && [ -f "$T/data/state_pins_c${C}_head.txt" ]; then echo "# --- reviewed hand rows of course $C (data/state_pins_c${C}_head.txt)"; grep -v '^#' "$T/data/state_pins_c${C}_head.txt"; fi
echo "# --- camera record region, at countdown start"
python3 "$T/mkpins.py" "$REF5/statehash.bin" 0 5 "$P" 8c5900-8c8000 | sed "s/\$/ camera records 0x8c5900-0x8c8000 | per-car camera records + record ring: residue that the camera filter carries into the race; pinned at countdown start, value = reference run $(basename $REF5) at frame 0/"
} > "$O"
grep -vc '^#' "$O"
