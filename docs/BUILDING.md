# Building / requirements

Nothing here builds without a game copy.

- `MFL.exe` of Mashed: Fully Loaded (check with `python3 tools/check_exe.py`). Put it (or a symlink) at `game/MFL.exe`, or set `MFL_EXE`.
- Python 3.
- Ghidra 12.1.4 + JDK 21 for the naming scripts (`GHIDRA_HOME` where a tool needs it).
- For byte-exact verification (later commits): Docker with the `tianon/wine` image and the Visual C++ .NET 2002 toolchain (cl 13.00.9466) extracted to `toolchain/vc7` (or `VC7_ROOT`).
- `TMPDIR`: scratch directory that is mounted into the containers (default `/tmp/claude-<uid>`; set your own if you prefer).

## Drop-in executable (`tools/build_mflng.sh`)

`tools/build_mflng.sh <your MFL.exe> <out dir>` builds `MFLng.exe` (every `MFL_FIX_*` switch off: original behaviour, the build the regression gate checks) and `MFLng_fixes.exe` (every switch in `src/mfl_fix.h` on; not gated). Rename the game's `MFL.exe` to `MFL_original.exe` and put `MFLng.exe` and `mflrw.dll` beside it: the resources and the library tables are read from your original at run time. The script checks these prerequisites first and prints one line per missing item:

1. Your `MFL.exe` (checked with `tools/check_exe.py`) inside the game install (the build reads the install's other files too).
2. Docker with `tianon/wine` at the digest pinned in `tools/vc7.sh`.
3. Visual C++ .NET 2002 (cl / link 13.00.9466) extracted to `toolchain/vc7` (or `VC7_ROOT`), including `Vc7/lib/libcmt.lib`. Not shipped here.
4. mingw-w64 (`i686-w64-mingw32-gcc`, `-as`) and GNU binutils (`objcopy`, `objdump`, `nm`).
5. `mflrw.dll` from the companion repository librw-mfl, built with its `tools/build_mflrw.sh` (or `RW_DLL=<path>`).
6. Upstream library sources in `MFL_REFS` (default: `upstream/` beside the repository): Lua 4.0 (`lua/src40`), qhull 2002.1 (`qhull/blender240`), zlib 1.1.3 (`zlib/src`), libpng 1.0.12 (`libpng/src`).

No DirectX SDK is needed (the import libraries are generated from `names/MFL_import_protos.txt`).
