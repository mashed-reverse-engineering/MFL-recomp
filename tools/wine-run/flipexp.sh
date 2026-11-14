#!/bin/bash
# flipexp.sh [OUTDIR] [LIFTS] [ROW] [RIGHT]: MashedTrainer flip experiment on the control build (harness exe): Quick Battle row ROW (variant RIGHT), opponents' controls disabled,
# settle, then flipexp.pl (lift sweep upright->roof and roof->upright).  Result OUTDIR/out/flipexp.txt.  Uses its own copies of play.sh / play_inner.sh (flip_play.sh, flip_inner.sh).
T=$(dirname "$(readlink -f "$0")"); R=$(readlink -f $T/../..); export MASHED_ROOT=$(readlink -f $R/..)
W=${1:-$R/output/flipexp}; LIFTS=${2:-0,0.5,1,1.5,2,3,4,6}; SETTLE=${5:-$SETTLE}; FLIPCMD=${6:-$FLIPCMD}; mkdir -p $W   # args: OUTDIR LIFTS ROW RIGHT [SETTLE s] [FLIPCMD, commands separated by |]
MAP=$R/output/scenario/_build/ctrl.exe.map; export MAP
. $T/scenarios/lib.sh; SCEN_DRIVE=8; SCEN_ROW=${3:-0}; SCEN_RIGHT=${4:-0}
. $T/scenarios/arcade.sh; SCEN_DRIVE=8
{ scenario_header; SCEN_AFTER=none; scenario | grep -v "dump cars\|poke 0x$(mapva freeze_target)\|waitmem 0x$(mapva frozen)"; echo "wait ${SETTLE:-5}"; echo "shot race"; echo "${FLIPCMD:-flipexp $LIFTS}" | tr "|" "\n"; echo "shot end_state"; } > $W/script.txt
rm -rf $W/run $W/out; python3 $T/prepare.py $R/../MashedGame/FullyLoaded/installed $W/run $R/output/scenario/_build/ctrl.exe > /dev/null || exit 1
PFX=wine_pfx_1 PERSIST=slot1 timeout -k 5 600 $T/flip_play.sh $W/run $W/script.txt $W/out
cat $W/out/flipexp.txt
