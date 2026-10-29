#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004c52f0*/ extern void clump_collect_atomics_by_id(undefined4 p0, void *p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*006592c4*/ extern undefined g_load_6592c4;
/*006592c8*/ extern char *g_hudpanel_6592c8;
/*006592d4*/ extern undefined4 g_hudpanel_6592d4;
/*006592d8*/ extern undefined4 g_hudpanel_6592d8;
/*006592dc*/ extern undefined4 g_hudpanel_6592dc;
#define KC_PANEL_LOAD_SIZE (0x1c)
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
  RwRGBA colour;

  colour.red = 0x32;
  colour.green = 0x32;
  colour.blue = 0x32;
  colour.alpha = 0xff;
  clump = clump_load_by_name("KCPanel.dff",0,0);
  mem_zero(&g_load_6592c4,KC_PANEL_LOAD_SIZE);
  g_hudpanel_6592d4 = clump;
  g_hudpanel_6592d8 = *(undefined4 *)(clump + 4);
  g_hudpanel_6592dc = 1;
  clump_collect_atomics_by_id(clump,&g_load_6592c4,4,0,0);
  *(undefined4 *)(**(int **)(*(char **)(g_hudpanel_6592c8 + 0x18) + 0x20) + 4) = *(undefined4 *)&colour;
  return;
}
