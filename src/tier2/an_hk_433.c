#include "mfl_tier2.h"
/*0040cad0*/ extern int players_get_points();
/*0040ced0*/ extern int players_reached_target();
/*00659798*/ extern undefined g_endpoint_panel_659798;
/*006597a8*/ extern undefined4 g_endpoint_panel_6597a8;
/*006597b8*/ extern undefined g_endpoint_6597b8;
/*00659a78*/ extern undefined4 g_endpoint_659a78;
static int endpoint_panel_entry_update_r(int reg_esi);
extern void endpoint_panel_entry_update(void);

#define ENDPOINT_ENTRY_STRIDE (0x160) /* bytes per endpoint entry in g_endpoint_6597b8 array */

__declspec(naked) int endpoint_panel_entry_update_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call endpoint_panel_entry_update
        pop esi
        ret
    }
}

/* endpoint_panel_update */

/* Per-frame update of the endpoint panel (inferred). Takes no parameters, returns nothing.
   Refreshes the target-reached and points readouts, then calls endpoint_panel_entry_update
   on every entry in g_endpoint_6597b8 up to g_endpoint_659a78, and bumps g_endpoint_659a78
   by one afterwards (inferred: a frame counter). */

void endpoint_panel_update(void)

{
  undefined *entry;

  players_reached_target(&g_endpoint_panel_659798);
  players_get_points(&g_endpoint_panel_6597a8);
  /* walk the entry array one ENDPOINT_ENTRY_STRIDE at a time until the end address */
  entry = &g_endpoint_6597b8;
  do {
    endpoint_panel_entry_update_r(entry);
    entry = entry + ENDPOINT_ENTRY_STRIDE;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_endpoint_659a78));
  /* (inferred) frame counter, incremented once per update after all entries are done */
  g_endpoint_659a78 = g_endpoint_659a78 + 1;
  return;
}


