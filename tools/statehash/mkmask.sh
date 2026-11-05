#!/bin/bash
# mkmask.sh <tag> ...: rebuild data/state_mask.txt = hand rows (data/state_mask_head.txt) + framerate-sample slots + stale menu residue rows
# (statecmp --mkstale) over the control pairs $SH_WORK/runs/<tag>_1 / <tag>_2.  Original tags: sp2 sp3 sp4 sp5 (see README, "Regenerating").
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f "$T/../.."); W=${SH_WORK:-$R/output/statehash}
O=$T/data/state_mask.txt
S=$W/stale_rows.tmp
mkdir -p "$W"
: > "$S"
for t in "$@"; do
  python3 "$T/statecmp.py" --mkstale "$W/runs/${t}_1/statehash.bin" "$W/runs/${t}_2/statehash.bin" "$T/data/state_ptrwords.txt" cc_$t >> "$S"
done
{
cat "$T/data/state_mask_head.txt"
echo "# --- framerate sample slots (framerate_sample_accumulate over g_app_state_883ea0, stride 0x44, field +0x28): profiling, all 96 differ every frame"
for k in $(seq 0 95); do printf "%08x 1 g_app_state_883ea0+0x%x | framerate sample slot %d field +0x28 (profiling counter, menu-frame dependent)\n" $((0x883ec8 + k * 0x44)) $((0x28 + k * 0x44)) $k; done
echo "# --- stale menu residue (statecmp.py --mkstale over the pairs: $*), first evidence row per word"
sort -u -k1,1 "$S"
} > "$O"
grep -vc '^#' "$O"
