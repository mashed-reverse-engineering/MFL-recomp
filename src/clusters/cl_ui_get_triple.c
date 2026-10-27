// cluster-entries: ui_get_triple
/* Installed cluster ui_get_triple (tools/cluster_install.py from output/cluster_pilot10/ui_get_triple.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*008c7fc0*/ extern undefined g_ui_8c7fc0;
/*008c7fc4*/ extern undefined g_ui_8c7fc4;
/*008c7fc8*/ extern undefined g_ui_8c7fc8;
/* ui_get_triple */

/* "reads the 3 dwords stored by ui_set_triple" */

void ui_get_triple_n(int a, int b, undefined4 *out_x, undefined4 *out_y, undefined4 *out_z)

{
  int offset;
  
  offset = (a + b * 2) * 0xc;
  *out_x = *(undefined4 *)((char *)&g_ui_8c7fc0 + offset);
  *out_z = *(undefined4 *)((char *)&g_ui_8c7fc8 + offset);
  *out_y = *(undefined4 *)((char *)&g_ui_8c7fc4 + offset);
  return;
}

/* entry thunk at the original address of ui_get_triple: register inputs (eax,ecx,s0,s1,s2) -> natural C ui_get_triple_n; ret 0 */
__declspec(naked) void ui_get_triple(void)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        push ecx
        push eax
        call ui_get_triple_n
        add esp, 20
        ret
    }
}
