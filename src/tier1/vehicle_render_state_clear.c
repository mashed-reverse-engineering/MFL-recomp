#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*006605a0*/ extern undefined g_game_6605a0;
/*006642f0*/ extern undefined4 g_game_6642f0;
#define VEHICLE_RENDER_STATE_SIZE (0xf54)
/* vehicle_render_state_clear */
/* "clears 0xF54 byte render state of vehicle n at 0x6605A0, resets g_game_6642f0"
 * Purpose: zero the render state block of one vehicle (inferred from the callee).
 * vehicle_index: which vehicle slot to clear; the block is at 0x6605A0 + index * 0xF54.
 * Returns nothing. */
void vehicle_render_state_clear(int vehicle_index)
{
  mem_zero((char *)&g_game_6605a0 + vehicle_index * VEHICLE_RENDER_STATE_SIZE,VEHICLE_RENDER_STATE_SIZE);
  /* Reset the global flag g_game_6642f0 only when it is set. */
  if (g_game_6642f0 != 0) {
    g_game_6642f0 = 0;
  }
  return;
}
