# RenderWare library layer for the all-C target

Status: static analysis, 2026-10-29. No builds were run. Sources: the librw-mfl notes and reviews (fork of aap/librw, MIT), the game's RenderWare API survey (api_used table: 454 imported or statically linked RW entry points with call-site counts), the Rwp block probe, `recomp/symbols.txt`, `recomp/aliases.txt` and `recomp/*.c`.

Requirement (user): librw-mfl and the RenderWare Physics functions (`rwp_*` game side, Rwp37Active library side) are part of the final all-C tree as library layers, not as original bytes or SDK `.lib` files.

## 1. Where RenderWare lives today

| Layer | Form today | In the all-C target |
|---|---|---|
| Game code that calls RW (about 1/3 of recomp C) | C in `recomp/*.c`, reaching RW through accessor macros (`RwFrame_modelling`, `RpAtomic_geometry`, ...) and a handful of direct calls | stays C; accessors become the contract with the library layer |
| RenderWare graphics (rwcore, rpworld, rpcollis, rt2d, rpmatfx, rpptank, ...) | original bytes, or SDK 3.7 d3d9 release `.lib` objects (byte match 39 % of all function bytes, 1851 unique functions) | replaced by librw-mfl (source) |
| Rwp physics (Rwp37Active) | original bytes only (no SDK lib, no headers, never open sourced) | must be lifted or rewritten as source; nothing to link |
| Game-side physics wrappers `rwp_*` (0x48e390 - 0x495af0, about 70 functions plus the Lua `RWP_Object*` API) | C in recomp (`physics.c` and tier 1/2 clusters) | stays C, becomes the caller of the Rwp source layer |

The game reads and writes RW structs by byte offset (42 accessor rows in the fields table, e.g. frame matrix at +0x10, atomic geometry +0x18, camera view window +0x68). librw's layouts differ and it is C++, so every accessor is either a macro over an RW-compatible struct or a function in a C facade. Raw-offset uses that bypass an accessor are a port blocker (pointers are 4 bytes in the original and 8 on the Switch).

## 2. API families the game uses

Counts are call sites in the original exe (survey table). "Coverage" is librw-mfl status: **Y** implemented, **P** partial or different shape, **N** absent.

| Family | Entry points / sites | Original today | Coverage in librw-mfl | Notes |
|---|---|---|---|---|
| RwCore: Frame, Matrix, V3d, BBox, Camera, Raster, Texture, Image, Stream, Engine, memory/file hooks | 108 / 1625 | bytes or SDK objects | Y for 88, P/N for 20 | missing: `RwMatrixOrthoNormalize`, `RwMatrixQueryRotate`, `RwFrameOrthoNormalize`, `RwCameraCalcPixelRay`, `RwRasterGetOffset`, image format registration, internal SList |
| RwD3D9 device layer | 21 / 148 | bytes | P (own state cache, shader-only) | 65 `SetTextureStageState` sites show the game relies on fixed function; see 4.2 |
| Rx pipelines (`RxPipelineCreate/Lock/AddFragment/Unlock/FindNodeByName`, `RxD3D9AllInOne*`) | 10 / 87 | bytes | N (ObjPipeline is flatter) | usage is tiny and fixed (one-node pipelines + swapped render callback); replace the calls, do not implement Rx |
| RpWorld: Atomic, Clump, Geometry, Material, Light, World, sectors | 63 / 543 | bytes | Y for 39, P/N for 24 | BSP loader written and differentially checked on 126 game BSPs (review: 4 files differed only in normal decode scale; the review-fix patch status was not re-checked); sector render callbacks, `ForAllMaterials`, `WorldInstance`, stream-rights callbacks, `RpWorldPluginAttach` missing |
| RpCollision (+ `Rt` intersection helpers) | 10 / 23 | bytes (SDK rpcollis) | Y (patch set 0014/0017, 217 sector trees byte-exact roundtrip, 454k random queries vs independent reference) | semantics of sphere/box distance and the `point` output were guessed; three Rt signatures assumed |
| RpMatFX | 8 / 15 | bytes | Y except sector enable | data: dual 160 materials, uv-transform 57, env 2, bump 1 |
| RpUVAnim | 4 / 5 | bytes | P (plugin and apply present) | |
| RpHAnim | 5 / 15 | bytes | P | `HierarchyUpdateMatrices` (7 sites) and `Attach` missing; no skinned geometry in the data |
| RpSkin | 2 / 4 | bytes | Y (attach), lighting callback is a stub in the original | |
| RpUserData | 13 / 29 | bytes | P (array API present) | `RpUserDataGetFormatSize` N |
| RpPTank | 5 / 63 (72 symbol names) | bytes | Y (patches 0013/0016, 81k checks, differential harness) | point-sprite path and custom callback table not reproduced |
| Rt2d (fonts, brushes, CTM, paths, Im2D sink) | 21 / 90 | bytes | Y (patches 0012/0015) | never run on a GPU; draw depth vs 3D scene untested |
| PI texture dictionary (`RtPITexDictionary*`) | 2 / 2 | bytes | Y (patches 0009/0011) | |
| RpLOD | 7 / 18 | bytes | N | atomic LOD with hooked render |
| RpMorph | 5 / 13 | bytes | N | interpolators on geometry/atomic |
| RpPatch | 5 / 6 | bytes | N | mesh lock/unlock/destroy; usage in data not verified |
| RpSpline | 6 / 16 | bytes | N | `FindMatrix`, `FindPosition`, stream read (camera and sound paths) |
| RpRandom | 3 / 6 | bytes | N | small deterministic generator; must match bit for bit (replays, sweeps) |
| RpPVS | 2 / 2 | bytes | N | probably a no-op query; not verified |
| Rt toolkit odds: FSManager, Dict schema, compressed keyframes, LtMap area light colour, quaternion slerp cache, charset | 30 / 89 | bytes | Y for 10, N for 16 | FSManager can be replaced by plain file I/O in the game layer |
| Rwp physics | 105 entry points / 388 | bytes | N, out of librw scope | see section 3 |
| Unidentified (21 entries) | 21 / 34 | bytes | not classified | not verified; need a pass before the inventory is closed |

`rxD3D9VertexShaderDefaultEndCallBack` (53 sites) is an ICF-folded empty function; nothing to implement.

## 3. Rwp physics

### 3.1 What it is and how the game uses it
Rwp37Active (Criterion RenderWare Physics 3.7), statically linked, 22 source files recovered from RCS id strings in link order, about 130 KB of pure library code at 0x56a000-0x56e890 and 0x571830-0x58c8a0 (about 300 functions; 33 functions in the middle are game code and already ordinary tier 2 candidates). The library calls only math, quaternion and RpCollision/RpWorld functions and nothing of D3D. It carries a qhull wrapper (the qhull sources are available separately).

Game entry surface: 105 library functions with 388 call sites, grouped as volume (16), object (15), sphere (14), pipeline (12), grid (9), batch (9), capsule (7), trilist (6), dyn (6), box (1), 10 not attributed. Game side: object creation from atomics and box/sphere/cylinder/capsule groups, matrix get/set, enable/in-world/activate, ray query, contact collection, car bodies (`rwp_cars_*`, `car_enter_rwp_mode`), world create/step/flush, trigger groups. Only box, sphere, cylinder, capsule, triangle-list (track geometry) and grid volumes are created from the game. The simulation is frame locked at 60 Hz.

Source-file sizes (bytes): Batch 31152, Aggregate 20176, Constraints 17744, Pipeline 12288, Farfield 5104, Box 5216, Triangle 4336, Capsule 4576, Cylinder 4400, Sphere 4304, Grid 3600, Dyn 3584, Volume 3504, Obj 3364, Math 3328, General primitive 3088, Trilist 2928, Convex hull mass props 2848, Agg children 2656, Gjk 2000, Mgr 1600, Null 736.

### 3.2 Options
| Option | What | Effort | Risk |
|---|---|---|---|
| A. Lift the block to C (decompiler output cleaned to readable C, vtable tables as C structs, x87 math kept in C double/float with the original operation order) | keeps behaviour, library code is "lifted, not byte identical" | L: 10-15 working days for the pieces the game reaches, plus differential harness per object type (diffemu cluster mode already exists for game code) | high but bounded: vtable-driven dispatch (144 slots at 0x5f6458-0x5f7374), x87 evaluation order and float/double mixing change results, determinism of replays and the 15-course sweeps; 32-bit pointer and struct layout assumptions inside the library |
| B. Rewrite as own physics matching observed behaviour | new code | L+: 15-30 days | very high: car crash and debris behaviour changes, sweeps cannot be EQUAL, only "plays like" can be judged |
| C. Keep Rwp bytes | not portable (x86); fine for the PC verification build only | 0 | blocks the Switch and the all-C goal |

Recommendation: A, in this order of work: (1) reachability pass over the 144 vtable slots from the 105 entry points to drop unused shapes (convex hull, ragdoll, joints, GJK likely unreachable; unverified), (2) lift Mgr/Dyn/Obj/Volume/Sphere/Box/Cylinder/Capsule/Trilist/Grid/Math, (3) Pipeline/Batch/Constraints/Aggregate/Farfield (the solver, 82 KB), (4) differential checks against the original per object type, then whole-course sweeps. The lifted code should call librw-mfl collision (`RpCollisionWorldForAllIntersections`, `RpAtomicForAllIntersections`) and the sector plugin registry, so those must be bit-compatible first (see 4.1).

### 3.3 Unverified
- Which shapes, joint types and the ragdoll code are actually reachable from the game (the string table lists only Box/Sphere/Cylinder/Capsule in Lua, the volume table lists more).
- Whether the qhull wrapper is reached at runtime (convex hulls from atomics).
- Library determinism: whether results depend on CPUID (a vendor check string lives in the block, 0x5850e1; an SSE/3DNow dispatch path is possible and would change results by machine).
- The attribution of the 10 unclassified entry points (141 call sites).
- The file-to-address mapping is by link-order inference with low/medium confidence for 8 of 22 files.

## 4. Gaps in librw-mfl the game needs (physics first)

### 4.1 Needed by physics
| Gap | Effort | Risk |
|---|---|---|
| Rwp source layer itself (3.2) | L | high |
| Collision query semantics proven against the original (sphere/box distance and `point`, two-sided line test, inclusive borders, 3 assumed `Rt` signatures) | M (1-2 d: differential harness on the original SDK routine in the emulator) | medium: the lifted solver consumes these results |
| `RpWorldSectorRegisterPlugin` + stream (sector plugin registry) and `RpWorldForAllWorldSectors`, `RpWorldSectorGetWorld`, `RpCollSectorBBoxIt*` iteration | M (1-2 d) | medium; the BSP loader keeps the collision blob raw per sector |
| `rpAtomicResyncInterpolatedSphere` / morph target bounding data in the RW-compatible layout | S | low |
| RW-compatible struct layout and accessors for the fields the physics glue touches (matrix, frame LTM, atomic, geometry vertex/triangle arrays) | M | medium: C++ layouts differ |

### 4.2 Needed by rendering and the rest
| Gap | Effort | Risk |
|---|---|---|
| Fixed-function D3D9 parity layer (lights, material, texture factor, fog, transforms, original default state block) per the Rx/D3D9 plan: pieces 0-7 about 7-9 days with a trace oracle | L | medium: draw-order and state inheritance are part of correctness; caps-dependent branches |
| Shader / GL3 / GLES port for the Switch (plan piece 8) | L (weeks) | high: image diff vs fixed-function output; JaCFX pixel shaders (ps_1_1 x3, vs_1_1) need translation |
| Real GPU runs of BSP, Rt2d, PTank (all only syntax- or null-device-tested) | M | medium |
| Review of existing librw code against RW 3.7 behaviour (struct layouts, plugin chunk versions, state flush; the notes list it as late TODO) | M-L (3-5 d) | medium |
| RpWorld API completion: sector render callbacks, `ForAllMaterials`, `WorldInstance`, render order (BACK2FRONT by frustum list; librw draws depth-first), matfx sector flag (46 sectors in 7 files lose it), 0x50E bin mesh | M (2-3 d) | medium (translucent world order) |
| Small plugins: RpLOD, RpMorph, RpSpline, RpRandom, RpPVS, RpPatch, HAnim update/attach, userdata format size, compressed keyframes, LtMap area light, FSManager replacement | M total (5-7 d); RpRandom needs a bit-exact generator | low each; RpPatch/RpPVS usage unverified |
| Remove Rx calls from the game, drop dead pipelines (g_app_771054/771048, material pipeline) | S (0.5 d) | low |
| C facade over librw (C++) and migration of raw offset uses to accessors; 64-bit pointer safety of all game-side RW structs | M-L | medium: the requirement says all-C; whether a C++ library layer with a C ABI is acceptable is not verified with the user |

## 5. Ranked blockers for the Switch port-readiness document
1. Rwp has no source and no port: 130 KB of x87, vtable-driven library; lift (10-15 d) with differential checks. Without it the game does not run off x86.
2. D3D9 rendering is fixed function in the original and shader-only in librw-mfl: parity layer (7-9 d), then a GL3/GLES port with JaCFX shader translation (weeks).
3. Collision semantics are guesses until diffed against the original; the physics layer depends on bit-compatible results.
4. RW struct access by raw offset and 32-bit pointers: accessor/facade migration is a prerequisite for 64-bit ARM.
5. RpWorld sector plugin registry and render/instance API gaps (physics registers a sector plugin).
6. Missing small plugins (LOD, Morph, Spline, Random, Patch, PVS, HAnim update): about a week, all low risk.
7. librw vs RW 3.7 review not yet done; GPU-side behaviour of BSP, Rt2d and PTank never observed.
8. Unclassified 21 API entries and the Rwp reachability pass are open inventory items.

## 6. What could not be verified
- Everything is static: no librw-mfl build, no GPU run, no execution of the lifted-physics options.
- Per-family call-site counts come from the survey table of the original exe; the recomp C reaches many RW functions through macros and `FUN_` names, so a grep of `recomp/*.c` for `Rw*` calls (101 direct call sites in 30 distinct names) understates use.
- Rwp file boundaries, reachability, determinism and the unclassified entry points (3.3).
- Whether RpPatch, RpPVS and RpMorph are exercised by shipped data.
- The effort numbers are estimates in working days of one agent plus review, not measured.

## 7. Original RenderWare code rewritten as C (T-R2, 2026-10-29)
Before the librw-mfl swap, the RenderWare code inside the exe is being rewritten as C that is byte identical to the original (tier 1), so the swap can be done function by function against a known-good C baseline. 383 library functions are done (rwcore, rpworld, rpcollis, rpmatfx, rpskin, rt2d, rtpng, and the Rwp physics block: plugin registration, module open / close, engine requests, accessors, list iterators, D3D9 state helpers, pixel converters, Rwp pointer stacks). The SDK objects were built by cl 12, so these files carry `/* compile: /G5 */` (see BYTEMATCH_PATTERNS "Round T-R2" for the patterns and what does not match). Ranked work list for the rest (size, state, call-graph height, reachability from game code): `output/rw_rewrite_queue.csv`, produced by `tools/rw_queue.py`; drafts are verified and installed with `tools/rw_tier1.py`.
