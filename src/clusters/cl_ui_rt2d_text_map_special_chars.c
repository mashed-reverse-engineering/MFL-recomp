// cluster-entries: ui_rt2d_text_map_special_chars
/* Installed cluster ui_rt2d_text_map_special_chars (tools/cluster_install.py from output/cluster_pilot11/ui_rt2d_text_map_special_chars.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* ui_rt2d_text_map_special_chars */

/* WARNING: Unknown calling convention */
/* "maps the special control codes 8..0xB of a counted wide string to font glyph codes" */

void ui_rt2d_text_map_special_chars_n(ushort *in_eax, short *in_ebx)

{
  ushort uVar1;
  short sVar2;
  short *psVar3;
  uint uVar4;
  
  uVar1 = *in_eax;
  if (0 < (short)uVar1) {
    uVar4 = (uint)uVar1;
    psVar3 = in_ebx;
    do {
      sVar2 = *(short *)((int)in_eax + (2 - (int)in_ebx) + (int)psVar3);
      if (sVar2 == 8) {
        sVar2 = 0x81;
      }
      else if (sVar2 == 9) {
        sVar2 = 0x7f;
      }
      else if (sVar2 == 10) {
        sVar2 = 0x81;
      }
      else if (sVar2 == 0xb) {
        sVar2 = 0x8d;
      }
      else if (sVar2 == 0xc) {
        sVar2 = 0x80;
      }
      else if (sVar2 == 0xd) {
        sVar2 = 0x87;
      }
      else if (sVar2 == 0xe) {
        sVar2 = 0x8f;
      }
      *psVar3 = sVar2;
      psVar3 = psVar3 + 1;
      uVar4 = uVar4 - 1;
    } while (uVar4 != 0);
    in_ebx[(short)uVar1] = 0;
    return;
  }
  in_ebx[(short)uVar1] = 0;
  return;
}

/* entry thunk at the original address of ui_rt2d_text_map_special_chars: register inputs (eax,ebx) -> natural C ui_rt2d_text_map_special_chars_n; ret 0 */
__declspec(naked) void ui_rt2d_text_map_special_chars(void)
{
    __asm {
        push ebx
        push eax
        call ui_rt2d_text_map_special_chars_n
        add esp, 8
        ret
    }
}
