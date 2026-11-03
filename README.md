# MFL-recomp

A readable C recompilation of **Mashed: Fully Loaded** (`MFL.exe`, PC, 2004).

**Status:** work in progress. A Windows x86 build made from this source starts and runs all 15 courses in the regression runs; roughly three quarters of its code is already generated from the C in this repository (about 504 KB of 1,945 KB is still original), the rest is still taken from the original executable (details below).

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
| Rewrite the RenderWare physics plugin (rwp) as verified C | in progress; 24 KB of original code left |
| Replace RenderWare with a clean-room library (`librw-mfl`), then D3DX | in progress; D3DX is down to 15 KB, RenderWare to 347 KB (math, streaming and the stateless groups are swapped, never-used code is removed); the camera, D3D9 and world code is next |
| Replace the streaming audio library, add a real video backend | planned |
| Zero original code, then readability and bug fixing, then the Switch/ARM port | planned |

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
| **Published in this repo** (`src/`: tier 1 + automatch + hand-written + tier 2 + clusters, each function once) | 2,978 | `█████████████████████` 107% |
| ...byte identical (tier 1, automatch, hand-written `MATCH`) | 1,629 | `████████████░░░░░░░░` 58% |
| ...tier 1, `src/tier1` (1640 files) | 1,281 | `█████████░░░░░░░░░░░` 46% |
| ...automatch, `src/automatch` (5 files) | 5 | `░░░░░░░░░░░░░░░░░░░░` 0% |
| ...hand-written, `src/*.c` (`manifest.txt`) | 381 | `███░░░░░░░░░░░░░░░░░` 14% |
| ...tier 2, `src/tier2` (971 files) | 720 | `█████░░░░░░░░░░░░░░░` 26% |
| ...clusters, `src/clusters` (642 files) | 678 | `█████░░░░░░░░░░░░░░░` 24% |
| ...plus linked library code (VA >= 0x4B0000), not in the percentages | 742 | |
| ...functions present in two kinds (counted once above) | 87 | |

**What the numbers mean.** A function is one VA below 0x4B0000; every row counts each function once, however many files or tiers mention it. A cluster file (`src/clusters/cl_*.c`) can hold several entry functions (`// cluster-entries:`), so *files* and *functions* differ (642 cluster files give 678 functions). *Byte identical* = ledger authority `bytes-reloc`: the original compiler turns the C into the original bytes (relocations checked). *Behaviour verified* = not byte identical, but a differential run against the original code in an emulator (diffemu) ended EQUAL with at least 20 cases and coverage of at least 0.8 on both sides, no unresolved DIFF; the evidence per function is `data/behaviour_evidence.csv` (source, cases, coverage, flags). Rows with the flags `STDIND` (only verified by a sweep whose stack-pop heuristic is known to misbehave) or `LADDER` (a weaker harness setting was needed) are still counted, see the flag counts in `progress/progress.json`. It is NOT a statement that the whole game is verified, and the share of *bytes* that is byte identical is far lower than the share of *functions*.

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

## Naming data

Apply with Ghidra 12.1.4 (headless) in this order: `RenameFns.java`, `ApplyStructs.java`, `RenameVars.java`. Details: [docs/NAMING.md](docs/NAMING.md).

## Naming conventions in the source

- `FUN_xxxxxxxx` is a function whose role is not known yet; the hex number is its address in `MFL.exe`. It is the name the decompiler gave it, and the same name appears at the definition, so you can search for it. Once a role is found it gets a real name.
- A name ending in an address (for example `shadow_project_to_ground_00499dc0`) is a named function whose address suffix keeps the name unique and ties it to the original.
- A comment marked `(inferred)` is a guess from reading the code (what a value or call is for), not something confirmed by the original or by a debug symbol. Comments without the marker describe what the code visibly does.
