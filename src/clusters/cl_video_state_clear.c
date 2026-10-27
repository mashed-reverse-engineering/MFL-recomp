// cluster-entries: video_state_clear
/* Installed cluster video_state_clear (tools/cluster_install.py from output/cluster_pilot10/video_state_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* video_state_clear */

/* "*param = 0" */

void video_state_clear_n(undefined4 *in_ecx)

{
  *in_ecx = 0;
  return;
}

/* entry thunk at the original address of video_state_clear: register inputs (ecx) -> natural C video_state_clear_n; ret 0 */
__declspec(naked) void video_state_clear(void)
{
    __asm {
        push ecx
        push edx
        push ecx
        call video_state_clear_n
        add esp, 4
        pop edx
        pop ecx
        ret
    }
}
