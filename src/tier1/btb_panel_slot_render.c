#include "mfl_tier2.h"
/*00658b50*/ extern undefined g_btb_panel_658b50;
/* byte stride of one panel slot in the g_btb_panel_* tables (inferred from the offset of the slot's fields) */
#define BTB_PANEL_SLOT_SIZE (0xc4)
/* offset of the render method in a clump object (inferred from the comment below) */
#define CLUMP_RENDER_METHOD_OFS (0x48)
/* number of part pointers in the array that starts at g_btb_panel_658b90 */
#define BTB_PANEL_PART_COUNT (4)
/* btb_panel_slot_render */
/* "calls the clump object's method +0x48 when visible" */
/*
 * Renders one btb panel slot. slot_index (inferred) selects the slot by byte offset slot_index * BTB_PANEL_SLOT_SIZE
 * into the g_btb_panel_* tables. If the slot's visible flag (g_btb_panel_658c10) is nonzero, calls the render method
 * (+CLUMP_RENDER_METHOD_OFS, passing the object as its argument) on each non-null clump: 658ba8, 658ba0, the
 * BTB_PANEL_PART_COUNT part pointers from 658b90, then 658ba4. Returns nothing.
 */
void __cdecl btb_panel_slot_render(int slot_index)
{
  int part;
  char *clump;
  int *part_slot;
  char *rec;

  rec = (char *)&g_btb_panel_658b50 + slot_index * BTB_PANEL_SLOT_SIZE;
  if (*(int *)(rec + 0xc0) != 0) {
    clump = *(char **)(rec + 0x58);
    if (clump != 0) {
      (**(code **)(clump + CLUMP_RENDER_METHOD_OFS))(clump);
    }
    clump = *(char **)(rec + 0x50);
    if (clump != 0) {
      (**(code **)(clump + CLUMP_RENDER_METHOD_OFS))(clump);
    }
    part_slot = (int *)(rec + 0x40);
    clump = BTB_PANEL_PART_COUNT;
    do {
      part = *part_slot;
      if (part != 0) {
        (**(code **)(part + CLUMP_RENDER_METHOD_OFS))(part);
      }
      part_slot = part_slot + 1;
      clump = clump + -1;
    } while (clump != 0);
    rec = (char *)*(int *)(rec + 0x54);
    if (rec != 0) {
      (**(code **)(rec + CLUMP_RENDER_METHOD_OFS))(rec);
    }
  }
  return;
}
