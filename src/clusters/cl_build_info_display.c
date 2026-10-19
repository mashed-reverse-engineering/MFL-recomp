// cluster-entries: build_info_display
/* Installed cluster build_info_display (tools/cluster_install.py from output/cluster_pilot9/build_info_display.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042d0a0*/ extern undefined __cdecl build_info_draw_page_0_0042d0a0(undefined4 p0, undefined4 p1);
/*0042d280*/ extern undefined __cdecl build_info_draw_page_1(undefined4 p0, undefined4 p1);
/*0042d3c0*/ extern undefined __cdecl build_info_display_draw_frame(undefined4 p0, undefined4 p1);
/*0069bb30*/ extern undefined4 g_build_info_69bb30;
/* build_info_display */

/* "switch over g_build_info page: build_info_display_a ..." */

void build_info_display_n(undefined4 param_1, undefined4 in_eax)

{
  
  switch(g_build_info_69bb30) {
  case 0:
    build_info_draw_page_0_0042d0a0(param_1,in_eax);
    return;
  case 1:
    build_info_draw_page_1(param_1,in_eax);
    return;
  case 3:
  case 4:
    build_info_display_draw_frame(param_1,in_eax);
  }
  return;
}

/* entry thunk at the original address of build_info_display: register inputs (ecx,eax) -> natural C build_info_display_n; ret 0 */
__declspec(naked) void build_info_display(void)
{
    __asm {
        push eax
        push ecx
        call build_info_display_n
        add esp, 8
        ret
    }
}
