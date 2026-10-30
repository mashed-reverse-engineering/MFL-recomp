/* compile: /G5 */
/* Cached smallest positive value of item->measure(item) + 2 * item->size over the items of the grid's world (cache at +0x7c, 1e10 when there is none); the cache is used when it is positive. */
struct RwpItem;
typedef struct { char pad[0x24]; float (*measure)(struct RwpItem *); } RwpItemVtbl;
typedef struct RwpItem { char pad[0x4c]; float size; char pad2[0xc]; RwpItemVtbl *vt; } RwpItem;
typedef struct { char pad[0xc]; RwpItem *items; unsigned count; char pad2[0x68]; float cached; } RwpGridWorld;
typedef struct { char pad[0x40]; RwpGridWorld *world; } RwpGrid;
float rwp_grid_00577150(RwpGrid *grid)
{
  float best = 1e10f;
  RwpGridWorld *w = grid->world;
  unsigned i;

  if (w->cached > 0.0f) return w->cached;
  for (i = 0; i < w->count; i++) {
    float f = w->items[i].vt->measure(&w->items[i]) + w->items[i].size * 2.0f;
    if (f > 0.0f && f < best) best = f;
  }
  w->cached = best;
  return best;
}
