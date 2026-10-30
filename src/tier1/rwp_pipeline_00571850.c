/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpBuf { void *data; unsigned count; unsigned capacity; } RwpBuf;
typedef struct RwpPipeline {
  char pad[0x70]; int *dyn; void *list_a; unsigned count_a; unsigned cap_a; void *list_b; unsigned count_b; unsigned cap_b;
  char pad2[0x28]; void *list_c; unsigned count_c; unsigned cap_c;
} RwpPipeline;
extern void rwp_dyn_0056a7c0(int dyn, void *a, unsigned cap_a, unsigned *count_a, void *b, unsigned cap_b, unsigned *count_b, void *c, unsigned cap_c, unsigned *count_c);
/* Calls rwp_dyn_0056a7c0 with the three (list, capacity, count) triples of the pipeline; returns the pipeline. (Rwp library) */
RwpPipeline *rwp_pipeline_00571850(RwpPipeline *p)
{
  rwp_dyn_0056a7c0(*p->dyn, p->list_a, p->cap_a, &p->count_a, p->list_b, p->cap_b, &p->count_b, p->list_c, p->cap_c, &p->count_c);
  return p;
}
