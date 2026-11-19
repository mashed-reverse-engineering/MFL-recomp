#!/bin/bash
# ci_checks.sh <out dir> <librw-mfl checkout>: the static checks of plan W16 section 3, in order; the first failure stops the job (exit 1).
# <out dir> holds MFLng.exe, MFLng_fixes.exe and mflrw.dll (build.yml layout); <librw-mfl checkout> is the pinned one (data/link/mflrw_pin.txt).
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
for e in MFLng MFLng_fixes; do
  $PY "$C/rw_stub_guard.py" "$L/capi/stubs_gen.cpp" "$O/$e.exe"
  $PY "$C/pe_check.py" "$O/$e.exe" --rwdll "$O/mflrw.dll" --manifest "$R/data/link/MFLng.manifest"
done
$PY "$C/no_original.py" "$R"
echo "ci_checks: all passed"
