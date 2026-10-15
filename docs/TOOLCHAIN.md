# Toolchain and verification

## What built `MFL.exe`
- Link time 2005-06-30, linker 7.0, game objects compiled with `cl` 13.00.9466 (Visual C++ .NET 2002), taken from the PE Rich header.
- The statically linked RenderWare 3.7 libraries were built with `cl` 12.00.9044 (Visual C++ 6) and are byte identical to the SDK's own release libs.
- Third-party code: Lua 4.0, qhull 2003.1 (built `/O2 /G5 /Ob0 /DNDEBUG`), zlib and libpng (via the RenderWare libs).

## Running the original compiler
`tools/vc7.sh <workdir> cl.exe /c /O2 file.c` runs the VC .NET 2002 tools under Wine in Docker (`tianon/wine`). You supply the compiler (`toolchain/vc7`, or `VC7_ROOT`).
`TMPDIR` (default `/tmp/claude-<uid>`) and the repo's parent directory are mounted into the container at the same paths.

## Comparing a function with the exe
`tools/cmpfn.py <file.c> <symbol> <VA hex> [cl flags]` compiles one function (`/Gy /O2` default), extracts its code from the COFF object and compares it with the bytes at that address of `MFL.exe` (relocation fields masked). `cmpfn.py --manifest src/manifest.txt` checks every listed function.

## Match statuses (`src/manifest.txt`)
- `MATCH`: byte identical.
- `EFFECTIVE`: instruction streams differ only in register names.
- `NEAR`: operands or structure differ; kept because it compiles and behaves like the original in the regression runs (reason in the manifest note).
- `DIFFERENT`: compiled and kept, status decided at check time.

## Hybrid exe and regression runs
`tools/build_overlay.py` compiles the C sources and adds them to a copy of `MFL.exe` as a new PE section, with a `jmp` at the original entry of each replaced function. The result is a game that runs the C versions.
`tools/wine-run/regress.sh` runs the original and the hybrid through the same scripted race under Wine, freezes at the same frame and compares memory dumps (deterministic, bit exact on the normal courses).
