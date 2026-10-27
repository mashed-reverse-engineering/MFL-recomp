// cluster-entries: course_objects_test_bbox
/* Installed cluster course_objects_test_bbox (tools/cluster_install.py from output/cluster_pilot_zx/course_objects_test_bbox.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*006f60c8*/ extern undefined g_course_6f60c8;
/*006f68c8*/ extern undefined4 g_course_6f68c8;
/*006f6908*/ extern undefined4 g_course_6f6908;
#define COURSE_BBOX_STRIDE (0x80) /* bytes per course entry in g_course_6f60c8 (inferred) */

/* course_objects_test_bbox */

/* "planes_test_bbox for each active course bounding volume" */

/* Runs planes_test_bbox against each active course's bounding volume, passing test_arg through.
   Returns 1 as soon as one volume reports a hit, 0 if none does. */

static undefined4 planes_test_bbox_r(int p0, int p1, undefined4 p2);
extern void planes_test_bbox(void);
undefined4 __cdecl course_objects_test_bbox(undefined4 test_arg)

{
  int hit;
  int course_index;
  undefined *course_bbox;

  course_index = 0;
  /* loop over the course slots, COURSE_BBOX_STRIDE bytes of bounding data each */
  if (0 < (int)g_course_6f6908) {
    course_bbox = &g_course_6f60c8;
    do {
      /* skip slots whose g_course_6f68c8 entry is zero (inferred: inactive course) */
      if ((&g_course_6f68c8)[course_index] != 0) {
        hit = planes_test_bbox_r((&g_course_6f68c8)[course_index],course_bbox,test_arg);
        if (hit != 0) {
          return 1;
        }
      }
      course_index = course_index + 1;
      course_bbox = course_bbox + COURSE_BBOX_STRIDE;
    } while (course_index < (int)g_course_6f6908);
  }
  return 0;
}

/* planes_test_bbox_r: C -> original planes_test_bbox (registers ebx,eax,s0) */
__declspec(naked) undefined4 planes_test_bbox_r(int p0, int p1, undefined4 p2)
{
    __asm {
        push ebx
        push dword ptr [esp + 16]
        mov ebx, dword ptr [esp + 12]
        mov eax, dword ptr [esp + 16]
        call planes_test_bbox
        add esp, 4
        pop ebx
        ret
    }
}
