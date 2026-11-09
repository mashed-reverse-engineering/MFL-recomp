# MFL-recomp

A readable C recompilation of **Mashed: Fully Loaded** (`MFL.exe`, PC, 2004).

**Status:** work in progress. A Windows x86 build made from this source starts and runs all 15 courses in the regression runs; about 91% of its code is no longer taken from the original executable (174 KB of 1,945 KB is still original machine code, measured 2026-11-09), the rest comes from the C in this repository, from open-source libraries, or has been removed after proving it is never used. Data and resources are still copied bytes. [Follow the progress](#follow-the-progress) below shows the numbers per layer and how they moved.

## What this project is

The goal is a playable recompilation with **readable, maintainable source** that can take bug fixes and enhancements, and later be ported (a Switch/ARM port is the long term target).
To get there safely, every function is checked against the original: a function is either *byte identical* to what the original compiler (MSVC 13.00.9466, Visual C++ .NET 2002) produces, or *behaviour verified* by running it side by side with the original code in an emulator (see [docs/VERIFICATION.md](docs/VERIFICATION.md)). This checking is scaffolding; the deliverable is the readable source.

**No game files are included.** You need your own copy of the game; see [docs/LEGAL.md](docs/LEGAL.md).
Supported executable: `MFL.exe` of Mashed: Fully Loaded, SHA-1 `d52ca4a5995875b6d052707188e2c6d065f41766` (`python3 tools/check_exe.py path/to/MFL.exe`).

## Roadmap

| Milestone | State |
|---|---|
| Name functions, globals and structs of the game code | done for about 96% of the game functions |
| Recompile the game code as C (byte identical or behaviour verified) | about 98% of the game functions; the rest are tiny stubs |
| Build a standalone executable from the C (jump stubs only for game code), with Lua, qhull and zlib/libpng built from open-source, no DirectShow | **done**, passes the 15-course regression runs |
| Replace the C runtime (libcmt) | mostly done; the math tables are left |
| Rewrite the RenderWare physics plugin (rwp) as verified C | nearly done; 1.6 KB of original code left |
| Replace RenderWare with a clean-room library (`librw-mfl`), then D3DX | in progress; D3DX is down to 1.2 KB, RenderWare to 152 KB (math, streaming, collision, spline/animation, bezier/patch, vertex-declaration, mesh building, image, PNG reading, 2D, LOD, D3D9 render-state, image conversion, 2D renderer, light, transform, raster, world pipeline and native texture groups are swapped bit-exactly, never-used code is removed; RenderWare memory now comes from the game's heap); what is left is mostly the stateful engine core and more D3D9 device code, which needs the engine switch |
| Replace the streaming audio library, add a real video backend | audio in progress (19 KB left, from 101 KB: the DirectSound output sits behind a small audio interface, the DirectShow/codec path is removed, voices, mixer and converters are C); the FFmpeg video backend (separate DLL) plays the intro and menu movies under Wine in our test runs, but is not shipped in this repository |
| Zero original code, then readability and bug fixing, then the Switch/ARM port | planned |

## How each part is produced

| Part | Approach |
|---|---|
| Game code, byte-identical (tier 1, automatch, hand-written, agent) | **Byte matching** with the original compiler (MSVC .NET 2002, 13.00.9466), checked function by function with a relocation-aware compare. This is scaffolding: it decides which functions are trusted fully. |
| Game code, behaviour-verified (tier 2) | **Differential testing**: the original bytes and our compiled C run side by side in an emulator on the same inputs. |
| Game code with register arguments (clusters) | Readable C behind a small entry stub, checked by a cluster sweep. |
| Lua, qhull, zlib, libpng, TIFF | **Open source**, built from source. |
| C runtime | Microsoft's VC7 `libcmt` for now; the libc step comes with the port. |
| Movie playback | The DirectShow code is dropped; the game calls a small video interface (blank backend, a real one later). |
| RenderWare physics plugin | Our hand-written C, differentially tested. |
| RenderWare graphics and D3DX | `librw-mfl` (our fork of the MIT-licensed [librw](https://github.com/aap/librw)) as `mflrw.dll`: math, streaming, object groups and the D3DX math entry points are swapped in. The rest is planned as a clean-room reimplementation. |
| Audio (RenderWare Audio) | Functions byte-identical to the public [codfh-win32](https://github.com/L2KSolkov/codfh-win32) decompilation, used with attribution; the PC-only DirectShow/DXAudio backend is planned as a platform layer over a swappable audio library. |
| Test harness | Our own code, for tests only; it is not part of the game. |

## Follow the progress

Original machine code still linked into the standalone build (the number we drive to zero), measured with `tools/delink/origbytes.py` at each full regression gate:

| Layer | 2026-11-09 | What is left |
|---|---:|---|
| RenderWare (graphics, world, camera, D3D9) | 152.1 KB | the engine itself; replaced by `librw-mfl` in steps |
| Audio (RenderWare Audio) | 18.5 KB | DirectSound voice and buffer code, thread and semaphore glue |
| rwp (RenderWare physics) | 1.6 KB | CPU feature probes, a few helpers that need hand-written C |
| D3DX | 1.2 KB | |
| C runtime | 0.9 KB | startup and exception frame |
| Game code | 0 KB | done |
| **Total** | **174.3 KB** | of about 1,945 KB (9%) |

How the total moved (code only; data and the resources are tracked separately):

| Date | Original code | What moved it |
|---|---:|---|
| 2026-11-03 | 485.0 KB | |
| 2026-11-04 | 459.3 KB | last exe-side RenderWare replacements |
| 2026-11-05 | 273.0 KB | about 120 new `librw-mfl` entry points in 18 groups (collision, spline/animation, bezier/patch, vertex declarations, mesh building, image, camera and others); D3DX CPU variants removed; audio output layer, mixer, voices and wave code; rwp and C runtime rewrites |
| 2026-11-06 | 246.4 KB | about 40 more audio functions as C; never-run RenderWare stream writers, legacy-format and skin readers removed (each proven unused by 172 coverage runs, an asset format scan and a call-graph check); D3DX half-float code removed; a BMP image reader in `librw-mfl`; RenderWare allocations now come from the game's own heap (a step toward the engine switch) |
| 2026-11-07 | 233.7 KB | 31 more audio functions as C (21 tier 2, 10 register clusters); `librw-mfl` groups for 2D path/brush kernels, PNG reading (libpng inside the DLL) and LOD; five x87 math routines; 179 RenderWare gap functions named so their bodies can be removed; the CRT `raise` function as C (with an exception-frame model in the checker); a few more never-run writers removed with proofs |
| 2026-11-08 | 209.7 KB | three D3D9 driver groups in `librw-mfl` (render state, image/raster conversion, 2D renderer with the dynamic vertex buffer ring; 51 functions, 24.0 KB), run on the game's own engine state and checked op for op against the original under Wine; the draw comparison now normalises the dynamic vertex buffer ring |
| 2026-11-08 (second run) | 196.3 KB | three more D3D9 groups in `librw-mfl` (lights, transforms, raster creation and conversion; 26 functions, 12.3 KB) and a first small batch of string/stream helpers adapted from codfh-win32 (1.4 KB; their source is not in this repository yet, pending credit wording and permission) |
| 2026-11-09 | 174.3 KB | four more D3D9 groups in `librw-mfl` (world pipeline, the rest of raster creation, native texture reading, tangent instancing; about 10 KB); 48 more RenderWare core functions adapted from codfh-win32 (memory, streams, errors, dictionaries, matrix/vector, file system, charset, 2D fonts; about 10.5 KB; their source is not in this repository yet, pending credit wording and permission); 1.7 KB of byte-identical RenderWare bodies that only lacked size rows |

Roughly 35 KB of the 2026-11-05 drop is code removed rather than rewritten (DirectShow, unused CPU variants, never-used wave converters): it is proven unused by coverage runs, an asset format scan and a call-graph closure proof, and every removal sits in a list that can be reverted on its own. Treat that part as the least certain. Every `librw-mfl` entry point is checked bit-exact against the original code on at least 100,000 random inputs, with negative tests that must fail.

Where to look:
- [`progress/progress.json`](progress/progress.json) and the generated tables below: function counts per tier, regenerated with every source drop.
- The git log of this repository: one commit per source drop, with the number of published functions.
- [docs/VERIFICATION.md](docs/VERIFICATION.md): what "verified" means and where it stops. Nothing here claims the whole game is verified.
- Next steps, in order: the remaining RenderWare groups in `librw-mfl`, the audio mixer, the engine switch to `librw-mfl`, then the video backend and data, then readability and bug fixing, then the Switch/ARM port.

## Progress (generated)

The numbers below are regenerated with every source drop by `tools/progress.py`.

<!-- progress:start -->
### Status at a glance

**Game code** (2,794 functions, the part above address 0x4B0000 that is the game itself):

| | Functions | |
|---|---:|:---|
| Named | 2,696 | `███████████████████░` 96% |
| Replaced by C | 2,735 | `████████████████████` 98% |
| of these byte identical to the original compiler output | 1,387 | `██████████░░░░░░░░░░` 50% |
| of these behaviour verified (differential test, not byte identical) | 1,184 | `████████░░░░░░░░░░░░` 42% |
| of these only weakly verified | 164 | `█░░░░░░░░░░░░░░░░░░░` 6% |

<details><summary>Function level detail, tiers and how the numbers are counted</summary>

**2,794 application functions** (VA < 0x4B0000, the rest is linked library code)

| Stage | Functions | Progress |
|---|---:|:---|
| Named | 2,696 | `███████████████████░` 96% |
| Replaced by recompiled C (all tiers, each function once) | 2,735 | `████████████████████` 98% |
| ...verified byte identical | 1,387 | `██████████░░░░░░░░░░` 50% |
| ...share of the code bytes that is byte identical | 93,054 B | `███░░░░░░░░░░░░░░░░░` 14% |
| ...behaviour verified, not byte identical (a diffemu EQUAL run on file: >= 20 cases, coverage >= 0.8) | 1,184 | `████████░░░░░░░░░░░░` 42% |
| ...replaced but weakly verified or without evidence (low coverage, DIFF, inconclusive, no run on file; see `data/behaviour_evidence.csv`) | 164 | `█░░░░░░░░░░░░░░░░░░░` 6% |
| ...share of the code bytes | 678,488 B | `████████████████████` 99% |
| ...of these register-argument clusters | 677 | `█████░░░░░░░░░░░░░░░` 24% |
| **Published in this repo** (`src/`: tier 1 + automatch + hand-written + tier 2 + clusters, each function once) | 3,179 | `███████████████████████` 114% |
| ...byte identical (tier 1, automatch, hand-written `MATCH`) | 1,783 | `█████████████░░░░░░░` 64% |
| ...tier 1, `src/tier1` (1935 files) | 1,435 | `██████████░░░░░░░░░░` 51% |
| ...automatch, `src/automatch` (5 files) | 5 | `░░░░░░░░░░░░░░░░░░░░` 0% |
| ...hand-written, `src/*.c` (`manifest.txt`) | 381 | `███░░░░░░░░░░░░░░░░░` 14% |
| ...tier 2, `src/tier2` (1190 files) | 720 | `█████░░░░░░░░░░░░░░░` 26% |
| ...clusters, `src/clusters` (670 files) | 725 | `█████░░░░░░░░░░░░░░░` 26% |
| ...plus linked library code (VA >= 0x4B0000), not in the percentages | 1277 | |
| ...functions present in two kinds (counted once above) | 87 | |

**What the numbers mean.** A function is one VA below 0x4B0000; every row counts each function once, however many files or tiers mention it. A cluster file (`src/clusters/cl_*.c`) can hold several entry functions (`// cluster-entries:`), so *files* and *functions* differ (670 cluster files give 725 functions). *Byte identical* = ledger authority `bytes-reloc`: the original compiler turns the C into the original bytes (relocations checked). *Behaviour verified* = not byte identical, but a differential run against the original code in an emulator (diffemu) ended EQUAL with at least 20 cases and coverage of at least 0.8 on both sides, no unresolved DIFF; the evidence per function is `data/behaviour_evidence.csv` (source, cases, coverage, flags). Rows with the flags `STDIND` (only verified by a sweep whose stack-pop heuristic is known to misbehave) or `LADDER` (a weaker harness setting was needed) are still counted, see the flag counts in `progress/progress.json`. It is NOT a statement that the whole game is verified, and the share of *bytes* that is byte identical is far lower than the share of *functions*.

**Layers.** All rows here are application code (VA < 0x4B0000). The library layers are in the table above; see [docs/RW_LIBRARY_LAYER.md](docs/RW_LIBRARY_LAYER.md), [docs/CRT_LAYER.md](docs/CRT_LAYER.md) and [docs/VIDEO_INTERFACE.md](docs/VIDEO_INTERFACE.md).

*Whole pipeline* rows above count everything the private working tree has replaced; *Published* rows are the part whose C source is in this repo (see [docs/VERIFICATION.md](docs/VERIFICATION.md)).


<details><summary>By tier (whole pipeline, each function once)</summary>

| Tier | Functions | Code bytes | Verified byte identical |
|---|---:|---:|---:|
| agent | 109 | 8,446 | 109 |
| bridge | 4 | 1,643 | 0 |
| cluster (not in ledger) | 589 | 334,378 | 0 |
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
| `src/` | Recompiled game code in C. `src/*.c` + `manifest.txt`: hand-written functions with address and status. `src/tier1/` and `src/automatch/`: byte-identical functions, one per file. `src/tier2/`: behaviour-verified (not byte identical) functions, `names.txt` maps address to file. `src/clusters/`: verified register-argument clusters, `enabled.txt` lists the installed ones. Only sources that use the curated headers `mfl_tier2.h` / `mfl_consts.h` are published. How "verified" is established and where it stops: [docs/VERIFICATION.md](docs/VERIFICATION.md). |
| `data/` | Small address tables the tools read (function sizes, library symbol tables). |
| `docs/` | Building, naming workflow, toolchain, verification, enum constants, legal note. Start here to understand the game: [docs/GAME_STRUCTURE.md](docs/GAME_STRUCTURE.md) (main loop, race phases, cars, camera, course loading, menus, weapons, effects, audio) and [docs/STRUCTS.md](docs/STRUCTS.md) (record layouts). |

## Acknowledgements

Many thanks to **L2KSolkov** and the contributors of [codfh-win32](https://github.com/L2KSolkov/codfh-win32), a Win32 port of Call of Duty: Finest Hour. Its `src/rwa/` directory is a byte-identical decompilation of Criterion's RenderWare Audio (RWA), the library behind the audio layer of `MFL.exe`. It let us identify the library and match a large part of our audio code exactly with the same compiler. Where we use or adapt that code, the file header says so and names the repository.

Many thanks to **tgsm** and the contributors of [pbg](https://github.com/tgsm/pbg), the CC0-licensed decompilation of Piglet's Big Game. The RWA headers `src/tier1/rwa.h` and `src/tier1/rwaint.h` take the basic RenderWare types, `RwLLLink`, `RwV3d`, `RwaUUID`, `RwaLLNode`, `RwaParamType`, `RwaParamTypeHandle` and `RwaType` from its `include/rwsdk` and `include/rwa` headers; the other structures there hold only the members our sources use, at the offsets measured in `MFL.exe`.

## Naming data

Apply with Ghidra 12.1.4 (headless) in this order: `RenameFns.java`, `ApplyStructs.java`, `RenameVars.java`. Details: [docs/NAMING.md](docs/NAMING.md).

## Naming conventions in the source

- `FUN_xxxxxxxx` is a function whose role is not known yet; the hex number is its address in `MFL.exe`. It is the name the decompiler gave it, and the same name appears at the definition, so you can search for it. Once a role is found it gets a real name.
- A name ending in an address (for example `shadow_project_to_ground_00499dc0`) is a named function whose address suffix keeps the name unique and ties it to the original.
- A comment marked `(inferred)` is a guess from reading the code (what a value or call is for), not something confirmed by the original or by a debug symbol. Comments without the marker describe what the code visibly does.
