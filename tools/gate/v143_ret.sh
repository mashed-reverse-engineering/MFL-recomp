#!/bin/bash
# v143_ret.sh link|scan <dir> (W17 DET): the RET gate step's two halves (gate.sh standalone / full).
#   link: CI-style v143 link of the tree (tools/delink/link_ci.sh CC=v143 FIXES=all AUDIO_DLL=1 = the shipped MFLng.exe; ~4 min CPU with a warm object cache) into <dir>/lnk, log <dir>/link.log, exit status <dir>/link.rc
#   scan: tools/gate/ret_scan.py on <dir>/lnk vs the original MFL.exe (~10 s), report <dir>/ret_scan.txt; exit 1 = a FAIL / FPTR row outside tools/gate/ret_allow.txt
# ponytail: FIXES=all only (MFLng.nofix.exe differs only in MFL_FIX branches); a second link when a nofix-only row ever matters
T=$(dirname "$(readlink -f "$0")"); G=$T/../..
[ $# = 2 ] || { sed -n 2,4p "$0"; exit 2; }
mkdir -p $2; O=$(readlink -f $2)
case $1 in
  link) CC=v143 FIXES=all AUDIO_DLL=1 $G/tools/delink/link_ci.sh $O/lnk > $O/link.log 2>&1; echo $? > $O/link.rc; exit $(cat $O/link.rc);;
  scan) python3 -I $T/ret_scan.py $O/lnk/ours.exe $O/lnk/ours.map --out $O/ret_scan.txt;;
  *) sed -n 2,4p "$0"; exit 2;;
esac
