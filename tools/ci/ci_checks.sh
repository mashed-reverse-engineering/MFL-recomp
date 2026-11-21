#!/bin/bash
# ci_checks.sh <out dir> <librw-mfl checkout>: the static checks of plan W16 section 3, in order; the first failure stops the job (exit 1).
# <out dir> holds MFLng.exe (fixes), MFLng.nofix.exe (original behaviour), mflrw.dll and the video DLL set (mfl_video_ffmpeg.dll + FFmpeg) (build.yml layout); <librw-mfl checkout> is the pinned one (data/link/mflrw_pin.txt).
# Runs in build.yml after both links, and locally on an msvcw-link output before anything reaches the Wine queue:
#   python3 tools/run.py env PY=python3 -- tools/ci/ci_checks.sh <out dir> <librw-mfl>
# PY defaults to `python` (the Windows runner's name).  The release guard over the release dir (no_original.py --release) runs in release.yml.
set -eu
O=$1 L=$2 PY=${PY:-python} C=$(dirname "$0")
R=$C/../..
$PY "$C/rw_stub_guard.py" --selftest
$PY "$C/pe_check.py" --selftest
$PY "$C/no_original.py" --selftest
$PY "$C/pe_check.py" --dll "$O/mflrw.dll"
for f in "$O"/mfl_video_ffmpeg.dll "$O"/avformat-*.dll "$O"/avcodec-*.dll "$O"/avutil-*.dll "$O"/swscale-*.dll; do      # W16 VIDCI: a missing one fails (unmatched glob)
  $PY "$C/pe_check.py" --video "$f"
done
for e in MFLng MFLng.nofix; do
  $PY "$C/rw_stub_guard.py" "$L/capi/stubs_gen.cpp" "$O/$e.exe"
  $PY "$C/pe_check.py" "$O/$e.exe" --rwdll "$O/mflrw.dll" --manifest "$R/data/link/MFLng.manifest" --icon "$R/assets/icon/mflng.ico"
done
$PY "$C/no_original.py" "$R"
echo "ci_checks: all passed"
