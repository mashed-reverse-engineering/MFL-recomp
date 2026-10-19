#include "mfl_tier2.h"
/*006597b8*/ extern undefined g_endpoint_6597b8;
static int endpoint_panel_entry_render_r(int reg_eax);
extern void endpoint_panel_entry_render(void);
extern char g_endpoint_659a78[];

/* Size in bytes of one endpoint panel entry (inferred: table stride). */
#define ENDPOINT_ENTRY_STRIDE (0x160)

__declspec(naked) int endpoint_panel_entry_render_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call endpoint_panel_entry_render
        ret
    }
}

/* endpoint_panel_render */

/*
 * Renders every endpoint panel entry in the table that starts at
 * g_endpoint_6597b8 and ends at g_endpoint_659a78 (inferred), one entry per
 * ENDPOINT_ENTRY_STRIDE bytes. Takes no parameters and returns nothing.
 */

void endpoint_panel_render(void)

{
  undefined *entry;

  /* Walk the table: render the entry at `entry`, then advance by one stride
     until the end of the table (inferred). */
  entry = &g_endpoint_6597b8;
  do {
    endpoint_panel_entry_render_r(entry);
    entry = entry + ENDPOINT_ENTRY_STRIDE;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_endpoint_659a78));
  return;
}


