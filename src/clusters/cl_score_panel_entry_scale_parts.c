// cluster-entries: score_panel_entry_scale_parts
/* Installed cluster score_panel_entry_scale_parts (tools/cluster_install.py from output/cluster_pilot10/score_panel_entry_scale_parts.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/* score_panel_entry_scale_parts */

/* "RwFrameScale of the parts selected by flags 0x2000/0x4000/0x8000" */

void score_panel_entry_scale_parts_n(int in_esi, RwV3d *in_edi)

{
  if ((*(uint *)(in_esi + 0x68) & 0x2000) != 0) {
    RwFrameScale(*(RwFrame **)(*(char **)(in_esi + 0x34) + 4),in_edi,rwCOMBINEREPLACE);
  }
  if ((*(uint *)(in_esi + 0x68) & 0x4000) != 0) {
    RwFrameScale(*(RwFrame **)(*(char **)(in_esi + 0x38) + 4),in_edi,rwCOMBINEREPLACE);
  }
  if ((char)((uint)*(undefined4 *)(in_esi + 0x68) >> 8) < '\0') {
    RwFrameScale(*(RwFrame **)(*(char **)(in_esi + 0x3c) + 4),in_edi,rwCOMBINEREPLACE);
  }
  return;
}

/* entry thunk at the original address of score_panel_entry_scale_parts: register inputs (esi,edi) -> natural C score_panel_entry_scale_parts_n; ret 0 */
__declspec(naked) void score_panel_entry_scale_parts(void)
{
    __asm {
        push edi
        push esi
        call score_panel_entry_scale_parts_n
        add esp, 8
        ret
    }
}
