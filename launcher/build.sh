#!/bin/bash
# build.sh <mode> <out dir> [version]: the MFLng launcher (MFLng-launch.exe, static CRT, system DLLs only, no third-party code).
#   msvc  : cl 19.44 x86 /MT through tools/msvcw/msvcw_sh.sh: the msvc-wine image locally, MSVCW_NATIVE=1 = cl / rc / link from PATH (CI)
#   mingw : i686-w64-mingw32-gcc (local check build, static libgcc)
#   test  : native gcc build of test_logic with AddressSanitizer + UBSan; run it with <releases.json> <zip> <tag> [deflate vectors]
# The exe holds two kinds of resource: RT_MANIFEST = data/link/MFLng.manifest as committed (asInvoker; no_original.py --release wants it
# byte-equal; a Windows checkout has it with CRLF, so the committed bytes come from git when there is one) and our application icon
# (RT_ICON / RT_GROUP_ICON from assets/icon/mflng.ico; pe_check.py --icon wants the images byte-equal).
set -e
M=$1 O=$(mkdir -p "$2" && cd "$2" && pwd) V=${3:-dev}
case $V in *[!A-Za-z0-9._-]*) echo "build.sh: version '$V' may hold only A-Z a-z 0-9 . _ -"; exit 2 ;; esac
L=$(cd "$(dirname "$0")" && pwd); R=$(dirname "$L")
git -C "$R" show HEAD:data/link/MFLng.manifest > "$O/mflng.manifest" 2>/dev/null || cp "$R/data/link/MFLng.manifest" "$O/mflng.manifest"
cp "$R/assets/icon/mflng.ico" "$O/mflng.ico"
printf '1 24 "mflng.manifest"\n1 ICON "mflng.ico"\n' > "$O/launcher.rc"      # the icon: our own drawing (assets/icon/), the exe's application icon
LIBS="user32 gdi32 shell32 advapi32 winhttp kernel32"
case $M in
  test)
    gcc -O1 -g -Wall -fsanitize=address,undefined -fno-sanitize-recover=all -o "$O/test_logic" "$L/test_logic.c" "$L/logic.c" ;;
  mingw)
    (cd "$O" && i686-w64-mingw32-windres -O coff -o launcher_res.o launcher.rc)
    i686-w64-mingw32-gcc -Os -Wall -municode -mwindows -static -DLAUNCHER_VER=$V -o "$O/MFLng-launch.exe" \
      "$L/launcher.c" "$L/logic.c" "$O/launcher_res.o" $(for l in $LIBS; do echo -l$l; done)
    i686-w64-mingw32-strip "$O/MFLng-launch.exe" ;;
  msvc)
    # relative paths from the out dir: the native (msys2, MSYS2_ARG_CONV_EXCL='*') form passes arguments unconverted
    RL=$(realpath --relative-to="$O" "$L"); B=/opt/msvc/bin/x86/
    C="${B}rc /nologo /fo launcher.res launcher.rc && ${B}cl /nologo /O1 /MT /W3 /GS /DUNICODE /D_UNICODE /D_CRT_SECURE_NO_WARNINGS /DLAUNCHER_VER=$V"
    C="$C /Fe:MFLng-launch.exe $RL/launcher.c $RL/logic.c launcher.res /link /SUBSYSTEM:WINDOWS /MANIFEST:NO /DYNAMICBASE /NXCOMPAT"
    C="$C $(for l in $LIBS; do printf '%s.lib ' $l; done)"
    bash "$R/tools/msvcw/msvcw_sh.sh" "$O" "$C" ;;
  *) echo "usage: build.sh msvc|mingw|test <out dir> [version]"; exit 2 ;;
esac
echo "build.sh $M: done in $O"
