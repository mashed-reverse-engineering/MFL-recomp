# winelock.sh: sourced by play.sh, scen_play.sh and d3dtrace/run.sh (W13 C4 = harness review finding 8, hx6).  A Wine run holds a SHARED flock on
# the gate lock for its whole life, on fd 9 of the host-side script (play.sh / scen_play.sh end in `exec docker ...`, so the docker client that
# lives as long as the game keeps it).  The gate's EXCLUSIVE flock (run.py gate: `flock gate.lock gate.sh`) therefore waits until every running
# Wine run has ended, and a run started while a gate holds the lock waits for the gate.  The lock is a kernel flock on an open descriptor, so a
# killed wrapper frees it at once and no lock file goes stale; a game left behind by a killed wrapper is removed by the next start in that slot
# (scen_inner.sh: stale driver by pid file; play_inner / scen_inner wineserver -k).  `exec 9>>` replaces a descriptor 9 inherited from a caller
# (d3dtrace/run.sh -> play.sh): every script holds its own.  Inside a gate (MFL_IN_GATE, the gate holds the exclusive lock) nothing is taken.
# A wait is reported on stderr AND appended to $WINELOCK_LOG (default <TMPDIR>/winelock.log): scenario.sh sends scen_play's output to /dev/null,
# so stderr alone is lost there (C4 real-Wine test part B).
if [ -z "$MFL_IN_GATE" ]; then
  exec 9>>"${GATE_LOCK:-/tmp/claude-$(id -u)/gate.lock}"
  if ! flock -s -n 9; then
    _wl0=$(date +%s.%N); echo "$(basename "$0"): a gate holds the gate lock: waiting until it ends" >&2
    flock -s 9
    LC_ALL=C printf '%s %s pid %d waited %.1fs for the gate lock (%s)\n' "$(date +%s.%N)" "$(basename "$0")" $$ "$(echo "$(date +%s.%N) $_wl0" | awk '{print $1-$2}')" "$*" \
      >> "${WINELOCK_LOG:-${TMPDIR:-/tmp/claude-$(id -u)}/winelock.log}" 2>/dev/null
  fi
fi
