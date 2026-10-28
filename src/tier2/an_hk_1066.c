#include "mfl_tier2.h"
/*006c3348*/ extern undefined1 g_text_pages_6c3348;
/*006c33c8*/ extern undefined4 g_gfx_6c33c8;
/*006c3d9c*/ extern undefined4 g_text_pages_6c3d9c;
/* text page table: entries of 0x84 bytes (128-byte string at +0, dword value at +0x80); inferred from g_gfx_6c33c8 - g_text_pages_6c3348 = 0x80 */
#define TEXT_PAGE_ENTRY_BYTES (0x84)
#define TEXT_PAGE_ENTRY_DWORDS (0x21)
/* course_helper_4801f0 */

/* callers course_egypt_cb12,course_rouabout_cb12 */

/* Appends string `text` plus a value `gfx_value` as a new entry of the text page table and bumps the entry count (inferred). */
void course_helper_4801f0(char *text,undefined4 gfx_value)

{
  char ch;
  int copy_delta;
  int slot;
  
  slot = g_text_pages_6c3d9c;
  copy_delta = g_text_pages_6c3d9c * TEXT_PAGE_ENTRY_BYTES - (int)text;
  do {
    ch = *text;
    text[(int)((char *)&g_text_pages_6c3348 + copy_delta)] = ch;
    text = text + 1;
  } while (ch != '\0');
  (&g_gfx_6c33c8)[slot * TEXT_PAGE_ENTRY_DWORDS] = gfx_value;
  g_text_pages_6c3d9c = g_text_pages_6c3d9c + 1;
  return;
}


