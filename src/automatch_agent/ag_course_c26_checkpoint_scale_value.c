#include "mfl_tier2.h"
/*005dd9c8*/ extern float k_f_0_16666667;
/*008bf150*/ extern undefined4 g_course_8bf150;
/*008bf154*/ extern undefined4 g_course_8bf154;
/*008c3fa0*/ extern undefined4 g_course_8c3fa0;


/* course_c26_checkpoint_scale_value */

/* Returns a float table entry (indexed by two per-slot values) scaled by 1/6. */

double course_c26_checkpoint_scale_value(int slot)

{
  return (double)((float *)&g_course_8c3fa0)[(&g_course_8bf154)[slot * 0x3f] +
                            (&g_course_8bf150)[slot * 0x3f] * 4] * (double)k_f_0_16666667;
}
