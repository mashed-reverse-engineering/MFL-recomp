# scenset (lane TS, W11): wraps scenario SCEN_BASE (any scenarios/<name>.sh) for the judged scenario set (sa_oracles.sh scenset, wine-run/scen_judge.py).
# After the base scenario it waits for an end EVENT and dumps the live GameState (gs_end) so two controls and ours can be byte-compared:
#   SCEN_END=menu:<depth>   press Return until the menu depth (g_menu_current, CUR) is <depth> (round end -> results -> back on a menu page), then dump
#   SCEN_END=phase:<n>      press Return until the race phase is <n> (10 = Race Results page), then dump; use with SCEN_AFTER=end (the base
#                           scenario then only waits SCEN_SECS for the round to end and does not run its own end screens)
#   SCEN_END=freeze:<frame> freeze the race at race frame <frame> (harness freeze_target, as race_freeze) and dump the cars too
#   SCEN_END=now            dump at once (menu scenarios that end on a page)
# SCEN_ENDPROBE=<secs>: probe race phase, menu depth and message box every 0.25 s from the start (out/probe.txt), to find an end event.
# The dump is taken at a game state (menu depth, race phase, frozen frame), never after a fixed time.
. $T/scenarios/${SCEN_BASE:?SCEN_BASE=<scenario name>}.sh
eval "base_$(declare -f scenario)"
scenario() {
  [ -n "$SCEN_ENDPROBE" ] && echo "probe $SCEN_ENDPROBE $PHASE $CUR $MSG"
  base_scenario
  case ${SCEN_END:-now} in
    # W12 hx3: the end event is REQUIRED (tag `end`): a timeout writes MISS end to out/missed.txt; scen_judge: ours -> FAIL, control -> UNJUDGED
    menu:*) echo "pressuntil Return $CUR ${SCEN_END#menu:} 150 1.0 end"; echo "wait 1.5"; echo "shot end_page";;
    phase:*) echo "pressuntil Return $PHASE ${SCEN_END#phase:} 150 1.0 end"; echo "wait 1.5"; echo "shot end_page";;
    freeze:*) race_freeze ${SCEN_END#freeze:};;
  esac
  echo "dump gs_end 0x7eebc0 0xb64"
}
