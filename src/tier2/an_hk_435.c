#include "mfl_tier2.h"
/*006597b8*/ extern undefined g_endpoint_6597b8;
static int endpoint_panel_entry_destroy_r(int reg_eax);
extern void endpoint_panel_entry_destroy(void);
extern char g_endpoint_659a78[];
#define ENDPOINT_PANEL_ENTRY_SIZE (0x160)
__declspec(naked) int endpoint_panel_entry_destroy_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call endpoint_panel_entry_destroy
        ret
    }
}

/* endpoint_panel_destroy */

/* Destroys every endpoint panel entry in the table g_endpoint_6597b8 .. g_endpoint_659a78
   (entries are ENDPOINT_PANEL_ENTRY_SIZE bytes apart; the table holds 2 entries (inferred)).
   Takes no parameters, returns nothing. */

void endpoint_panel_destroy(void)

{
  undefined *entry;

  entry = &g_endpoint_6597b8;
  /* walk the table one entry at a time, calling the destructor on each */
  do {
    endpoint_panel_entry_destroy_r(entry);
    entry = entry + ENDPOINT_PANEL_ENTRY_SIZE;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_endpoint_659a78));
  return;
}


