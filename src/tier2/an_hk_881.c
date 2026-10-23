#include "mfl_tier2.h"
/*004c52f0*/ extern void FUN_004c52f0(undefined4 p0, void *p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004c5380*/ extern void FUN_004c5380(undefined4 p0, void *p1);
/*004c5410*/ extern void FUN_004c5410(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*00658b50*/ extern undefined g_btb_panel_658b50;
/*00658b90*/ extern undefined g_btb_panel_658b90;
/*00658bfc*/ extern undefined g_btb_panel_658bfc;
/*00658c00*/ extern undefined g_btb_panel_658c00;
/*00658c10*/ extern undefined g_btb_panel_658c10;

#define BTB_PANEL_SLOT_SIZE (0xc4)
#define BTB_PANEL_HANDLE_COUNT (7)
#define DEFAULT_SLOT_COLOUR_RGB (0xc8)
#define DEFAULT_SLOT_COLOUR_ALPHA (0x80)

/* btb_panel_slot_init */

/* "(slot, clump): 0xC4 byte slot at 0x658B50, colour 0x80C8C8C8" */

/*
 * Initialises one BTB panel slot (inferred from the body): zeroes the slot's
 * BTB_PANEL_SLOT_SIZE bytes, stores clump and the word at clump+4 in it, sets
 * the word at slot+0xc0 to 1 (inferred: "in use" flag), registers the handle
 * array with FUN_004c52f0 (inferred), then applies a colour to every non-zero
 * handle.
 * Parameters: slot = panel slot index; clump = clump pointer;
 * colour = RGBA colour, or NULL for the default light grey 0x80C8C8C8.
 * Returns nothing.
 */
void btb_panel_slot_init(int slot,int clump,undefined4 *colour)

{
  int handle;
  int handles_left;
  int *handle_slot;
  struct { unsigned char r, g, b, a; int pad_top; } default_colour;

  /* byte offset of this slot inside the slot table */
  slot = slot * BTB_PANEL_SLOT_SIZE;
  default_colour.r = DEFAULT_SLOT_COLOUR_RGB;
  default_colour.g = DEFAULT_SLOT_COLOUR_RGB;
  default_colour.b = DEFAULT_SLOT_COLOUR_RGB;
  default_colour.a = DEFAULT_SLOT_COLOUR_ALPHA;
  mem_zero((char *)&g_btb_panel_658b50 + slot,BTB_PANEL_SLOT_SIZE);
  *(int *)((char *)&g_btb_panel_658bfc + slot) = clump;
  *(undefined4 *)((char *)&g_btb_panel_658c00 + slot) = *(undefined4 *)(clump + 4);
  *(undefined4 *)((char *)&g_btb_panel_658c10 + slot) = 1;
  handle_slot = (int *)((char *)&g_btb_panel_658b90 + slot);
  FUN_004c52f0(clump,handle_slot,0x10,0,0);
  /* no colour given: fall back to the default colour block on the stack */
  if (colour == (undefined4 *)0x0) {
    colour = (undefined4 *)&default_colour;
  }
  /* walk the handle array, skipping empty entries */
  handles_left = BTB_PANEL_HANDLE_COUNT;
  do {
    handle = *handle_slot;
    if (handle != 0) {
      FUN_004c5410(handle,0x40,1);
      FUN_004c5380(handle,colour);
    }
    handle_slot = handle_slot + 1;
    handles_left = handles_left + -1;
  } while (handles_left != 0);
  return;
}


