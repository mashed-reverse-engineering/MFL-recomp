#include "mfl_tier2.h"
/*00659a7c*/ extern undefined4 g_load_659a7c;
static int stopclock_render_timed_parts_r(int * reg_eax);
extern void stopclock_render_timed_parts(void);
__declspec(naked) int stopclock_render_timed_parts_r(int * reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call stopclock_render_timed_parts
        ret
    }
}

/* stopclock_render_timed_parts_forward */

/* "calls stopclock_render_timed_parts" */

/* Forwarder for the stopclock render routine: calls stopclock_render_timed_parts with
 * &g_load_659a7c in eax. Takes no parameters and returns nothing. (inferred: the global is
 * the stopclock render's state/argument block) */
void stopclock_render_timed_parts_forward(void)

{
  stopclock_render_timed_parts_r(&g_load_659a7c);
  return;
}


