/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpObjTable { void *arena; void *pipeline; void **ids; unsigned used; unsigned capacity; void **flags; } RwpObjTable;
/* Stores an id and a flag value in the first free entry of the object table (growing the used count when needed); returns the table or NULL when full. (Rwp library) */
RwpObjTable *rwp_obj_0056e3b0(RwpObjTable *t, void *id, void *flag)
{
  unsigned i, n = t->used;
  for (i = 0; i < n && t->ids[i]; i++) ;
  if (i == t->capacity) return 0;
  if (i == n) t->used = n + 1;
  t->ids[i] = id;
  t->flags[i] = flag;
  return t;
}
