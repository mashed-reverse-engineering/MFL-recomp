// cluster-entries: course_city_cb7 course_city_cb6
/* Installed cluster course_city_cb7 (tools/cluster_install.py from output/cluster_pilot_k/course_city_cb7.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* course_city_cb7, course_city_cb6: course callbacks that set up the city course effects and then
   run the shared "objects visible" step with 1 (cb7) or 0 (cb6) */

static void SUB_00455b80_r(int p0);
extern void SUB_00455b80(void);
static void SUB_00455dd0_r(int p0);
extern void SUB_00455dd0(void);
void course_city_cb7(int course)
{
    SUB_00455b80_r(course);
    SUB_00455dd0_r(1);
}

void course_city_cb6(int course)
{
    SUB_00455b80_r(course);
    SUB_00455dd0_r(0);
}

/* SUB_00455b80_r: C -> original SUB_00455b80 (registers ebx) */
__declspec(naked) void SUB_00455b80_r(int p0)
{
    __asm {
        push ebx
        mov ebx, dword ptr [esp + 8]
        call SUB_00455b80
        pop ebx
        ret
    }
}

/* SUB_00455dd0_r: C -> original SUB_00455dd0 (registers esi) */
__declspec(naked) void SUB_00455dd0_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call SUB_00455dd0
        pop esi
        ret
    }
}
