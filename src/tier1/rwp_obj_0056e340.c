/* compile: /G5 */
#include "mfl_tier2.h"
extern char *RwEngineInstance;
typedef struct RwpObjTable { void *arena; void *pipeline; void *ids; unsigned used; unsigned capacity; void *flags; unsigned slot_count; void *slots; } RwpObjTable;
extern int rwp_pipeline_005717d0(void *pipeline);
extern int FUN_0056a170(void *arena);
/* Destroys an object table and all its arrays; returns 1. (Rwp library) */
int rwp_obj_0056e340(RwpObjTable *t)
{
  rwp_pipeline_005717d0(t->pipeline);
  FUN_0056a170(t->arena);
  RwFree(t->ids);
  RwFree(t->flags);
  t->capacity = 0;
  RwFree(t->slots);
  RwFree(t);
  return 1;
}
