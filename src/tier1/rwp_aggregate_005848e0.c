/* compile: /G5 */
/* Smallest positive value of item->measure(item) + 2 * item->size over the items of the aggregate's list (1e10 when there is none). */
struct RwpItem;
typedef struct { char pad[0x24]; float (*measure)(struct RwpItem *); } RwpItemVtbl;
typedef struct RwpItem { char pad[0x4c]; float size; char pad2[0xc]; RwpItemVtbl *vt; } RwpItem;
typedef struct { RwpItem *items; unsigned count; } RwpItemList;
typedef struct { char pad[0x40]; RwpItemList *list; } RwpAggregate;
float rwp_aggregate_005848e0(RwpAggregate *agg)
{
  float best = 1e10f;
  RwpItemList *l = agg->list;
  unsigned i;

  for (i = 0; i < l->count; i++) {
    float f = l->items[i].vt->measure(&l->items[i]) + l->items[i].size * 2.0f;
    if (f > 0.0f && f < best) best = f;
  }
  return best;
}
