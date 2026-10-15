/* functions of batch28.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch28.c is kept as it was */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "rwrs.h"
extern void object_destroy_frames(int);
extern void pool_list_destroy(int);
extern void course_clumps_destroy(int);
extern int g_course_city_6a2150, g_course_sands_8bf088;
extern int g_course_atomics_6a2830, g_course_atomics_6a2838, g_course_atomics_6a2840;
extern int g_course_atomics_6a2834, g_course_atomics_6a283c, g_course_atomics_6a2844;
extern int g_course_atomics_60d9a8, g_course_atomics_60d9b4, g_course_atomics_60d9c0, g_course_atomics_6a27e8;
extern void RpAtomicDestroy(int);
extern float g_camera_shake, k_f_0;
extern int g_course_60d9c8, g_course_60d9cc;
extern void rwp_object_set_active(int, int);
extern void RpClumpRender(int);
extern char g_course_6a2888[];
extern void anim_stop(int);
extern void course_player_markers_setup(void);
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x0045d560
void course_clump_render_with_physics(int *p)
{
    RpClumpRender(*p);
    rwp_object_set_active(g_course_60d9c8, 0);
    rwp_object_set_active(g_course_60d9cc, 0);
}
