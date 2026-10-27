#include "mfl_tier2.h"

#define TEXT_PAGE_START_OFFSET -0x20
#define TEXT_PAGE_STRIDE_DWORDS 0x21
#define TEXT_PAGES_END_ADDR ((int)&g_text_pages_6c3e18)

/*006c33c8*/ extern undefined4 g_gfx_6c33c8;
/*006c3d9c*/ extern undefined4 g_text_pages_6c3d9c;
/*006c3e18*/ extern char g_text_pages_6c3e18[];


/* text_pages_clear */

/* "clears the 0x84 byte records at 0x6C33C8" */
/* Clears all text page records and resets the active text page count. */

void text_pages_clear(void)
{
  undefined4 *page_ptr;
  
  page_ptr = &g_gfx_6c33c8;
  /* Clear each of the 20 text page records (unknown: byte at start and dword at +0x80) */
  do {
    *(undefined1 *)(page_ptr + TEXT_PAGE_START_OFFSET) = 0;
    *page_ptr = 0;
    page_ptr = page_ptr + TEXT_PAGE_STRIDE_DWORDS;
  } while ((int)page_ptr < TEXT_PAGES_END_ADDR);
  /* Reset active text page count */
  g_text_pages_6c3d9c = 0;
  return;
}

