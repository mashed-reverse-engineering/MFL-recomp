/* MFL.exe course: functions moved verbatim from batch10.c, batch12.c, batch18.c, batch19.c, batch22.c, batch26.c, batch27.c, batch28.c, batch5.c, batch6.c, batch9.c, race_flow.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_course_set_8c4000[];

/* naked: the original leaves EAX = out, ECX/EDX = the last two copied words, and callers (whole program optimised) rely on it; see tools/clobber.py.
   Equivalent C: *out = *(struct V3i *)(*(int **)(g_course_8bf140[i * 0x3f] + 4) + 0x10) */
// FUNCTION: MFL 0x00459100
__declspec(naked) void course_object_get_position(int i, struct V3i *out)
{
    __asm {
        mov eax, [esp+4]
        imul eax, eax, 0xfc
        mov ecx, [eax+g_course_8bf140]
        mov edx, [ecx+4]
        mov eax, [esp+8]
        add edx, 0x40
        mov ecx, [edx]
        mov [eax], ecx
        mov ecx, [edx+4]
        mov [eax+4], ecx
        mov edx, [edx+8]
        mov [eax+8], edx
        ret
    }
}

// FUNCTION: MFL 0x00459130
int course_object_is_inactive(int i)
{
    if (*(float *)((char *)g_course_8bf140 + 0x34 + i * 0xfc) < k_f_n0_015)
        return 1;
    return 0;
}

// FUNCTION: MFL 0x004584c0
void course_race_phase_hook(void)
{
    if (race_get_phase() == RACE_PHASE_SETUP) {
        course_marker_positions_init();
        return;
    }
    if (race_get_phase() == RACE_PHASE_PRE_LOAD)
        nop_stub();
}

// FUNCTION: MFL 0x004577c0
void course_tierra_n_rotating_object_update(int dt)
{
    course_rotating_object_update(&g_course_8c53e0, dt);
}

// FUNCTION: MFL 0x00459090
void course_set_lod(int lod)
{
    int *p;
    if (g_course_set_6a26b8 != lod) {
        p = g_course_8bf140;
        do {
            if (lod != 0) {
                RpLODAtomicHookRender(*p);
            } else {
                RpLODAtomicSetCurrentLOD(*p, 0);
                RpLODAtomicUnHookRender(*p);
            }
            p += 0x3f;
        } while ((int)p < ((int)&g_course_set_8c4000));
        g_course_set_6a26b8 = lod;
    }
}

/* tail call: the original overwrites its own argument slot with the object and jumps to object_render (the autocompiled version passed no argument at all and crashed
   object_render with a NULL object on course 3) */
// FUNCTION: MFL 0x004558d0
__declspec(naked) void course_object_render_6a1f28_b(void)
{
    __asm {
        mov eax, g_course_6a1f28
        mov [esp+4], eax
        jmp object_render
    }
}

/* tail calls like course_object_render_6a1f28_b: the argument slot is replaced by the course object and the callee is jumped to */
// FUNCTION: MFL 0x004558e0
__declspec(naked) void course_object_destroy_frames_6a1f28(void)
{
    __asm {
        mov eax, g_course_6a1f28
        mov [esp+4], eax
        jmp object_destroy_frames
    }
}

// FUNCTION: MFL 0x00458de0
__declspec(naked) void course_tierra_n_cb10(void)
{
    __asm {
        mov eax, g_course_tierra_6a265c
        mov [esp+4], eax
        jmp object_render
    }
}

// FUNCTION: MFL 0x00474340
void course_object_pair_add(int a, int b, int c)
{
    g_car_6af5e0[g_course_object_pair_count * 3] = b;
    g_car_6af5e4[g_course_object_pair_count * 3] = a;
    g_lua_expobj_6af5e8[g_course_object_pair_count * 3] = c;
    g_course_object_pair_count++;
}

// FUNCTION: MFL 0x004589b0
void course_storm_cb10(void)
{
    RS_SET(0x14, 1);
    RS_SET(0x14, 2);
}

// FUNCTION: MFL 0x004585f0
void course_sandsnow_cb10(void)
{
    RS_SET(8, 0);
    course_sky_atomic_render();
}

// FUNCTION: MFL 0x004556f0
void course_neustein_cb10(void)
{
    ptank_atomic_render();
    RS_SET(RS_ZWRITE, 0);
    course_sky_atomic_render();
}

// FUNCTION: MFL 0x004577e0
void course_objects_render(void)
{
    RS_SET(RS_CULLMODE, 2);
    (*(void (**)(int))(g_course_objects_8c53ec + 0x48))(g_course_objects_8c53ec);
    (*(void (**)(int))(g_course_8c53e0 + 0x48))(g_course_8c53e0);
    (*(void (**)(int))(g_load_8c53e4 + 0x48))(g_load_8c53e4);
    (*(void (**)(int))(g_load_8c53e8 + 0x48))(g_load_8c53e8);
}

// FUNCTION: MFL 0x0045e000
void course_warzone_cb10(void)
{
    RS_SET(RS_FOGENABLE, 1);
    RS_SET(RS_ZWRITE, 0);
    object_render(g_course_warzone_6a2998);
    object_render(g_course_warzone_6a299c);
    object_render(g_course_warzone_6a29a0);
    RS_SET(RS_ZWRITE, 1);
    ptank_atomic_render();
    RS_SET(RS_FOGENABLE, 0);
}

// FUNCTION: MFL 0x004741f0
void course_object_lists_reset(void)
{
    int i = 0;
    g_course_object_count = 0;
    g_course_object_pair_count = 0;
    do {
        g_car_6af5c0[i >> 3] &= ~(1 << (i & 7));
        i++;
    } while (i < 0x100);
}

// FUNCTION: MFL 0x00474850
void course_objects_render_all(void)
{
    int i = 0;
    if (0 < g_course_object_pair_count) {
        do {
            object_render(g_car_6c2658[i]);
            i++;
        } while (i < g_course_object_pair_count);
    }
}

// FUNCTION: MFL 0x0042ab20
void course_camera_add_to_collision_world(int camera)
{
    if (g_course_lap_data.split_sectors[8] != 0) {
        if (g_course_lap_data.split_sectors[7] != 0) {
            RpWorldAddCamera(g_course_world.collision_bsps[0], camera);
            return;
        }
        RpWorldAddCamera(g_course_world.world, camera);
    }
}

// FUNCTION: MFL 0x0042ab60
void course_camera_remove_from_collision_world(int camera)
{
    if (g_course_lap_data.split_sectors[8] != 0) {
        if (g_course_lap_data.split_sectors[7] != 0) {
            RpWorldRemoveCamera(g_course_world.collision_bsps[0], camera);
            return;
        }
        RpWorldRemoveCamera(g_course_world.world, camera);
    }
}

// FUNCTION: MFL 0x0042b0e0
void course_get_lap_line(int i, short *a, short *b)
{
    *a = (short)g_course_lap_data.lap_lines[i + 0x200];
    *b = *(short *)((char *)g_course_lap_data.lap_lines + i * 4 + 0x802);
}

// FUNCTION: MFL 0x0042a150
int *course_get_object_record(int i)
{
    return (int *)((char *)&g_course_8c8894 + i * 0x50);
}

// FUNCTION: MFL 0x0042a110
void course_destroy_extra_clumps(void)
{
    int *rec = g_course_8c8860;
    int *p;
    int n;
    do {
        if (*rec != 0)
            RpClumpDestroy(*rec);
        p = rec;
        for (n = 0x14; n != 0; n--)
            *p++ = 0;
        rec = p;
    } while ((int)rec < ((int)&g_player_stats_race));
}

// FUNCTION: MFL 0x0044a020
void course_sky_atomic_destroy(void)
{
    int frame = *(int *)(g_sky_atomic + 4);
    RpAtomicDestroy(g_sky_atomic);
    RwFrameDestroy(frame);
}

// FUNCTION: MFL 0x004535a0
void course_atomics_destroy_6a07c4(void)
{
    object_destroy_frames(g_course_6a07c4);
    object_destroy_frames(g_course_6a07c8);
    object_destroy_frames(g_course_6a07cc);
    object_destroy_frames(g_course_6a07d0);
}

// FUNCTION: MFL 0x00454300
void course_keys_cb5(void)
{
    pool_list_destroy(g_course_keys_8c5724);
    course_sky_atomic_destroy();
}

// FUNCTION: MFL 0x00454440
void course_nt1_s_cb5(int w)
{
    course_object_destroy_frames_6a1f28(w);
    pool_list_destroy(g_course_nt1_8c5720);
    course_sky_atomic_destroy();
}

// FUNCTION: MFL 0x00458610
void course_sandsnow_cb5(int w)
{
    course_clumps_destroy(w);
    course_sandsnow_destroy_frames();
    course_sky_atomic_destroy();
}

// FUNCTION: MFL 0x00455710
void course_neustein_cb5(void)
{
    course_sky_atomic_destroy();
    ptank_atomic_destroy();
    rwp_world_current_activate_b();
    object_destroy_frames(g_course_neustein_6a1d88);
    object_destroy_frames(g_course_neustein_6a1d8c);
    object_destroy_frames(g_course_neustein_6a1d90);
    nop_stub();
}

// FUNCTION: MFL 0x00458c90
void course_rouabout_cb3(void)
{
    int m = get_viewport_camera_matrix(0);
    course_sky_clumps_update(m);
    course_sky_clumps_render();
}

// FUNCTION: MFL 0x0045c790
void course_atomics_destroy_3(void)
{
    RpAtomicDestroy(g_course_atomics_6a2830);
    RpAtomicDestroy(g_course_atomics_6a2838);
    RpAtomicDestroy(g_course_atomics_6a2840);
}

// FUNCTION: MFL 0x0045c7c0
void course_atomics_flags_clear(void)
{
    g_course_atomics_6a2834 = 0;
    g_course_atomics_6a283c = 0;
    g_course_atomics_6a2844 = 0;
    g_course_atomics_60d9a8 = 0;
    g_course_atomics_60d9b4 = 0;
    g_course_atomics_60d9c0 = 0;
    g_course_atomics_6a27e8 = 0;
}

// FUNCTION: MFL 0x0045c7f0
void course_atomics_render_3(void)
{
    int *p = &g_course_atomics_6a2830;
    do {
        if (p[1] == 0)
            (*(void (**)(int))(*p + 0x48))(*p);
        p += 2;
    } while ((int)p < ((int)&g_course_atomics_6a2844 + 0x4));
}

// FUNCTION: MFL 0x0045d890
void course_effect_destroy(int e)
{
    object_destroy_frames(*(int *)(e + 8));
    (*(void (**)(int))(RwEngineInstance + 0x10c))(*(int *)(e + 4));
    (*(void (**)(int))(RwEngineInstance + 0x10c))(e);
}

// FUNCTION: MFL 0x0045dee0
void course_warzone_cb7(void)
{
    anim_stop((int)g_course_6a2888);
    course_player_markers_setup();
}

// FUNCTION: MFL 0x00408fd0
int course_token_list_contains(int id, int start)
{
    int *base = course_token_list_base();
    int *p = base + start;
    int *q;
    int v;
    if (*p != -1) {
        q = base + 2 + start;
        do {
            v = *p;
            if (v == -10)
                return 0;
            if (p[1] == 0x100) {
                if (id == v || id == *q)
                    return 1;
                q += 2;
                p += 2;
            } else {
                if (v <= id && id <= p[1])
                    return 1;
                q += 1;
                p += 1;
            }
        } while (*p != -1);
    }
    return 0;
}

// FUNCTION: MFL 0x004091d0
int course_token_list_count_entries(int start)
{
    int *base = course_token_list_base();
    int *p = base + start;
    int v;
    int n = 0;
    while (p[1] != -10 && (v = p[1]) != -1) {
        if (v == 0x100) {
            n++;
            p += 2;
        } else {
            n += v - *p;
            p += 1;
        }
    }
    return n + 1;
}

// FUNCTION: MFL 0x0040a8a0
int course_token_list_entry_start(int count)
{
    int *p = course_token_list_base();
    int v;
    if (0 < count) {
        do {
            v = *p;
            while (v != -1 && v != -10) {
                p++;
                v = *p;
            }
            p++;
            count--;
        } while (count != 0);
    }
    return *p;
}

// FUNCTION: MFL 0x0040a8d0
int course_token_list_skip_to_entry(int id, int skip)
{
    int *p = course_token_list_base();
    int *q;
    int v;
    int n = skip;
    if (n > 0) {
        do {
            v = *p;
            while (v != -1 && v != -10) {
                p++;
                v = *p;
            }
            p++;
            n--;
        } while (n != 0);
    }
    v = *p;
    q = p;
    for (;;) {
        if (v == -1 || v == -10)
            return *p;
        if (q[1] == 0x100) {
            if (id == v)
                return q[2];
        } else if (v <= id && id < q[1]) {
            return id + 1;
        }
        v = q[1];
        q++;
    }
}

// FUNCTION: MFL 0x00452ea0
void course_dem_state_reset(void)
{
    g_course_dem_6a07ac = 0;
    g_course_dem_6a07b4 = 0;
    g_course_dem_6a07b0 = 0;
    g_course_dem_6a07b8 = 0;
    g_course_dem_6a07bc = 0;
    g_course_dem_60d808 = 1;
    g_course_dem_60d80c = 1;
    g_course_dem_6a07c0 = 0;
}

// FUNCTION: MFL 0x00410dc0
void course_load_time_report(void)
{
    int i = race_get_setup_index();
    race_course_loaded(g_race_setup, i);
}
