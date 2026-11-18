# loadscale.sh (W13 C4, harness review finding 9): the one load rule of the run tools, sourced by scenario.sh, scen_inner.sh, gate.sh and
# delink/state_compare.sh and (W14 A2) d3dtrace/run.sh; memwait.pl (perl) computes the same factor with the same default cap.
#   load:    max(1-minute load average, tasks running right now) (/proc/loadavg fields 1 and 4): the running count follows a burst of
#            game starts at once, the average lags it by a minute
#   factor:  load / 4, at least 1, at most LOADCAP (default 3)
#   lscale <secs>          -> secs x factor (integer, rounded up)
#   script_limit <script>  -> the sum of the limits of a play.sh / scen_inner script (wait, waitmem, waitevent, pressuntil, keywait x2,
#                            keychange x2, waitframes, returnloop, trainloop; the defaults of scen_inner.sh): the outer timeout of a run is
#                            lscale(script_limit) + a margin, so it can never end a run whose own waits are still inside their limits
loadnow() { awk '{split($4, r, "/"); l = $1 + 0; if (r[1] + 0 > l) l = r[1] + 0; print l}' "${LOADAVG_FILE:-/proc/loadavg}"; }      # LOADAVG_FILE: test fixtures only
lscale() { awk -v t="$1" -v l="$(loadnow)" -v c="${LOADCAP:-3}" 'BEGIN{s = l / 4; if (s < 1) s = 1; if (s > c) s = c; printf "%d\n", t * s + 0.999}'; }
script_limit() {
  awk '{ c = $1
    if (c == "wait") s += $2
    else if (c == "waitmem" || c == "waitevent") s += ($4 == "" ? 30 : $4)
    else if (c == "pressuntil") s += ($5 == "" ? 20 : $5)
    else if (c == "keywait") s += 2 * ($5 == "" ? 4 : $5)
    else if (c == "keychange") s += 2 * ($4 == "" ? 2 : $4)
    else if (c == "waitframes") s += ($4 == "" ? 5 : $4)
    else if (c == "returnloop") s += ($5 == "" ? 300 : $5)
    else if (c == "trainloop") s += ($5 == "" ? 600 : $5)
    else if (c == "key" || c == "shot" || c == "dump") s += 0.5 }
    END { printf "%d\n", s + 0.999 }' "$1"
}
