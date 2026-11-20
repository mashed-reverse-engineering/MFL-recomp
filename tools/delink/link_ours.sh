#!/bin/bash
# Link OUR recompiled C (the objects build_overlay.py compiles; tier 1 + agent matches + hand written, --automatch) together with the delinked
# image: no overlay section, no jmp patches by a script: mkobj --objs puts a REL32 `jmp` to our function at each original entry and defines every
# symbol our objects need at its original address, link.exe resolves the rest.  Usage: link_ours.sh <work dir> [base=0x400000] [build_overlay args]
# RUN=1: crash-only race on COURSE (default 0) afterwards.  PAD / MKOBJ_ARGS as in roundtrip.sh.
set -e
T=$(dirname "$(readlink -f "$0")"); G=$T/../..; W=$(readlink -f "$1"); B=${2:-0x400000}; shift 2 || shift $#
[ -z "$REUSE" ] && [ -n "$(ls -A $W 2>/dev/null)" ] && { echo "work dir $W is not empty: use a fresh one (REUSE=1 to override)"; exit 1; }
mkdir -p $W/o
# an_hk_975 (FUN_004acca0, DirectShow) writes the fs:0 pseudo global ExceptionList: it stays original in a linked build (the movie code is stubbed anyway)
export SKIP_FILES=an_hk_975.c${SKIP_FILES:+,$SKIP_FILES}
# CC=v143 (W16 V143 item 2, only with LIB_CRT=v143md): our C, Lua / qhull / zlib, the shim and the link by MSVC v143 /MD (tools/msvcw/msvcw_sh.sh,
# msvc-wine); CC unset = the VC7 link, unchanged.  Our VC7 CRT bodies whose names the v143 CRT supplies leave the build (INV 6): raise (_getptd),
# i10_output, amsg_exit, mlock, heap thunks, memcpy / memmove, _control87 / _controlfp.
# W16 MT (V143 item 4): LIB_CRT=v143mt = the same with the static CRT (/MT, libcmt + libvcruntime + libucrt): no vcruntime140 / ucrtbase /
# api-ms-win-crt-* import, runs without the VC++ redistributable.  CRTF / CRTL: the cl switch and the CRT libs of the link line.
V143=; CRTF=/MD; CRTL="msvcrt.lib vcruntime.lib ucrt.lib"
if [ "$CC" = v143 ]; then
  case $LIB_CRT in v143md) ;; v143mt) CRTF=/MT; CRTL="libcmt.lib libvcruntime.lib libucrt.lib";; *) echo "CC=v143 needs LIB_CRT=v143md or v143mt"; exit 1;; esac      # SK2 W16: NOEXE too (link_ci.sh with no ref dir: data/link/*_v143 tables)
  V143=1; export SKIP_FILES=an_crt_raise.c,an_crt_i10_output.c,an_crt_amsg_exit.c,an_crt_mlock.c,an_crt_heap_thunks.c,an_crt_memcpy.c,cl__control87.c,$SKIP_FILES
elif [ "${LIB_CRT#v143}" != "$LIB_CRT" ]; then echo "LIB_CRT=$LIB_CRT needs CC=v143"; exit 1; fi
# LIB_ZLIB=src (A5): zlib 1.1.3 + libpng 1.0.12 (RenderWare rtpng.lib members, not the rtpng.obj wrapper), same mechanism.
# LIB_LUA=src LIB_QHULL=src (A4, MKRANGE/GEN_DATA builds): Lua 4.0 / qhull 2002.1 come from upstream source (output/libmatch2/build_libs.sh, cl 13.00 /G5 /O2 /Ob0), the exe's code range of the lib is dropped
# (tools/delink_sdk/plan_srclib.py, libs.cfg) and the lib's old data rows are not emitted (LIBSRC=1 for globgen/origbytes).  Without them the image keeps the original bytes of the lib.
LSRC=; for L in lua qhull zlib; do eval "v=\$LIB_$(echo $L | tr a-z A-Z)"; [ "$v" = src ] && LSRC="$LSRC $L"; done
if [ -n "$LSRC" ]; then
  export LIBSRC="${LSRC# }"; mkdir -p $W/lib; ITEMS=
  # W16 CIN cin3: MFL_REFS on the command line: the docker form passes no environment into the container (an export tree's default
  # <tree>/../upstream does not exist; link_ci.sh sets MFL_REFS=<tree>/third_party)
  if [ -n "$V143" ]; then $G/tools/msvcw/msvcw_sh.sh $W "CC=v143 V143_CRT=$CRTF MSVC_BIN=/opt/msvc/bin/x86 ${MFL_REFS:+MFL_REFS=$MFL_REFS }bash $G/output/libmatch2/build_libs.sh $W/lib/src $LSRC" > $W/lib/src.log 2>&1 || { tail -3 $W/lib/src.log; exit 1; }
  else
  VC7_CONTAINER=libsrc $G/output/libmatch2/build_libs.sh $W/lib/src $LSRC > $W/lib/src.log 2>&1 || { tail -3 $W/lib/src.log; exit 1; }
  fi
  : > $W/lib/extra_objs.txt; : > $W/lib/members.txt
  for L in $LSRC; do python3 $G/tools/delink_sdk/plan_srclib.py $L $W/lib/src/$L/_o $W/lib/$L $ITEMS | tail -1; cat $W/lib/$L/extra_objs.txt >> $W/lib/extra_objs.txt; cat $W/lib/$L/members.txt >> $W/lib/members.txt; ITEMS=$W/lib/$L/items.csv; done
  cp $G/tools/delink_sdk/stubs.c $W/lib/
  if [ -n "$V143" ]; then $G/tools/msvcw/msvcw_sh.sh $W/lib "/opt/msvc/bin/x86/cl /nologo /c /O2 /arch:IA32 /GS- $CRTF /Zl stubs.c" > $W/lib/stubs.log 2>&1 || { tail -3 $W/lib/stubs.log; exit 1; }      # W16 CIN: no VC7 step in the v143 path
  else VC7_CONTAINER=ac $G/tools/vc7.sh $W/lib cl.exe /nologo /c /O2 stubs.c > /dev/null; fi
  echo "stub:stubs $W/lib/stubs.obj" >> $W/lib/extra_objs.txt      # stdio the exe does not link (dead in Lua / qhull), IID_IUnknown, errno
  EXTRA_OBJS=$W/lib/extra_objs.txt; MKOBJ_ARGS="$MKOBJ_ARGS --drop-members $W/lib/members.txt --drop-items $ITEMS"; export LIBSRC_ITEMS=$ITEMS
fi
# LIB_CRT=libcmt (B5, D3: VC7 libcmt now): the libcmt.lib members that plan_split.py can split out WITH their data (libs.cfg [crt], tools/delink_sdk/ownership.py with the wide-twin fix) replace the exe's CRT code and
# data; blocked members (members needing imports the exe lacks, unanchored data) stay original.  Same plumbing as the source libs: EXTRA_OBJS + mkobj --drop-members.
# W9 E: wincrt0.obj splits too (its _WinMain@16 = app_initialise_on_bootup, src/aliases.txt "WinMain"): the entry is then libcmt's WinMainCRTStartup.  crt0init is not linked:
# _cinit (crt0dat) walks the original initialiser tables at 0x5fb000 through __xi_a / __xi_z / __xc_a / __xc_z (symbols.txt).  Without the split the entry stays the original one (`entry`).
CRTSPLIT=; ENTRY=entry
if [ "$LIB_CRT" = libcmt ]; then
  CRTSPLIT=1; export CRT_SPLIT=$W/lib/crt; mkdir -p $W/lib; [ -s $W/lib/extra_objs.txt ] || { : > $W/lib/extra_objs.txt; : > $W/lib/members.txt; }
  if [ -n "$NOEXE" ]; then cp -r $REF/lib/crt $W/lib/crt      # SK W16 NOEXE (link_ci.sh): plan_split.py reads MFL.exe; the split plan of a normal link of the same HEAD (its object paths stay REF's)
  else
  python3 $G/tools/rw_overlay_prep.py $W/lib/obj > /dev/null || { echo "rw_overlay_prep failed"; exit 1; }
  ITEMS_IN=$ITEMS python3 $G/tools/delink_sdk/plan_split.py crt $W/lib/obj $W/lib/crt | tail -1
  fi
  if [ -s $W/lib/crt/items.csv ]; then      # B7: + the static code pieces of split members (chains after the source libs' items.csv)
    [ -n "$ITEMS" ] && { old_="--drop-items $ITEMS"; MKOBJ_ARGS="${MKOBJ_ARGS/"$old_"/--drop-items $W/lib/crt/items.csv}"; }; ITEMS=$W/lib/crt/items.csv
  fi
  cat $W/lib/crt/extra_objs.txt >> $W/lib/extra_objs.txt; cat $W/lib/crt/members.txt >> $W/lib/members.txt
  grep -q 'wincrt0\.obj$' $W/lib/crt/members.txt && ENTRY=WinMainCRTStartup
  if [ -z "$LSRC" ]; then      # the stubs (IID_IUnknown, errno) the source libs block would have built
    cp $G/tools/delink_sdk/stubs.c $W/lib/; VC7_CONTAINER=ac $G/tools/vc7.sh $W/lib cl.exe /nologo /c /O2 stubs.c > /dev/null; echo "stub:stubs $W/lib/stubs.obj" >> $W/lib/extra_objs.txt
    EXTRA_OBJS=$W/lib/extra_objs.txt; MKOBJ_ARGS="$MKOBJ_ARGS --drop-members $W/lib/members.txt${ITEMS:+ --drop-items $ITEMS}"
  fi
fi
# default build = the full gate build
FULL="--automatch --automatch-near --bridges --clusters"
INST=$G/../MashedGame/FullyLoaded/installed
# HARNESS=1: also link the test harness (src/harness/freeze.c: frame freeze, scripted driving, speed-ups) so the state compare of regress.sh can run on the linked exe.
# Its raw data addresses are shifted by DELTA (hex, the .data move printed at the end; rerun with the right value when it says "DELTA mismatch"), the hook list goes to mkobj.
if [ -n "$HARNESS" ]; then mkdir -p $W/harness; python3 - "$W/harness/freeze.c" $((0x${DELTA:-b000})) $G/src/harness/freeze.c <<'PY'
import re, sys
d = int(sys.argv[2])
s = open(sys.argv[3]).read()
s = re.sub(r"\b0x[0-9A-Fa-f]{6}\b", lambda m: "(0x%X + %d)" % (int(m.group(0), 16), d) if 0x5dd000 <= int(m.group(0), 16) < 0x9f9000 else m.group(0), s)
s = s.replace("extern void __stdcall Sleep", "__declspec(dllimport) void __stdcall Sleep")      # the linked exe has no thunk for it: use the IAT slot
open(sys.argv[1], "w").write(s)
PY
# the harness hooks patch call sites inside the ORIGINAL body of these cluster entries (src/harness/cluster_exclude.txt): leave those clusters out (build_overlay does this only for --harness),
# except app_main_loop / message_box_update: the objcopy renames below carry their hooks into our object (B3 2026-11-04)
export CLUSTER_ENABLED=$(python3 -c "
import sys
x = {l.split()[0] for l in open(sys.argv[2]) if l.strip() and not l.startswith('#')} - {'app_main_loop', 'message_box_update'}
print(','.join(n for n in (l.strip() for l in open(sys.argv[1])) if n and not n.startswith('#') and n not in x))" $G/src/clusters/enabled.txt $G/src/harness/cluster_exclude.txt)
set -- "$@" ; [ $# -eq 0 ] && set -- $FULL; set -- "$@" --src=$W/harness/freeze.c; HMK="--hook-calls $G/src/harness/hooks.txt --dwords $G/src/harness/dwords.txt"; fi
JOBS=${JOBS:-4} OBJLIST=$W/objs.txt OBJLIST_ONLY=$NOEXE$V143 TMPDIR=$W python3 $G/tools/build_overlay.py $W/ovl.exe ${@:-$FULL} > $W/build_overlay.log 2>&1
# NOEXE=1 REF=<normal link dir of the same HEAD> TABLE=<objtable.py emit> (SK W16, link_ci.sh): no MFL.exe and no install are opened; what still needs the exe comes from REF
# SK2 W16, CC=v143 NOEXE with no REF: LINKDATA (crt_xcu.txt) + IMPTAB + TABLE + DSDATA = data/link tables (tools/ci/link_tables.py); v143md_plan.py is skipped (it only feeds mkobj options, --table ignores them)
if [ -z "$NOEXE" ]; then
python3 $G/tools/wine-run/prepare.py $INST $W/game >/dev/null
cp $W/game/App_Executables/MFL.exe $W/mfl_patched.exe
fi
# CC=v143 LIB_CRT=v143md: no libcmt split (the image keeps the VC7 CRT bytes; nothing of ours calls them, v143md_plan.py lists the exceptions;
# V143e1: the VC7 CRT entries with a v143 counterpart jmp to it, v143/crt_redirect.txt, the rest + the EH helpers stay VC7: crt_redirect_skip.txt);
# entry = msvcrt.lib's WinMainCRTStartup; the game's 12 initialisers as .CRT$XCU in table order (crt_xcu.py) + v143md_shim.c, both by v143
if [ -n "$V143" ]; then
  mkdir -p $W/v143; ENTRY=WinMainCRTStartup
  XCU=$W/mfl_patched.exe; [ -n "$NOEXE" ] && XCU=$LINKDATA/crt_xcu.txt      # SK2: NOEXE reads the 12 committed slot values (tools/ci/link_tables.py)
  python3 $G/tools/msvcw/crt_xcu.py $XCU $W/v143/crt_xcu.c || exit 1
  cp $G/tools/msvcw/v143md_shim.c $W/v143/
  $G/tools/msvcw/msvcw_sh.sh $W/v143 "/opt/msvc/bin/x86/cl /nologo /c /O2 /arch:IA32 /GS- $CRTF $([ $CRTF = /MT ] && echo /DMFL_V143MT) /Zl crt_xcu.c v143md_shim.c" > $W/v143/cl.log 2>&1 || { tail -5 $W/v143/cl.log; exit 1; }
  { [ -n "${EXTRA_OBJS:-}" ] && cat $EXTRA_OBJS; echo "v143:crt_xcu $W/v143/crt_xcu.obj"; echo "v143:shim $W/v143/v143md_shim.obj"; } > $W/v143/extra_objs.txt; EXTRA_OBJS=$W/v143/extra_objs.txt
fi
# LIB_RW=librw:<group> (L2, plan_librw_integration 2.3; needs IMPLIBS=1 and RW_DLL=<mflrw.dll>): the functions named in tools/delink/rw_<group>.txt (g0 = stateless math) come from the librw facade DLL.  A thunk object (gas) defines
# each under the exe function's own name (`_Name: jmp *__imp__Name`), so mkobj puts its usual 5 byte entry jmp at the original address like for any object of ours; mk_importlibs.py builds mflrw.lib from the DLL's exports.
# Without LIB_RW nothing changes (the A/B reference).  The DLL is copied next to ours.exe at the end.
[ "$LIB_RW" = orig ] && LIB_RW=      # escape: LIB_RW=orig = no librw swap (A/B reference)
if [ -n "$LIB_RW" ]; then
  RWG=${LIB_RW#librw:}; RWL=; for g_ in ${RWG//,/ }; do [ -f $T/rw_$g_.txt ] && RWL="$RWL $T/rw_$g_.txt" || RWL=; [ -n "$RWL" ] || break; done      # LIB_RW=librw:g0,stream = several lists
  : ${RW_DLL:=$T/../../output/librw/mflrw.dll}      # pinned build (B8 2026-11-01, sha256 in output/librw/mflrw.dll.sha256); rebuild: GEN=<ghidra> librw-mfl/tools/build_mflrw.sh <dir>, copy the d3d9 mflrw.dll over it deliberately
  [ "$RWG" != "$LIB_RW" ] && [ -n "$RWL" ] && [ -n "$IMPLIBS" ] && [ -f "$RW_DLL" ] || { echo "LIB_RW=librw:<group>[,<group>] needs $T/rw_<group>.txt, IMPLIBS=1 and RW_DLL=<mflrw.dll>"; exit 1; }
  mkdir -p $W/rw; export RW_DLL=$(readlink -f $RW_DLL)
  cat $RWL | grep -v '^#' | awk 'NF{split($1,a,"="); d=(a[2]!=""?a[2]:a[1]); printf ".text\n.globl _%s\n_%s:\n\tjmp *__imp__%s\n", a[1], a[1], d}' > $W/rw/thunks.s      # "exe_name=dll_export" when they differ
  i686-w64-mingw32-as -o $W/rw/thunks.obj $W/rw/thunks.s || exit 1
  { [ -n "${EXTRA_OBJS:-}" ] && cat $EXTRA_OBJS; echo "rw:thunks $W/rw/thunks.obj"; } > $W/rw/extra_objs.txt; EXTRA_OBJS=$W/rw/extra_objs.txt
else unset RW_DLL; fi
# DROP_DINPUT=1 (S7, default off; needs GEN_DATA): the dinput8.lib object-format tables in .text (0x4ab230, 4,800 B, tools/delink/dinput_dead.txt) come from src/dinput/dinput_formats.c (const data,
# mingw-compiled): it defines func_0x004ab230 / func_0x004ab4f0, which the generated .rdata DIDATAFORMAT structures reference; mkobj fills the old bytes.
if [ -n "$DROP_DINPUT" ]; then
  mkdir -p $W/dinput; i686-w64-mingw32-gcc -c -O1 -fno-ident -o $W/dinput/dinput_formats.obj $G/src/dinput/dinput_formats.c || exit 1
  { [ -n "${EXTRA_OBJS:-}" ] && cat $EXTRA_OBJS; echo "dinput:formats $W/dinput/dinput_formats.obj"; } > $W/dinput/extra_objs.txt; EXTRA_OBJS=$W/dinput/extra_objs.txt; export DROP_DINPUT
fi
# W11 B2: stable object names o/<name>.obj (tools/delink/objnames.py: our C by file name, extra objects by their tag; a duplicate stops the link), not o/<n>.obj
python3 $T/objnames.py $W/objs.txt ${EXTRA_OBJS:-} > $W/objnames.txt || exit 1
python3 $T/objnames.py --copy $W $W/objnames.txt > $W/objs_local.txt      # EXTRA_OBJS=<file "src obj" per line>: more objects, e.g. SDK lib members (M2 lib step); W16 SPD: one process, not one cp per object
# LITRELOC (T-E6): raw code addresses in the recompiled objects (tools/delink/code_literals.txt) become relocations against func_0x<va> (the copies in $W/o; the sources stay)
# A7 (always, W12 DS2: the DROP_DSHOW=0 form is gone): the DirectShow movie code is dead (the game plays movies through src/video/mfl_video): its objects leave the link, mkobj fills the functions, globgen zeroes the pointers to them
python3 $T/dshow_drop.py filter $W; export DROP_DSHOW=1      # the env var is only the channel to mkobj.py / globgen.py
[ -n "$DROP_RWTRAP" ] && export DROP_RWTRAP      # S5 (default off): tools/delink/rw_trap.txt filled by mkobj with int3, pointers kept
[ -n "$DROP_RW" ] && export DROP_RW    # S3 (default off): dead RenderWare/D3DX pool code (tools/delink/rw_dead.txt, dshow_drop.py pool) filled by mkobj, pointers zeroed by globgen
[ -n "$MKRANGE" ] && python3 $T/litreloc.py $W/objs_local.txt
# GEN_DATA: initialised .data of the harness object would sit between the generated .data and .bss and shift the bss delta (the race scripts use ONE delta): give it a section of its own
[ -n "$GEN_DATA" ] && [ -n "$HARNESS" ] && while read -r src obj; do case $src in freeze.c) objcopy --rename-section .data=.hdat $obj;; esac; done < $W/objs_local.txt
# HARNESS: freeze.c's controller_present (scen_flags bit 128: two human players) replaces 0x4A5B20; mkobj's entry jmp only catches the image's callers, our C calls
# that function by name (controller_present_thunk_4a5b20), so those callers are pointed at the harness too (the defining tier1 object is left alone)
[ -n "$HARNESS" ] && while read -r src obj; do if nm $obj 2>/dev/null | grep -q " U _controller_present_thunk_4a5b20$"; then objcopy --redefine-sym _controller_present_thunk_4a5b20=_controller_present $obj; fi; done < $W/objs_local.txt
# HARNESS: dwords.txt / hooks.txt patch the ORIGINAL bodies of menu_update + menu_items_animate_update (every g_frame_seconds read -> fast_ui_seconds) and
# app_state_menu_render (every call of RwCameraBeginUpdate = 0x4d1b90 -> begin_update_hook).  Where our C replaces such a function that body is dead (int3), so the
# same redirect is made in our object by renaming the symbol (the lists name EVERY original site in these functions, so the rename is exactly the patch).
# B3: message_box_update (dwords 0x445bf9 / 0x445eeb = its two g_frame_seconds reads) and app_main_loop (hooks 0x4a15e9 input_update -> input_hook, 0x4a15fd app_frame_wait -> freeze_hook, 0x4a1602 timer_ticks ->
# frame_end_ticks; the C loop calls timer_ticks twice, frame_end_ticks fast-forwards only the call right after freeze_hook, see freeze.c).
# Without HARNESS nothing changes.  harness_drift.py checks that each such object references the harness symbol.
[ -n "$HARNESS" ] && nm -A -g --defined-only $(cut -d' ' -f2 $W/objs_local.txt) 2>/dev/null | awk '{n=index($1,":"); o=substr($1,1,n-1)} $2=="T" && $3=="_app_state_menu_render" {print o, "_RwCameraBeginUpdate=_begin_update_hook"} $2=="T" && ($3=="_menu_update" || $3=="_menu_items_animate_update" || $3=="_message_box_update") {print o, "_g_frame_seconds=_fast_ui_seconds"} $2=="T" && $3=="_app_main_loop" {print o, "_app_frame_wait=_freeze_hook"; print o, "_timer_ticks=_frame_end_ticks"; print o, "_input_update=_input_hook"}' | while read -r obj ren; do objcopy --redefine-sym $ren $obj; done
# T-N13: the source-built libs' own initialised .data (zlib 0x14fc B, ...) would likewise sit between the generated .data and .bss and give the bss a bigger delta than .data (harness / race scripts use ONE delta: the
# game then ran with the harness patches at wrong bss addresses = fullscreen ChangeDisplaySettings, exit): own section behind everything
[ -n "$LSRC$CRTSPLIT" ] && while read -r src obj; do case $src in lib:*|stub:*|pull:*) objcopy --rename-section .data=.ldat $obj;; esac; done < $W/objs_local.txt
# IMPLIBS=1 (S2): the import table is built by the linker from generated import libs (tools/mk_importlibs.py): every import site references __imp__Name@N, the original IAT and import tables are dropped
IMPLIBS_ARG=; IMPLIBS_LNK=
if [ -n "$IMPLIBS" ]; then
  if [ -n "$NOEXE" ] && [ -z "$IMPTAB" ]; then cp -r $REF/imp $W/imp      # mk_importlibs.py reads MFL.exe's import table: REF's libs
  else OBJLIST=$W/objs_local.txt python3 $G/tools/mk_importlibs.py $W/imp ${IMPTAB:+--table $IMPTAB} > $W/mk_importlibs.log 2>&1; fi      # SK2: IMPTAB = committed .def table, no exe
  IMPLIBS_ARG="--imports-as-symbols $W/imp"; IMPLIBS_LNK="/libpath:imp $(cd $W && ls imp/*.lib | tr '\n' ' ')"
fi
# GEN_DATA=1 (S5, needs IMPLIBS=1): the data layout comes from tools/globals/globgen.py (app data and library spans as generated C), mkobj --drop-app removes .rdata/.data/.bss and the replaced function bodies from the image object
GEN_ARG=; GEN_OBJ=
if [ -n "$GEN_DATA" ]; then
  mkdir -p $W/gen
  if [ -n "$NOEXE" ] && [ -z "$DSDATA" ]; then cp $REF/gen/rdata.obj $REF/gen/data.obj $REF/gen/bss.obj $W/gen/      # globgen.py reads MFL.exe: the data objects of REF (no committed data given)
  else
  if [ -n "$NOEXE" ]; then python3 -I $G/tools/globals/dsform.py join $DSDATA $W/gen ${V143:+--held-zero} || exit 1      # W16 DS: the committed data (data/link/gen) = globgen's exact text
  else
  python3 -I $G/tools/globals/globgen.py $W/gen > $W/gen/globgen.log 2>&1 || { tail -5 $W/gen/globgen.log; exit 1; }
  [ -n "$V143" ] && { python3 -I $G/tools/globals/dsform.py split $W/gen $W/ds && python3 -I $G/tools/globals/dsform.py join $W/ds $W/gen --held-zero || exit 1; }      # CC=v143: held rows {0} as in CI
  fi
  # W16 CIN cin2: v143 cl orders and pads the generated definitions differently (gen .rdata rotated, .data +8 B, .bss +38 B; link FAIL "not 0 mod 16"),
  # so the pinned data layout is compiled by mingw gcc in source order (-fno-toplevel-reorder, ABI alignment only, zero rows kept in .data / .rdata),
  # section alignment set to the VC7 objects' (16 / 8 / 8).  agents/cin_w16/gensec.py: bytes, relocations and symbols SAME as the VC7-compiled objects.
  if [ -n "$V143" ]; then
  GCCF="-c -O0 -w -fpermissive -fno-toplevel-reorder -fno-common -malign-data=abi -fno-ident -fno-asynchronous-unwind-tables"
  (cd $W/gen && for x in rdata data bss; do i686-w64-mingw32-gcc $GCCF $([ $x = bss ] || echo -fno-zero-initialized-in-bss) -o $x.obj $x.c > cl_$x.log 2>&1 || cat cl_$x.log >> cl.log; done)
  (cd $W/gen && for x in rdata data bss; do al=".$x=8"; [ $x = rdata ] && al=".rdata=16"; [ $x = data ] && al=".data=8 --set-section-alignment .rdata=8"
    objcopy @objcopy_$x.txt --set-section-alignment $al $x.obj $x.o2 && mv $x.o2 $x.obj; done)      # @file: 2,268 options exceed the Windows command line on the runner
  # W16 MT2: the image's .data and .bss are one span (g_cup_64cbe8, 31 x 0x68 B cup scene entries, runs 0x64cbe8..0x64d880 across the
  # 0x64d000 boundary).  .data$zz sorts data.obj behind every other .data contribution, so bss.obj follows it with no gap.  Without it the
  # CRT .data sat in that gap and cup_scene_compute_bounds wrote floats over it: /MT 0xb40 B (__acrt_initial_locale_data: UCRT page fault),
  # /MD 0xa0 B (msvcrt.lib __security_cookie_complement, __isa_enabled ...).  Both CRT forms (v143md and v143mt).
  # Page aligned as in the image (its size is 0x52000): the end is page aligned too, so bss.obj's .bss starts right at it.
  objcopy --set-section-alignment .data=4096 --rename-section '.data=.data$zz' $W/gen/data.obj $W/gen/data.o2 && mv $W/gen/data.o2 $W/gen/data.obj || exit 1
  else
  (cd $W/gen && VC7_CONTAINER=ac $G/tools/vc7.sh $W/gen cl.exe /nologo /c /Od /Zl rdata.c data.c bss.c > cl.log 2>&1; for x in rdata data bss; do objcopy $(tr '\n' ' ' < objcopy_$x.txt) $x.obj $x.o2 && mv $x.o2 $x.obj; done)
  fi
  [ -f $W/gen/bss.obj ] || { echo "gen compile failed"; tail -5 $W/gen/cl.log; exit 1; }
  fi
  GEN_ARG="--drop-app $W/gen"; GEN_OBJ="gen/rdata.obj gen/data.obj gen/bss.obj"
fi
# S3: the resources come from output/res/mfl.res (tools/res/build_res.sh) on the link line, pefix appends no .rsrc.  RES=<path> overrides, RES=0 keeps the old pefix .rsrc
# W10 A5: RES=none = no resources at all (no .res on the line, NO_RSRC=1: pefix appends nothing); the exe reads them from MFL_original.exe beside it (Z-rsrc, prepare.py)
# W10 GZ: RES=none is the default of GEN_DATA links (src/mfl_resources.c); RES=$G/output/res/mfl.res = the old S3 form (control for this window)
RESF=; [ -z "$RES" ] && [ -n "$GEN_DATA" ] && RES=none
[ "$RES" = none ] && { RES=; export NO_RSRC=1; }
[ "$RES" = 0 ] && RES=; [ -n "$RES" ] && { cp $RES $W/mfl.res; RESF=mfl.res; export NO_RSRC=1; }
# W16 DS2: _mfl_held (held-row table, dsform join --held-zero -> gen/data.obj) is in no symbols.txt and mkobj counts no gen definition as defined: leave it to the linker
[ -n "$V143" ] && [ -z "$NOEXE" ] && { python3 $G/tools/msvcw/v143md_plan.py $W | tail -1; echo _mfl_held >> $W/v143/crt_extern.txt; MKOBJ_ARGS="$MKOBJ_ARGS --crt-extern $W/v143/crt_extern.txt --crt-names $W/v143/crt_names.txt --crt-redirect $W/v143/crt_redirect.txt"; }      # CRT names -> the v143 CRT libs (V143c8: the image's labels of CRT names get _vc7; V143e1: the image's VC7 CRT entries jmp to them)
python3 $T/mkobj.py $W/mfl.obj --exe $W/mfl_patched.exe --objs $W/objs_local.txt ${PAD:+--pad $PAD} $HMK $IMPLIBS_ARG $GEN_ARG $MKOBJ_ARGS ${NOEXE:+--table $TABLE} 2> $W/mkobj.err | tee $W/mkobj.log | head -12
[ -n "$LSRC$CRTSPLIT" ] && python3 $G/tools/delink_sdk/undup.py $W/objs_local.txt      # A5: library functions our C defines as well get another name after mkobj (it resolves the references to dropped lib code by the lib objects' names), so no LNK4006
# MKRANGE=1 (Brief G): the code of the image goes into one object per layer (mkrange.py: RenderWare.obj, app.obj, ...), blocks `.text$NNNNN` that link.exe sorts into address order; our own plain `.text`
# must sort behind them (.text$zz)
MFLOBJ=mfl.obj
[ -n "$MKRANGE" ] && { python3 $T/mkrange.py $W/mfl.obj $W/layers > $W/mkrange.log || { tail -3 $W/mkrange.log; exit 1; }; python3 $T/mkrange.py --rename-text $W/o/*.obj
  MFLOBJ="layers/rest.obj $(cd $W && ls layers/*.obj | grep -v rest.obj | tr '\n' ' ')"; }
python3 $T/objnames.py --rsp $W/objs_local.txt > $W/link.rsp      # W11 B2: our C by name (byte order), then the extra objects in chain order (was `ls o/*.obj`: locale order of numbers)
# LIB_RW: the thunk object goes FIRST so its definition wins over our own C definition of the same function (link.exe keeps the first one; LNK4006 "second definition ignored" otherwise depends on the file name sort)
[ -n "$LIB_RW" ] && { T1=$(awk '$1=="rw:thunks"{print $2}' $W/objs_local.txt); T1=${T1#$W/}; { echo $T1; grep -vxF $T1 $W/link.rsp; } > $W/link.rsp.new && mv $W/link.rsp.new $W/link.rsp; }
if [ -n "$V143" ]; then      # MSVC 14.44 link, same layout switches (+ the v143 defaults that /fixed and the VC7 objects need off); CRT = msvcrt (start-up) + vcruntime
  # + ucrt (+ legacy_stdio_definitions: printf / scanf family for our own prototypes, oldnames); SDK import libs AFTER ours: only the start-up's extra kernel32 names
  # V143c5: the SDK libs by absolute path (a bare kernel32.lib finds /libpath:imp first = our own import lib again, so the SDK was never read);
  # (the 11 ___vcrt_* / ___acrt_* / __is_c_termination_complete start-up names: tools/msvcw/v143md_shim.c, V143c6); legacy_stdio_wide_specifiers.lib: legacy_stdio_definitions /include:s its symbol
  # (VC7 wide printf semantics, %s = wide string in the w-functions)
  if [ -n "$MSVCW_NATIVE" ]; then      # W16 CIN, CI runner: the SDK of the VS environment (WindowsSdkDir has spaces): the 4 libs copied below the work dir, a relative path on the link line
    mkdir -p $W/v143/um; for l in kernel32 user32 gdi32 advapi32; do cp "$(cygpath -u "$WindowsSdkDir")Lib/${WindowsSDKLibVersion%\\}/um/x86/$l.lib" $W/v143/um/ || exit 1; done; UM=v143/um
  else
  UM=/opt/msvc/kits/10/Lib/$(cd $HOME/.cache/mfl/msvc-wine/msvc/kits/10/Lib && ls -d */ | tail -1)um/x86
  fi
  LO=; [ -s $W/mfl.obj.linkopts ] && LO=@mfl.obj.linkopts      # (a failing $(...) inside an assignment would stop set -e)
  LNKCMD="/opt/msvc/bin/x86/link /nologo /machine:x86 /subsystem:windows /entry:$ENTRY /fixed /base:$B /nodefaultlib /force:multiple /dynamicbase:no /safeseh:no /opt:noicf $LINKFLAGS /map:ours.map /out:ours.exe $MFLOBJ $GEN_OBJ $RESF @link.rsp $IMPLIBS_LNK $CRTL legacy_stdio_definitions.lib legacy_stdio_wide_specifiers.lib oldnames.lib $UM/kernel32.lib $UM/user32.lib $UM/gdi32.lib $UM/advapi32.lib $LO"
  $G/tools/msvcw/msvcw_sh.sh $W "$LNKCMD" > $W/link.log 2>&1 || true
  # W16 MT: names a libucrt / libvcruntime member defines as well as ours (VC7 CRT data of gen/, image labels) -> ours renamed _vc7, link again
  if [ $CRTF = /MT ]; then cp $W/link.log $W/v143/link_pass1.log; python3 $G/tools/msvcw/v143mt_dups.py $W || exit 1; $G/tools/msvcw/msvcw_sh.sh $W "$LNKCMD" > $W/link.log 2>&1 || true
    python3 $G/tools/msvcw/v143mt_dups.py --check $W || exit 1; fi
  grep -E "LNK(4006|2005|2001|2019|4217|4286)" $W/link.log > $W/v143/lnk_pairs.txt || true
  echo "v143 link: $(grep -c . $W/v143/lnk_pairs.txt) LNK4006/2005/2001/2019/4217/4286 lines -> $W/v143/lnk_pairs.txt"
else
VC7_CONTAINER=ac $G/tools/vc7.sh $W link.exe /nologo /machine:ix86 /subsystem:windows /entry:$ENTRY /fixed /base:$B /nodefaultlib /force:multiple $LINKFLAGS /map:ours.map /out:ours.exe $MFLOBJ $GEN_OBJ $RESF @link.rsp $IMPLIBS_LNK $([ -s $W/mfl.obj.linkopts ] && echo @mfl.obj.linkopts) > $W/link.log 2>&1 || true
fi
echo "link: $(grep -c -E "error" $W/link.log || true) errors, $(grep -c LNK4006 $W/link.log || true) LNK4006"; grep -E "error" $W/link.log | head -8
grep -q "^not found:" $W/mkobj.err && { grep -h "^not found:" $W/mkobj.err | head -5; echo "mkobj: references not found"; exit 1; }
[ -f $W/ours.exe ] || { echo "link failed"; exit 1; }
# the generated .rdata (first symbol g_pespan_005dd000) must start 0 mod 16: SSE tables (_pow_pentium4 movapd) fault otherwise (T-N11)
[ -n "$GEN_DATA" ] && { A=$(awk '$2=="_g_pespan_005dd000"{print $3}' $W/ours.map); [ -n "$A" ] && [ $((0x$A % 16)) -eq 0 ] || { echo "FAIL: generated .rdata starts at 0x$A, not 0 mod 16"; exit 1; }; }
[ -n "$V143" ] && awk '$2=="__rng_00401000"{print "v143 layout: first image block at " $3 " (pinned = 00401000)"}' $W/ours.map
if [ -z "$NOEXE" ]; then      # NOEXE: pefix is a no-op in the release form (IMPLIBS + NO_RSRC; link_ci.sh's cmp proves it), no install clone
python3 $T/pefix.py $W/ours.exe $W/mfl_patched.exe >/dev/null
python3 $G/tools/wine-run/prepare.py $INST $W/game $W/ours.exe $W/ours.map >/dev/null      # movie stubs also at our C copies (found through the link map)
[ -n "$LIB_RW" ] && cp $RW_DLL $W/game/App_Executables/mflrw.dll      # next to ours.exe (Wine loads it from the exe's directory)
[ -f $W/game/App_Executables/MFL_original.exe ] || { echo "FAIL: no MFL_original.exe beside the exe in $W/game/App_Executables (prepare.py; the resources are read from it at run time)"; exit 1; }
fi
D=$(objdump -h $W/ours.exe | python3 -c "import sys; print('%x' % next(int(l.split()[3], 16) - 0x5fb000 for l in sys.stdin if len(l.split()) > 3 and l.split()[1] == '.data'))")
[ -n "$V143" ] && D=$(python3 -c "import sys; print(next('%x' % (int(l.split()[2], 16) - 0x5fb03c) for l in open(sys.argv[1]) if l.split()[1:2] == ['_g_data_5fb03c']))" $W/ours.map)      # W16 MT2: data.obj sits behind the CRT .data, not at the section start
echo "linked: $W/ours.exe (.data moved by 0x$D: the script delta)"
[ -n "$HARNESS" ] && [ "$D" != "${DELTA:-b000}" ] && { echo "DELTA mismatch: rerun with DELTA=$D"; exit 2; }
[ -n "$RUN" ] && $T/race_relinked.sh $W/game $W/r ${D} ${COURSE:-0}
true
