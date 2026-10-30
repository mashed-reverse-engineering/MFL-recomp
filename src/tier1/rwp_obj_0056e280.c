/* compile: /G5 */
#include "mfl_tier2.h"
extern char *RwEngineInstance;
typedef void *(__cdecl *RwCallocFn)(unsigned, unsigned, unsigned);
#define RwCalloc RW_GLOBAL(0x114, RwCallocFn)
typedef struct RwpObjDesc { char pad0[4]; unsigned slot_count; char pad1[0x1c]; unsigned capacity; } RwpObjDesc;
typedef struct RwpSlot { int a, b; } RwpSlot;
typedef struct RwpObjTable { void *arena; void *pipeline; void *ids; unsigned used; unsigned capacity; void *flags; unsigned slot_count; RwpSlot *slots; } RwpObjTable;
extern void *rwp_mgr_0056a120(RwpObjDesc *desc, int a, int b, int c);
extern void *rwp_pipeline_00570e50(RwpObjTable *t, RwpObjDesc *desc, int a);
/* Builds the object table: arena, pipeline, id and flag arrays and the zeroed slot array. (Rwp library) */
RwpObjTable *rwp_obj_0056e280(RwpObjTable *t, RwpObjDesc *desc, int a, int b)
{
  unsigned i, n;
  t->arena = rwp_mgr_0056a120(desc, b, a, 0);
  t->pipeline = rwp_pipeline_00570e50(t, desc, a);
  n = desc->capacity;
  t->used = 0;
  t->capacity = n;
  t->ids = RwMalloc(n * 4, 0x30900);
  t->flags = RwMalloc(n * 4, 0x30900);
  t->slot_count = desc->slot_count;
  t->slots = RwCalloc(desc->slot_count, 8, 0x30900);
  for (i = 0; i < desc->slot_count; i++) {
    t->slots[i].a = 0;
    t->slots[i].b = 0;
  }
  return t;
}
