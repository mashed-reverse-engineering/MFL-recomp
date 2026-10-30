/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpObjTable { void *arena; void *pipeline; void **ids; unsigned used; } RwpObjTable;
/* Removes an id from the object table; returns 1 when it was present. (Rwp library) */
int rwp_obj_0056e440(RwpObjTable *t, void *id)
{
  unsigned i, n = t->used;
  for (i = 0; i < n; i++) {
    if (t->ids[i] == id) {
      t->ids[i] = 0;
      n = t->used;
      if (i + 1 == n) t->used = n - 1;
      return 1;
    }
  }
  return 0;
}
