# Building / requirements

Nothing here builds without a game copy.

- `MFL.exe` of Mashed: Fully Loaded (check with `python3 tools/check_exe.py`). Put it (or a symlink) at `game/MFL.exe`, or set `MFL_EXE`.
- Python 3.
- Ghidra 12.1.4 + JDK 21 for the naming scripts (`GHIDRA_HOME` where a tool needs it).
- For byte-exact verification (later commits): Docker with the `tianon/wine` image and the Visual C++ .NET 2002 toolchain (cl 13.00.9466) extracted to `toolchain/vc7` (or `VC7_ROOT`).
- `TMPDIR`: scratch directory that is mounted into the containers (default `/tmp/claude-<uid>`; set your own if you prefer).
