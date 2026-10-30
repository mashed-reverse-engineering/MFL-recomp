# MFL-recomp

Reverse engineering and recompilation of **Mashed: Fully Loaded** (`MFL.exe`, PC, 2004).

The goal is a playable recompilation with readable, maintainable source code that can later take bug fixes and enhancements.
Machine-code matching (byte-identical output from the original compiler, MSVC 13.00.9466 = Visual C++ .NET 2002) is used as the
safety net while the source is made readable.

**No game files are included.** You need your own copy of the game; see [docs/LEGAL.md](docs/LEGAL.md).
Supported executable: `MFL.exe` of Mashed: Fully Loaded, SHA-1 `d52ca4a5995875b6d052707188e2c6d065f41766` (`python3 tools/check_exe.py path/to/MFL.exe`).

## Progress

<!-- progress:start -->
**2,784 application functions** (VA < 0x4B0000, the rest is linked library code)

| Stage | Functions | Progress |
|---|---:|:---|
| Named | 2,686 | `███████████████████░` 96% |
| Replaced by recompiled C (all tiers, each function once) | 2,722 | `████████████████████` 98% |
| ...verified byte identical | 1,385 | `██████████░░░░░░░░░░` 50% |
| ...share of the code bytes that is byte identical | 92,353 B | `███░░░░░░░░░░░░░░░░░` 14% |
| ...behaviour verified, not byte identical (a diffemu EQUAL run on file: >= 20 cases, coverage >= 0.8) | 1,180 | `████████░░░░░░░░░░░░` 42% |
| ...replaced but weakly verified or without evidence (low coverage, DIFF, inconclusive, no run on file; see `data/behaviour_evidence.csv`) | 157 | `█░░░░░░░░░░░░░░░░░░░` 6% |
| ...share of the code bytes | 675,409 B | `████████████████████` 99% |
| ...of these register-argument clusters | 664 | `█████░░░░░░░░░░░░░░░` 24% |
| **Published in this repo** (`src/`: tier 1 + automatch + hand-written + tier 2 + clusters, each function once) | 2,931 | `█████████████████████` 105% |
| ...byte identical (tier 1, automatch, hand-written `MATCH`) | 1,594 | `███████████░░░░░░░░░` 57% |
| ...tier 1, `src/tier1` (1608 files) | 1,246 | `█████████░░░░░░░░░░░` 45% |
| ...automatch, `src/automatch` (5 files) | 5 | `░░░░░░░░░░░░░░░░░░░░` 0% |
| ...hand-written, `src/*.c` (`manifest.txt`) | 381 | `███░░░░░░░░░░░░░░░░░` 14% |
| ...tier 2, `src/tier2` (819 files) | 721 | `█████░░░░░░░░░░░░░░░` 26% |
| ...clusters, `src/clusters` (631 files) | 665 | `█████░░░░░░░░░░░░░░░` 24% |
| ...plus linked library code (VA >= 0x4B0000), not in the percentages | 513 | |
| ...functions present in two kinds (counted once above) | 87 | |

**What the numbers mean.** A function is one VA below 0x4B0000; every row counts each function once, however many files or tiers mention it. A cluster file (`src/clusters/cl_*.c`) can hold several entry functions (`// cluster-entries:`), so *files* and *functions* differ (631 cluster files give 665 functions). *Byte identical* = ledger authority `bytes-reloc`: the original compiler turns the C into the original bytes (relocations checked). *Behaviour verified* = not byte identical, but a differential run against the original code in an emulator (diffemu) ended EQUAL with at least 20 cases and coverage of at least 0.8 on both sides, no unresolved DIFF; the evidence per function is `data/behaviour_evidence.csv` (source, cases, coverage, flags). Rows with the flags `STDIND` (only verified by a sweep whose stack-pop heuristic is known to misbehave) or `LADDER` (a weaker harness setting was needed) are still counted, see the flag counts in `progress/progress.json`. It is NOT a statement that the whole game is verified, and the share of *bytes* that is byte identical is far lower than the share of *functions*.

**Layers.** All of the rows above are application code (VA < 0x4B0000). Not counted and still original bytes in the playable build: the C runtime (libcmt, about 81 KB, including 23.6 KB floating-point/exception code without source), RenderWare Rw/Rp/Rx/Rt (about 480 KB; to be replaced by a clean-room librw-mfl, not byte-matched), the RenderWare physics plugin Rwp (121 KB, no source), RtPNG/libpng/zlib, D3DX, Lua 4.0, qhull and DirectShow video code, plus the data, resources and the link without the original image. The D3D9, DirectSound, DirectInput and Win32 platform calls are made directly from the application C. See [docs/RW_LIBRARY_LAYER.md](docs/RW_LIBRARY_LAYER.md), [docs/CRT_LAYER.md](docs/CRT_LAYER.md) and [docs/VIDEO_INTERFACE.md](docs/VIDEO_INTERFACE.md).

*Whole pipeline* rows above count everything the private working tree has replaced; *Published* rows are the part whose C source is in this repo (see [docs/VERIFICATION.md](docs/VERIFICATION.md)).


<details><summary>By tier (whole pipeline, each function once)</summary>

| Tier | Functions | Code bytes | Verified byte identical |
|---|---:|---:|---:|
| agent | 109 | 8,446 | 109 |
| bridge | 4 | 1,643 | 0 |
| cluster (not in ledger) | 577 | 331,915 | 0 |
| hand | 381 | 14,744 | 344 |
| tier1 | 932 | 71,245 | 932 |
| tier2 | 719 | 247,416 | 0 |

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
