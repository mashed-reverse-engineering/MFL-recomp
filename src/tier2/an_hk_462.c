#include "mfl_tier2.h"
/*0041d480*/ extern int btb_panel_slot_init();
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*00658cd8*/ extern undefined4 g_btb_658cd8;
/*00658d2c*/ extern undefined4 g_load_ghost_658d2c;
/*00658dac*/ extern char g_score_658dac[];

/* load_ghost_model */

/* ghost.dff */

/* Loads the ghost.dff clump once per ghost slot and resets that slot's entry.
 * For each slot: stores the clump in g_btb_658cd8 (4 bytes per slot, at
 * slot_base offset), registers it with btb_panel_slot_init, then resets the
 * 16-word entry in g_load_ghost_658d2c. Loops until entry reaches g_score_658dac.
 * unused_arg: not read by this function.
 * slot_base: first value passed as the slot argument; also the base offset
 * into g_btb_658cd8 (inferred). Returns nothing. */
void load_ghost_model(char *unused_arg,int slot_base)

{
  undefined4 clump;
  int slot_arg;
  int slot_index;
  undefined4 *entry;

  slot_index = 0;
  entry = &g_load_ghost_658d2c;
  slot_arg = slot_base;
  /* One pass per slot: load the clump, register it, reset the entry (16 words). */
  do {
    clump = clump_load_by_name("ghost.dff", 0x0, 0x0);
    *(undefined4 *)((LONG_PTR)&g_btb_658cd8 + (slot_arg - slot_base)) = clump;
    btb_panel_slot_init(slot_index,clump,slot_arg);
    entry[5] = FLOAT_ONE_BITS;
    *entry = FLOAT_ONE_BITS;
    entry[-5] = FLOAT_ONE_BITS;
    entry[-1] = 0;
    entry[-3] = 0;
    entry[-4] = 0;
    entry[4] = 0;
    entry[3] = 0;
    entry[1] = 0;
    entry[9] = 0;
    entry[8] = 0;
    entry[7] = 0;
    entry[-2] = entry[-2] | 0x20003;
    entry = entry + 0x10;
    slot_index = slot_index + 1;
    slot_arg = slot_arg + 4;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_score_658dac));
  return;
}


