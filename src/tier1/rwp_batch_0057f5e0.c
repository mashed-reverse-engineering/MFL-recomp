/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpBatchState { char pad00[0x4]; int f04; char pad08[0xc]; int f14; char pad18[0x8]; int f20; char pad24[0x8]; int f2c; char pad30[0x8]; int f38; char pad3c[0x8]; int f44; char pad48[0x8]; int f50; char pad54[0x8]; int f5c; char pad60[0x8]; int f68; char pad6c[0xc]; int f78; char pad7c[0x8]; int f84; char pad88[0x4]; int f8c; char pad90[0x8]; int f98; char pad9c[0x8]; int fa4; char pada8[0x8]; int fb0; char padb4[0x8]; int fbc; char padc0[0x8]; int fc8; char padcc[0x8]; int fd4; char padd8[0x8]; int fe0; char pade4[0x8]; int fec; char padf0[0x4]; int ff4; int ff8; int ffc; int f100; int f104; int f108; } RwpBatchState;
/* Clears the batch state counters, list lengths and cursors. (Rwp library) */
void rwp_batch_0057f5e0(RwpBatchState *s)
{
  s->f04 = 0;
  s->f14 = 0;
  s->f20 = 0;
  s->f2c = 0;
  s->f38 = 0;
  s->f44 = 0;
  s->f50 = 0;
  s->f5c = 0;
  s->f68 = 0;
  s->f8c = 0;
  s->fb0 = 0;
  s->fbc = 0;
  s->fec = 0;
  s->fe0 = 0;
  s->fd4 = 0;
  s->fc8 = 0;
  s->fa4 = 0;
  s->f98 = 0;
  s->f78 = 0;
  s->f84 = 0;
  s->ff4 = 0;
  s->ffc = 0;
  s->ff8 = 0;
  s->f100 = 0;
  s->f104 = 0;
  s->f108 = 0;
}
