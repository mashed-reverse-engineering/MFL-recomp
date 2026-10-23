/* MFL.exe physics: functions moved verbatim from batch15.c, batch16.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x0048e390
void rwp_object_set_radius_squared(int i, float r)
{
    g_rw_6fa190[i] = r * r;
}

// FUNCTION: MFL 0x0048e3b0
void rwp_object_set_user_data(int i, int v)
{
    if (i >= 0 && i < MAX_RWP_OBJECTS)
        g_rw_6fcdd8[i] = v;
}

// FUNCTION: MFL 0x0048e3d0
int rwp_object_get_user_data(int i)
{
    if (i >= 0 && i < MAX_RWP_OBJECTS)
        return g_rw_6fcdd8[i];
    return 0;
}

// FUNCTION: MFL 0x0048e3f0
int rwp_object_get_id(int i)
{
    if (0 < i && i <= 200)
        return g_rw_6fa4b0[i];
    return -1;
}

// FUNCTION: MFL 0x0048e410
int rwp_object_find_index_by_id(int id)
{
    int i = 0;
    if (0 < id) {
        do {
            if (g_rw_6fa4b0[i] == id)
                return i;
            i++;
        } while (i < MAX_RWP_OBJECTS);
    }
    return -1;
}

// FUNCTION: MFL 0x0048e450
int rwp_object_get_type(int i)
{
    if (i >= 0 && i < MAX_RWP_OBJECTS)
        return g_rw_6fd0f8[i];
    return -1;
}

// FUNCTION: MFL 0x0048e6c0
void rwp_object_set_flag(int i, int v)
{
    g_rw_6fa858[i] = v;
}

// FUNCTION: MFL 0x0048e6d0
int rwp_object_get_handle(int i)
{
    if (i < 0 || i >= MAX_RWP_OBJECTS)
        return 0;
    return g_rw_6fab78[i];
}

// FUNCTION: MFL 0x00495590
int physics_buffer_destroy(int p)
{
    (*(void (**)(int))(RwEngineInstance + 0x10c))(*(int *)(p + 0x14));
    (*(void (**)(int))(RwEngineInstance + 0x10c))(p);
    return 0;
}

// FUNCTION: MFL 0x00495c90
int rw_free_with_inner_buffer(int p)
{
    if (p != 0) {
        if (*(int *)(p + 0x10) != 0)
            (*(void (**)(int))(RwEngineInstance + 0x10c))(*(int *)(p + 0x10));
        (*(void (**)(int))(RwEngineInstance + 0x10c))(p);
    }
    return 1;
}

// FUNCTION: MFL 0x00496ea0
int smplfzx_get_object(unsigned int i)
{
    if (i <= g_smplfzx_get_71abec)
        return *(int *)(*(char **)(g_particles_71abf4 + 0x10) + i * 4);
    return 0;
}
