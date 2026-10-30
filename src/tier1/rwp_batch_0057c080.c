/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpBatchRoot { char pad[0xc018]; int limit; } RwpBatchRoot;
typedef struct RwpBatch { char pad[0x70]; RwpBatchRoot ***ctx; char pad2[0xc]; int list_a; int list_b; } RwpBatch;
extern void rwp_batch_0057be50(RwpBatchRoot ***ctx, int a, int b, int limit);
extern void rwp_convexhullmassprops_00583d20(RwpBatch *p, int list_a, int list_b, int c, int d, int e, int f);
/* Resets the batch list, queues the pairs against the context limit and hands them to the solver (rwp_convexhullmassprops_00583d20); returns 1. (Rwp library) */
int rwp_batch_0057c080(RwpBatch *p, int a, int b, int c, int d, int e, int f)
{
  p->list_b = 0;
  rwp_batch_0057be50(p->ctx, a, b, (**p->ctx)->limit);
  rwp_convexhullmassprops_00583d20(p, p->list_a, p->list_b, c, d, e, f);
  return 1;
}
