/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpBatchItem { char pad[0x18]; unsigned char flags; char pad2[0xf]; } RwpBatchItem;
typedef struct RwpBatch { char pad[0x70]; void *ctx; char pad2[0x54]; unsigned item_count; char pad3[8]; RwpBatchItem *items; } RwpBatch;
extern void rwp_batch_0057c0e0(void *ctx, unsigned index);
/* Runs rwp_batch_0057c0e0 for every batch item that has no flag 1 set; returns the batch. (Rwp library) */
RwpBatch *rwp_batch_0057c1e0(RwpBatch *p)
{
  unsigned i;
  for (i = 0; i < p->item_count; i++)
    if (!(p->items[i].flags & 1)) rwp_batch_0057c0e0(p->ctx, i);
  return p;
}
