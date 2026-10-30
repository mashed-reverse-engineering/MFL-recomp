/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpBatchState {
  char pad0[0xc4]; int *p1; int n1; char padcc[4]; int *p3; char padd4[8]; int *p2; int n2; char pade4[4]; int *p0; int n0;
} RwpBatchState;
/* Appends a value to the first list and a zero entry to the other three lists of the batch state. (Rwp library) */
void rwp_batch_0057f670(RwpBatchState *s, int v)
{
  s->p0[s->n0] = v;
  s->p1[s->n1] = 0;
  s->p2[s->n2] = 0;
  s->p3[s->n2] = 0;
}
