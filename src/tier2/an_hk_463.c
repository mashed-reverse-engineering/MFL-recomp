#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004c52f0*/ extern void FUN_004c52f0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*006592c4*/ extern undefined g_load_6592c4;
/*006592c8*/ extern undefined4 g_hudpanel_6592c8;
/*006592d4*/ extern undefined4 g_hudpanel_6592d4;
/*006592d8*/ extern undefined4 g_hudpanel_6592d8;
/*006592dc*/ extern undefined4 g_hudpanel_6592dc;

#define KC_PANEL_LOAD_SIZE (0x1c)
#define KC_PANEL_COLOUR (0xff323232)

/*
 * Loads the KC panel model (KCPanel.dff) and sets up the HUD panel globals.
 * Takes no parameters and returns nothing.
 * Results: g_hudpanel_6592d4 = loaded clump, g_hudpanel_6592d8 = clump field at +4 (inferred),
 * g_hudpanel_6592dc = 1 (inferred: panel loaded flag), g_load_6592c4 = zeroed load block.
 */

/* KCPanel.dff */

void load_kc_panel(void)

{
  int clump;
  char *panel_name;
  undefined4 load_arg1;
  undefined4 load_arg2;

  load_arg2 = 0;
  load_arg1 = 0;
  panel_name = "KCPanel.dff";
  /* Load the panel model; the two extra load arguments are passed as 0. */
  clump = clump_load_by_name("KCPanel.dff",load_arg1,load_arg2);
  /* Zero the load block before handing it to FUN_004c52f0 (inferred: sets up the load). */
  mem_zero(&g_load_6592c4,KC_PANEL_LOAD_SIZE);
  g_hudpanel_6592d8 = *(undefined4 *)(clump + 4);
  g_hudpanel_6592dc = 1;
  g_hudpanel_6592d4 = clump;
  FUN_004c52f0(clump,&g_load_6592c4,4,0,0);
  /* Set the panel colour (opaque dark grey) on the colour field reached through g_hudpanel_6592c8 (inferred). */
  *(undefined4 *)(**(int **)(*(char **)(g_hudpanel_6592c8 + 0x18) + 0x20) + 4) = KC_PANEL_COLOUR;
  return;
}


