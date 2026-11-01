# rwtap: RW-API tap + replay oracle (B7)

Records the RenderWare 3.7 calls the game makes (device table: RwRenderStateSet / Get, Im2D*; RwD3D9*; RwCamera*; RwIm3D*; RwRasterShowRaster ...)
together with the D3D9 draw trace, so a librw piece can be checked against the original one call / one draw at a time.

| file | purpose |
|---|---|
| `gen_funcs.py` | writes `funcs.txt` (52 hooks; line `name va len nargs spec`) from recomp/symbols.txt + the device table at 0x62e7c8 (RenderStateSet 4e5be0, Get 4e5070, Im2D Line / Triangle / Primitive / IndexedPrimitive 4eb6d0 / 4eb960 / 4ebb80 / 4ebdb0); needs the diffemu venv (capstone) |
| `rwtap.c`, `Makefile` | `mfltap.dll`: inline detours (displaced prologue + jmp back, `VirtualAlloc` stubs), logs on entry; return values are NOT captured |
| `rwcheck.py` | summary of one log, call-by-call diff of two logs, `--states` CSV of the RW render-state table at every Im2D / Im3D draw |
| `pe.py` | PE reader for gen_funcs |

## Run
`RWTAP=1 tools/d3dtrace/run.sh <scenario> <out> <frames>` (any scenario, also `SA_EXE=... SA_DELTA=...` for the standalone link: the hooks get the delta via cfg `rwtap_delta`).
The proxy loads `mfltap.dll` next to the exe when `d3dtrace.cfg` has `rwtap=funcs.txt`; result `<out>/rwtap.log` next to `trace.raw`. Without `RWTAP` nothing changes.
Hooks work on the ORIGINAL code (ctrl build, VAs unshifted) and on the standalone link (VA + delta) as long as the RW functions live at their original shape; a function the link replaces by a
thunk (LIB_RW) must be removed from funcs.txt (the thunk's first instruction is a jmp, gen_funcs skips such prologues).

## Format (`RWTAP 1`)
```
RWTAP 1 delta=<hex>
H <idx> <name> <va hex>                       one per installed hook
C <seq> <name> d=<n> f=<n> a=<nargs dwords hex, comma separated> [p<K>=<hex bytes> ...]
```
`d` = D3D draw count so far and `f` = Present count (the same counters as `D <index>` / `PRESENT <n>` in trace.raw: the call at `d=n` happens before draw n), `a` = the first `nargs` stack dwords at entry (funcs.txt column 4, from output/cdecl_args.txt / call_pushmin.txt, default 8)
(cdecl arguments, floats appear as raw bits), `pK` = the memory behind argument K as named by the spec column of funcs.txt (`pK:N` N bytes, `pK:nM*S` (arg M) * S bytes, capped at 64 KiB; `!` = unreadable).
Struct layouts are the RW 3.7 ones: RwIm2DVertex 28 B, RxObjSpace3DLitVertex 36 B, RwMatrix 64 B (D3D transforms 64 B), D3DLIGHT9 104 B; RwCamera is dumped as its first 256 B only.
Only logged while the proxy is `active` (same window as trace.raw).

## Replay / oracle
* `rwcheck.py A B` (args of the dynamic VB handle calls are not compared): two recordings of the same scenario must give an identical RW stream (self-consistency of the original; exit 1 otherwise).
* `rwcheck.py A --states f.csv`: per Im2D / Im3D draw the RW state table (state=value, RwRenderStateSet order) in effect = the expected input of the reimplemented RwRenderStateSet / fpRenderStateSet;
  its D3D9 side is the `rs.N` block of the same draw in `d3dtrace/normalise.py` output (join on `d=`).
* The replay of the RW stream into mflrw.dll (real D3D9 device) is NOT built yet: it needs mflrw exports for the device table entries (RwRenderStateSet / Get, Im2D*) and RwRaster / RwCamera objects built from
  the log (pointer arguments are heap addresses of the original, only camera 256 B and vertex / index / matrix data are dumped). Planned: B8 adds the exports, a replayer maps the `p` blobs to calls and records its own d3d9 trace with the same proxy, `d3dtrace/diff.py --mask` then gives the per-draw mhash diff.

## Replay (B9)
`rwreplay_gen.py rwtap.log trace.d3d.gz > replay.txt` + `librw-mfl/tests/rwreplay.cpp` (`tests/build_rwreplay.sh <build dir>`, static link of the d3d9 objects, fixed function) + `rwreplay.sh log orig.d3d.gz <build dir> <work dir>`:
replays Camera clear / show, RwRenderStateSet, RwD3D9Set{RenderState,TextureStageState,SamplerState}, RwIm2D* into librw under Wine with the d3dtrace proxy and diffs the per-draw traces
(tex content hashes blanked: the log has no texture data; rasters are created from the tex.0 size of the original trace).  Needs the docker image mfl-wine and the volume wine_pfx_rp (`tools/wine-run/mkpfx.sh rp`).
