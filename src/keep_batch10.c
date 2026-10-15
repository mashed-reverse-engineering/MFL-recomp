/* functions of batch10.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch10.c is kept as it was */
#pragma auto_inline(off)
#include "mfl_consts.h"
extern int *rwp_object_get_matrix(int);
extern int g_course_6a188c[];
extern int race_get_phase(void);
extern void course_marker_positions_init(void);
extern void nop_stub(void);
extern void course_rotating_object_update(void *, int);
extern int g_course_8c53e0;
extern int g_course_8bf140[];
extern float g_course_object_8bf174[];
extern float k_f_n0_015;
extern int g_course_set_6a26b8;
extern void RpLODAtomicSetCurrentLOD(int, int);
extern void RpLODAtomicUnHookRender(int);
extern void RpLODAtomicHookRender(int);
struct V3i { int x, y, z; };
extern int g_course_6a1f28;
extern void object_render(int);
extern int g_course_tierra_6a265c;
extern void object_destroy_frames(int);
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x00455750
int course_get_car_body_position(int i)
{
    return (int)rwp_object_get_matrix(g_course_6a188c[i * 10]) + 0x30;
}
