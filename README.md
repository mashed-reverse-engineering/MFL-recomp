# MFLng: Mashed: Fully Loaded, rebuilt

A readable C recompilation of **Mashed: Fully Loaded** (`MFL.exe`, PC, 2004) that runs on Windows 10/11 (and under Wine) and fixes some of the old game's problems.

**You need your own copy of the game.** This project contains no game files, no original code and no original resources; it reads the game's pictures, texts and dialogs from your installed `MFL.exe` at start-up. Not affiliated with the game's publisher or developer ([docs/LEGAL.md](docs/LEGAL.md)).

## Play it

1. Download the newest zip from the [Releases](../../releases) page. Every release is a **pre-release**: it builds automatically, and it is tested by hand only on a few machines. Please report what you find.
2. Unpack it into your game folder (the one with `MFL.exe`).
3. Start `MFLng-launch.exe` (or `MFLng.exe` directly).

`mflng\INSTALL.txt` in the zip has the same steps. You need the Microsoft Visual C++ 2015-2022 Redistributable (x86) once; the launcher itself does not.

What is in the zip:

| File | What |
|---|---|
| `MFLng.exe` | The game with the bug fixes and improvements below. |
| `MFLng.nofix.exe` | The same rebuild with the original behaviour, for comparing. |
| `MFLng-launch.exe` | Optional launcher with the look of the original one. It starts either exe and updates the game and itself from the GitHub releases (releases only, or including pre-releases). |
| `mflrw.dll` | Graphics and physics library. Required, beside the exes. |
| `mflng\mfl_audio_miniaudio.dll` | Sound output. Required. |
| `mflng\mfl_video_ffmpeg.dll`, `av*.dll`, `swscale-*.dll` | Intro movies, the menu backdrop movie and menu music (FFmpeg, LGPL). Without them the game runs silent in the menus and shows no movies. |
| `mflng\LICENSES\` | Third-party notices. |

The zip adds only the exes, `mflrw.dll` and one folder, `mflng`, to your game folder; the other DLLs, the install text and the notices live in that folder.

## What MFLng.exe does better than the original

- **Widescreen.** The picture keeps its proportions at any window size; the Start screen, the winner camera and the spinning loading CD are no longer stretched.
- **A Preferences page** in the Options menu (sub-menus Graphics, Comfort, Cheats and Other), with a description box:
  - display mode (window, fullscreen, borderless) and resolution, language, menu speed;
  - skip intro movies, skip the course fly-over, winner camera rotation, shorter save wait, unlock everything.
  Settings are stored in `mflng_settings.cfg` next to the exe.
- **Language** is chosen from your Windows language the first time (English, French, German, Spanish, Italian), and can be changed in Preferences.
- **Alt-Tab** works; the game no longer closes without a message when you press Escape for the pause menu.
- **Frame pacing:** a precise 60 Hz limiter; the frame rate cap and vsync are options.
- **The save is loaded at start-up**, behind the loading screen, without the confirm and "Loading" boxes.
- **A crash report** (`mflng-crash.txt`) is written next to the exe if the game ever crashes: please attach it to an issue.
- Several crashes and glitches of the original are fixed (a hang after pressing Start, bounding-sphere and heap bugs, trail glitches).

Each of these is a compile-time switch `MFL_FIX_*`, off in `MFLng.nofix.exe`: [docs/MFL_FIX_FLAGS.md](docs/MFL_FIX_FLAGS.md).

## Known limits

- Pre-releases: expect rough edges. Test on your machine and tell us.
- Controller support is DirectInput only; there is no rebinding menu yet.
- The number of players is as in the original; more players are only an idea so far.
- There is no built-in online play. To play with friends online, the way to go is a tool that streams your screen and passes their controller input to your PC: Steam Remote Play, [Parsec](https://parsec.app) or [Sunshine](https://github.com/LizardByte/Sunshine) (with a Moonlight client). We have not tested any of them with MFLng; setup, delay and picture quality depend on the tool and on your connection.
- The Preferences texts in French, German, Spanish and Italian have not been checked by native speakers yet.

## For developers

The goal is a playable recompilation with **readable, maintainable source** that takes bug fixes and enhancements, and is later ported (Linux first, then a Switch homebrew port).

- None of the machine code is taken from the original executable (0 of 1,945 KB, measured 2026-11-15). The code comes from the C in this repository, from open-source libraries, or was removed after proving it is never used. The build contains no original resources (0 B).
- Every function was checked against the original: either *byte identical* to what the original compiler (MSVC 13.00.9466, Visual C++ .NET 2002) produces, or *behaviour verified* by running it side by side with the original code in an emulator ([docs/VERIFICATION.md](docs/VERIFICATION.md)). This checking is scaffolding; the deliverable is the readable source.
- The release zips are built by GitHub Actions with MSVC 2022 (x86) from this tree; the pinned VC7 build used for the proofs is a second variant.
- Not finished: library tables copied from the original are still copied bytes (5,874 B left), the source layout still follows how each function was matched, and two runs of the same race are not yet identical, so a whole race cannot be a regression test yet.
- Next: replay proofs, a compiler other than MSVC (GCC), then the Linux and Switch ports.

Build it yourself: `tools/build_mflng.sh <your MFL.exe> <out dir>` ([docs/BUILDING.md](docs/BUILDING.md)). Supported executable: `MFL.exe`, SHA-1 `d52ca4a5995875b6d052707188e2c6d065f41766` (`python3 tools/check_exe.py path/to/MFL.exe`).

Milestones and open work: [docs/ROADMAP.md](docs/ROADMAP.md). Numbers per layer and how they moved: [docs/PROGRESS_HISTORY.md](docs/PROGRESS_HISTORY.md).

### Progress (generated)

The numbers below are regenerated with every source drop by `tools/progress.py`.

<!-- progress:start -->
### Status at a glance

**Game code** (2,794 functions, the part above address 0x4B0000 that is the game itself):

| | Functions | |
|---|---:|:---|
| Named | 2,696 | `███████████████████░` 96% |
| Replaced by C | 2,664 | `███████████████████░` 95% |
| of these byte identical to the original compiler output | 1,387 | `██████████░░░░░░░░░░` 50% |
| of these behaviour verified (differential test, not byte identical) | 1,112 | `████████░░░░░░░░░░░░` 40% |
| of these only weakly verified | 165 | `█░░░░░░░░░░░░░░░░░░░` 6% |

<details><summary>Function level detail, tiers and how the numbers are counted</summary>

**2,794 application functions** (VA < 0x4B0000, the rest is linked library code)

| Stage | Functions | Progress |
|---|---:|:---|
| Named | 2,696 | `███████████████████░` 96% |
| Replaced by recompiled C (all tiers, each function once) | 2,664 | `███████████████████░` 95% |
| ...verified byte identical | 1,387 | `██████████░░░░░░░░░░` 50% |
| ...share of the code bytes that is byte identical | 93,054 B | `███░░░░░░░░░░░░░░░░░` 14% |
| ...behaviour verified, not byte identical (a diffemu EQUAL run on file: >= 20 cases, coverage >= 0.8) | 1,112 | `████████░░░░░░░░░░░░` 40% |
| ...replaced but weakly verified or without evidence (low coverage, DIFF, inconclusive, no run on file; see `data/behaviour_evidence.csv`) | 165 | `█░░░░░░░░░░░░░░░░░░░` 6% |
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

### Contents

| Path | What |
|---|---|
| `names/` | Function, global and struct names for `MFL.exe` as CSV / struct files, applied to a Ghidra project by `scripts/` (Ghidra 12.1.4 headless: `RenameFns.java`, `ApplyStructs.java`, `RenameVars.java`; see [docs/NAMING.md](docs/NAMING.md)). |
| `tools/` | Compile / compare / hybrid-exe / regression tooling (Python, shell, Docker + Wine). See [docs/TOOLCHAIN.md](docs/TOOLCHAIN.md). |
| `src/` | Recompiled game code in C. `src/*.c` + `manifest.txt`: hand-written functions with address and status. `src/tier1/` and `src/automatch/`: byte-identical functions, one per file. `src/tier2/`: behaviour-verified functions, `names.txt` maps address to file. `src/clusters/`: verified register-argument clusters, `enabled.txt` lists the installed ones. |
| `data/` | Small address tables the tools read (function sizes, library symbol tables). |
| `launcher/` | `MFLng-launch.exe`: C + Win32 launcher and updater ([launcher/README.md](launcher/README.md)). |
| `docs/` | Documentation: [GAME_STRUCTURE.md](docs/GAME_STRUCTURE.md) and [STRUCTS.md](docs/STRUCTS.md) (start here to understand the game), [MFL_FIX_FLAGS.md](docs/MFL_FIX_FLAGS.md), [BUILDING.md](docs/BUILDING.md), [TOOLCHAIN.md](docs/TOOLCHAIN.md), [VERIFICATION.md](docs/VERIFICATION.md), [NAMING.md](docs/NAMING.md), [ENUMS.md](docs/ENUMS.md), [ROADMAP.md](docs/ROADMAP.md), [LEGAL.md](docs/LEGAL.md). |

### Naming conventions in the source

- `FUN_xxxxxxxx` is a function whose role is not known yet; the hex number is its address in `MFL.exe`. Once a role is found it gets a real name.
- A name ending in an address (for example `shadow_project_to_ground_00499dc0`) is a named function whose address suffix keeps the name unique and ties it to the original.
- A comment marked `(inferred)` is a guess from reading the code, not something confirmed by the original. Comments without the marker describe what the code visibly does.

## Acknowledgements

Many thanks to **L2KSolkov** and the contributors of [codfh-win32](https://github.com/L2KSolkov/codfh-win32), a Win32 port of Call of Duty: Finest Hour. Its `src/rwa/` directory is a byte-identical decompilation of Criterion's RenderWare Audio (RWA), the library behind the audio layer of `MFL.exe`, and its `src/rw/` directory holds decompiled RenderWare Graphics core and plugin sources. It let us identify both libraries and match a large part of our audio code exactly with the same compiler. The files `src/tier2/an_codfh_*.c` are adapted from it, not byte-identical copies: each file header names the codfh-win32 file and commit it comes from and lists what we changed (calling convention, inlined helpers, alignment, `MFL.exe` data addresses). They are checked against the original machine code like every other tier-2 file. Where we use or adapt other code from it, the file header says so and names the repository.

Many thanks to **tgsm** and the contributors of [pbg](https://github.com/tgsm/pbg), the CC0-licensed decompilation of Piglet's Big Game. The RWA headers `src/tier1/rwa.h` and `src/tier1/rwaint.h` take the basic RenderWare types, `RwLLLink`, `RwV3d`, `RwaUUID`, `RwaLLNode`, `RwaParamType`, `RwaParamTypeHandle` and `RwaType` from its `include/rwsdk` and `include/rwa` headers; the other structures there hold only the members our sources use, at the offsets measured in `MFL.exe`.

The movie and menu-music DLL uses [FFmpeg](https://ffmpeg.org) (LGPL 2.1), and sound output uses [miniaudio](https://miniaud.io) (public domain / MIT-0); notices are in `mflng\LICENSES\` in each release.
