#include "mfl_tier2.h"
/*00456ed0*/ extern void __cdecl func_0x00456ed0(void *p0);
/*00457520*/ extern void __cdecl func_0x00457520(void);
/*008c53e0*/ extern undefined4 g_course_8c53e0;
/* course_rouabout_cb7 */
/* "18-byte thunk of the original: push &g_course_8c53e0; call 0x456ed0 (clears five fields of that record); add esp,4; jmp 0x457520 (tail call: the
   per-frame update of the three spline objects, no arguments).  Both callees stay original code; a C tail call is `call; ret`, equivalent to the jmp." */
void course_rouabout_cb7(void)
{
  func_0x00456ed0(&g_course_8c53e0);
  func_0x00457520();
  return;
}
