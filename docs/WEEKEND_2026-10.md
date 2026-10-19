# Weekend 2026-10-16 .. 2026-10-18: tier 2, clusters, bridges

Factual summary of one weekend of work on the MFL.exe recompilation. Sources: ROADMAP entries of those dates, `output/tier_ledger.csv` (current state, no history is kept in the file), `output/fnsizes.txt`, the `results.csv` of the isolated `t2_auto` runs. "App region" = functions below VA 0x4B0000 (2,557 functions, 638,747 bytes).

## 1. State at start and at end

| | start (about 2026-10-16 morning) | end (2026-10-18 09:44 ledger) |
|---|---|---|
| tier 1 (byte identical, bytes-reloc) | 398 | 398 |
| agent (byte identical) | 109 (app region; 135 rows in the ledger incl. other regions) | 135 rows |
| hand (bytes-reloc / static) | 344 / 37 | 344 / 37 |
| tier 2 (compiled, diffemu EQUAL, not byte identical) | about 367 `an_hk_*` files (an_hk_001..367 installed by 2026-10-15; the ledger has no history) | 990 ledger rows (984 diffemu, 6 static), 900 files in `recomp/automatch_near/` |
| cluster entries (natural C for usercall roots + callers) | 0, first install on 2026-10-16: 11 clusters, 22 entries, 2,265 original bytes | 184 installed clusters (`recomp/clusters/cl_*.c`), 194 entry functions, 29,652 bytes |
| bridges (hand written register function stubs, no machine authority) | 92 (11,292 bytes) | 4 (`car_world_collision_test`, `fatal_assert_loop`, `endpoint_panel_entry_render` [flaky at 400 cases], `movie_guid_compare` [reginputs row lacks eax / stack slot]) |

The ledger total is 1,908 rows. In the app region (computed 2026-10-18 from fnsizes + ledger + cluster entry lists): 1,979 of 2,557 functions (266,888 of 638,747 bytes = 41.8 %) are replaced by C, 576 functions / 371,849 bytes are not. On 2026-10-17 12:37 the same count was 1,715 functions / 188,825 bytes (29.6 %). Intermediate point from `output/remaining_plan.md` (2026-10-17 12:37): tier 2 640 functions / 119,443 bytes, 80 installed clusters (95 entries), 92 bridges.

Gate state at the end: `gate.sh tier2` PASSED (last run 10-18 09:42), 133 ledger rows are STALE (evidence for another source / harness hash, they drop to authority `static` until `diffemu.py --all-tier2` is rerun, because the harness hash changed several times), 10 diffemu INCONCLUSIVE rows.

## 2. What each stage achieved (runs, in order)

Run names are the isolated `t2_auto.py --run` directories (kept outside the repository). "PASS" = strict build clean, diffemu EQUAL with at least 20 cases and coverage >= 0.8, lint clean.

- **10-16 clusters**: `tools/cluster_install.py` and `build_overlay.py --clusters`; pilots 1-3 installed (11 clusters). `gate.sh cluster` PASSED with thin game coverage (4 of 11 clusters run at all).
- **10-16 t2_auto fix rounds (hk_e, hk_h3, hk_dx)**: postfix kinds `frame`, `tailfwd`, `fret`, `ftol`, `stalereg`, `scalararr`, `flit`, `headjoin`, `fstore`. Rerun of 265 failed names: 23 PASS (c4). **t2run_f6**: 143 DIFF names -> 38 PASS (before 0 of 143), 7 EQUAL with low coverage, 98 DIFF.
- **10-16 installs**: an_hk_368..456 (label / goto functions, functions with register callee adapters), an_hk_555..602 (former DIFF).
- **10-16 / 10-17 t2run_g1, g2, h1**: an_hk_603..621 (19 label / menu functions), an_hk_623..627 (5 former DIFF / LOWCOV); compare-guided diffemu inputs.
- **10-17 portability pass 1**: pointer typed code in `automatch_near` (portcheck cc_int2ptr 1845 -> 900, cc_ptr2int 614 -> 219, h_addr_cast 246 -> 58, h_int_to_ptr 872 -> 375), each edit checked with `readable_check.py`; 13 header fields retyped to pointers (`names/MFL_ptr_fields.txt`); 5 formerly uninstalled functions reinstalled after the `sigs.txt` fix.
- **t2run_gs1 / gs2** (/GS cookie class, `gsfix.py`, `/* compile: /GS */`): an_hk_628..648 (21) and an_hk_649..662 (14).
- **t2run_x87**: an_hk_663..674 (12 trig functions: plain sin / cos / atan2 / fabs compile to inline x87 at /O2).
- **t2run_pu1 / pu2** (`pseudofix.py`, diffemu `keeps_reg`): an_hk_675..688 (14 pseudo register usercall functions).
- **t2run_fl1**: an_hk_689..693 (5 float fidelity fixes, volatile float temporaries allowed in `cheat_allow.txt`).
- **t2run_ftol1** (`t2_unify.py`): an_hk_694..705 (12 `_ftol` / x87 survivor functions).
- **t2run_st1**: an_hk_706..731 (26 small tail functions).
- **t2run_all2** (corrected mass rerun after the callee model fixes): an_hk_732..789 (58 functions).
- **t2run_ct0..ct2** (151 `diffemu:call-trace-count` functions, 144 rerun, 0 PASS before): 30 PASS, installed as an_hk_790..819. Every installed call site re-checked by an independent argument-count script: 0 mismatches.
- **t2run_mw1** (`t2_mwfix.py`): an_hk_820..836 (17 memory-write / tail functions). **t2run_fw1**: an_hk_838 (`missile_load_assets`).
- **10-18 bridge tail**: 32 bridges migrated to verified clusters (`output/cluster_pilot11`), 4 to tier 2 (an_hk_839..842: `course_sandsnow_cb1`, `camera_shake_decay`, `anim_update_hierarchy`, `sphere_vs_box_test`); 92 -> 4 bridges. Cluster pilots 5-11 raised the installed clusters from 11 to 184.
- **hand1 / hl1** (hand repaired functions and their lessons, `t2_hand.py`): an_hk_837 and an_hk_843..886 (45 functions), an_hk_887..897 (11). Not matched: `car_set_bounds`, `camera_record_set_path`, `course_tierra_n_cb0`, `ai_target_check`, `fx_wander_target`, `ai_drive_a/b/c`.
- **Readability**: Haiku pass kept 153 of 154 on the newest 154 tier 2 functions (`readable_check.py` invariance check).

Tier 2 growth in files: an_hk_001..367 at the start, an_hk_897 by the end of the numbered installs (plus hk_838..842 and later), i.e. about 530 files installed over the weekend; the ledger shows 990 tier 2 rows because some files hold several functions and some rows come from other regions.

## 3. Defect classes found (latent bugs)

All of these passed compile and the old diffemu or looked EQUAL; they were found by in-game gates, by independent re-checks or by harness fixes, and are now caught by diffemu / defectlint / postfix.

1. **Pointer argument blind spot** (`oil_load_assets`, `load_ghost_model`; `clump_load_by_name` rewrites its slot and tail jumps): crashed the game with diffemu EQUAL. Fix: diffemu `callee_info` / `writes_through`.
2. **x87 stack residue** (`hud_player_marker_position_calc_with_facing`): `dot_product_3d_clamped` leaves its result in ST0, C converted the residue (alpha byte) and leaked an x87 slot per call. Both diffemu stubs left the stack empty, so it was EQUAL by accident. Fix: `float_returners()`, postfix `fret`.
3. **Float passed as int literal bit pattern**: `FUN_005b7300(.., 0x46ac4400)` = wrong pitch in 15 sound functions; `g_float = 0xbf800000;` converts the integer (ai_bot_skill, race_check_winner_by_game_mode, course_powerups_setup); `float param_1` that is really a pointer / index (`(int)param_1` emits `_ftol`: camera_record_update, camera_follow_update, vehicle_setup_references, frame_follow_path_step). Fix: `flit`, `flitarg`, `fparam`, `fmulmem`, `fcastfield`.
4. **K&R `()` prototype promotes float arguments to double** (`ui_rt2d_text_draw`, `voice_set_volume`, `random_range`, `ptank_set_blend_modes`, `hud_bomb_timer_text`): `floatproto` from `callsite_floats.py`.
5. **Wrong stack-argument counts**: `ui_rt2d_text_draw` argument lists in `hud_flash_text`, `build_info_draw_page_0`, `credits_draw`, `race_lose_banner_draw`; `gsd_load` sprintf / `file_read_into_buffer` arity; printf-like callees with the format arguments missing (`texture_read_log`, `texture_read_png_logged`, `dinput_release_keyboard`); stale `reg_eax` parameters of 47 `sigs.txt` entries (voice_set_volume callers); tail forwarders that rewrite their own stack slot (`fx_set_blend_modes`, `weapon_mines_fire`). Fix: `argsite` printf mode, `Tables.stale`, `sig_regcheck.py`, `gsfix.ARITY`, `core.PRINTF`.
6. **Register inputs of interior call targets** (gap items holding several functions): `course_city_cb0` / `cb1` (targets 0x455b80, 0x455e60, 0x455ee0, 0x455fd0), `course_city_cb6` / `cb7` dropped `mov esi,1|0` before the call (0x455dd0 reads ESI). Fix: `interior_reginputs.py` -> `reginputs_interior.csv`, `cluster_regcheck.py`.
7. **`extern char PTR_x[];` where the label holds a pointer** (`texdict_load_named_a/b/c/d`, `rtdict_load_named`, `clump_load_by_name`: the address of the variable was passed instead of the string pointer, e.g. the ".tex" suffix). The 21 installed users of that form were checked: none loads the content, no installed defect. Fix: `ptrdata`.
8. **Scalar global walked as an array** (`scene_tables_clear`: cl dropped the first 0x28 dword fill as a dead store), plus `camera_view_params_pack` (float stored into a dword member, hidden by a wrapped head, defectlint skipped it). Fix: `scalararr`, `headjoin`, `fstore`.
9. **Dropped initial values after `goto`** (`ai_editor_cursor_move`), wrong 0.0 constant (`k_f_0`), raw loop bounds (`t2_hand.py`).
10. **Generator bug**: reversed constant arguments in `orig_const_args` (oil_load_assets).
11. **Rounding order / 1 ulp**: float temporaries the original spills (`t2_round.py`); `acos` expanded inline by cl while the original calls the CRT function (10 functions, `crtcall`).
12. **Harness limits seen as DIFF** (not C defects): frame windows reading other locals (`ptr_limits`, `asm_limits`), batched `add esp` cleanup hiding the argument count (indirect calls compare the common prefix), `RwStreamClose` / `RwStreamRead` with 17 / 4 Ghidra parameters.

Lesson: the in-game gates found latent bugs that diffemu missed, in three families: stack-argument counts, interior register inputs, float / int bit patterns. Each was turned into a harness or lint rule and the installed code was rescanned (defectlint over `recomp/automatch_near`: no other finding).

## 4. Remaining pools (app region, 2026-10-18)

576 functions / 371,849 bytes are not replaced. Cause = first failure of the LATEST row per function over all isolated runs of the weekend (so a class is the first blocker hit; the previous 10-17 plan counted 842 functions / 449,922 bytes). Cluster entries are counted as replaced.

| cause (latest result) | functions | bytes |
|---|---|---|
| gen-skip usercall (register convention, `extraout_*` / `unaff_*`, no cluster yet) | 178 | 139,590 |
| DIFF call trace (argument count, loop register arguments, constants pushed at several call sites) | 106 | 65,618 |
| DIFF other (frame objects, ret N, retval) | 38 | 33,388 |
| DIFF memory writes | 34 | 30,940 |
| LOWCOV (EQUAL, coverage < 0.8) | 52 | 28,334 |
| DIFF float result / x87 stack | 6 | 13,716 |
| gen-skip float-literal call | 11 | 10,164 |
| gen-skip plain register callee | 3 | 9,697 |
| compile error (cast 9+1, call arity 11+2, undeclared 6, other 3, syntax 1) | 33 | 21,521 |
| INCONCLUSIVE (WILD_LOOP on random memory, unresolved reloc) | 13 | 6,313 |
| PASS but not yet published / installed | 15 | 2,682 |
| lint | 8 | 2,042 |
| already defined (duplicate definition elsewhere) / already candidate | 15 | 2,374 |
| never run (no result row) | 38 | 875 |
| small skips (unproto callee 10, dup-symbol 4, switch table 3, text mismatch 5, regin callee 1) | 23 | 1,665 |
| DIFF stack delta / return value | 3 | 630 |

Notes: the usercall pool overlaps the clusters that are not installed yet (`output/callgraph_clusters.csv`); "label / goto" is no gate of its own any more. Tier 2 unlocks still open from `output/remaining_plan.md`: sigs regeneration without bogus `reg_` parameters (enabler), multi-root clusters (draw_sprite_uv, voice_set_volume), callee register returns in ecx / edx in the diffemu stubs, a smarter input generator for LOWCOV / INCONCLUSIVE. `output/remaining_plan.md` itself is the 2026-10-17 12:37 version and was not rewritten.

## Final hours (2026-10-19)

- Weak-EQUAL evidence pass (hand-written inputs, 122 functions + 255 cluster entries): no cluster DIFF; two tier-2 defects found and fixed (`camera_editor_setup_parallel` NaN branch order, `spring_grid_prepare` 1-ulp x87 reassociation).
- NaN-order audit of float `<=`/`>=` compiles: nine more functions rewritten to the original branch form (ordered values unchanged).
- Milestone gate (8 steps incl. AI route-following scenarios) passes with all of the above installed and published.
- Open: `car_update_drivetrain`, `trail_particle_add`, `course_collision_edges_build` (stack-layout/harness class), `car_car_collision`, `car_wheel_contact_update` (need hand-written clusters), `camera_follow_update` (EQUAL, coverage 0.64, needs spec + cheat_allow).
