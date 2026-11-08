# perf_c0: V4 frame-time probe (report-only). Quick Battle course 0, DRIVE 27 (accelerate, steer, opponents frozen, countdown throttle; no weapons:
# with 31 the round ends at race frame ~263, with 9 the car leaves the track at ~182), race WITHOUT the 60 fps limiter (SCEN_NOLIMIT 6:
# loading + race), the harness race frame counter sampled every 0.25 s into probe.txt while SCEN_FRAMES (default 1500) race frames run.
# ctrl and ours run at the same time (scenario.sh SCEN_EXE mode, NOCACHE=1), so both see the same host load.  tools/perf_probe.py <scenario out dirs> -> median ms/frame.
SCEN_DRIVE=${SCEN_DRIVE-27}; SCEN_NOLIMIT=${SCEN_NOLIMIT-6}
scenario() {
  to_single
  echo "keywait Down $(sel 3) 1 3"; echo "keywait Down $(sel 3) 2 3"
  echo "keywait Return $CUR 4 3"; echo "keywait Return $CUR 5 3"; echo "keywait Return $CUR 6 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $CUR 7 3"; echo "waitframes 0x$(mapva frames_total) 40"
  echo "keywait Return $PHASE !0 5"; echo "keywait Return $PHASE !1 6"; echo "waitmem $PHASE 6 90"
  echo "probe 300 0x$(mapva race_frames)"
  race_freeze ${SCEN_FRAMES:-1500}
}
