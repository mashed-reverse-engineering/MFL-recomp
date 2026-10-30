/* compile: /G5 */
#include "mfl_tier2.h"
typedef union RwpFarCell { unsigned short w[10]; unsigned dw[5]; } RwpFarCell;
typedef struct RwpFarfield { char pad[0x8820]; struct { unsigned short lo; unsigned short pad; } index[1024]; RwpFarCell cells[1]; } RwpFarfield;
/* Replaces the low 10 bits of a far-field cell word. (Rwp library) */
void rwp_farfield_00576b10(RwpFarfield *ff, unsigned short cell, unsigned short slot, int bits)
{
  unsigned short *w = &ff->cells[cell].w[slot];
  *w = (*w & 0xfc00) | bits;
}
