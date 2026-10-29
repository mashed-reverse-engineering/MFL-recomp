#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004c52f0*/ extern void clump_collect_atomics_by_id(undefined4 p0, void *p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004c5410*/ extern void FUN_004c5410(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*005de3ac*/ extern undefined g_stopclock_load_5de3ac;
/*00659ac0*/ extern undefined4 g_hudpanel_659ac0;
/*00659b00*/ extern undefined4 g_stopclock_659b00;
/*00659b04*/ extern undefined4 g_ai_659b04;
/*00659b08*/ extern undefined4 g_stopclock_659b08;
#define HUDPANEL_BYTES (0x54) /* size of the hud panel block cleared at load (inferred) */
#define HUDPANEL_SLOT_COUNT (0x10) /* number of entries in g_hudpanel_659ac0 (inferred) */
#define CLOCK_PANEL_INDEX (2) /* slot that gets the clock colour (inferred) */
/* stopclock_load */
/* "loads the stopclock clump by name and registers its parts" */
/*
 * Loads the "RacePanel.dff" clump (stopclock HUD panel) and registers it:
 * clears the hud panel slot table (HUDPANEL_BYTES), stores the clump in
 * g_stopclock_659b00, copies clump+4 (the clump's RwFrame) into g_ai_659b04 (the global name "ai" is a leftover),
 * sets g_stopclock_659b08 to 1, and hands the table to clump_collect_atomics_by_id (inferred:
 * registers the slots). Then, for slots 0..HUDPANEL_SLOT_COUNT-1, slot
 * CLOCK_PANEL_INDEX gets extra setup and the CLOCK_COLOUR text colour.
 * Takes no parameters and returns nothing. load_arg_1 / load_arg_2 are passed
 * to clump_load_by_name as 0; their meaning is unknown.
 */
void stopclock_load(void)
{
  int panel_entry;
  RwRGBA col;
  int clump;
  undefined *stopclock_state_ptr;
  undefined4 load_arg_1;
  undefined4 load_arg_2;

  load_arg_2 = 0;
  load_arg_1 = 0;
  stopclock_state_ptr = &g_stopclock_load_5de3ac;
  clump = clump_load_by_name("RacePanel.dff",load_arg_1,load_arg_2);
  mem_zero(&g_hudpanel_659ac0,HUDPANEL_BYTES);
  g_stopclock_659b00 = clump;
  g_ai_659b04 = *(undefined4 *)(clump + 4);
  g_stopclock_659b08 = 1;
  clump_collect_atomics_by_id(clump,&g_hudpanel_659ac0,0x10,0,0);
  /* walk every hud panel slot; only the clock slot gets the extra setup */
  clump = 0;
  do {
    panel_entry = (&g_hudpanel_659ac0)[clump];
    /* clock slot: FUN_004c5410 setup, then set the colour (inferred) */
    if (clump == CLOCK_PANEL_INDEX) {
      col.red = 0x32;
      col.green = 0x32;
      col.blue = 0x32;
      col.alpha = 0xff;
      FUN_004c5410(panel_entry,0x40,1);
      *(int *)(**(int **)(*(char **)(panel_entry + 0x18) + 0x20) + 4) = *(int *)&col;
    }
    clump = clump + 1;
  } while (clump < HUDPANEL_SLOT_COUNT);
  return;
}
