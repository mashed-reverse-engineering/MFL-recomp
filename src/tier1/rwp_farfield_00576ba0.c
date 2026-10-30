/* compile: /G5 */
#include "mfl_tier2.h"
typedef union RwpFarCell { unsigned short w[10]; unsigned dw[5]; } RwpFarCell;
typedef struct RwpFarfield { char pad[0x9820]; RwpFarCell cells[1]; } RwpFarfield;
/* Bumps the 6-bit counter in the top bits of a far-field cell word unless it is saturated. (Rwp library) */
void rwp_farfield_00576ba0(RwpFarfield *ff, unsigned short cell, unsigned short slot)
{
  unsigned short *w = &ff->cells[cell].w[slot];
  if ((*w & 0x7c00) != 0x7c00) *w += 0x400;
}
