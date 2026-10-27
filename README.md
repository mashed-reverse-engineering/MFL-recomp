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
| Replaced by recompiled C (all tiers, each function once) | 2,680 | `███████████████████░` 96% |
| ...verified byte identical | 1,301 | `█████████░░░░░░░░░░░` 47% |
| ...behaviour verified, not byte identical (tier 2 and clusters: diffemu + game gates) | 1,379 | `██████████░░░░░░░░░░` 50% |
| ...share of the code bytes | 654,748 B | `███████████████████░` 96% |
| ...of these register-argument clusters | 620 | `████░░░░░░░░░░░░░░░░` 22% |
| **Published in this repo** (`src/`: tier 1 + automatch + hand-written + tier 2 + clusters, each function once) | 2,583 | `███████████████████░` 93% |
| ...byte identical (tier 1, automatch, hand-written `MATCH`) | 1,196 | `█████████░░░░░░░░░░░` 43% |
| ...tier 1, `src/tier1` (908 files) | 848 | `██████░░░░░░░░░░░░░░` 30% |
| ...automatch, `src/automatch` (5 files) | 5 | `░░░░░░░░░░░░░░░░░░░░` 0% |
| ...hand-written, `src/*.c` (`manifest.txt`) | 381 | `███░░░░░░░░░░░░░░░░░` 14% |
| ...tier 2, `src/tier2` (1016 files) | 788 | `██████░░░░░░░░░░░░░░` 28% |
| ...clusters, `src/clusters` (615 files) | 648 | `█████░░░░░░░░░░░░░░░` 23% |
| ...plus linked library code (VA >= 0x4B0000), not in the percentages | 344 | |
| ...functions present in two kinds (counted once above) | 87 | |

*Whole pipeline* rows above count everything the private working tree has replaced; *Published* rows are the part whose C source is in this repo. The published sources are all that is needed to read the code; the difference to the whole-pipeline rows is the tier 2 / cluster drafts and a few sources that still need the decompiler's type dump (see [docs/VERIFICATION.md](docs/VERIFICATION.md)). Tier 1, automatch and hand-written `MATCH` code is byte identical; tier 2 and cluster code is behaviour verified, not byte identical.


<details><summary>By tier (whole pipeline)</summary>

| Tier | Functions | Code bytes | Verified byte identical |
|---|---:|---:|---:|
| agent | 135 | 8,703 | 135 |
| bridge | 4 | 1,643 | 0 |
| hand | 381 | 14,744 | 344 |
| tier1 | 908 | 69,877 | 908 |
| tier2 | 1070 | 244,664 | 0 |

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
