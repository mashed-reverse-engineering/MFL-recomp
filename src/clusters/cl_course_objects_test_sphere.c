// cluster-entries: course_objects_test_sphere
/* Installed cluster course_objects_test_sphere (tools/cluster_install.py from output/cluster_pilot_zx/course_objects_test_sphere.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*006f60c8*/ extern undefined g_course_6f60c8;
/*006f68c8*/ extern undefined4 g_course_6f68c8;
/*006f6908*/ extern undefined4 g_course_6f6908;
#define COURSE_VOLUME_STRIDE (0x80)

/* course_objects_test_sphere */

/* "planes_test_sphere for each active course bounding volume" */

/* Tests `sphere` against every active course bounding volume, stopping at the first hit.
   sphere: passed through to planes_test_sphere (inferred: a bounding sphere pointer).
   Returns 1 if any active volume reports a hit, 0 otherwise (inferred). */
static undefined4 planes_test_sphere_r(int p0, int p1, undefined4 p2);
extern void planes_test_sphere(void);
undefined4 __cdecl course_objects_test_sphere(undefined4 sphere)

{
  int hit;
  int course_index;
  undefined *volume;
  
  course_index = 0;
  if (0 < (int)g_course_6f6908) {
    /* walk every volume record; skip slots whose course object id is 0 (inactive) */
    volume = &g_course_6f60c8;
    do {
      if ((&g_course_6f68c8)[course_index] != 0) {
        hit = planes_test_sphere_r((&g_course_6f68c8)[course_index],volume,sphere);
        if (hit != 0) {
          return 1;
        }
      }
      course_index = course_index + 1;
      volume = volume + COURSE_VOLUME_STRIDE;
    } while (course_index < (int)g_course_6f6908);
  }
  return 0;
}

/* planes_test_sphere_r: C -> original planes_test_sphere (registers ebx,eax,s0) */
__declspec(naked) undefined4 planes_test_sphere_r(int p0, int p1, undefined4 p2)
{
    __asm {
        push ebx
        push dword ptr [esp + 16]
        mov ebx, dword ptr [esp + 12]
        mov eax, dword ptr [esp + 16]
        call planes_test_sphere
        add esp, 4
        pop ebx
        ret
    }
}
