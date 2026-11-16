#include "mfl_tier2.h"
/* course_city_helper_455b80 takes the course value in EBX (the original keeps it live from `mov ebx,[esp+8]`
 * across course_city_cb0_sub_455a40): adapter loads EBX and preserves the caller's. */
extern void course_city_helper_455b80(void);
static void func_0x00455b80_r(undefined4 reg_ebx);
__declspec(naked) void func_0x00455b80_r(undefined4 reg_ebx)
{
    __asm {
        push ebx
        mov ebx, dword ptr [esp + 8]
        call course_city_helper_455b80
        pop ebx
        ret
    }
}

/* course_city_cb0 */

/* Course callback for the City course (inferred): calls course_city_cb0_sub_455a40(param_1, 0xc, 0)
 * with the caller's value, then course_city_helper_455b80(param_1 in EBX). Callee meanings are not in the dossier.
 * param_1: passed through unchanged to course_city_cb0_sub_455a40 (inferred: a course/context pointer or id). */
void course_city_cb0(undefined4 param_1)

{
  course_city_cb0_sub_455a40(param_1,0xc,0);
  func_0x00455b80_r(param_1);
  return;
}


