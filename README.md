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
| Replaced by recompiled C (all tiers) | 2,498 | `██████████████████░░` 90% |
| ...verified byte identical | 1,387 | `██████████░░░░░░░░░░` 50% |
| ...share of the code bytes | 339,631 B | `██████████░░░░░░░░░░` 50% |
| ...behaviour verified, not byte identical (tier 2: diffemu + game gates) | 1,070 | `████████░░░░░░░░░░░░` 38% |
| ...of these register-argument clusters | 622 | `████░░░░░░░░░░░░░░░░` 22% |
| **Published in this repo** (`src/`: hand-written + `src/tier2` + `src/clusters`) | 2,063 | `███████████████░░░░░` 74% |
| ...hand-written, `src/*.c` (`manifest.txt`) | 381 | `███░░░░░░░░░░░░░░░░░` 14% |
| ...byte identical (`MATCH`) | 343 | `██░░░░░░░░░░░░░░░░░░` 12% |
| ...tier 2, `src/tier2` (1016 files) | 1,070 | `████████░░░░░░░░░░░░` 38% |
| ...clusters, `src/clusters` (612 files) | 612 | `████░░░░░░░░░░░░░░░░` 22% |

*Whole pipeline* rows above count everything the private working tree has replaced; *Published* rows are the part whose C source is in this repo. Tier 1 / most automatch sources still use the decompiler's type dump and are not published (see [docs/VERIFICATION.md](docs/VERIFICATION.md)). Tier 2 and cluster code is behaviour verified, not byte identical.


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
| `src/` | Recompiled game code in C. `src/*.c` + `manifest.txt`: hand-written functions with address and status. `src/tier2/`: behaviour-verified (not byte identical) functions, `names.txt` maps address to file. `src/clusters/`: verified register-argument clusters, `enabled.txt` lists the installed ones. Only sources that use the curated headers `mfl_tier2.h` / `mfl_consts.h` are published. How "verified" is established and where it stops: [docs/VERIFICATION.md](docs/VERIFICATION.md). |
| `data/` | Small address tables the tools read (function sizes, library symbol tables). |
| `docs/` | Building, naming workflow, toolchain, verification, enum constants, legal note. Start here to understand the game: [docs/GAME_STRUCTURE.md](docs/GAME_STRUCTURE.md) (main loop, race phases, cars, camera, course loading, menus, weapons, effects, audio) and [docs/STRUCTS.md](docs/STRUCTS.md) (record layouts). |

## Naming data

Apply with Ghidra 12.1.4 (headless) in this order: `RenameFns.java`, `ApplyStructs.java`, `RenameVars.java`. Details: [docs/NAMING.md](docs/NAMING.md).
