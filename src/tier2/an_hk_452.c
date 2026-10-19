#include "mfl_tier2.h"
/*006f60c8*/ extern undefined g_course_6f60c8;
/*006f68c8*/ extern undefined4 g_course_6f68c8;
/*006f6908*/ extern undefined4 g_course_6f6908;
static undefined4 planes_test_bbox_r(int reg_ebx, int reg_eax, undefined4 param_3);
extern void planes_test_bbox(void);
#define COURSE_BBOX_STRIDE (0x80) /* bytes per course entry in g_course_6f60c8 (inferred) */
__declspec(naked) undefined4 planes_test_bbox_r(int reg_ebx, int reg_eax, undefined4 param_3)
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

/* course_objects_test_bbox */

/* "planes_test_bbox for each active course bounding volume" */

/* Runs planes_test_bbox_r against each active course's bounding volume, passing test_arg through.
   Returns 1 as soon as one volume reports a hit, 0 if none does. */

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


