# Progress history

Moved out of the README. The README keeps the current status; this file keeps the per-layer numbers, the dated history and the notes on how each part is produced. The generated function counts are in the README's progress block (`tools/progress.py`).

## Follow the progress

Original machine code still linked into the standalone build (the number we drive to zero), measured with `tools/delink/origbytes.py` at each full regression gate:

| Layer | 2026-11-15 | What is left |
|---|---:|---|
| RenderWare (graphics, world, camera, D3D9) | 0 KB | done |
| Audio (RenderWare Audio) | 0 KB | done |
| rwp (RenderWare physics) | 0 KB | done |
| D3DX | 0 KB | done |
| C runtime | 0 KB | done |
| Game code | 0 KB | done |
| **Total** | **0 KB** | of about 1,945 KB |

Still copied from the original executable, and not part of the zero above: library tables (4,466 B of initialised data on 2026-11-17: C runtime 2,422 B, RenderWare 948 B, Lua 657 B, qhull 432 B, 7 B in two rows without an owner layer; plus 1,408 B of padding between data rows). On 2026-11-17 the audio, physics and live RenderWare tables and the D3DX dispatch tables became typed C definitions, and the unused D3DX math constants, HLSL / shader-assembler tables and the inverse square root table were removed after a static reference check (81,280 -> 5,874 B). Since 2026-11-16 the library variables (.bss, 41.5 KB) are named zero definitions in C, the Lua tables are gone (the Lua built from source brings its own), and the C runtime's data-only members (math tables, time zone and day tables) are linked from libcmt instead of copied, which took the copied data from 134,233 B to 81,280 B. The resources (710,568 B) are no longer in the exe: they are read at runtime from your own `MFL.exe`.

## How the total moved

Code only; data is tracked separately.

| Date | Original code | What moved it |
|---|---:|---|
| 2026-11-03 | 485.0 KB | |
| 2026-11-04 | 459.3 KB | last exe-side RenderWare replacements |
| 2026-11-05 | 273.0 KB | about 120 new `librw-mfl` entry points in 18 groups (collision, spline/animation, bezier/patch, vertex declarations, mesh building, image, camera and others); D3DX CPU variants removed; audio output layer, mixer, voices and wave code; rwp and C runtime rewrites |
| 2026-11-06 | 246.4 KB | about 40 more audio functions as C; never-run RenderWare stream writers, legacy-format and skin readers removed (each proven unused by 172 coverage runs, an asset format scan and a call-graph check); D3DX half-float code removed; a BMP image reader in `librw-mfl`; RenderWare allocations now come from the game's own heap (a step toward the engine switch) |
| 2026-11-07 | 233.7 KB | 31 more audio functions as C (21 tier 2, 10 register clusters); `librw-mfl` groups for 2D path/brush kernels, PNG reading (libpng inside the DLL) and LOD; five x87 math routines; 179 RenderWare gap functions named so their bodies can be removed; the CRT `raise` function as C (with an exception-frame model in the checker); a few more never-run writers removed with proofs |
| 2026-11-08 | 209.7 KB | three D3D9 driver groups in `librw-mfl` (render state, image/raster conversion, 2D renderer with the dynamic vertex buffer ring; 51 functions, 24.0 KB), run on the game's own engine state and checked op for op against the original under Wine; the draw comparison now normalises the dynamic vertex buffer ring |
| 2026-11-08 (second run) | 196.3 KB | three more D3D9 groups in `librw-mfl` (lights, transforms, raster creation and conversion; 26 functions, 12.3 KB) and a first small batch of string/stream helpers adapted from codfh-win32 (1.4 KB) |
| 2026-11-09 | 174.3 KB | four more D3D9 groups in `librw-mfl` (world pipeline, the rest of raster creation, native texture reading, tangent instancing; about 10 KB); 48 more RenderWare core functions adapted from codfh-win32 (memory, streams, errors, dictionaries, matrix/vector, file system, charset, 2D fonts; about 10.5 KB); 1.7 KB of byte-identical RenderWare bodies that only lacked size rows |
| 2026-11-10 | 152.9 KB | 76 more RenderWare functions adapted from codfh-win32 (plugin callbacks, stream readers, frame/world/geometry/material lifecycle, particle tank, material effects setters; 19.3 KB of RenderWare code in total); 10 more audio functions as C plus 4 register clusters (2.1 KB) |
| 2026-11-12 | 104.6 KB | nine more groups in `librw-mfl` (texture and image reading, material effects, D3D9 device and device system, raster quads, the all-in-one pipeline nodes; 94 functions), checked against the original with a fake Direct3D device; 17 more RenderWare functions adapted from codfh-win32 (collision and user data plugins, particle tank, 2D font); 48.4 KB of RenderWare code in total |
| 2026-11-13 | 61.1 KB | four more D3D9 groups in `librw-mfl` (patch, default instance callbacks, submit and immediate-mode nodes, particle tank; 12 functions, about 13 KB); 68 more RenderWare functions adapted from codfh-win32 (pipelines, world, material effects, UV animation, 2D; about 11.5 KB) and 8 Windows file system functions; 18 more audio functions as C, 36 never-run DirectShow COM methods removed (2.9 KB); CPU feature probes as C for one CPU class (SSE2, no 3DNow); never-used physics convex code removed; padding behind removed bodies is now removed with them |
| 2026-11-13 (second run) | 28.4 KB | 59 more RenderWare functions adapted from codfh-win32 (engine start/stop, clump/world/light lifecycle, pipeline building, heaps, user data, material effects, HAnim, 2D outline drawing; about 19 KB); about 25 patch-mesh functions and the Windows file system init written by hand from the disassembly; two vertex shader cache groups in `librw-mfl`; about 15 more audio functions as C plus 6 register clusters; the last rwp and D3DX stubs; a few more never-reached functions trapped after a static proof |
| 2026-11-14 | 3.3 KB | the D3D9 vertex shader generator (7.3 KB), the vertex shader render and lighting callbacks and the native size / usage-flag functions in `librw-mfl` (checked against the original on the full descriptor space); about 65 more RenderWare functions adapted from codfh-win32 or written from the disassembly (stream readers, plugin attach functions, 2D path and font reading, skin and material effects pipelines); the audio async I/O thread, codec registration and the soft output sites as C plus 2 register clusters; the x87 control-word helpers; two call targets in game code corrected to the original's |
| 2026-11-15 | 0 KB | the last nine RenderWare functions (vector math helpers built with the original's float consistency flag, a stream writer, two pipeline building functions, a dynamic geometry helper, a dictionary destructor), the last two audio functions (one as a register cluster), and the C runtime start-up taken from libcmt itself; four functions whose differential tests had shown differences were re-checked, one of them corrected to the original's evaluation order |

Roughly 35 KB of the 2026-11-05 drop is code removed rather than rewritten (DirectShow, unused CPU variants, never-used wave converters): it is proven unused by coverage runs, an asset format scan and a call-graph closure proof, and every removal sits in a list that can be reverted on its own. Treat that part as the least certain. Every `librw-mfl` entry point is checked bit-exact against the original code on at least 100,000 random inputs, with negative tests that must fail.

## Where to look

- [`progress/progress.json`](../progress/progress.json) and the generated tables in the README: function counts per tier, regenerated with every source drop. A function counts as game code or library code by its address in `data/fnsizes.txt`, so a new row there can move a function from the game count to the library count (six RenderWare / rwp functions moved this way in November 2026: tier 1 1435 -> 1429).
- The git log of this repository: one commit per source drop, with the number of published functions.
- [VERIFICATION.md](VERIFICATION.md): what "verified" means and where it stops. Nothing here claims the whole game is verified.
- What is open and planned: [ROADMAP.md](ROADMAP.md).

## Note on the earlier release

The source drop 4c1168d (2026-11-15) could produce links with two library calls bound to the wrong C runtime function (`exit` to `_exit`, and a vector helper to `tolower`), because our object writer chose between two similarly named runtime functions in hash order. The choice now follows the original address, and links are reproducible.

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

Some rows above are older than the milestones table in the README: RenderWare graphics, D3DX, the audio backend and the C runtime start-up are already replaced in code (see the table above). The row text is kept as it was published.
