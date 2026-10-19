#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*00434bc0*/ extern int event_type_is_race_basic();
/*004c45c0*/ extern int rp_random_int_range(int p0, int p1);
/*0060d808*/ extern undefined4 g_course_dem_60d808;
/*0060d80c*/ extern undefined4 g_course_dem_60d80c;
/*006a07ac*/ extern undefined4 g_course_dem_6a07ac;
/*006a07b0*/ extern undefined4 g_course_dem_6a07b0;
/*006a07bc*/ extern undefined4 g_course_dem_6a07bc;
/*006a07c0*/ extern undefined4 g_course_dem_6a07c0;
static void course_dem_advance_r(int reg_edx, int stk0);
extern void func_0x00452ee0(void);
/* adapter for the register convention callee 0x00452ee0: edx = amount added to g_course_dem_6a07ac, stack argument = param_1 of course_dem_cb1 */
__declspec(naked) void course_dem_advance_r(int reg_edx, int stk0)
{
    __asm {
        mov edx, dword ptr [esp + 4]
        push dword ptr [esp + 8]
        call func_0x00452ee0
        add esp, 4
        ret
    }
}

/* course_dem_cb1 */

void course_dem_cb1(undefined4 param_1, undefined4 st_arg2)

{
  int race_phase;
  
  race_phase = race_get_phase();
  if (race_phase == 6) {
    race_phase = event_type_is_race_basic();
    if (race_phase == 0) {
      course_dem_advance_r(st_arg2, param_1);
      return;
    }
  }
  else {
    g_course_dem_6a07ac = rp_random_int_range(2,7);
    g_course_dem_6a07bc = 0;
    g_course_dem_6a07b0 = 0;
    g_course_dem_60d808 = 1;
    g_course_dem_60d80c = 1;
    g_course_dem_6a07c0 = 0;
  }
  return;
}


