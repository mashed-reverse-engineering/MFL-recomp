#include "mfl_tier2.h"
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004c52f0*/ extern void FUN_004c52f0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004c5410*/ extern void FUN_004c5410(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*00658b30*/ extern undefined g_load_658b30;
/*00658b34*/ extern undefined4 g_hudpanel_658b34;
/*00658b40*/ extern undefined4 g_btb_658b40;
/*00658b44*/ extern undefined4 g_hudpanel_658b44;
/*00658b48*/ extern undefined4 g_hudpanel_658b48;

#define BTB_LOAD_STATE_SIZE (0x1c)
#define BTB_LOOP_COUNT (4)

/*
 * Loads the "BTBPanel.dff" clump (beat the bomb panel) and sets up the HUD panel state.
 * Takes no parameters and returns nothing.
 * Stores the clump in g_btb_658b40 and the word at clump + 4 in g_hudpanel_658b44,
 * zeroes the BTB_LOAD_STATE_SIZE-byte block g_load_658b30 (inferred: load state),
 * and passes that block to FUN_004c52f0.
 */

/* "BTBPanel.dff (beat the bomb panel)" */

void load_btb_panel(void)

{
  int clump;
  char *panel_name;
  undefined4 clump_load_arg1;
  undefined4 clump_load_arg2;

  clump_load_arg2 = 0;
  clump_load_arg1 = 0;
  panel_name = "BTBPanel.dff";
  /* Load the clump; both extra arguments are 0 (callee parameters unnamed). */
  clump = clump_load_by_name("BTBPanel.dff",clump_load_arg1,clump_load_arg2);
  mem_zero(&g_load_658b30,BTB_LOAD_STATE_SIZE);
  g_btb_658b40 = clump;
  g_hudpanel_658b44 = *(undefined4 *)(clump + 4);
  g_hudpanel_658b48 = 1;
  FUN_004c52f0(clump,&g_load_658b30,4,0,0);
  /* Loop over BTB_LOOP_COUNT slots (inferred); `clump` is reused as the loop index here.
     Slot 1 gets a call to FUN_004c5410 with g_hudpanel_658b34. */
  clump = 0;
  do {
    if (clump == 1) {
      FUN_004c5410(g_hudpanel_658b34,0x40,1);
    }
    clump = clump + 1;
  } while (clump < BTB_LOOP_COUNT);
  return;
}


