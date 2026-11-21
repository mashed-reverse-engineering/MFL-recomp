#!/bin/bash
# c1_calib.sh [drive] [frames]: RPL-T C1 chaos calibration (agents/rplt_w17/design.md section 4), VC7 control exe only, no v143 needed.
# 3 Wine runs of course 0 (~40 s each, one.sh -> run.sh, refuses while a gate holds the lock): control, poke 1 ulp, poke 0x400 on player_car[0] pos.x
# (SH_POKE=150:8b1038:<xor>, applied by the proxy after frame 150 is recorded), then statecmp with the float band at 0 so that the report
# shows the first DISCRETE frame and the FLOAT maxulp growth.  Answers Q2: frames from a 1-ulp perturbation to the first discrete difference.
# Env: CTRL_BUILD=<gate dir>/state (ctrl.exe + ctrl.exe.map) or SA_EXE/SA_DELTA (standalone VC7 harness link); SH_WORK (default output/statehash).
# Run it twice for the two chaos cases: c1_calib.sh 31 (single car, opponents frozen) and c1_calib.sh 23 (AI opponents on).
# Determinism pins as in the W11 proof: course 0, ALOCK=1 SNDPIN=1, STEP_US=0, VSNAP=3, NOLIMIT=0.
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f "$T/../.."); W=${SH_WORK:-$R/output/statehash}
D=${1:-31}; FR=${2:-400}
export DRIVE=$D ALOCK=1 SNDPIN=1 STEP_US=0 VSNAP=3 NOLIMIT=0
TAG=c1d$D
SH_POKE= "$T/one.sh" ${TAG}_ctl 0 $FR || exit 1
for X in 1 400; do
  SH_POKE=150:8b1038:$X "$T/one.sh" ${TAG}_p$X 0 $FR || exit 1
  # the poke run vs the control: same exe, same layout, so no map; --ftol 0 = every float word counts, class lines show first frame + maxulp
  python3 "$T/statecmp.py" "$W/runs/${TAG}_ctl/statehash.bin" "$W/runs/${TAG}_p$X/statehash.bin" \
    --ptr "$T/data/state_ptrwords.txt" --mask "$T/data/state_mask.txt" --ftol 0 --show 8 > "$W/runs/${TAG}_p$X.cmp.txt"
  echo "== drive $D poke xor $X"; grep -E "^(first divergent|divergent frames|T1 |VERDICT)" "$W/runs/${TAG}_p$X.cmp.txt"
done
