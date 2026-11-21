#!/bin/bash
# c1_calib.sh [drive] [frames]: RPL-T C1 chaos calibration (agents/rplt_w17/design.md section 4), VC7 control exe only, no v143 needed.
# 4 Wine runs of course 0 (~15-40 s each, one.sh -> run.sh, which WAITS while a gate holds the lock): control, control again (noise floor of
# this pin set: must be IDENTICAL or the poke results mean nothing), poke 1 ulp, poke 0x400 on player_car[0] pos.x
# (SH_POKE=150:8b1038:<xor hex>, applied by the proxy after frame 150 = race frame 38 is recorded).  Per poke: statecmp with the float band at 0
# (first DISCRETE / FLOAT frame) and growth.py (per frame and class: float words differing, max ulp, max relative error; first frame over
# 1 ulp / 100 ulp / 1e-3).  Answers Q2: frames from a 1-ulp perturbation to the first discrete difference.
# Env: CTRL_BUILD=<dir with ctrl.exe + ctrl.exe.map> or SA_EXE/SA_DELTA (standalone VC7 harness link); SH_WORK (default output/statehash).
# Run it twice for the two chaos cases: c1_calib.sh 31 (single car, opponents frozen) and c1_calib.sh 23 (AI opponents on).
# Pins: course 0, ALOCK=1 SNDPIN=1 (W11 proof) with the FIXED_STEP-safe clock of fps_w17 (STEP_US=0 VSNAP=3 NOLIMIT=0: one 1/60 s step per
# Present) and scen_flags bit 2048 (as one.sh's default: no host key can cut the intro fly-over short).
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f "$T/../.."); W=${SH_WORK:-$R/output/statehash}
D=${1:-31}; FR=${2:-400}
export DRIVE=$((D | 2048)) ALOCK=1 SNDPIN=1 STEP_US=0 VSNAP=3 NOLIMIT=0
TAG=c1d$D
CMP="--ptr $T/data/state_ptrwords.txt --mask $T/data/state_mask.txt"
SH_POKE= "$T/one.sh" ${TAG}_ctl 0 $FR || exit 1
SH_POKE= "$T/one.sh" ${TAG}_ctl2 0 $FR || exit 1
python3 "$T/statecmp.py" "$W/runs/${TAG}_ctl/statehash.bin" "$W/runs/${TAG}_ctl2/statehash.bin" $CMP --ftol 0 --show 8 > "$W/runs/${TAG}_ctl2.cmp.txt"
echo "== drive $D control pair (noise floor)"; grep -E "^(compared|first divergent|divergent frames|T1 |VERDICT)" "$W/runs/${TAG}_ctl2.cmp.txt"
for X in 1 400; do
  SH_POKE=150:8b1038:$X "$T/one.sh" ${TAG}_p$X 0 $FR || exit 1
  # the poke run vs the control: same exe, same layout, so no map; --ftol 0 = every float word counts, class lines show first frame + maxulp
  python3 "$T/statecmp.py" "$W/runs/${TAG}_ctl/statehash.bin" "$W/runs/${TAG}_p$X/statehash.bin" $CMP --ftol 0 --show 8 > "$W/runs/${TAG}_p$X.cmp.txt"
  python3 "$T/growth.py" "$W/runs/${TAG}_ctl/statehash.bin" "$W/runs/${TAG}_p$X/statehash.bin" $CMP > "$W/runs/${TAG}_p$X.growth.txt"
  echo "== drive $D poke xor $X"; grep -E "^(compared|first divergent|divergent frames|T1 |VERDICT)" "$W/runs/${TAG}_p$X.cmp.txt"; grep -E "^(SUMMARY|CLASS)" "$W/runs/${TAG}_p$X.growth.txt"
done
