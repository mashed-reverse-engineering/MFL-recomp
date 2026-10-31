# d3dtrace: D3D9 draw-trace oracle for MFL.exe under Wine

Piece 0 of `librw-mfl/RX_D3D9_PLAN.md`.  A `d3d9.dll` proxy logs every DrawPrimitive / DrawIndexedPrimitive / DrawPrimitiveUP / DrawIndexedPrimitiveUP of the
ORIGINAL game together with the complete fixed-function / shader state, `normalise.py` turns the log into one deterministic record per draw and `diff.py`
compares two such traces (ulp tolerance on floats), so a reimplementation can be checked draw by draw against the original.

| file | purpose |
|---|---|
| `d3d9.c`, `Makefile` | the proxy (`make` -> `d3d9.dll`, i686-w64-mingw32-gcc, no SDK needed besides mingw's d3d9.h) |
| `run.sh` | records one scripted scenario in the wine-run harness (ONE game process through `gameslot.sh`) |
| `patch_import.py` | renames the `d3d9.dll` import of the run copy of MFL.exe (see "Loading the proxy") |
| `normalise.py` | raw change stream -> `trace.d3d.gz`, one full record per draw |
| `diff.py` | draw-by-draw diff of two normalised traces, `--ulp`, `--no-ids`, `--ignore`, `--only`, `--mask`, `--align` |
| `selftest.py` | no-game check of normalise + diff (`python3 selftest.py`) |

## Usage
```
make                                         # builds d3d9.dll (once)
./run.sh course:0 out/c0 120                 # quick battle, course page moved down 0 times, race frames 0..119 -> out/c0/trace.raw, trace.d3d.gz
SEG=countdown ./run.sh course:2 out/c2 20    # only the countdown (race phase 5, fly-over of the whole course): byte-identical between runs
./run.sh cup out/cup 60                      # Single Player > Challenge Cup (SCEN_ROW / SCEN_RIGHT pick the event), race frames 0..59
./run.sh menu out/menu                       # walks the main menu pages (all draws from process start)
./normalise.py out/c0/trace.raw out/c0/trace.d3d.gz     # run.sh already does this
./diff.py a/trace.d3d.gz b/trace.d3d.gz --ulp 16 --no-ids --max 5
./diff.py new.d3d.gz old.d3d.gz --mask old2.d3d.gz     # ignore keys that differ between two recordings of the OLD one (run-to-run noise)
```
`run.sh` environment: `SEG` (`race` default | `countdown`), `DRIVE` (harness `scen_flags`, default 9 = accelerate + opponents frozen), `GEOM=0` (no vertex/index hashes),
`MAX_FRAMES`, `PHASE_MIN`, `SLOT`, `RUN_TIMEOUT`, `CTRL_BUILD` (dir with `ctrl.exe` + `ctrl.exe.map`; default `output/scenario/_build`, built with
`build_overlay.py --no-recomp --harness` if missing), `MFL_INSTALL`, `TMPDIR`/`MASHED_ROOT` as in wine-run.  Scenarios: `menu`, `cup`, `course:<N>`,
`script:<file>` (any play.sh script).  Everything lives in a fresh hardlink copy of the install (`prepare.py`); nothing in wine-run, recomp/ or the containers is changed.
The game dumped is the control build (original code + the freeze harness, no recompiled functions); race scenes stop at race frame N (harness `race_frames`).
The play.sh script also writes `game/course.bin` (the course index `g_menu_event_course_index`).

Output of one run: `trace.raw` (change stream), `trace.d3d.gz` (normalised), `run.txt`, `game/` (wine.log, screenshots, harness dumps), `work/` (the game copy with `d3dtrace.log`/`d3dtrace.cfg`).

## What is logged
Per draw (`R <index> TAB key=value ...`, keys in canonical order, floats `~%.9g`, hex for DWORD states):
`call` (DP/DIP/DPUP/DIPUP), `frame` (Present count), `phase`/`racef` (game race phase and race frame, read from the game's memory), `prim` (type, base vertex, min index, num vertices,
start index, primitive count / for UP the stride), `fvf`, `decl` (vertex elements), `vs`/`ps` (bytecode hash + shader version + id), `vp`, `rt`/`zs`, `src.N` (offset|stride|vb id), `freq.N`, `ib`,
`vh` / `ih` (hash of the vertex bytes of all bound streams / index bytes the draw reads, from a CPU shadow of everything locked into the buffers: comparable between implementations;
UP draws hash the user data), `tex.N` (content hash of all mip levels | D3DFORMAT | WxH | levels | #creation-order id), every render state that was ever set (`rs.N`),
texture stage states (`tss.S.T`), sampler states (`ss.S.T`), transforms (`xf.<D3DTS>`: VIEW 2, PROJECTION 3, TEXTURE0-7 16-23, WORLD 256), material (`mat`: diffuse ambient specular emissive power),
lights (`light.N` type + all fields, `len.N` enable flags), VS float constants (`vsc.R`), PS float constants (`psc.R`), int/bool VS constants, clip planes, scissor.
`pre.events` attaches the Clear calls / device resets that happened before the draw.  The header (`# ...`) has the adapter, the full D3DCAPS9 (`# CAPS ...`), CreateDevice parameters,
and every distinct CheckDeviceFormat call with its result and a count (the game's format probing decides texture formats).
The proxy only logs when the state is "active" (`start_phase` etc. in `d3dtrace.cfg`): the state at activation is dumped, so the first record is complete.

Normalisation rules that make two recordings comparable: ids are creation order, never pointers; dynamic vertex/index buffers (`D3DUSAGE_DYNAMIC`) are the game's ring of buffers whose pick depends
on history, so they show as `#dyn` (the data is in `vh`/`ih`); CheckDeviceFormat repeats are collapsed; no times, no addresses.

## Determinism: what is identical and what is not
* The virtual clock (`vclock=1`): the proxy patches the game's IAT (QueryPerformanceCounter, GetTickCount, timeGetTime, GetSystemTimeAsFileTime, Sleep): the main thread sees a clock
  that advances by 20 us per call and by its own `Sleep`s, other threads see the plain value; when the race phase first reaches 3 the clock jumps to the next multiple of 100 s.
  Without it the camera of the countdown is different in every run (clock-seeded), with it the whole countdown segment is identical.
* **Countdown segment (`SEG=countdown`, race phase 5, 112 frames, ~25000 draws, the camera flies over the finished course): recordings are byte-identical in most runs** (md5 of the normalised text; proof below: 3 of 4 recordings of course 0 are identical over all 25183 countdown draws, the 4th diverges from frame 102 on in two transforms, 2258 draws).  So the oracle is exact for the countdown but needs the mask for anything longer; record twice and compare the two before trusting a recording.
* Race segment (phase 6): the game's physics is not bit-reproducible run to run (the harness' own dumps show the same noise, see ghidra/README "noise"): the first divergence appears after a
  few tens of frames in car / camera transforms (a few 1e-4).  Use `diff.py --mask second_recording_of_the_original`: keys that differ between two recordings of the ORIGINAL are ignored
  when comparing against a reimplementation.  Draw counts per frame stay equal in all runs seen.
* Menu scenarios: the number of presented frames between two key presses depends on real time, so draw indices shift; align with `diff.py --align frame` only for a fixed frame window.

## Loading the proxy (deviation from the plan: no `WINEDLLOVERRIDES=d3d9=n,b`)
`play.sh` hard-codes `WINEDLLOVERRIDES` in its `docker exec -e` list and cannot be changed from outside without editing wine-run, and Wine prefers its builtin d3d9 to a native file in the
application directory (tested: with `d3d9.dll` next to MFL.exe nothing is loaded).  `run.sh` therefore renames the import of the RUN COPY of MFL.exe (`d3d9.dll` -> `d3dp.dll`, same length,
`patch_import.py`) and installs the proxy as `d3dp.dll` in the game copy; it loads the real Wine `d3d9.dll` from the system directory.  The override mechanism itself
(`WINEDLLOVERRIDES=d3d9=n,b` plus the proxy named `d3d9.dll`) works with the same DLL when a harness allows the env variable; the proxy does not care about its own name.

## How the proxy works
`Direct3DCreate9` loads the real d3d9, patches (in place, VirtualProtect) the vtables of IDirect3D9 (CreateDevice, CheckDeviceFormat) and, at CreateDevice, of IDirect3DDevice9 (44 methods),
and lazily of textures (UnlockRect, GetSurfaceLevel), level surfaces (UnlockRect) and vertex/index buffers (Lock/Unlock).  No wrapper objects: `this` pointers stay real, so nothing the game does
with COM identity changes.  A shadow copy of all state is kept and only changes are written (`RS`, `TSS`, `XF`, ... lines, then `D` lines), which keeps a 60 frame race at ~200 000 lines.
Texture content hashes are taken after each UnlockRect by re-locking the level read-only (whole level, rows without pitch padding; DXT by blocks); UpdateSurface copies the hash.
Config file `d3dtrace.cfg` in the game's working directory (written by `run.sh`): `out`, `start_phase`, `stop_phase`, `phase_addr`, `racef_addr`, `stop_racef`, `max_frames`, `geom`, `vclock`, `step_us`, `anchor_phase`.

## Limitations
* **wined3d is not a real D3D9 driver.**  The caps in the header are what Wine's d3d9 reports on this host (Mesa llvmpipe through wined3d; adapter name is spoofed as a GTX 470, shader model 3.0,
  8 simultaneous textures, 8 lights, anisotropy 16 ...).  Caps-dependent branches of the game (multitexture / matfx passes, fog mode, anisotropy, max lights, shader path) took the path
  these caps select; a reimplementation must be run with the same caps (or compared per caps profile).  Wine does not validate states like a real runtime, and it may drop a few things silently.
* Only calls through the device vtable are seen.  State blocks (`CreateStateBlock`/`BeginStateBlock`) are not shadowed (a `WARN` line is logged if used; MFL does not use them in the scenarios recorded).
  SetStreamSource offsets are logged but a vertex stream that is not locked through the buffer interface (DEFAULT pool filled by UpdateSurface/StretchRect) has no `vh`.  Render-target textures have hash `none`.
  Cube/volume textures are only identified by id.  ProcessVertices, SetCursorProperties, gamma, palettes, N-patches are not logged.  Pixel/vertex shader int/bool constants: VS only; PS only floats.
* Content hashes of textures are taken from what the game LOCKED (CPU data in D3D format), not from what Wine uploads.
* The game is the harness control build, with the movie code of `prepare.py` patched out and `NOSHOW=0` (loading drawn).  Fast menus (`fast_menu_scale 6`) are on as in all wine-run scenarios.
* Runs take ~15 s (course scenarios) when the host is idle, several minutes under load (the sweep in the proof ran at load ~8).  Logs: ~3.5 MB per 25000-draw countdown.
* Time: the clock hook is not transparent to code on other threads (sound, timeSetEvent callbacks), which see the unmodified clock.

## Proof (2026-10-19, host load ~9, scripts in $TMPDIR/d3dtrace, not kept in the repo)
Same scenario twice, `SEG=countdown ./run.sh course:0 <dir> 60`, both normalised:
```
zcat ha/trace.d3d.gz | md5sum   e709883670beb96255f1781321d4794b      (25183 draws, 112 frames)
zcat hb/trace.d3d.gz | md5sum   e709883670beb96255f1781321d4794b
diff.py ha/trace.d3d.gz hb/trace.d3d.gz   -> compared 25183 draw pairs (ulp 0): 0 draws differ ... IDENTICAL
```
Without the virtual clock the same pair differed in every draw (clock-seeded countdown camera); with the clock but without `#dyn` the pair differed in 788 draws (the game picks a different one of its
8 dynamic vertex buffers).  Race segment, `./run.sh course:0 out/c0_a 120` and `c0_b` (51857 draws, 232 frames, draw count per frame equal in both, geometry hashes equal except 299 draws):
`diff.py c0_a c0_b` -> first difference draw 22924 (frame 102): `xf.2` (view) and `xf.256` (player car world matrix) differ in the 4th digit; differing keys over all draws:
xf.2 19302, xf.17 15755, xf.16 11555 (texture transforms derived from the view), vh 299, xf.256 94; everything else (render/texture-stage/sampler states, textures, shaders, lights, material) equal.
That is the physics noise the mask is for.  Recorded scenarios (all normalised, `trace.raw` 5-10 MB per race, 84 MB for the menu walk): `course:0` (no shaders, no lines), `course:8`
(JaCFX: 464 draws with vs/ps 1.1 shaders, 68 DrawPrimitiveUP line lists), `course:9` (no shaders, 530 DrawPrimitiveUP line lists = the rain effect), `cup` (33458 draws), `menu`.
`game/course.bin` of each run holds the course index (equal to N here); course NAMES were not verified (race screenshots only).  Sample record, course 8, first race draw (shortened):
```
call=DIP frame=112 phase=6 racef=0 prim=5|3740|0|22|0|31 fvf=142 decl=0:0:2:0:0:0,0:12:4:0:10:0,0:16:1:0:5:0|#6
vs=69b825f1d351f457|fffe0101|#0 ps=2d157fbc97c61b37|ffff0101|#8 vp=0 0 640 480 ~0 ~1 src.0=0|24|#8 ib=#26 vh=e72919e31657a75a ih=e4b0c13a7e69f252
tex.0=12bd7fc58897a987|21|64x64|7|#222 tex.1=83907517d6dfe4ba|22|128x64|8|#227 rs.7=1 rs.14=1 rs.19=5 rs.20=6 rs.27=1 rs.28=1 tss.0.1=2 tss.0.4=1 ss.0.5=2
xf.2=~-0.999988973 ~0.00259134243 ... xf.256=~1 ~0 ~0 ~0 ~0 ~1 ~0 ~0 ~0 ~0 ~1 ~0 ~0 ~0 ~0 ~1  mat=~1 ~1 ~1 ~1 ~0.200000003 ... light.0=3 ~0.800000012 ... len.0=1
```
(course 0, first race draw: no vs/ps, `tex.1=none`, `rs.7=0` ZENABLE, `mat` ambient 0.1.)
Unit check without a game: `python3 selftest.py` -> `selftest ok`.

## Standalone exe (T-L3, 2026-10-31)
`SA_EXE=<link dir>/ours.exe SA_DELTA=<hex .data delta> ./run.sh title|course:<N> <out> <frames>` records the standalone link (game dir = `<link dir>/game`, harness addresses from `ours.map`, script addresses moved by the delta
as in `tools/delink/state_compare.sh`; the phase address moves too; `anchor_phase` defaults to 5 there, `ANCHOR=n` changes it).  New scenario `title`: title/menu start, frame locked by `frames_total` (comparable run to run).
`tocsv.py trace.d3d.gz <prefix>` writes `<prefix>.draws.csv` (draw,frame,phase,racef,call,prim,hash,mhash) and `<prefix>.frames.csv`; `mhash` leaves out the keys that follow the clock / physics (xf.2, xf.16-23, xf.256, vh, ih, rt, dynamic ring start index).
Results with `<link dir>/ours.exe` (LIB_LUA/QHULL/ZLIB=src DROP_DSHOW=1, delta 112000), `ref/`: caps profile `caps_wine.txt` (Wine/wined3d, spoofed GTX 470, SM3.0), draw hashes `title.draws.csv.gz`, `c0_drive31*.draws.csv.gz`.
* `title 300`: 12299 draws, 292 frames, two runs **byte-identical** (draws.csv).
* `course:0 DRIVE=31 90`: 46652 draws, two runs: exact hash differs in 35942 draws (only camera / car / texture-transform / geometry keys), `mhash` differs in 1 draw (rs.136 once).
* countdown only (`SEG=countdown DRIVE=9`): the standalone is NOT identical (the ctrl build is: 25183 draws, 2258 differ late).  Camera start position differs run to run from draw 0 (cause open: not the IAT clock, anchor 3/4/5 tried); masked: 19 of 25391 differ (a 64x64 texture creation-order id).
* The full menu walk is not frame locked (1.1M vs 1.6M draws): only the `title` window is an oracle there.
