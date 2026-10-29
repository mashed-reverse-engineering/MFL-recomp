# Byte-match patterns (tier-2 near-misses), 2026-10-19

## Round A

Metric in RESULT.csv: number of +/- lines of check_one.py diff (0 = MATCH, relocation targets checked). Scratch copies in <scratch><name>/<name>.c.
Result: 20 byte-identical (12 were already identical in the installed file, only needed promotion; 8 fixed here, all diffemu EQUAL), 2 improved, 19 failed.

## Fixed patterns

1. noreturn exit (2 functions: course_error_exit, line_rain_error; ~13 automatch_near chunks declare `extern int exit();`)
   asm: original `call printf; add esp,N; push code; call exit` (no cleanup, no ret after exit); ours defers cleanup `add esp,N+4; pop; ret`.
   fix: `extern int exit();` -> `extern __declspec(noreturn) void __cdecl exit(int);`. Script: regex replace of the extern line in files whose function ends with exit(...).
   Same likely for other noreturn calls (abort, ExitProcess, lua error paths): any original call with no code after it.

2. `pop ecx` vs `add esp,4` after the only call of a forwarder (1: video_raster_create_forward)
   asm: ours `push; call; pop ecx; ret`, original `push eax; call; add esp,4; ret`.
   cause: the callee result is dead in ours (cl uses pop ecx); in the original it is live in eax (return value forwarded).
   fix: make the wrapper return the callee's value: callee `extern undefined` -> `extern int`, wrapper `void` -> `int`, `return callee(x);`. (diffemu EQUAL; return type of `undefined` callee gives movzx eax,al, so use int.)
   Candidates for scripting: every `void f(a){ g(a); }` whose original has `add esp,4` (not `pop ecx`) before ret.

3. Float compare operand order (2: weapon_mortar_clear_effects, hudpanel_countdown_update)
   asm: original `fld [var]; fcomp [k]; fnstsw; test ah,0x41; jne`  (var > k), ours `fld [k]; fcomp [var]; test ah,5; jp`.
   source `if (k_f_X < var)` -> `if (var > k_f_X)`. Rule: a Ghidra `k < var` with k a float global becomes `var > k`; test ah,0x41 + jne = `>`/`<=`, test ah,5 + jp = the mirrored operand order.
   Count: appears in every tier 2 function that compares a global float to a k_f_ constant (script: swap sides of every relational op whose left operand is a `k_f_*` identifier, check, keep if identical).

4. fmul operand order with a float constant global (1: hudpanel_countdown_update, 12 -> 4 diff lines)
   asm: original `fld [var]; fmul [k]`, ours `fld [k]; fmul [var]`. `var * k_f_1_125` -> `var * 1.125f` (literal instead of the extern global; the literal lands in the same rdata value, reloc check accepted). 
   Rule: replace `<local> * k_f_<value>` by the float literal when the original loads the local first.

5. Extra stack arg misattributed by Ghidra: callee takes (matrix, float bits) (2: course_superg_cb1, course_nt1_s_cb1)
   asm: original `... push eax(float bits) ... push 0; call get_viewport_camera_matrix; add esp,4; push eax; call func_0x00449c40; add esp,8`: the float is pushed FIRST as 2nd argument of func_0x00449c40, Ghidra passed it to get_viewport_camera_matrix(0, bits).
   fix: `m = get_viewport_camera_matrix(0, bits); func_0x00449c40(m);` -> `func_0x00449c40(get_viewport_camera_matrix(0), bits);`
   With a float-valued expression (nt1_s: fstp into the dead argument slot [esp+0x14]) also declare `extern void __cdecl func_0x00449c40(int m, float f);` and pass `(float)viewport_scale` (no *(int*)& temp).
   Other users of the same callee (an_hk_561, an_hk_839, ...) likely have the same shape: grep `get_viewport_camera_matrix(0,` followed by `func_0x00449c40(`.

6. Constant-offset returns folded into lea (1: career_cell_index)
   asm: original `xor ecx,ecx` (after the call) + `mov eax,1; add eax,esi` per branch; ours `lea eax,[esi+1]`.
   fix: accumulator variable assigned AFTER the call: `zero = 0; if (t == 4) zero = 1; else if (t == 5) zero = 2; return zero + base;` (assigning it before the call puts it in edi: extra push/pop).

7. int global times float constant (1: race_get_time_seconds)
   asm: original `fild [g]; fmul [k]`, ours `fld [g]; fimul [k]` (note: ours folded the int into fimul).
   fix: `(float10)(int)g * (float10)k` -> `(float10)k * (float10)(int)g` (swap the factors; `(float)(int)g * k` also works but rounds).

## Partial: player_set_precise_position (18 -> 14)
   `if (MAX < idx)` (cmp 3, jle) -> `if (idx >= 4)` (cmp 4, jl) fixed the head. Rest: original computes `mov ecx,esi; imul ecx,ecx,0x30c` after loading the float bits to edx; ours imul in place.

## Not fixable by a small C edit (all failed, reason)
- Register-argument callees / tail jmp thunks (7: hudpanel_render_if_loaded_b, stopclock_render_timed_parts_forward, course_city_cb0, vehicle_markers_render_if_loaded, hudpanel_reset_all_b, particle_system_set_current_forward, course_sandsnow_cb2): original passes an argument in eax/ebx/edi or ends in `jmp`; plain C has no usercall and VC7 does not emit tail jmp. These need the naked-asm wrapper and are not a source-tweak class (CALLCONV/INLINE/SIZE tags of the triage are misleading for them).
- Loop induction-variable base (7: weapon_drum/shotgun/flamethrower_clear_effects, message_box_active_count, triggers_assign_ids, cup_scene_destroy_clumps, career_row_for_event): ours rebases the pointer IV to the HIGHEST field offset ([esi-0x18] ... [esi]) and compares via `lea; cmp`, original keeps IV = record start (`cmp esi,END` directly, fields -8..+0x10). Tried: pointer vs (LONG_PTR) compare, != compare, for/index forms, char* byte offsets: no change. Probably needs the original struct-array source form; worth a Haiku/permuter round.
- Others: lua_Vehicle_Shininess_Range (float10 spill is qword in ours, original spills float and reloads with fld), particle_pool_add ([edi+edx] vs [edx+edi] addressing, operand order/types/casts do not change it), menu_state_save (scheduling around rep movs), sound_stub_return_0 (original = 7-case switch with identical targets, cl folds empty cases), vehicle_slots_clear_flags (original mov [m],0 immediates, ours xor + reg stores).

## Workflow notes
- check_one.py on a copy of the installed file works (headers copied); 2.5 s per check. tv.py (replace + check) and ours.sh (objdump -dr of our obj, shows the real reloc names; check_one's disassembly of masked bytes is garbled after the first reloc) are in <scratch>
- The 12 "already MATCH" functions (ui_2d_layout_rects_init, course_call_cb0/2/4/6/8/11, multibyte_to_wide_checked, course_hook9/11, fx_set_blend_modes, course_object_get_float_16c) are byte-identical in the installed file and diffemu EQUAL in output/diffemu_results.csv: bm_a was stale for them, they can be promoted to tier 1 as they are.

## Round B

Result: 10 fixed by source edits (all diffemu EQUAL, 200 cases, cov 1.00/1.00), 13 were already byte-identical with the current
automatch_near source (stale triage; see RESULT.csv `byte-identical-already`, also diffemu EQUAL), 2 improved, 16 failed.
Scratch tooling: <scratch> (cmpfn + unified asm diff), fixed sources in <scratch>

## A. Nested call vs temp variable (arg evaluation order)  -- 4 functions
cup_scene_camera_attach, course_angels_cb0, course_sandsnow_cb0, weapon_mortar_unequip (also pattern B)
asm: original pushes the other args FIRST, then calls the inner function, then `push eax` (`push esi; call get_world; push eax; call RpWorldRemoveCamera`,
`push -2.0f; push 1.0f; push "smoke"; call find; add esp,4; push eax; call sky_create`). Ours called the inner function first into a temp (`call; ... push imm; push imm; push eax`).
fix: delete the temp (`t = f(); g(t, a, b);`) and write `g(f(), a, b)` (ghidra's `x = f(); g(x,..)` split is the cause). Script: find `v = CALL1(...);` followed by a single use of v in the next call
statement, inline it when the other args of that call are constants/locals/params.
Detect by asm: in the orig, a `push X` sequence precedes the `call` whose result is pushed; in ours the call precedes the pushes.

## B. Record base symbol + byte pointer + nested call  -- 1 (weapon_mortar_unequip)
asm: orig `imul esi,esi,0x1c; add esi,BASE; mov ecx,[esi+0x14]; ... mov [esi+8],0`; ours folded the displacement into every access (`mov ecx,[esi+SYM+0xc]`).
fix: `char *rec = (char *)&g_weapon_6a29b8 + idx*0x1c;` using the real record-base symbol (recomp/symbols.txt already has g_weapon_6a29b8 = base, ghidra code used the field symbol
g_weapon_6a29c0 [+8]), then `*(T*)(rec+off)`. Using `&field - 8` does NOT work (disp folded); the base must be its own symbol. Script: when orig has `add reg,IMM` and ours folds,
look up symbols.txt for IMM and re-base.

## C. REGALLOC "dest != source of the last load": function really returns its pointer arg  -- 1 (vec3_make_from_param)
asm diff: only `mov eax,[eax+0x60]` (ours) vs `mov ecx,[eax+0x60]` (orig) at the tail, 2 lines.  The original keeps `out` (eax) live to the `ret`, i.e. the function RETURNS its
last pointer parameter (void in ghidra).  fix: change return type to `undefined4 *` and `return out;`. Callers declare `int f()` / ignore the result, so harmless.
Script: for REGALLOC diffs whose only difference is the destination of a final load in a function that ends with `pop`/`ret` and leaves eax live: try `return <ptr param>`.
(Check the other tier-2 TUs' extern decl, they were `extern int f()`/`void f(...)`, both compile.)

## D. Statement (store) order = asm order  -- 1 (clump_has_uv_anim)
asm: stores of a local aggregate in a different sequence (`mov [esp+0x10],eax` earlier in ours). fix: reorder the C assignments to the order of the stores in the original
(flag=0; callback=...; flag_ptr=&flag). Generic: for pure local/aggregate stores, sort the C statements by the original store order.

## E. Missing call argument ghidra dropped (implicit-decl callee)  -- 1 (object_debug_print)
asm: orig `mov eax,[esp+4]; push esi->push eax; push fmt; call printf-like`: the first call has 2 stack args, source passed only the format (`wprintf("%s\n")`).
Visible as: orig has one more `push` before a call and ours has `add esp,N` smaller by 4 (and the param load is missing). fix: pass the unused parameter (`("%s\n", name)`).
Script: params that are never read in the C body but whose `[esp+N]` load appears in the orig before a call.

## F. Wrong callee prototype from ghidra (arg count / float arg)  -- 1 (course_angels_cb1)
asm: orig `push 0; call A; fstp dword [esp]; push 0; call B; add esp,4; push eax; call C; add esp,8` = C(B(0), (float)A(0)) with C taking (int,float).
Ours had B(0,bits-of-float) and a double/int juggling with a float10 temp. fix: declare `extern void C(int,float);` (A is `float A()` already), write the nested call. Sign: fstp into the
pushed arg slot, `add esp,8` for 2 args after only 1 explicit push.

## G. Two induction variables -> one  -- 1 (weapon_machinegun_clear_effects), also fx_slots_clear
asm: orig walks one pointer: `mov eax,[esi-0x48]; sub esi,0x48; test eax,eax ... mov [esi],0; cmp esi,END` / `sub eax,0x68; cmp eax,END; mov [eax-4],ecx; mov [eax],1`.
Ours: `lea esi,[eax-0x48]` + extra mov (a second variable `slot = cursor - N; ... cursor = slot;`) or compare via `lea edx,[eax-4]`.
fix: a single pointer `p = p + -N; ... if (*p) ... } while (p != END)`; the loop-carried copy `cursor = fired_flag` must go; bias stores as `p[-1]`.
Script: pattern `X = Y + -K; ... Y = X;` inside a do/while -> collapse into Y.

## H. Not fixed -- register-input callee wrappers (CALLCONV / INLINE / SIZE)  -- 12 functions
smplfzx_object_get_position, stopclock_render, weapon_mines_update, weapon_flashbang_update, hudpanel_render_if_loaded, debris_slot_clear_forward, course_city_cb2 (tail jmp to eax-arg callee),
weapon_oil_pick_slot_forward, course_dump_cb1, object_frames_create_forward (eax/ecx + stack hybrid), plus the adapter-based ones.  The original passes a value in esi/edi/eax/ecx to the callee
(`mov esi,IMM; call X` or `mov eax,[esp+4]; jmp X`); our C goes through a generated naked `_r` adapter (an extra call/push/pop frame) or a plain tail `jmp`. Not reachable by C edits without
__asm (cheatgrep forbids) -> needs the cluster mechanism (usercall root) or a compiler-aware inline-register annotation.  Try `tools/cluster_install.py` for these.
Recognise by: orig has `mov esi|edi|eax,IMM` right before a call to a function with no stack push for that value, and/or the callee is already an `_r` adapter in the source.

## I. Not fixed -- seen but unsolved (notes for scripting)
- rain_splash_add (9 -> 7): `if (7 < (int)g)` -> `if ((int)g >= 8)` gives `cmp 8; jl` (pattern: signed compare of `undefined4` global, ghidra's `7 < x` produces `jle`; write `x >= 8`). Remaining 7 lines: orig reloads param_2 for the last store (no CSE) -> unknown source shape.
- weapon_flashbang_equip (8 -> 6): record pointer `rec` + `clump = (&g)[idx*5]` (read before stores) + nested `RpWorldAddClump(course_get_rpworld(), clump)`; remaining: orig loads `slot` (arg) before `push esi`.
- controller_dialog_copy_name: orig layout puts the `return 0` copy inline after `jne`; `char *dest` param + `dest[(int)src]` gives the orig operand order `[edx+eax]`, but the branch layout/`lea` vs `add` stays.
- menu_table_clear_b: the compiler converts the loop into a counted loop whenever the end is the array symbol; orig compares against a *different* symbol address (end+4). Keeping the `(LONG_PTR)` end constant kept the count-less compare but the base register is s+8 instead of s+4.
- particle_batch_set_uv_a, video_get_window_size_a, fx_system_flush, lua_Physics_Scenario_Add_Animated_Object: scheduling / hoisting differences (orig loads global before the param; orig `add esp,8` before the global store; orig re-reads count after stores). No source shape found in 3 tries.

## Generic lessons
1. Many "diffs" in the triage are stale: 13 of the 41 functions already compile byte-identical (+reloc check) from the installed source. Re-run cmpfn on the candidate list before spending effort.
2. A cheap scripted sweep that would have caught A/E/F/G: for each near function, try (a) inline single-use call temps, (b) add unused parameters to the call that precedes them, (c) collapse `X=Y-K; Y=X` loops, (d) `return <last pointer param>` for REGALLOC-only diffs.

## Round C
Metric: cmpfn.check (reloc-masked) via <scratch>; scratch copies src/w_<name>.c; merged home files fix/an_*.c (git merge-file per home).

1. Switch case-block order = asm block order (gamesave_cb_save_state, medal_asset_names; 2)
   asm: same code, case blocks laid out in a different sequence; `default:` that shares a tail with `case 5:` -> put `default:` body first and let it fall into `case 5:`.
   edit: reorder the C `case` groups to the order of the blocks in the original (jump-table targets sorted by address). Also `999 < x` -> `x >= 1000` (cmp 0x3e8; jl).
2. Struct assignment instead of member-wise copies (atomic_world_bbox_update, smplfzx_object_init_bounds; 2)
   asm: `lea eax,[esi+0xc]; mov ecx,[eax]; ... [eax+4] ... [eax+8]` (3 dword copy through a pointer). edit: `world_inf = bbox->inf;` / `*(RwV3d*)(dst) = *(RwV3d*)(src)`.
   Same with a local 4-float struct typedef (explosion_queue: `*(Vec4F*)dst = *(Vec4F*)src`). Also swap two local-pointer init statements to fix eax/ecx swaps.
3. Float-typed stores of constants (7: team_colour_table_init_4, sound_spline_create_b, app_counters_clear, ...; ui_colour_a_default partly)
   asm: original `mov dword [x],0x3f800000` / `mov dword [x],0` immediates; ours hoists repeated int constants/zero into a register (`xor eax,eax` + reg stores, `mov ecx,IMM`).
   edit: store through float: `*(float*)&g = 1.0f;` / `= 0.0f;` or make the local table `float t[12]` with float literals (cl does not CSE float immediates). Script: f32.py converts hex bit patterns to shortest float literal.
4. Record base pointer + field offsets (ktc_new_copter 23 fields, weapon_oil_equip; same as Round B pattern B)
   edit: `char *rec = (char*)&BASE_SYMBOL + idx*SIZE;` then `*(T*)(rec + (field_va - BASE_va))`. Script converts `(&g_x_XXXXXX)[idx*N]` -> rec offsets.
5. `v->field |= X` / `&= ~X` through a record pointer (vehicle_set_flag_1000/10/40; 3)
   asm: orig reloads `[eax+0x294]` before each branch, ours kept a `new_flags` temp (CSE). edit: `struct R *v=&tbl[i]; if(on) v->flags |= F; else v->flags &= ~F;` (no temps, `on != 0` first = `je clear` layout).
6. Early if/else restructure + last `return a+b` -> accumulate (player_stat_get_*_team/total; 5)
   asm: team test `je` jumps to a solo block placed AFTER the team code; last team term `add eax,[mem]` not `mov ecx,[mem]; add eax,ecx`.
   edit: `if (is_team_mode() != 0) { ...; agg = agg + x[3]; return agg; } return solo;` -- i.e. no `x = is_team_mode()` temp, last branch assigns instead of `return a+b`.
7. lua_tonumber double -> float local without float10 (lua_RainSetDirection, lua_RWP_Object_Box/Capsule; 3)
   asm: orig `call lua_tonumber; fstp dword [mem]` directly; ours float10 temp + qword spill. edit: `desc->f = (float)lua_tonumber(...)` (no float10 local), or `float x = (float)lua_tonumber(..)`.
8. Compare through the memory lvalue, no float10 temp (noise_table_init, sound_ducking_values_decay; 2)
   asm: `fcom dword [k]; test ah,5; jp` / `fcomp [k]; test ah,0x41; jne` on the value just stored; ours `fld [k]; fld st(1); fcompp`.
   edit: `*e = (float)random(); if (*e < k) *e = 0; else if (*e > one) *e = 1;`  and for Ghidra's `(a<b)==(a==b)` NaN idioms: `x > k` (jne after test 0x41), `x <= k` (jp after test 0x41).
9. Single-use call temp inlined (nested call args) (best_time_store; same as Round B A)
   asm: `lea esi,[eax+eax*2]` after FIRST call (row*3) before the second call. edit: `arr[get_row()*3 + get_menu()] = ...` (no `row = get_row(); menu = get_menu();` temps).
10. Uninitialised/dead argument = live register (attract_timer_update)
   asm: `push eax` (value left in eax by the preceding test) where Ghidra shows `f(0)`. edit: pass the variable that was last in eax (`message_state` which was tested ==0) instead of a literal 0.
   + `if (flag != 0) {A} else {B}` order = `je B` layout; zero loop becomes `rep stos` with `int *p; for(n=0x98;n;n--) *p++ = 0;`.
11. Callee prototype has Ghidra phantom parameters (app_state_frontend_frame: video_mode_name_format(p0,p1) -> (p0); car_damage_at_world_point: 11 params -> 3)
   asm: `add esp,N` smaller than the declared param count; the pushes of the extra args are missing in the original. edit: shrink the extern prototype to the real count and drop the garbage args (check callers in the same file).
12. Matrix `flags |= X` statement position (camera_save; 1; hudpanel_load_indicator_update partially)
   asm: `mov [esp+x],esi` (the flags store) sits right before the first call, after the 12 element stores. edit: move `m.flags = m.flags | X;` to after `pos.x = 0.0;` (last init statement).

## Improved but not identical (notes)
- hermite_interpolate 50->2: float literals instead of k_f_* (pattern 4 of Round A); remaining: orig loads `mov eax,[esp+0xc]` before `fld t`.
- vec3_is_valid_small 39->4, career_time_under_target 65->6, lua_Physics_Scenario_Trigger_Add_Objects 28->8: need exact source expression shape: cl's operand load order is independent of the left-assoc term order (tried all 6 permutations); `lea` vs `add` for `&sym+idx*stride` base pointers not reachable (folded).
- hudpanel_fuel_meter_update 20->4 (scheduling of a float load across arg pushes), training_text_box_c 21->6 (two byte[4] colour locals: store order differs), car_get_velocity_any_mode 29->19, vector_normalize_copy 21->10, hsv_phase_to_rgb 41->17 (`fld k; fsub st,st(1)` vs `fsubr [k]`), hudpanel_load_indicator_update (register-input callees at the tail: unreachable).
- Not fixable here: ai_bot_weapon_logic (orig shrink-wraps `push esi` after an early return 0), register-argument callees (weapon_mines_fire, course_city_cb5 tail jmp), loop IV rebase (weapon_mines_course_33_update), menu_select_pressed_any / menu_event_column_progress_unless_special (different branch structure, not tried beyond reading), round_stats_reset (three alternating zero registers eax/ecx/edx, unknown source).

## Round D

Targets: <scratch> (82 functions, 11-48 differing lines). Scratch: <scratch> (src/ working copies, fix/ = full-file replacements
spliced into the CURRENT recomp/automatch_near files (byte-identical, diffemu EQUAL), fix_improved/ = partial progress, RESULT.csv).  Tools: t.py (cmpfn + asm diff), rf.py (replace one function and check), splice.py, regress.py.
NOTE: recomp/automatch_near/*.c changes underneath (another agent / gate): never copy a stale scratch file back, re-run splice.py against the installed file.

Result: 41 of 82 byte-identical (all diffemu EQUAL), 12 improved (fix_improved/, near/ has the function snippets), 29 unchanged.  "(n: names)" below = functions fixed by that edit.

## New patterns (asm symptom -> source edit)

1. Early `return` block instead of nested if (1: viewport_raster_destroy)
   asm: original has `pop edi; xor eax,eax; pop esi; ret` duplicated after an early `jne`/`je`, ours has one shared tail and `je` to the end.
   fix: nested `if (a) { if (b) {...; return 1;} } return 0;` -> flat `if (!a) return 0; if (!b) return 0; ...; return 1;` (and `VAL < x` -> `x >= VAL+1` for the cmp 4 / jb form).

2. if-chain on 0/1/2 -> switch (2: atomic_lookup_error, voice_queue_select_tables)
   asm: original `mov eax,[arg]; sub eax,0; je A; dec eax; je B; dec eax; je C; default` (or `dec eax; je ...`), ours `cmp eax,1; jne ...; cmp eax,2`.
   fix: `switch (x) { case 0: ... break; case 1: ...; default: }`; the case ORDER in the source is the code layout order (event_type_from_menu_choice: `case 5` moved behind `case 8`
   so its block is emitted after case 8; the jump table is unchanged).

3. Loop copy of 6/12/16 dwords -> struct assignment (rep movs) (4: vehicle_get_matrix, material_plugin_stream_read, + wip btb_panel_slot_set_matrix, particle_system_set_matrix_table)
   asm: original `mov ecx,N; rep movs`; ours a dword loop `mov esi,[ecx]; mov [eax],esi; add ...; dec edx; jne`.
   fix: `typedef struct { undefined4 d[N]; } DwN;  *(DwN *)dst = *(DwN *)src;`.  3-dword vectors: `*translation = matrix.pos;` / `to_origin = frame->modelling.pos;` (RwV3d assignment) gives
   the `mov edx,[ecx]; mov [esp+4],edx ...` through-a-pointer copy.  Hand-written clear loops -> `memset` (see 9).

4. Record base symbol + char* + nested call (B again) (vehicle_get_matrix, weapon_machinegun_unequip, weapon_flamethrower_unequip, btb_panel_slot_render, lua_Physics_Scenario_Create_Trigger, fx_ring_add_point, text_page_add_string_id)
   asm: original has ONE base register per record (`lea esi,[eax+eax*8]; lea esi,[esi*8+SYM]`, `mov [esi+0x10],3`), ours folds the displacement into every access (`[esi*4+SYM+off]`) or recomputes imul.
   fix: `char *rec = (char *)&g_BASE + idx*STRIDE;` then `*(T*)(rec+off)`.  The BASE symbol must be the record start (recomp/symbols.txt has e.g. g_physics_scenarios 0x89de20 for the
   lua scenario slots, add the `extern` if the TU lacks it).  A second counter that lives in the same record is `*(int *)(rec + 0x2428)` (not a separate global).

5. Single-use temporaries around calls (A again) (weapon_machinegun_unequip, best_time_store_previous, course_arctic_cb3, course_apply_clip_planes, race_lose_banner (wip))
   asm: original pushes the other args first and calls the inner function last (`push 0x2; push 1.0f; push color; push 60.0f; call get_x; fadd`), ours calls first into a temp.
   fix: inline the call into the argument list; for index expressions `arr[get_row()*3 + get_slot()]` write the calls inside the subscript (the `*3` of the first call is then done BEFORE the second call).

6. lua_* wrappers: drop `float10 value` and the index temp (5: lua_RWP_Object_Cylinder/CentreOfMassOffset/Properties, lua_RainSetPosition, lua_Physics_Scenario_Create_Trigger)
   asm: register roles differ (state edi / count ebx / record esi in the original), ours has an extra `mov ebx,eax` and `add esp,0x1c` after the last fstp.
   fix: `obj = &g_rwp_objects[(int)lua_tonumber(L, n-2)];` (no `object_index` temp), `obj->f = (float)lua_tonumber(L, n);` directly (no `value = (float10)...; obj->f = (float)value`).
   RainSetPosition: six `float xmin = (float)lua_tonumber(...)` locals, then `impl(*(int *)&xmin, ...)`.

7. Packed RGBA constant -> RwRGBA locals built from byte stores (3 + 1 wip: training_text_box_a/b, course_training_cb1, race_lose_banner_draw)
   asm: original `mov al,0xd8; mov [esp+..],al ...; mov esi,[esp+0xc]; push esi` (colour assembled from byte stores, loaded as a dword), ours `push 0xff000000`.
   fix: `RwRGBA c; c.red=..; c.green=..; c.blue=..; c.alpha=..;` and pass `*(int *)&c`.  Order matters for which byte register is reused: initialise the colour that has the
   non-zero bytes FIRST completely (all four fields), then the other one.  A colour that lives in an overwritten parameter slot: `((RwRGBA *)&param_1)->red = 0x6c` (course_training_cb1).

8. float/double return type of the callee decides fcom m32 vs fld+fcompp (2: ai_find_leader_b, course_arctic_cb3; also dot_product_3d_clamped partially)
   asm: original `call f; fcomp DWORD PTR [k]` (compare straight against the float global), ours `call f; fld [k]; fcompp`.
   fix: declare the callee `extern float f();` instead of `double` (player_distance_get, random_range) and compare the float result directly with the float global (`f() > k_f_0_9`, no float10 casts);
   in the same spirit float arithmetic (`float dot = a*b + ...` instead of float10 casts) gives `fcom m32` in dot_product_3d_clamped.

9. memset intrinsic for hand-written clear loops (1: rwp_world_clear_arena)
   asm: original `rep stos DWORD; mov ecx,esi; and ecx,3; rep stos BYTE` ; ours a counted store loop.
   fix: `extern void *__cdecl memset(void *, int, unsigned int); #pragma intrinsic(memset)` and `memset(ptr, 0, count*stride)` (inline expansion at /O2).

10. `%` computed as multiply (1: ui_compare_sign)
   asm: original `imul edx,edx,0x1770; sub ...` (magic-multiply division, remainder by subtraction), ours `idiv`.
   fix: compute `q = a / C; r = a - q * C;` explicitly instead of `a % C` next to `a / C` (cl then uses idiv for the pair).

11. Narrow (byte) globals passed to a callee: `xor eax,eax; mov al,[g]` (1: course_apply_clip_planes)
   asm: original `xor eax,eax; mov al,ds:g; push eax`, ours `movzx eax,BYTE PTR g`.
   fix: give the callee a prototype with `unsigned char` parameters (`extern int game_set_option_flags(unsigned char, unsigned char, unsigned char);`).

12. float-typed parameter whose value is copied twice (1: weapon_particle_spark_emit)
   asm: original `mov ecx,eax` before the second store of the parameter, ours stores eax twice.
   fix: declare the parameter and the 2-element destination as `float` (`float size; float size_pair[2];`), stack ABI unchanged.

13. float constant arithmetic written as literal / float expression (3: spline_get_length, course_rotating_object_integrate, + frame_rotate (wip))
   asm: original `fld [var]; fmul [k]` / `fld [t]; fadd [k]` (variable first), ours `fld [k]; fmul [var]`.
   fix (Round A #4 again): `var * 0.99f`, `t + 0.001f` (literal; cmpfn compares the rdata constant by value).  For `<=` loops written as `a < b != (a == b)` use `while (t <= 1.0f)`.
   Hex constants stored through `undefined4` fields (sound_spline_create_a: 33 immediates hoisted into eax) become immediates again when written as float literals in an RwV3d[11].

14. Ghidra junk call arguments / wrong prototypes (1: race_load_common_assets, + weapon_flamethrower_unequip)
   asm: original pushes fewer arguments; ours has runs of `push 0` (`load_cup_model(a,b,c,...)` with nine unused locals, `texdict_load_named_b(..)` fine, `fx_dust_create_default(100,h,name)`,
   `object_set_flag_4(x,0,clump,flag)`).  Count the `add esp,N` against the pushes; fix the prototype (K&R `extern int f();` accepts anything, so the junk compiled) and delete the unused locals.

15. Chained condition tests of a float field as int (weapon_flamethrower_course_33_update)
   asm: original `mov eax,[esi+0x30]; test eax,eax; je` (field != 0 tested as an int), ours `fld; fcomp QWORD; test ah,0x44`.
   fix: `*(int *)&translation[4].x != 0` instead of `translation[4].x != 0.0`.  Also `flags = m.flags | 0x20003; ...stores...; m.flags = flags;` keeps the `or` early and the store late.

16. Single induction pointer with post-increment offsets (G again) (weapon_rocket_course_33_update)
   fix: `active = p[-0x1e]; p = p + -0x1b; if (active) {... p[0] ...; *(V3 *)(p - 0x14) = *(V3 *)(frame + 0x30);} } while (p != END)`: read the flag BEFORE decrementing, use the post-decrement offsets
   after, struct copy for the 3 floats.

17. Global loaded before the branch (cup_scene_render, + pattern 5/8 together)
   asm: original `mov eax,ds:RwEngineInstance` and `mov eax,[eax]` (curCamera) are done before the `cmp esi,-1; je`, ours loads them inside the taken branch.
   fix: `cam = RwEngineInstance->curCamera;` as a statement right before the `if (car_index != NO_CAR)` (after the other stores), use `cam` inside.  Same function: the float10 temp
   `car_value` is replaced by `vehicle_get_float_184(car) <= threshold` with `extern float vehicle_get_float_184();` (pattern 8), the viewport camera calls are inlined as arguments (pattern 5).

18. Branch-specific arguments feeding ONE call (phi variable + tail duplication) (lua_ExpObj_Clump_Mts_Pair, players_get_wins/compare-order below)
   asm: original has two identical-looking call sequences with `xor eax,eax; push eax` for the constant argument and the earlier results parked in ebx/ebp, ours has two calls with `push 0`.
   fix: `if (n == 2) { a = f(1); b = f(n); c = 0; } else { a = ...; b = ...; c = f(n); }  g(b, a, c); return 0;`  (c is a phi, so the 0 is not folded into a push; cl duplicates the small tail itself).
   The `n` in `f(n)` for the second argument is the variable (not the folded constant 2): once the join is shared cl no longer propagates the branch condition.

19. Operand order of a compare decides the load order (players_get_wins)
   asm: original loads `wins[pos+1]` first (eax) then `wins[pos]` (edx), `cmp edx,eax`; ours loads wins[pos] first.
   fix: `next = wins[pos + 1]; if (wins[pos] < next) { cur = wins[pos]; wins[pos] = next; wins[pos+1] = cur; ... }` (temp for the right operand first; a temp for the left operand changed the register allocation).
   The out_order[] swap through `out_order + (wins - ...)` pointer arithmetic of the Ghidra source is just `out_order[pos]` / `out_order[pos+1]`; cl rebuilds the same `lea esi,[ebx-wins]` form.

20. then/else order = code layout; return inside the else-chain breaks it (5: player_stat_get_kills/turbo_used_team/unforced_errors/aggression_total_team/wall_hits_total_team)
   asm: original `call is_team_mode; test eax,eax; mov ecx,[esp+4]; je SOLO_AT_END ... team code ... ret`, the player index is loaded once before the branch and the solo arm
   (`imul ecx,ecx,0x138; mov eax,[ecx+X]`) sits behind the team arm sharing the final `ret`.  Ours put the solo arm first (`if (v == 0) {solo} else {team}`) and the team arm had its own `return v + stat3`.
   fix: `if (v != 0) { team...; if (...) v = v + stat3; } else { v = solo; } return v;`  i.e. TEAM FIRST, no `return` inside the arms.  Scripted (ps_tr.py): applies to every
   Ghidra `if (x == 0) { x = A; } else { ... return x + B; } return x;` shape.

## Not fixed in this round (reason, closest edit)
- Register-argument callees / `_r` adapters (menu_options_defaults_reset, weapon_drum_fire, fx_explosion_spawn, powerups_*): same as Round B #H.
- Pointer IV rebasing (ai_bots_reset: the dword store of 0x3e8 is right, base is 4 higher than the original; weapon_fx_grid_clear: cl moves `sub eax,0x34` to the loop bottom, the
  original keeps it on top; weapon_*_clear_effects: only the `lea esp,[esp+0]` loop-alignment pad differs; fx_styles_init_all).
- menu_back/pause_pressed_any: loops match as `do { ... } while (p < END)` (END = symbol+1, byte pointer), but the original pushes esi/edi before the input-mode test (prologue not shrink-wrapped).
- Operand-order / scheduling near-misses: frame_rotate_around_own_origin (struct copy + `* -1.0f` literal ok, fmul/mov interleave differs), car_push_from_matrix (all instructions right, one fmul vs push scheduled
  one slot apart), dot_product_3d_clamped (float arithmetic + fcom m32 ok, `mov ecx,[esp+8]` vs first fld order), pool/particle_system_set_matrix_table (`[ecx+eax]` vs `[eax+ecx]` SIB order of an
  `atomic->4 + g_plugin_offset` expression is canonicalised by cl), ptank_set_size (size pointer in ecx vs eax), btb_panel_slot_set_matrix (flags RMW `or [mem]` vs load/or/store; zero-register compare is automatic).
- Struct-ish jump-table/branch layout: placement_counter_increment (goto-shared blocks A..D are right, the count-3 dispatcher shares the count-2 chain in the original but not in ours), frontend_enter_menu (tail `jmp`).

## Round F

Tools (scratch <scratch>): all.py / metric.py (batch cmpfn + diff-line metric over bm_f.txt), sh.sh (source + asm diff of one function), ours.sh (objdump -dr of our obj),
f32.py (rdata float -> shortest literal), auto.py / inl.py / swp.py (cheap automatic transform sweeps: compare flips, single-use call inlining, adjacent store swaps; no full match came out of them, hand work did),
splice.sh (3-way splice onto the live files).
Classification of the 221 failures: 39 register-argument callees (`_r` adapters), 18 need the /GS security cookie (`xor eax,[esp+N]` against 0x62c5d8, arrays on the stack), 3 tail jmp, rest scheduling / regalloc.

### New patterns (asm symptom -> source edit)

1. Count guard of a loop is unsigned: original `test ecx,ecx; jbe` (FUN_005c2180, FUN_005c21e0, piz_fs_exists), ours `je`.
   fix: `if (0 < *(uint *)(...))` / `if (0 < (uint)g)` instead of `!= 0` / `(int)` compare. Same for the loop test `channel < (uint)g` (jb vs jl).

2. `-(uint)(x != 0) & (uint)p` idiom (FUN_005c4340): original `neg eax; sbb eax,eax; and eax,ecx`, ours `setne/neg/and`.
   fix: `(x != 0 ? 0xffffffffU : 0U) & (uint)p` (or `x ? p : 0`).

3. Early-return chain instead of nested ifs (atomic_plugin_register, material_plugin_register): original has ONE shared `return 0` block right behind the checks.
   fix: `if (a < 0) return 0; ...; if (g != r) return 0; post(); return 1;` instead of `if (a>=0) { if (..) { if (g==r) {...; return 1;} } } return 0;`.
   NOT reproducible: rw_plugins_attach / joypad_get_axis_info (return-0 block laid out first, backward jumps); cl normalises goto/label tricks.

4. `x / 4U` then `* 4U` in two statements (FUN_005bf0e0): `((raw + 4) >> 2) << 2` and `& ~3` fold into `and`; `aligned = (raw + 4U) / 4U; aligned = aligned * 4U;` gives `shr 2; shl 2`.
   (also `if (0 < (uint)raw)` for `test; jbe`.)

5. If/else arm order decides the layout (rwp_object_set_in_world): write the arm the original falls into FIRST: `if (add != 0) {add...} else if (link != 0) {remove}`.
   `while(true)` with the bound check at the bottom -> `do { ...; return 1 inside } while (p < END); return 0;` (sound_streams_are_idle).
   `x >= 0` instead of `-1 < x`; `object_id <= 200` instead of `< 201`.

6. Phantom call arguments / wrong prototype: texture_read_png_logged (RtPNGImageRead(p0..p3) -> 1 arg), course_rouabout_cb2 (course_quad_render_b 3 args -> 1),
   course_sandsnow_cb1 (float arg stored into the pushed slot: `void __cdecl f(float)` and `f2(get_matrix(0), g(0))`, `float10` local for the int->float conversion),
   sound_music_fade_step_b (`extern int __cdecl f(float)` gives `push ecx; fstp [esp]` instead of the double spill).
   A callee result that is returned: `int` instead of void (rx_pipeline_create_for_node_5003b0; FUN_004b1140 returns len, which also keeps the indexed form).

7. Nested call as argument, other args pushed first (camera_world_attach, trail_clumps_create, screen_clear_black): `RpWorldAddCamera(course_get_rpworld(), get_viewport_camera(0))`, no temps;
   delete the Ghidra junk locals (zero_offset / float_arg_b.. in trail_clumps_create: pass 0.0f, 0x3dcccccd.. directly).

8. Record base symbol + char* record (player_rumble: base symbol g_player_inputs_0__field23_0x20 = 0x7ef840; weapon_oil_unequip; vehicle_render_state_init):
   `char *rec = (char *)&SYM + idx * STRIDE;` then `*(T *)(rec + off)`; the base must be the symbol at the `add reg, IMM` constant (look it up in symbols.txt). Do not leave a use of the old offset variable.

9. Struct copies (cl emits rep movs / register-pair copies): `typedef struct { undefined4 d[N]; } DwN; *(DwN *)dst = *(DwN *)src;` (FUN_004c6740 N=0x102b, voice_queue_add N=10, vehicle_render_state_init N=4);
   3 floats: `*(RwV3d *)dst = *(RwV3d *)src;` (course_active_object_get_info, course_zone_read_frame_state, sound_voices_set_position_start).
   Read through a local pointer first: `sub = frame + 0x40; ... *(float *)(sub + 0x30)` reproduces `lea eax,[eax+0x40]` + `[eax+0x30]` (course_zone_read_frame_state, with the `360.0f` literal).

10. Colour as RwRGBA bytes (fx_object_update_frame, stopclock_load, cup_scene_update_light_colour): `RwRGBA c; c.red=..; c.green=..; c.blue=..; c.alpha=..; *(int *)&c`.
    The byte stores must be written BEFORE the call that precedes the use of the colour (stopclock_load: colour built, then FUN_004c5410, then the store). Three colours = three locals initialised in the order of the original stores.

11. Store order = asm order for global stores around a call result (load_btb_panel, stopclock_load: `g_clump = clump;` first, then the field copies; camera_create_a: up-vector locals first, then `g_camera = ...`).
    A global that points at a freshly created object: `m = RwMatrixCreate(); ...stores through m...; f = m->flags | X; g_ptr = m; m->flags = f;` (spring_grid_init) instead of re-reading the global.

12. float typed copies (course_smoke_particles_create): `float local_8, local_4` copied from `*(float *)&g` gives the `mov ecx,eax` second register for the second store.
    `float10 xy_radius` local (random_unit_vector) keeps the sqrt result on the x87 stack across both fmuls; `float angle` local + `cos(angle)`.

13. Compare operand order / float compares: `g > float_1_0` not `float_1_0 < g` (sound_music_fade_step); `(timer <= k_f_0)` not the NaN idiom and `1.0f` literal as the first operand (weapon_rocket_spin_update);
    `piz_fs_eof`: write `limit >= pos` (limit loaded first).

14. Dead param slot is reused by cl for an address-taken local (btb_panel_slot_init): the 4-byte colour local lands in the param slot (no `push ecx`), unreachable by reordering; keep in mind when an original has `push ecx` for a small local.

15. Boolean argument `(uint)(x != 0)` (window_create AdjustWindowRect): the original duplicates the call: `if (x != 0) f(...,1); else f(...,0);` (no setne).

16. Constant 0 argument that is also the result (line_exceptions_load): the original keeps a zero register for `RwStreamClose(stream, 0)` and `return 0`:
    `int result = 0; ... RwStreamClose(stream, (void *)result); result = 1; ... return result;` (same value; cl then keeps the zero in edi).

17. sound_stop_all_voices_and_streams: `if (c > 0) { x = t[c]; if (x != t2[c]) f(x); } else f(t[c]);` (reload in the else arm gives `mov ecx,[esi*4+X]; push ecx`) instead of a goto-shared tail.

### Tried, not fixable by C edits here
FUN_005c1eb0 / FUN_005b72d0 / FUN_005b7330 (callee argument evaluation order: movzx word before the extra arg load), noise_interp_cos / noise_sample_row (x87 stack kept across calls, cl spills),
voice_queue_advance_timers ([esi+ecx] SIB order), effect_slot_alloc (zero register kept for `return 0`), FUN_0056f900 / FUN_0056f3a0 / FUN_0056f0d0 (`and eax,0x4100` compare form: RenderWare SDK code built with other flags),
course_collision_objects_build (store of a call result delayed past an argument load), geometry_transform_points (cursor assigned inside the if changes register allocation), FUN_005c1dc0 (node/state register swap; list-tail fixed: `head_slot = list + 0xc; *node_slot = *head_slot`),
fx_explosion_systems_init (cursor IV rebasing, cl folds the first iteration constant), course_script_run (float constant stores hoisted into a register), FUN_005b6730 (switch block order).

### Not fixable (reason)
- register-argument callees (39): car_damage_update_all, hudpanel_slots_clear, the team/endpoint/debris/machinegun `*_clear/_render/_update_all` loops, player_equip_weapon, weapon_*_hook_2c, sector_table_clear, ... need the cluster/usercall mechanism.
- /GS cookie (18): ai_file_name, led_file_name, load_copter_model, build_info_draw_page_*, ... need the security cookie prologue.
- tail jmp: texdict_load_named_b / _d.

## Round G (/GS)

- Functions with the /* compile: /GS */ marker must be checked with check_one.py <file> <fn> /O2 /GS --va=0x...; cmpfn alone does not read the marker (batch F's 'gs-cookie' failures were false).

## Round E

Result: 60 byte-identical (cmpfn incl. relocation targets, all diffemu EQUAL, files spliced onto the live automatch_near files: fix/*.c, 60 files),
40 not attempted beyond a look (naked `_r` register-argument adapters), 164 failed (best partial diffs in RESULT.csv), 5 skipped (already in recomp/tier1).
Tools: t.py (cmpfn + asm diff), ap.py (replace snippets + check), fn.py (replace one function from stdin), fc.py (fastcall symbol), hill*.py / auto*.py (scripted sweeps; they found nothing on their own),
splice.py (3-way merge onto live), run_de.sh (diffemu), final.py (RESULT.csv).  Notes per function in RESULT.notes.csv.
NB: `/* compile: /GS */` marker needed for stack-cookie functions (t.py honours it).

1. (C again) setter that loads param 0 first: return the object pointer param (FUN_00571830).
2. Zero-initialised result variable: `r = 0; if (p) r = f(p); return r;` gives `xor esi,esi ... mov eax,esi` shape (FUN_004c4020, debris_model_get_first_clump).
3. `if (count != 0)` on uint with `jbe` in the original: write `if (count > 0)` (FUN_005b9fa0).
4. Local struct whose members hold address-of values: member init statement order = store order (atomic_set_material_alpha_value).
5. `-(uint)(x != 0) & (uint)p` -> `x != 0 ? p : 0` (single neg/sbb/and) (FUN_005c9380).
6. `bool` return type emits `mov al,cl`; original `mov eax,ecx`: return int (lua_run_buffer_ok).
7. Wrapper `void f(){g(h());}` -> return the callee value (pop ecx vs add esp,4) (rx_pipeline_create_for_node_5009c0).
8. unsigned/int sign test: original 'test r,r; jns' = (x & 0x80000000) != 0 in source (FUN_005bf4e0). Compare operand order: 'a <= *p' -> '*p >= a' (FUN_005bcf00).
9. Vtable call with 'mov ecx,[x]; mov eax,[ecx]; call [eax+N]' (this in ecx): cast the slot to a __fastcall function pointer taking the object and pass it (FUN_004b0580).
10. Range check 'if (a && b) {body} return 0' -> 'if (!a) return 0; if (!b) return 0; body' with SEPARATE ifs (a||b is merged by cl) (rwp_object_get_matrix).
11. Phantom call args again (pattern 14 R-D): add esp,N smaller than pushes: shrink prototype (texture_read_log).
12. Float argument loaded as `*(float*)(p+off)` directly becomes `mov eax,[..]; push eax`; the original `fld [..]; ... fstp [esp]` needs a float local (`RwReal t = *(RwReal*)(p+off); f(.., t)`) (anim_update_hierarchy).
13. `bool f(){ ..; if (r==0) return false; ..}` where orig returns the dead eax (0): int return type and `return r;` (rw_register_image_formats). (bool also yields `xor al,al`/`mov al,cl`.)
14. Call temps (smoke texture, scale, offset locals) -> inline: f(find("smoke"), 1.0f bits, -2.0f bits) (course_nt1_s_cb0).
15. 'x & 0xffff' on a uint param where orig has movzx r,word [esp+N]: declare the param unsigned short (pool_create).
16. Loop counter increment statement position decides whether cl hoists 'inc' above the call: put 'i = i + 1' last (FUN_004c52f0).
17. Local record passed by pointer is bigger than the declared fields (original 'sub esp,0x8c' vs ours 0x20): add 'char pad[N]' to the local struct; RwV3d struct assignment for xyz copy (trigger_register_object_type8).
18. /GS stack-cookie functions: the file needs the '/* compile: /GS */' marker (tools/gsfix.py) or cl emits no cookie; then source order of 'dst = buf' decides the register (course_model_path_copy). Other candidates: functions with a char[] local whose original starts with 'mov eax,ds:___security_cookie'.
19. Fastcall caller whose first call has 'push ecx' (stack alloc) then 'call' with ecx still holding the own arg: callee is __fastcall(arg) too; declare `extern int __fastcall f(undefined4)` and call `f(arg)`; ours had pushed the arg (race_ai_slot_assign_or_adjust). Name for cmpfn: @name@4.
20. 'g = g + k' / 'g = g - k' in two branches where the original loads g BEFORE the test: `float b = g; if (c) b = b + k; else b = b - k; g = b;`, and the branch condition polarity is the else/then layout ('!= 0' first) (race_ai_slot_assign_or_adjust).
21. Loop 'p < (T*)&end_symbol' compiles to unsigned jb; original has signed jl: compare as '(int)p < (int)&end' (camera_create_b). Also stores order: init the locals first, then the global store.
22. NaN-form compare 'a <= k' gives 'test ah,0x41; jp' but orig 'je': write '!(a > k)'. A double expression (fabs()) compared with a float global gives fld k; fcompp, orig 'fcomp [k]': cast the double to (float) (course_object_distance_table_build).
23. In '||' chains of float compares 'a < b' gives 'test ah,5; jnp'; orig 'test ah,1; jne' = '!(a >= b)' (sphere_vs_box_test).
24. Bounding sphere/vec literal block assigned field-wise through a macro lvalue: build a local RwSphere right after the first call and assign the struct (16-byte copy through ptr); 'p=&f; *p = *p | X' -> '*(uint*)(addr) |= X' gives 'or [mem],imm' (fx_ptank_atomic_b_create).
25. switch(x){case k: v="str"; break; ...} followed by f(a,b,v): original has 'push "str"; jmp common_pushes_and_call' = each case contains the full call (cl cross-jumps the identical tail): write the call in every case (game_content_exists_prefixed).
26. 'string_funcs = &stringFuncs; (*vec(string_funcs))(..)' folded wrong: orig keeps the slot pointer in a register (engine+0xcc): 'slot = &RwStringFunctions_vecStrcpy(&RwGlobalsRelease_stringFuncs(RwEngineInstance)); (*slot)(a,b)' (lua_RWP_Object).
27. 'if (c) v = a; else v = b; f(v)' with float constants: original has the call duplicated per branch ('push imm; jmp common'): write 'if (!c) f(b); else f(a);' - polarity follows 'je ..; push b; jmp' (sound_car_engine_start).
28. A position read at frame+0x40 in a loop: original hoists 'add edi,0x10' = pointer to the frame's embedded RwMatrix; write 'm=(RwMatrix*)(frame+0x10)' once and '&m->pos' (course_arctic_camera_relative_update).
29. Groups of consecutive dword copies: RwV3d (3) / RtQuat (4) struct assignment through the same pointers; 'idx < field' written 'field > idx' (replay_buffer_record_frame).
30. Register roles ecx/eax swapped in a pointer-walking function: the original advances the PARAMETER itself (param in ecx, 'mov eax,ecx' copy kept as the return value): modify the param and keep 'start = param' (FUN_005bf940).
31. 'f(*(p+off), param)' -> 'h = *(p+off); f(h, param)' swaps which operand takes the first-loaded register (FUN_004ae050); try it on every eax/ecx 'REGISTERS' near-miss.
32. bool locals (byte regs 'xor bl,bl') -> int flags (edi): the original uses int variables; operand order 'lo <= x' -> 'x >= lo' (course_world_uva_dicts_set_ranks).
33. Bitset test `(*(uint*)(tbl + (idx16 >> 5)*4) & 1 << (idx16 & 0x1f))` with idx16 = *(ushort*)(obj+0x20): the original loads the ushort ONCE (single movzx): keep it in a `ushort bit_index` local assigned by a comma expression inside the condition `(world != 0) && (bit_index = *(ushort*)(..), (..))` (rwp_object_set_matrix, rwp_object_set_rotation_degrees, debris_apply_velocity_a).
34. Record tables: `char *rec = (char *)&g_<record base symbol> + idx*STRIDE;` then `*(T*)(rec + off)`; the base symbol must be the record start (weapon_rocket_unequip g_rocket_state, weapon_mortar_equip g_weapon_6a29b8, weapon_flamethrower_equip g_weapon_6a9588, load_groundbeam g_game_65c8f8, lua_RWP_Object g_rwp_objects). Stores written in the ORIGINAL order, nested calls inline (`f(g(), rec->x)`), phantom trailing call args dropped (sfx_play_voice_at 3 args, object_set_flag_4 2 args).
35. Constants stored through int/hex immediates get hoisted into a register (`mov eax,0x3f800000` once): store through the float lvalue with float literals (`*(float*)&g = 1.0f`, local float members): course_marker_positions_init (28 immediates), load_smoke_texture, debris_apply_velocity_a (zero floats avoid the zero register ebx).
36. Zero-init local float/int frame members + `if (p == 0) p = &local` : with `float` members and 0.0f the compare becomes `test edi,edi` (debris_apply_velocity_a).
37. Signed vs unsigned globals: `undefined4` division/compare gives mul/shr + jb; original is signed (imul/sar, jl): declare the extern global `int` (debug_keys_and_profile).

Lint notes for the review (tools/defectlint.py on fix/): class 1 `signed` flags `if (g_x > 0)` on undefined4 globals in sound_stop_all_and_release (intended: orig is unsigned jbe, same semantics as != 0);
class 4 `proto` flags the shrunk prototypes FUN_0056e340 (1 vs 3), sound_frame_begin (1 vs 2), RtBMPImageRead (1 vs 4): the dossier counted args from deferred `add esp`; the bytes are identical (pushes of earlier calls stay on the stack);
class 6 `vtable_conv` flags FUN_004af0f0 (`__fastcall` slot call: callee takes this in ecx, no stack args).  declcheck.py / cheatgrep.py: no new findings.
