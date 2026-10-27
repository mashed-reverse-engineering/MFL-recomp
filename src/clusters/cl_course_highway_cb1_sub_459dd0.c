// cluster-entries: course_highway_cb1_sub_459dd0
/* Installed cluster course_highway_cb1_sub_459dd0 (tools/cluster_install.py from output/cluster_pilot_k/course_highway_cb1_sub_459dd0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005e64a4*/ extern float k_f_0;
/*008c4478*/ extern char g_course_highway_8c4478[];
/* course_highway_cb1_sub_459dd0 */

/* course_highway_cb1_sub_459dd0: nearest highway line segment in front of / behind a point.
   Table at g_course_highway_8c4478 + 8: 4 groups per side, each 4 lines of 0x14 bytes (the Y coordinate is at +8) followed by the line count.
   side 1 looks for lines above the point, otherwise below.  Returns the closest line (0 if none) and, when out is given, its distance. */
typedef struct HighwayLine { float x; float z; float y; float u; float v; } HighwayLine;
typedef struct HighwayGroup { HighwayLine lines[4]; int count; } HighwayGroup;

HighwayLine *course_highway_cb1_sub_459dd0_n(int side, int group, float *point, float *out)
{
    HighwayGroup *g = (HighwayGroup *)(g_course_highway_8c4478 + 8) + (group + side * 4);
    HighwayLine *line = g->lines;
    HighwayLine *best = 0;
    float best_dist = 10000.0f;
    int i;

    for (i = 0; i < g->count; i++, line++) {
        float d;
        if (side == 1)
            d = line->y - point[2];
        else
            d = point[2] - line->y;
        if (k_f_0 < d && d < best_dist) {
            best = line;
            best_dist = d;
            if (out != 0)
                *out = d;
        }
    }
    return best;
}

/* entry thunk at the original address of course_highway_cb1_sub_459dd0: register inputs (ebx,eax,s0,s1) -> natural C course_highway_cb1_sub_459dd0_n; ret 0 */
__declspec(naked) void course_highway_cb1_sub_459dd0(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push eax
        push ebx
        call course_highway_cb1_sub_459dd0_n
        add esp, 16
        ret
    }
}
