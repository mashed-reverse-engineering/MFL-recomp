/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpBatchState { char pad[0x110]; int f110; char pad2[8]; int f11c; } RwpBatchState;
typedef struct RwpBatchCursor { char pad[4]; int f04; char pad2[8]; int f10; char pad3[0x20]; int f34; char pad4[8]; int f40; char pad5[8]; int f4c; } RwpBatchCursor;
extern void rwp_batch_0057f5e0(RwpBatchState *s);
/* Clears the batch state counters and rewinds the cursor (offsets +4, +0x10, +0x34, +0x40, +0x4c). (Rwp library) */
void rwp_batch_005782b0(RwpBatchState *s, RwpBatchCursor *c)
{
  s->f11c = 0;
  s->f110 = 0;
  rwp_batch_0057f5e0(s);
  c->f40 = 0;
  c->f4c = 0;
  c->f34 = 0;
  c->f04 = 0;
  c->f10 = 0;
}
