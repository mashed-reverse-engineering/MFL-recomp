# Building / requirements

Nothing here builds without a game copy.

- `MFL.exe` of Mashed: Fully Loaded (check with `python3 tools/check_exe.py`). Put it (or a symlink) at `game/MFL.exe`, or set `MFL_EXE`.
- Python 3.
- Ghidra 12.1.4 + JDK 21 for the naming scripts (`GHIDRA_HOME` where a tool needs it).
- For byte-exact verification (later commits): Docker with the `tianon/wine` image and the Visual C++ .NET 2002 toolchain (cl 13.00.9466) extracted to `toolchain/vc7` and the Visual C++ Toolkit 2003 (cl 13.10) to `toolchain/vc71`.
- `TMPDIR`: scratch directory that is mounted into the containers (default `/tmp/claude-<uid>`; set your own if you prefer).

## Drop-in executable (`tools/build_mflng.sh`)

`tools/build_mflng.sh <your MFL.exe> <out dir>` builds `MFLng.nofix.exe` (every `MFL_FIX_*` switch off: original behaviour, the build the regression gate checks) and `MFLng.exe` (every switch in `src/mfl_fix.h` on; not gated; the switches are listed in [MFL_FIX_FLAGS.md](MFL_FIX_FLAGS.md)). Put `MFLng.exe` (or `MFLng.nofix.exe`) and `mflrw.dll` beside the game's `MFL.exe`: the resources and the library tables are read from your original at run time. The script checks these prerequisites first and prints one line per missing item:

1. Your `MFL.exe` (checked with `tools/check_exe.py`) inside the game install (the build reads the install's other files too).
2. Docker with `tianon/wine` at the digest pinned in `tools/vc7.sh`.
3. Visual C++ .NET 2002 (cl / link 13.00.9466) extracted to `toolchain/vc7`, including `Vc7/lib/libcmt.lib`. Not shipped here.
   Leave `VC7_ROOT` unset (the script refuses it: `tools/vc7.sh` would use it for the cl 13.10 files as well); a symlink at `toolchain/vc7` is fine.
4. Visual C++ Toolkit 2003 (cl 13.10) extracted to `toolchain/vc71` (`Vc7/bin/cl.exe`): the 16 sources marked `/* compile: cl1310 */` were built with it. Not shipped here.
5. mingw-w64 (`i686-w64-mingw32-gcc`, `-as`) and GNU binutils (`objcopy`, `objdump`, `nm`).
6. `mflrw.dll` from the companion repository librw-mfl, built with its `tools/build_mflrw.sh` (or `RW_DLL=<path>`).
7. Upstream library sources in `MFL_REFS` (default: `upstream/` beside the repository): Lua 4.0 (`lua/src40`), qhull 2002.1 (`qhull/blender240`), zlib 1.1.3 (`zlib/src`), libpng 1.0.12 (`libpng/src`).

The exe loads the sound and movie DLLs (`mfl_audio_miniaudio.dll`, `mfl_video_ffmpeg.dll` with the FFmpeg DLLs) from the folder `mflng\` beside the exe first, then from beside the exe. The release zips use `mflng\`. `tools/build_mflng.sh` does not build these DLLs or create the folder: to use them with your own build, copy the `mflng` folder from a release zip of the same source version.

No DirectX SDK is needed (the import libraries are generated from `names/MFL_import_protos.txt`).
