/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpRange { int base; short extra; } RwpRange;
typedef struct RwpObj {
  void *pool; unsigned short id; char pad06[6]; int f0c, f10, f14, pad18, f1c, f20, f24, pad28, f2c, f30, f34, pad38, f3c, f40, f44, pad48;
  int *counter; int base_a; short extra_a; char pad56[2]; int base_b; short extra_b; char pad5e[2];
  int f60, f64, f68; float scale; int f70, f74, f78, f7c, f80, f84, pad88, f8c, f90, f94;
} RwpObj;
/* Initialises an object: pool, counter, id (0xffff), the two ranges, zeroed state and unit scale. (Rwp library) */
void rwp_obj_0056e8f0(RwpObj *o, void *pool, int *counter, RwpRange *a, RwpRange *b)
{
  o->counter = counter;
  ++*counter;
  o->pool = pool;
  o->id = 0xffff;
  o->extra_a = b->extra;
  o->base_a = b->base;
  o->extra_b = a->extra;
  o->base_b = a->base;
  o->f0c = 0;
  o->f10 = 0;
  o->f14 = 0;
  o->f1c = 0;
  o->f20 = 0;
  o->f24 = 0;
  o->f2c = 0;
  o->f30 = 0;
  o->f34 = 0;
  o->f3c = 0;
  o->f40 = 0;
  o->f44 = 0;
  o->f70 = 0;
  o->f74 = 0;
  o->f78 = 0;
  o->f60 = 0;
  o->f64 = 0;
  o->f68 = 0;
  o->scale = 1.0f;
  o->f7c = 0;
  o->f80 = 0;
  o->f84 = 0;
  o->f8c = 0;
  o->f90 = 0;
  o->f94 = 0;
}
