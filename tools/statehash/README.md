# tools/statehash: N1 per-frame state hash (control vs ours)

The d3d9 proxy (`tools/d3dtrace`, cfg `statehash=1`) writes, at every logged Present, the changed 4 KB pages of the game's writable image
(0x5fb000-0x949d64, + `sh_delta` in the standalone link) plus the committed private (heap) ranges into `statehash.bin` (~43 MB per 232 frames, ~22 s run).
`statecmp.py` rebuilds both runs frame by frame and compares them word by word: first divergent frame, its words (symbol, values, float view, ulp),
per-word table, VERDICT line.

## Files
| file | purpose |
|---|---|
| `statecmp.py` | the compare tool (`--selftest`, `--mkptr`, `--mkstale`, `--peek`; header documents all options) |
| `one.sh <tag> <course> [frames]` | one STATEHASH run through `tools/d3dtrace/run.sh` (Wine) -> `$SH_WORK/runs/<tag>/statehash.bin` |
| `cc.sh <tag> <course> [frames]` | determinism baseline: two control runs + statecmp of the pair -> `$SH_WORK/cc_<tag>.txt` |
| `cmp.sh <A> <B> <out> [opts]` | statecmp of two finished runs + `groups.py` grouped summary (no Wine) |
| `groups.py`, `series.py` | group a statecmp report by symbol; per-frame values of chosen words in both runs |
| `mkpins.py`, `pinall.py` | write pin rows (phase:va:value) from a reference run / from a statecmp report |
| `mkptr.sh`, `mkmask.sh`, `mkpinfile.sh` | regenerate the three data lists below from reference control runs |
| `data/state_ptrwords.txt` | words that only ever hold pointers (heap / image); compared via pointer translation |
| `data/state_mask.txt` | masked words (profiling counters, RW dyn VB ring, audio-thread words, stale menu residue ...), each row with evidence; masks on car/vehicle/AI/physics/transform/camera names or dump ranges are refused by statecmp |
| `data/state_pins.txt` | determinism pins for the proxy: counters / menu residue at loading start, camera record region at countdown start (course 0 reference values) |
| `data/*_head.txt`, `data/pins_load.txt` | hand-written head rows and loading residue pins, inputs of mkmask.sh / mkpinfile.sh |

Run directory `$SH_WORK` defaults to `output/statehash` (untracked). Input of every run: `CTRL_BUILD=<dir with ctrl.exe + ctrl.exe.map>` (e.g. a
gate's `state` dir) or `SA_EXE=<link dir>/ours.exe SA_DELTA=<hex>` for the standalone link; scenario `course:<N>`; refuses to run while a gate holds the lock.

## Usage
    export CTRL_BUILD=<gate>/state
    tools/statehash/one.sh ctrl 0 120                                  # control run, course 0, 120 race frames
    SA_EXE=<gate>/link/ours.exe SA_DELTA=14f000 tools/statehash/one.sh ours 0 120
    tools/statehash/cmp.sh ctrl ours oc --ptr tools/statehash/data/state_ptrwords.txt --mask tools/statehash/data/state_mask.txt \
        --map-b <gate>/link/ours.map --dead-layers Lua,qhull,zlib/PNG/TIFF,CRT,DirectShow --frames 222
    tools/statehash/cc.sh base 0 120                                   # ctrl vs ctrl determinism check
    python3 tools/statehash/statecmp.py --selftest

Equal word = same value; or in `state_ptrwords` and (both heap pointers | B's value maps through the link map to A's value); or in `state_mask`
(counted "masked", never silent); with `--dead-layers` also words owned by library layers the link replaces or that the link left zero and nobody writes.
Pass `--frames 222` (or run < 110 race frames): from race frame 110 the voice queue (`g_sound_8d9940`, `g_weapon_8be1e0`) follows the real-time RWA audio
thread and drags weapons/fx/particles along.

## Determinism pinning (control and ours alike)
`vsnap=2` (vclock jumps to the next whole second at each race-phase change, frames start on a 50 ms grid); `srand=3:<va>:1` (CRT srand at loading start;
rand() state is per-thread, menus consume it, the countdown camera uses it); `pin=` rows of `state_pins.txt`. Control vs control is 0 words for frames 0-221
(race frames 0-109) with 21-32 masked words. Limits: the pins hold course-0 values; other courses need their own pin set (the `course:4` menu script
landed on courses 4/5/6, so course 4 is not calibrated).

## Fixture results (course 0, DRIVE=31)
* Car poke (`SH_POKE=<frame>:<va>:<xor>`): diverges at the next frame, first word `player_car[0].matrices[12]` (pos.x).
* T-P19 overlay with the bug diverges at frame 0 (`g_race_652de8`, `g_camera_follow_6a06f4`); with the fixed file identical to control for frames 0-220.

## Known open: ours (1103_2045 link) vs control, 122 words at frame 0 after masks / pointer translation / dead layers
Not 0 yet. Classification (nothing traced to a defect in one of our functions):
* Class 2/3, explained, candidates for masks or pointer translation: RwMatrix pad words (`g_vehicle_setup_659e04/659e38`, `player_car[0..3]`
  +0x954/0x964, `g_rwp_world_6fead0+0x45c..`: uninitialised pads, ctrl holds stack residue, librw-mfl keeps 0); five copies of one HWND value; the RW
  string/memory function table (CRT addresses the map cannot translate); `g_log_770f74` (FILE* into `_iob`); static RwFreeList `g_data_7d212c+0x10/0x14`; QPC start `g_data_7daf38`.
* **Open float fields (input differences, 1..17 ulp):**
  * `g_vehicle_load_65a1a8+0x4b0..0x4cc, +0x534` (9 words) = vehicle[2] ground-light history and average (+0x200..0x21c, +0x284) written by
    `vehicle_ground_probe_update` 0x4226d0 from `course_world_ray_hit_light_sample` 0x4891b0 (barycentric vertex colour at the hit): hit_t identical, light
    differs by 1 ulp in about half the samples. diffemu EQUAL for the light sample and RtBaryGetTransform, and restoring original bytes of
    0x4891b0 / 0x569610 / 0x499dc0 / 0x499cc0 changes nothing, so the inputs (vehicle frame LTM x/z or world sector vertices) differ.
  * `g_effects_update_6af870..` (22 words, course-open records 0x6af658 + k*0x130, +0xf0.. matrix rows): written at course open, 1..9 ulp.
  * `g_rw_6f6f90 / g_effects_update_6f6fb0/6f6fd0` (25 words, shadow matrices of the rwp objects): written by `shadow_project_to_ground_00499dc0` from
    `rwp_objects_reset_all` 0x492ff0; right vector 5..17 ulp (tan(acos(width)) amplifies), pos 1 ulp; unchanged with the original 0x499dc0 restored.
* Excluded causes (identical float words): original RW math restored over the mflrw.dll thunks, original D3DX SSE2 bodies, original light sample / bary transform / shadow projection,
  sqrt/invsqrt tables. Next suspects (load time): our C on the course/world/object load path, libcmt math (sin/cos/atan2/acos), librw stream/texture groups.
* Also seen: `g_camera_records_8c5a1c+0x84` differs from frame 1 (huge) and `g_car_8c5990+0x38` from frame 112: check that the camera-record pin
  (8c5900-8c8000) is applied at the delta-moved address in the standalone run before reading them as behaviour.
The bisect scratch tools (restore_va.py, patchcall.py, xref.py ...) were not packaged; they live in the scratch dir `p6b`.

## Regenerating the data lists
`mkptr.sh <runA> <runB>` (two control runs of one course), `mkmask.sh sp2 sp3 sp4 sp5` (control pairs `<tag>_1/<tag>_2` made with `cc.sh`), `mkpinfile.sh`
(needs reference runs `ph3_1` and `sp3_1`; `pins_load.txt` comes from `pinall.py <cc_ph3 report> <ph3_1 statehash.bin> 0 3 pins_load.txt`).

## Changes vs the scratch version
Scripts moved from `/tmp/.../p6` to repo-relative paths (`$SH_WORK`, default `output/statehash`); `CTRL_BUILD` no longer defaults to a stale gate dir;
statecmp/pinall/series import by script dir; `tools/d3dtrace/run.sh` and `d3d9.c` are the STATEHASH-enabled versions (run.sh: repo path from its own location, gate
lock path `${GATE_LOCK:-/tmp/claude-$UID/gate.lock}`); after installing, `make -C tools/d3dtrace` and add `tools/d3dtrace/run.sh` to run.py's game-tool/gate refusal. Evidence strings in
regenerated pin rows no longer cite the scratch cc_*.txt files.
