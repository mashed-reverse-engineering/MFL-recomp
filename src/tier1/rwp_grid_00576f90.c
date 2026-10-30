/* compile: /G5 */
#include "mfl_tier2.h"
extern char *RwEngineInstance;
typedef struct RwpShape { char data[0x60]; } RwpShape;
typedef struct RwpGridData {
  char pad0[0xc]; RwpShape *shapes; unsigned shape_count; void *table_a; void *table_b; char pad1[4]; void *table_c; void *table_d; void *table_e;
  char pad2[0x44]; unsigned refs;
} RwpGridData;
typedef struct RwpGrid { char pad[0x40]; RwpGridData *data; } RwpGrid;
extern void rwp_sphere_0056c0a0(RwpShape *s);
extern void rwp_capsule_00575b80(void *p);
/* Drops one reference of the grid data and on the last one frees the shapes, tables and the data block; returns 1. (Rwp library) */
int rwp_grid_00576f90(RwpGrid *g)
{
  RwpGridData *d = g->data;
  if (d->refs-- <= 1) {
    unsigned i;
    for (i = 0; i < d->shape_count; i++) rwp_sphere_0056c0a0(&d->shapes[i]);
    rwp_capsule_00575b80(d->shapes);
    if (d->table_a) {
      RwFree(d->table_a);
      RwFree(d->table_b);
      RwFree(d->table_d);
      RwFree(d->table_c);
      RwFree(d->table_e);
    }
    RwFree(d);
    g->data = 0;
  }
  return 1;
}
