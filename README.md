# MFL-recomp

A readable C recompilation of **Mashed: Fully Loaded** (`MFL.exe`, PC, 2004).

## Status

- A Windows x86 build made from this source starts and runs all 15 courses in the regression runs.
- None of its machine code is taken from the original executable any more: 0 of 1,945 KB (measured 2026-11-15 on the standalone and the release build). The code comes from the C in this repository, from open-source libraries, or was removed after proving it is never used.
- The build contains no original resources (0 B, measured 2026-11-15). Dialogs, texts and pictures are read at runtime from your own copy of `MFL.exe`.
- Not covered by the two points above: library tables copied from the original are still copied bytes (4,466 B of initialised data plus 1,408 B of padding between data rows, measured 2026-11-17). The library variables (.bss) are already named C definitions.
- The source is not yet pleasant to read in all places: it still carries scaffolding from the byte-matching work (see [docs/ROADMAP.md](docs/ROADMAP.md)).
- Numbers per layer and how they moved: [docs/PROGRESS_HISTORY.md](docs/PROGRESS_HISTORY.md).

## What this project is

The goal is a playable recompilation with **readable, maintainable source** that can take bug fixes and enhancements, and later be ported (a Switch homebrew port via libnx is the long term target).
To get there safely, every function is checked against the original: a function is either *byte identical* to what the original compiler (MSVC 13.00.9466, Visual C++ .NET 2002) produces, or *behaviour verified* by running it side by side with the original code in an emulator (see [docs/VERIFICATION.md](docs/VERIFICATION.md)). This checking is scaffolding; the deliverable is the readable source.

Anything that changes the original behaviour sits behind a switch that is off by default: [docs/MFL_FIX_FLAGS.md](docs/MFL_FIX_FLAGS.md).

## No game files, and how to build

**No game files are included.** You need your own copy of the game; see [docs/LEGAL.md](docs/LEGAL.md).
Supported executable: `MFL.exe` of Mashed: Fully Loaded, SHA-1 `d52ca4a5995875b6d052707188e2c6d065f41766` (`python3 tools/check_exe.py path/to/MFL.exe`).

Drop-in build: `tools/build_mflng.sh <your MFL.exe> <out dir>` makes `MFLng.nofix.exe` (original behaviour, the build the regression gate checks) and `MFLng.exe` (every `MFL_FIX_*` switch on; **not gated**).
Rename the game's `MFL.exe` to `MFL_original.exe` and put `MFLng.exe` (or `MFLng.nofix.exe`) and `mflrw.dll` beside it. The build reads its resources from `MFL_original.exe` and shows a message box naming the expected place when no original is found.
Movies and menu music: the release zip also ships `mfl_video_ffmpeg.dll` and the FFmpeg DLLs it uses (`avformat-*`, `avcodec-*`, `avutil-*`, `swscale-*.dll`: FFmpeg 7.1, LGPL 2.1, unmodified shared libraries, built by `tools/video/build_ffmpeg.sh`; notice and source in `LICENSES/ffmpeg.txt` / `tools/video/SOURCES`). Copy them beside the exe too; without them there are no intro movies, no menu backdrop movie and no menu music.
Prerequisites, in order: [docs/BUILDING.md](docs/BUILDING.md#drop-in-executable-toolsbuild_mflngsh).

Launcher (optional): `MFLng-launch.exe` from the release zip goes into the same folder.
- It shows the picture of the game's own launcher, which it reads at run time from your `launch.exe`.
- It starts `MFLng.exe` or `MFLng.nofix.exe`.
- It can update both, and itself, from the GitHub releases. The update channel is either releases only or releases plus pre-releases.

Source, settings and the update steps: [launcher/README.md](launcher/README.md).

## Milestones

| Milestone | State |
|---|---|
| Name functions, globals and structs of the game code | done for about 96% of the game functions |
| Recompile the game code as C (byte identical or behaviour verified) | done for about 98% of the game functions; the rest are tiny stubs |
| Standalone executable from the C (Lua, qhull, zlib/libpng from open source, no DirectShow) | done; passes the 15-course regression runs |
| Replace the C runtime, the RenderWare physics plugin, RenderWare and D3DX, the streaming audio library | done for code (`librw-mfl`, a clean-room library, is the RenderWare and D3DX replacement) |
| Zero original code | done 2026-11-15 |
| Library data as typed C (copied data to 0 B) | in progress: 5,874 B left on 2026-11-17, down from 134,233 B |
| Self-contained build from the public repository | in progress: the drop-in script exists; the build from an export tree in a clean container is being proven |
| Deterministic replays over whole races | in progress: a race diverges between two runs of the original; the cause is under investigation |
| Readability (module layout, plain calling conventions) | next |
| Bug fixes and quality-of-life options behind switches | started: two switches implemented, more planned |
| Renderers beyond Direct3D 9, Linux build, Switch homebrew port | planned |

## Open

- Copied library data is not yet 0 B (5,874 B).
- Two runs of the same race are not identical, so a whole race cannot yet be used as a regression test. The first difference lies in car physics floats at about 2.4 s of race time.
- Source layout still follows how each function was matched, not the game's modules.
- No controller support beyond DirectInput, no widescreen, no in-game settings menu.
- The build runs from the maintainers' private layout and from the drop-in script; building from a clean checkout of this repository is not yet proven end to end.

Details and the full list: [docs/ROADMAP.md](docs/ROADMAP.md).

## Progress (generated)

The numbers below are regenerated with every source drop by `tools/progress.py`.

<!-- progress:start -->
### Status at a glance

**Game code** (2,794 functions, the part above address 0x4B0000 that is the game itself):

| | Functions | |
|---|---:|:---|
| Named | 2,696 | `███████████████████░` 96% |
| Replaced by C | 2,664 | `███████████████████░` 95% |
| of these byte identical to the original compiler output | 1,387 | `██████████░░░░░░░░░░` 50% |
| of these behaviour verified (differential test, not byte identical) | 1,113 | `████████░░░░░░░░░░░░` 40% |
| of these only weakly verified | 164 | `█░░░░░░░░░░░░░░░░░░░` 6% |

<details><summary>Function level detail, tiers and how the numbers are counted</summary>

**2,794 application functions** (VA < 0x4B0000, the rest is linked library code)

| Stage | Functions | Progress |
|---|---:|:---|
| Named | 2,696 | `███████████████████░` 96% |
| Replaced by recompiled C (all tiers, each function once) | 2,664 | `███████████████████░` 95% |
| ...verified byte identical | 1,387 | `██████████░░░░░░░░░░` 50% |
| ...share of the code bytes that is byte identical | 93,054 B | `███░░░░░░░░░░░░░░░░░` 14% |
| ...behaviour verified, not byte identical (a diffemu EQUAL run on file: >= 20 cases, coverage >= 0.8) | 1,113 | `████████░░░░░░░░░░░░` 40% |
| ...replaced but weakly verified or without evidence (low coverage, DIFF, inconclusive, no run on file; see `data/behaviour_evidence.csv`) | 164 | `█░░░░░░░░░░░░░░░░░░░` 6% |
| ...share of the code bytes | 670,469 B | `████████████████████` 98% |
| ...of these register-argument clusters | 606 | `████░░░░░░░░░░░░░░░░` 22% |
| **Published in this repo** (`src/`: tier 1 + automatch + hand-written + tier 2 + clusters, each function once) | 3,123 | `██████████████████████` 112% |
| ...byte identical (tier 1, automatch, hand-written `MATCH`) | 1,778 | `█████████████░░░░░░░` 64% |
| ...tier 1, `src/tier1` (1935 files) | 1,430 | `██████████░░░░░░░░░░` 51% |
| ...automatch, `src/automatch` (5 files) | 5 | `░░░░░░░░░░░░░░░░░░░░` 0% |
| ...hand-written, `src/*.c` (`manifest.txt`) | 381 | `███░░░░░░░░░░░░░░░░░` 14% |
| ...tier 2, `src/tier2` (1492 files) | 720 | `█████░░░░░░░░░░░░░░░` 26% |
| ...clusters, `src/clusters` (611 files) | 674 | `█████░░░░░░░░░░░░░░░` 24% |
| ...plus linked library code (VA >= 0x4B0000), not in the percentages | 1760 | |
| ...functions present in two kinds (counted once above) | 87 | |

**What the numbers mean.** A function is one VA below 0x4B0000; every row counts each function once, however many files or tiers mention it. A cluster file (`src/clusters/cl_*.c`) can hold several entry functions (`// cluster-entries:`), so *files* and *functions* differ (611 cluster files give 674 functions). *Byte identical* = ledger authority `bytes-reloc`: the original compiler turns the C into the original bytes (relocations checked). *Behaviour verified* = not byte identical, but a differential run against the original code in an emulator (diffemu) ended EQUAL with at least 20 cases and coverage of at least 0.8 on both sides, no unresolved DIFF; the evidence per function is `data/behaviour_evidence.csv` (source, cases, coverage, flags). Rows with the flags `STDIND` (only verified by a sweep whose stack-pop heuristic is known to misbehave) or `LADDER` (a weaker harness setting was needed) are still counted, see the flag counts in `progress/progress.json`. It is NOT a statement that the whole game is verified, and the share of *bytes* that is byte identical is far lower than the share of *functions*.

**Layers.** All rows here are application code (VA < 0x4B0000). The library layers are in the table above; see [docs/RW_LIBRARY_LAYER.md](docs/RW_LIBRARY_LAYER.md), [docs/CRT_LAYER.md](docs/CRT_LAYER.md) and [docs/VIDEO_INTERFACE.md](docs/VIDEO_INTERFACE.md).

*Whole pipeline* rows above count everything the private working tree has replaced; *Published* rows are the part whose C source is in this repo (see [docs/VERIFICATION.md](docs/VERIFICATION.md)).


<details><summary>By tier (whole pipeline, each function once)</summary>

| Tier | Functions | Code bytes | Verified byte identical |
|---|---:|---:|---:|
| agent | 109 | 8,446 | 109 |
| bridge | 4 | 1,643 | 0 |
| cluster (not in ledger) | 518 | 326,359 | 0 |
| hand | 381 | 14,744 | 344 |
| tier1 | 934 | 71,946 | 934 |
| tier2 | 718 | 247,331 | 0 |

</details>

</details>
<!-- progress:end -->

## Contents

| Path | What |
|---|---|
| `names/` | Function, global and struct names for `MFL.exe` as CSV / struct files, applied to a Ghidra project by the scripts below. |
| `scripts/` | Ghidra headless scripts (`RenameFns.java`, `ApplyStructs.java`, `RenameVars.java`, ...). |
| `tools/` | Compile / compare / hybrid-exe / regression tooling (Python, shell, Docker + Wine). See [docs/TOOLCHAIN.md](docs/TOOLCHAIN.md). |
| `src/` | Recompiled game code in C. `src/*.c` + `manifest.txt`: hand-written functions with address and status. `src/tier1/` and `src/automatch/`: byte-identical functions, one per file. `src/tier2/`: behaviour-verified (not byte identical) functions, `names.txt` maps address to file. `src/clusters/`: verified register-argument clusters, `enabled.txt` lists the installed ones. Only sources that use the curated headers `mfl_tier2.h` / `mfl_consts.h` are published. |
| `data/` | Small address tables the tools read (function sizes, library symbol tables). |
| `launcher/` | `MFLng-launch.exe`: C + Win32 launcher and updater (see its README). |
| `docs/` | See the table below. |

| Document | What |
|---|---|
| [docs/ROADMAP.md](docs/ROADMAP.md) | Milestones and open work, grouped, each marked done / in progress / planned. |
| [docs/MFL_FIX_FLAGS.md](docs/MFL_FIX_FLAGS.md) | Every `MFL_FIX_*` switch: what it changes, status, how to turn it on, how to add one. |
| [docs/PROGRESS_HISTORY.md](docs/PROGRESS_HISTORY.md) | Original code left per layer, dated history, how each part is produced. |
| [docs/BUILDING.md](docs/BUILDING.md), [docs/TOOLCHAIN.md](docs/TOOLCHAIN.md) | Build prerequisites and the tools. |
| [docs/VERIFICATION.md](docs/VERIFICATION.md) | What "verified" means and where it stops. Nothing here claims the whole game is verified. |
| [docs/GAME_STRUCTURE.md](docs/GAME_STRUCTURE.md), [docs/STRUCTS.md](docs/STRUCTS.md) | Start here to understand the game: main loop, race phases, cars, camera, course loading, menus, weapons, effects, audio; record layouts. |
| [docs/NAMING.md](docs/NAMING.md), [docs/ENUMS.md](docs/ENUMS.md) | Naming workflow, enum constants. |
| [docs/LEGAL.md](docs/LEGAL.md) | What is and is not included. |

## Acknowledgements

Many thanks to **L2KSolkov** and the contributors of [codfh-win32](https://github.com/L2KSolkov/codfh-win32), a Win32 port of Call of Duty: Finest Hour. Its `src/rwa/` directory is a byte-identical decompilation of Criterion's RenderWare Audio (RWA), the library behind the audio layer of `MFL.exe`, and its `src/rw/` directory holds decompiled RenderWare Graphics core and plugin sources. It let us identify both libraries and match a large part of our audio code exactly with the same compiler. The files `src/tier2/an_codfh_*.c` are adapted from it, not byte-identical copies: each file header names the codfh-win32 file and commit it comes from and lists what we changed (calling convention, inlined helpers, alignment, `MFL.exe` data addresses). They are checked against the original machine code like every other tier-2 file. Where we use or adapt other code from it, the file header says so and names the repository.

Many thanks to **tgsm** and the contributors of [pbg](https://github.com/tgsm/pbg), the CC0-licensed decompilation of Piglet's Big Game. The RWA headers `src/tier1/rwa.h` and `src/tier1/rwaint.h` take the basic RenderWare types, `RwLLLink`, `RwV3d`, `RwaUUID`, `RwaLLNode`, `RwaParamType`, `RwaParamTypeHandle` and `RwaType` from its `include/rwsdk` and `include/rwa` headers; the other structures there hold only the members our sources use, at the offsets measured in `MFL.exe`.

## Naming data

Apply with Ghidra 12.1.4 (headless) in this order: `RenameFns.java`, `ApplyStructs.java`, `RenameVars.java`. Details: [docs/NAMING.md](docs/NAMING.md).

## Naming conventions in the source

- `FUN_xxxxxxxx` is a function whose role is not known yet; the hex number is its address in `MFL.exe`. It is the name the decompiler gave it, and the same name appears at the definition, so you can search for it. Once a role is found it gets a real name.
- A name ending in an address (for example `shadow_project_to_ground_00499dc0`) is a named function whose address suffix keeps the name unique and ties it to the original.
- A comment marked `(inferred)` is a guess from reading the code (what a value or call is for), not something confirmed by the original or by a debug symbol. Comments without the marker describe what the code visibly does.
