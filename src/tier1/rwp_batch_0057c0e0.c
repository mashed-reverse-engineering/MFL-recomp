/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpBatchRoot { char pad[0xc018]; int limit; } RwpBatchRoot;
typedef struct RwpBatchRec { char pad[0xdc]; struct RwpBatchRec *next; } RwpBatchRec;
typedef struct RwpBatchItem { int a; RwpBatchRec *head; int pad; int b; char pad2[0x18]; } RwpBatchItem;
typedef struct RwpBatchData {
  char pad0[0x70]; RwpBatchRoot ***ctx; char pad1[0xc]; int list_a; int list_b; char pad2[0x4]; RwpBatchRec *recs; unsigned used; unsigned cap;
  char pad3[0x3c]; RwpBatchItem *items;
} RwpBatchData;
typedef struct RwpBatch { int pad; RwpBatchData *data; } RwpBatch;
extern void rwp_batch_0057be50(RwpBatch *p, int a, int b, int limit);
extern unsigned rwp_batch_00579040(RwpBatchData *d, int list_a, int list_b, RwpBatchRec *recs, unsigned room);
/* Queues the item with the given index against the context limit, moves the new records onto the item's record list and returns whether any were produced. (Rwp library) */
int rwp_batch_0057c0e0(RwpBatch *p, unsigned index)
{
  RwpBatchData *d = p->data;
  RwpBatchItem *item;
  unsigned n, i;
  d->list_b = 0;
  if (d->used >= d->cap) return 0;
  item = &d->items[index];
  rwp_batch_0057be50(p, item->a, item->b, (**d->ctx)->limit);
  n = rwp_batch_00579040(d, d->list_a, d->list_b, d->recs + d->used, d->cap - d->used);
  if (n != 0) {
    for (i = 0; i < n; i++) {
      d->recs[d->used + i].next = item->head;
      item->head = &d->recs[d->used + i];
    }
    d->used += n;
  }
  return n > 0;
}
