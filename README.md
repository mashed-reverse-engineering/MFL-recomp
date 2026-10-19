# MFL-recomp

Reverse engineering and recompilation of **Mashed: Fully Loaded** (`MFL.exe`, PC, 2004).

The goal is a playable recompilation with readable, maintainable source code that can later take bug fixes and enhancements.
Machine-code matching (byte-identical output from the original compiler, MSVC 13.00.9466 = Visual C++ .NET 2002) is used as the
safety net while the source is made readable.

**No game files are included.** You need your own copy of the game; see [docs/LEGAL.md](docs/LEGAL.md).
Supported executable: `MFL.exe` of Mashed: Fully Loaded, SHA-1 `d52ca4a5995875b6d052707188e2c6d065f41766` (`python3 tools/check_exe.py path/to/MFL.exe`).

## Progress

<!-- progress:start -->
**2,557 application functions** (VA < 0x4B0000, the rest is linked library code)

| Stage | Functions | Progress |
|---|---:|:---|
| Named | 2,459 | `███████████████████░` 96% |
| Replaced by recompiled C (all tiers) | 1,914 | `███████████████░░░░░` 75% |
| ...verified byte identical | 877 | `███████░░░░░░░░░░░░░` 34% |
| ...share of the code bytes | 254,624 B | `████████░░░░░░░░░░░░` 40% |
| Published in `src/` | 381 | `███░░░░░░░░░░░░░░░░░` 15% |
| ...byte identical (`MATCH`) | 343 | `███░░░░░░░░░░░░░░░░░` 13% |

<details><summary>By tier (whole pipeline)</summary>

| Tier | Functions | Code bytes | Verified byte identical |
|---|---:|---:|---:|
| agent | 135 | 8,703 | 135 |
| bridge | 4 | 1,643 | 0 |
| hand | 381 | 14,744 | 344 |
| tier1 | 398 | 17,690 | 398 |
| tier2 | 996 | 211,844 | 0 |

</details>
<!-- progress:end -->

## Contents

| Path | What |
|---|---|
| `names/` | Function, global and struct names for `MFL.exe` as CSV / struct files, applied to a Ghidra project by the scripts below. |
| `scripts/` | Ghidra headless scripts (`RenameFns.java`, `ApplyStructs.java`, `RenameVars.java`, ...). |
| `tools/` | Compile / compare / hybrid-exe / regression tooling (Python, shell, Docker + Wine). See [docs/TOOLCHAIN.md](docs/TOOLCHAIN.md). |
| `src/` | Recompiled game code in C: one file per module, `manifest.txt` lists every function with its address and verification status. 42 of 49 files pass `gcc -m32 -fsyntax-only`; the others use MSVC inline `__asm`. |
| `data/` | Small address tables the tools read (function sizes, library symbol tables). |
| `docs/` | Building, naming workflow, toolchain, legal note. |

## Naming data

Apply with Ghidra 12.1.4 (headless) in this order: `RenameFns.java`, `ApplyStructs.java`, `RenameVars.java`. Details: [docs/NAMING.md](docs/NAMING.md).
