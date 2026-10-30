/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpGridCounts { char pad00[0x8]; int f08; char pad0c[0x8]; int f14; char pad18[0x8]; int f20; char pad24[0x8]; int f2c; char pad30[0x8]; int f38; char pad3c[0x8]; int f44; char pad48[0x8]; int f50; char pad54[0x14]; int f68; char pad6c[0x8]; int f74; char pad78[0x4]; int f7c; int f80; int f84; char pad88[0x4]; int f8c; char pad90[0x8]; int f98; } RwpGridCounts;
typedef struct RwpGridCursor { char pad00[0x4]; int f04; char pad08[0x8]; int f10; } RwpGridCursor;
/* Initialises the grid counters from a body count (divided into quarters) and a limit. (Rwp library) */
void rwp_grid_00577c80(RwpGridCursor *c, RwpGridCounts *g, int count, int limit)
{
  int quarter;
  g->f20 = 0;
  g->f2c = 0;
  g->f38 = 0;
  g->f14 = 0;
  g->f44 = 0;
  g->f08 = 0;
  g->f50 = 0;
  g->f74 = 0;
  g->f68 = 0;
  quarter = count / 4;
  g->f7c = limit;
  g->f8c = quarter;
  g->f98 = quarter;
  g->f80 = quarter * 2;
  g->f84 = quarter;
  c->f04 = quarter;
  c->f10 = quarter;
}
