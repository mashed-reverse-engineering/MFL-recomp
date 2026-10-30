/* compile: /G5 */
#include "mfl_tier2.h"
typedef union RwpFarCell { unsigned short w[10]; unsigned dw[5]; } RwpFarCell;
typedef struct RwpFarfield { char pad[0x8820]; struct { unsigned short lo; unsigned short pad; } index[1024]; RwpFarCell cells[1]; } RwpFarfield;
/* Links a far-field cell to a slot: saves the cell's low bits in the slot index and stores the slot in the cell. (Rwp library) */
void rwp_farfield_00576b50(RwpFarfield *ff, unsigned short cell, unsigned short slot)
{
  RwpFarCell *c = (RwpFarCell *)ff + (cell + 0x79b);
  ff->index[slot].lo = c->dw[0] & 0x3ff;
  c->dw[0] = (c->dw[0] & 0xfffffc00) | slot;
}
