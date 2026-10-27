// cluster-entries: ui_rt2d_wide_string_copy
/* Installed cluster ui_rt2d_wide_string_copy (tools/cluster_install.py from output/cluster_pilot_zy/ui_rt2d_wide_string_copy.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
typedef unsigned int (*RwStrlenFunction)(const char *);
#define rwstrlen RW_GLOBAL(0xf4, RwStrlenFunction)      /* stringFuncs.vecStrlen */
/* ui_rt2d_wide_string_copy */

/* "copies a string widening it to 16 bit characters" */

void ui_rt2d_wide_string_copy_n(byte *in_eax, ushort *in_ecx)

{
  unsigned int sVar1;
  
  sVar1 = rwstrlen((const char *)in_eax);
  if (0 < (int)sVar1) {
    do {
      *in_ecx = (ushort)*in_eax;
      in_ecx = in_ecx + 1;
      in_eax = in_eax + 1;
      sVar1 = sVar1 - 1;
    } while (sVar1 != 0);
  }
  *in_ecx = 0;
  return;
}

/* entry thunk at the original address of ui_rt2d_wide_string_copy: register inputs (eax,ecx) -> natural C ui_rt2d_wide_string_copy_n; ret 0 */
__declspec(naked) void ui_rt2d_wide_string_copy(void)
{
    __asm {
        push ecx
        push eax
        call ui_rt2d_wide_string_copy_n
        add esp, 8
        ret
    }
}
