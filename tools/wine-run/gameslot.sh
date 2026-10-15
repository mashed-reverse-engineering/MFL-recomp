#!/bin/bash
# gameslot.sh <cmd...>: run cmd while holding one of GAME_SLOTS (default 4) global flock slots in $TMPDIR/gameslots, so that no combination of jobs starts
# more than 4 game containers.  Waits (polled, one log line per ~30 s) while all slots are busy.  Put it OUTSIDE a `timeout`: the wait must not count against the run timeout.
# Nested calls (GAMESLOT_HELD=1, set here) just exec.  The lock dies with the process (also on kill -9).
[ -n "$GAMESLOT_HELD" ] && exec "$@"
SD=${TMPDIR:-/tmp/claude-$(id -u)}/gameslots; mkdir -p $SD; n=0
while :; do
  for s in $(seq 1 ${GAME_SLOTS:-4}); do exec {fd}>$SD/slot$s; flock -n $fd && break 2; exec {fd}>&-; done
  [ $((n % 6)) = 0 ] && echo "gameslot: all ${GAME_SLOTS:-4} game slots busy, waiting ($(basename "$1") pid $$)" >&2; n=$((n+1)); sleep 5
done
export GAMESLOT_HELD=$s; exec "$@"
