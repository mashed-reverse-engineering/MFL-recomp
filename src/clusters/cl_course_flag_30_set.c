// cluster-entries: course_flag_30_set
/* Installed cluster course_flag_30_set (tools/cluster_install.py from output/cluster_pilot10/course_flag_30_set.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*008c86d0*/ extern undefined4 g_course_8c86d0;
/* course_flag_30_set */

/* WARNING: Unknown calling convention */
/* "stores 0x30 at +3 and g_course_8c86d0=1" */

void course_flag_30_set_n(int in_eax)

{
  *(undefined1 *)(in_eax + 3) = 0x30;
  g_course_8c86d0 = 1;
  return;
}

/* entry thunk at the original address of course_flag_30_set: register inputs (eax) -> natural C course_flag_30_set_n; ret 0 */
__declspec(naked) void course_flag_30_set(void)
{
    __asm {
        push eax
        push ecx
        push edx
        push eax
        call course_flag_30_set_n
        add esp, 4
        pop edx
        pop ecx
        pop eax
        ret
    }
}
