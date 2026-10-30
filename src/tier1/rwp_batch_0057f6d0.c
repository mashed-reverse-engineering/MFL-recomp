/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpBatchState {
  char pad0[0xc8]; int n1; char padcc[8]; int n3; char padd8[8]; int n2; char pade4[4]; int *p0; int n0;
  char padf0[4]; int total; char padf8[4]; int cursor; int align;
} RwpBatchState;
/* Advances the batch state by one entry: aligns the byte cursor to 4, adds the entry size to the total and bumps the four counters and the cursor. (Rwp library) */
void rwp_batch_0057f6d0(RwpBatchState *s)
{
  s->align = (s->align + 3) & 0xfffffffc;
  s->total += s->p0[s->cursor];
  s->n0++;
  s->n1++;
  s->n2++;
  s->n3++;
  s->cursor++;
}
