#!/bin/bash
# link_ci.sh <work dir> [<ref dir>] (SK W16): the release link (link_release.sh switches) in the NOEXE form: no MFL.exe and no install are opened.
# The image object comes from a table (tools/delink/objtable.py; TABLE=<file>).
# With <ref dir> (= a normal link_release.sh dir of the SAME HEAD and the same CC): the table is emitted from <ref dir>/mfl.obj, the CRT split plan
# (lib/crt, VC7 only) and the import libs (imp/) are <ref dir>'s; the data from the committed data/link/gen (tools/globals/dsform.py join; W16 DS),
# or with DSDATA= (empty) the data objects of <ref dir>/gen.  Result compared with <ref dir>: mfl.obj byte-identical, ours.exe / ours.map equal
# except the link timestamp (SK acceptance).
# SK2 W16, no <ref dir> (CC=v143 only; the self-contained CI link): every input from the committed tables in LINKDATA (default data/link), written
# by tools/ci/link_tables.py emit from a normal CC=v143 release link: mfl_image_v143.txt (TABLE), imports_v143.txt (IMPTAB, mk_importlibs --table),
# crt_xcu.txt (crt_xcu.py), gen_v143/ (DSDATA, joined with the held rows zero).  No compare (nothing to compare with).
# W16 MT: LIB_CRT=v143mt (static CRT) uses the same tables: mfl.obj of a normal v143mt link is byte-identical to the v143md one.
set -e
T=$(dirname "$(readlink -f "$0")"); G=$T/../..
[ $# -eq 1 ] || { [ $# -eq 2 ] && [ -f "$2/ours.exe" ]; } || { echo "usage: link_ci.sh <new work dir> [<normal release link dir of the same HEAD>]"; exit 1; }
mkdir -p "$1"; W=$(readlink -f "$1")
if [ $# -eq 2 ]; then export REF=$(readlink -f "$2")
else unset REF; [ "$CC" = v143 ] || { echo "link_ci.sh with no ref dir: CC=v143 only (VC7 needs <ref dir>/lib/crt, the libcmt split plan: not tabled)"; exit 1; }; fi
# ponytail: switch defaults copied from link_release.sh (which stays unedited); keep the two in step until CI owns its own list
RWDEF=$(grep -o 'lr=${LIB_RW-librw:[^}]*' $T/../gate.sh | head -1); RWDEF=${RWDEF#*LIB_RW-}
[ -n "$RWDEF" ] || { echo "LIB_RW default not found in tools/gate.sh"; exit 1; }
V=; [ "$CC" = v143 ] && { V=_v143; : ${LIB_CRT:=v143md}; }
: ${LIB_LUA:=src} ${LIB_QHULL:=src} ${LIB_ZLIB:=src} ${LIB_CRT:=libcmt} ${DROP_RW:=1} ${DROP_RWTRAP:=1} ${DROP_DINPUT:=1}
LIB_RW=${LIB_RW-$RWDEF}
for v in DROP_RW DROP_RWTRAP DROP_DINPUT; do [ "${!v}" = 0 ] && unset $v; done
export LIB_LUA LIB_QHULL LIB_ZLIB LIB_CRT LIB_RW DROP_RW DROP_RWTRAP DROP_DINPUT MKRANGE=1 GEN_DATA=1 IMPLIBS=1 NOEXE=1
unset HARNESS RUN DELTA
export LINKDATA=$(readlink -f ${LINKDATA:-$G/data/link})
: ${DSDATA=$LINKDATA/gen$V}; [ -f "$DSDATA/data.c" ] && export DSDATA=$(readlink -f $DSDATA) || unset DSDATA      # W16 DS: committed data; DSDATA= (empty) = REF's data objects
if [ -n "$REF" ]; then
  [ -n "$TABLE" ] || { TABLE=$W.image.txt; python3 $T/objtable.py emit $REF/mfl.obj $TABLE; }
else
  : ${TABLE:=$LINKDATA/mfl_image$V.txt}; export IMPTAB=$(readlink -f ${IMPTAB:-$LINKDATA/imports$V.txt})
  for f in $TABLE $IMPTAB $LINKDATA/crt_xcu.txt $DSDATA/data.c; do [ -f $f ] || { echo "link_ci.sh: no $f (tools/ci/link_tables.py emit)"; exit 1; }; done
fi
export TABLE=$(readlink -f $TABLE)
# W16 CIN (the GitHub link job): MFL_REFS defaults to the vendored third_party/ (Lua / qhull / zlib / libpng for output/libmatch2/build_libs.sh);
# FIXES=all = MFLng_fixes.exe: every `#define MFL_FIX_X 0` line of src/mfl_fix.h on (the build_mflng.sh rule; a define with a comment, UNLOCK_ALL, stays off);
# AUDIO_DLL=1 = mfl_audio_null.c with MFL_AUDIO_DLL (loads mfl_audio_miniaudio.dll), as link_release.sh.  All unset = the link as before.
[ -z "$MFL_REFS" ] && [ -d $G/third_party/lua ] && export MFL_REFS=$(readlink -f $G/third_party)
[ "$FIXES" = all ] && export MFL_FIX_DEFINES="$(awk 'NF==3 && $1=="#define" && $2~/^MFL_FIX_[A-Z0-9_]+$/ && $3=="0" {printf "/D%s=1 ", $2}' $G/src/mfl_fix.h)" RELCHECK=warn      # HEAD: TRAIL_NAN DETERMINISTIC_AUDIO_RNG EQUIP_FLUSH WIDESCREEN PREFS
ARGS="--automatch --automatch-near --bridges --clusters"
if [ -n "$AUDIO_DLL" ]; then mkdir -p $W.src; { echo "#define MFL_AUDIO_DLL"; cat $G/src/audio/mfl_audio_null.c; } > $W.src/mfl_audio_rel.c
  ARGS="$ARGS --src=$W.src/mfl_audio_rel.c"; export SKIP_FILES=mfl_audio_null.c${SKIP_FILES:+,$SKIP_FILES}; fi
echo "link_ci: MFL_FIX_DEFINES='$MFL_FIX_DEFINES' AUDIO_DLL='$AUDIO_DLL' MFL_REFS='$MFL_REFS'"
$T/link_ours.sh $W 0x400000 $ARGS
[ -n "$REF" ] || { echo "link_ci: $W/ours.exe (self-contained, tables from $LINKDATA; no ref dir, no compare)"; exit 0; }
cmp $W/mfl.obj $REF/mfl.obj && echo "link_ci: mfl.obj = $REF/mfl.obj (byte-identical)"
python3 $T/linkrepro.py --compare $REF $W      # ours.exe + ours.map, EQUAL except the link timestamp (rule 19; the only difference two links of one tree have)
