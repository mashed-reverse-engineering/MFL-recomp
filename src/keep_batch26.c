/* functions of batch26.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch26.c is kept as it was */
#pragma auto_inline(off)
#include "mfl_types.h"
extern struct CourseLap g_course_lap_data;
extern struct CourseWorld g_course_world;
extern struct PlayerStatRecord g_player_stats_race[4], g_player_stats_total[4];
extern int g_game_6614e4, g_game_6614e8, g_game_660568;
extern char g_game_6614a0[];
extern void RpWorldAddCamera(int, int);
extern void RpWorldRemoveCamera(int, int);
extern void RpClumpForAllAtomics(int, void *, void *);
extern void geometry_transform_points(void);
extern struct AiData g_ai_data;
extern int g_ai_editor_mode, g_ai_editor_menu_cursor;
extern int g_course_8c8860[], g_course_8c8894;
extern void RpClumpDestroy(int);
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x00425dc0
void bbox_init(int *p, int clump)
{
    p[2] = 0xff7fffff;
    p[1] = 0xff7fffff;
    p[0] = 0xff7fffff;
    p[5] = 0x7f7fffff;
    p[4] = 0x7f7fffff;
    p[3] = 0x7f7fffff;
    RpClumpForAllAtomics(clump, geometry_transform_points, p);
}
