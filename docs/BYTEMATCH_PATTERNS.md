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
