/* MFL.exe geometry: functions moved verbatim from batch12.c, batch14.c, batch25.c, batch6.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x00475730
int get_viewport_camera(int vp)
{
    if (ui_get_menu_69d1ec() == 3 && vp != -1)
        return get_second_camera();
    return g_aux_camera;
}

// FUNCTION: MFL 0x004757a0
int get_viewport_camera_matrix(int vp)
{
    if (ui_get_menu_69d1ec() == 3) {
        if (vp == -1)
            return *(int *)(g_aux_camera + 4) + 0x10;
        return *(int *)(get_second_camera() + 4) + 0x10;
    }
    return *(int *)(g_aux_camera + 4) + 0x10;
}

// FUNCTION: MFL 0x00482010
int random_int_range(int lo, int hi)
{
    unsigned int r = RpRandom();
    return (r & 0x7fffffff) % (unsigned int)((hi - lo) + 1) + lo;
}

// FUNCTION: MFL 0x0040d0d0
void find_texture_dict_b(const char *name)
{
    RwTexDictionaryFindNamedTexture(g_load_652dd8, name);
}

// FUNCTION: MFL 0x0040d0f0
void find_texture_dict_c(const char *name)
{
    RwTexDictionaryFindNamedTexture(g_effects_652ddc, name);
}

// FUNCTION: MFL 0x0040d110
void find_texture_dict_d(const char *name)
{
    RwTexDictionaryFindNamedTexture(g_effects_652de0, name);
}

// FUNCTION: MFL 0x0040d130
void find_texture_a(const char *name)
{
    RwTexDictionaryFindNamedTexture(g_effects_652de4, name);
}

// FUNCTION: MFL 0x00409210
int line_exceptions_resolve_index(int start, int id)
{
    int *base = course_token_list_base();
    int *p = base + start;
    int hi = p[1];
    int count = 0;
    int result = id;
    int lo;
    while (hi != -10 && (hi = p[1], hi != -1)) {
        if (hi == 0x100) {
            count++;
            p += 2;
        } else {
            lo = *p;
            if (lo <= id && id <= hi && base[1] < lo)
                result += count - lo;
            count += hi - lo;
            p++;
        }
        hi = p[1];
    }
    return result;
}
