# check_run.sh: sourced by the rw_*_check.sh scripts (W13 C1).  `check_run <container command ...>` runs the check exe with its stdout in
# $O/replay.log (and on the terminal) and the container's stderr in $O/stderr.log, so a DLL stderr line ("mflrw: heap exit ...") can no longer
# split the exe's DONE line; pipefail keeps the exe's exit code (CHECK_RC) instead of tee's.  `check_done` (the script's last line, after its
# verdict-word grep) passes only with a DONE line AND exit code 0, else it prints one reason line (exit code, DONE count, last stderr lines).
set -o pipefail
check_run() { CHECK_RC=0; "$@" 2>"$O/stderr.log" | tee "$O/replay.log" || CHECK_RC=$?; }
check_done() {
  grep -q "^DONE" "$O/replay.log" && [ "$CHECK_RC" = 0 ] && return 0
  echo "check ended without a verdict word: exit $CHECK_RC, $(grep -c '^DONE' "$O/replay.log") DONE line(s); stderr: $(tail -2 "$O/stderr.log" | tr '\n' ' ' | cut -c1-200)"
  return 1
}
