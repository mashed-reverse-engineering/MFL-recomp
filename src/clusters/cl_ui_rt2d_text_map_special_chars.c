// cluster-entries: ui_rt2d_text_map_special_chars
/* Installed cluster ui_rt2d_text_map_special_chars (tools/cluster_install.py from output/cluster_pilot11/ui_rt2d_text_map_special_chars.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* ui_rt2d_text_map_special_chars */

/* WARNING: Unknown calling convention */
/* "maps the special control codes 8..0xB of a counted wide string to font glyph codes" */

void ui_rt2d_text_map_special_chars_n(ushort *text, short *out)

{
  ushort count;
  short ch;
  short *out_cursor;
  uint left;
  
  count = *text;
  if (0 < (short)count) {
    left = (uint)count;
    out_cursor = out;
    do {
      ch = *(short *)((int)text + (2 - (int)out) + (int)out_cursor);
      if (ch == 8) {
        ch = 0x81;
      }
      else if (ch == 9) {
        ch = 0x7f;
      }
      else if (ch == 10) {
        ch = 0x81;
      }
      else if (ch == 0xb) {
        ch = 0x8d;
      }
      else if (ch == 0xc) {
        ch = 0x80;
      }
      else if (ch == 0xd) {
        ch = 0x87;
      }
      else if (ch == 0xe) {
        ch = 0x8f;
      }
      *out_cursor = ch;
      out_cursor = out_cursor + 1;
      left = left - 1;
    } while (left != 0);
    out[(short)count] = 0;
    return;
  }
  out[(short)count] = 0;
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
