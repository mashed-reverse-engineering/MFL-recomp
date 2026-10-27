// cluster-entries: score_panel_entry_scale_parts_low
/* Installed cluster score_panel_entry_scale_parts_low (tools/cluster_install.py from output/cluster_pilot9/score_panel_entry_scale_parts_low.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/* score_panel_entry_scale_parts_low */

/* "RwFrameScale of parts selected by flag bits 0..5" */

void score_panel_entry_scale_parts_low_n(int in_esi, RwV3d *in_edi)

{
  if ((*(byte *)(in_esi + 0x68) & 1) != 0) {
    RwFrameScale(*(RwFrame **)(*(char **)(in_esi + 0x1c) + 4),in_edi,rwCOMBINEREPLACE);
  }
  if ((*(byte *)(in_esi + 0x68) & 2) != 0) {
    RwFrameScale(*(RwFrame **)(*(char **)(in_esi + 0x20) + 4),in_edi,rwCOMBINEREPLACE);
  }
  if ((*(byte *)(in_esi + 0x68) & 4) != 0) {
    RwFrameScale(*(RwFrame **)(*(char **)(in_esi + 0x24) + 4),in_edi,rwCOMBINEREPLACE);
  }
  if ((*(byte *)(in_esi + 0x68) & 8) != 0) {
    RwFrameScale(*(RwFrame **)(*(char **)(in_esi + 0x28) + 4),in_edi,rwCOMBINEREPLACE);
  }
  if ((*(byte *)(in_esi + 0x68) & 0x10) != 0) {
    RwFrameScale(*(RwFrame **)(*(char **)(in_esi + 0x2c) + 4),in_edi,rwCOMBINEREPLACE);
  }
  if ((*(byte *)(in_esi + 0x68) & 0x20) != 0) {
    RwFrameScale(*(RwFrame **)(*(char **)(in_esi + 0x30) + 4),in_edi,rwCOMBINEREPLACE);
  }
  return;
}

/* entry thunk at the original address of score_panel_entry_scale_parts_low: register inputs (esi,edi) -> natural C score_panel_entry_scale_parts_low_n; ret 0 */
__declspec(naked) void score_panel_entry_scale_parts_low(void)
{
    __asm {
        push edi
        push esi
        call score_panel_entry_scale_parts_low_n
        add esp, 8
        ret
    }
}
