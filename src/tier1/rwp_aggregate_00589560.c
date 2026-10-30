/* compile: /G5 */
#include "mfl_tier2.h"
extern char *RwEngineInstance;
typedef struct RwpShape { char data[0x60]; } RwpShape;
typedef struct RwpAggregateData { RwpShape *children; unsigned child_count; char pad[0x5c]; int refs; } RwpAggregateData;
extern void rwp_sphere_0056c0a0(RwpShape *child);
extern void rwp_capsule_00575b80(void *p);
/* Drops one reference of an aggregate primitive; on the last one releases its children and the block; returns 1. (Rwp library) */
int rwp_aggregate_00589560(RwpAggregateData *a)
{
  if (--a->refs == 0) {
    unsigned i;
    for (i = 0; i < a->child_count; i++) rwp_sphere_0056c0a0(&a->children[i]);
    rwp_capsule_00575b80(a->children);
    a->child_count = 0;
    RwFree(a);
  }
  return 1;
}
