// cluster-entries: endpoint_panel_render
/* Installed cluster endpoint_panel_render (tools/cluster_install.py from output/cluster_pilot_zx/endpoint_panel_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*006597b8*/ extern undefined g_endpoint_6597b8;
extern char g_endpoint_659a78[];

/* Size in bytes of one endpoint panel entry (inferred: table stride). */
#define ENDPOINT_ENTRY_STRIDE (0x160)


/* endpoint_panel_render */

/*
 * Renders every endpoint panel entry in the table that starts at
 * g_endpoint_6597b8 and ends at g_endpoint_659a78 (inferred), one entry per
 * ENDPOINT_ENTRY_STRIDE bytes. Takes no parameters and returns nothing.
 */

static int endpoint_panel_entry_render_r(int p0);
extern void endpoint_panel_entry_render(void);
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

/* endpoint_panel_entry_render_r: C -> original endpoint_panel_entry_render (registers eax) */
__declspec(naked) int endpoint_panel_entry_render_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call endpoint_panel_entry_render
        ret
    }
}
