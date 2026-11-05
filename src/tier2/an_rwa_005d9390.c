/* rwa_soft_output_int_lookup_005d9390 (0x005d9390): software output: interface lookup of the output object */
#include "mfl_tier2.h"

/*005ba5c0*/ extern void *FUN_005ba5c0(int a, const void *uuid, int b);      /* RwaObjDefFindByUUID */
/*005f834c*/ extern undefined g_rwa_soft_output_lookup_uuid;

/* link = { next lookup, its first argument, object }.  The own answer is the object's data at +0x34 when def is the
   definition the uuid names; the next lookup in the chain wins when it answers non-zero. */
typedef void *(*RwaLookupFn)(void *def, void *arg, void *a3, void *a4);
typedef struct RwaLookupLink { RwaLookupFn next; void *arg; char *obj; } RwaLookupLink;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_output_int_lookup_005d9390(void *def, RwaLookupLink *link, void *a3, void *a4)
{
  void *own = 0;
  void *found;

  if (def == FUN_005ba5c0(0, &g_rwa_soft_output_lookup_uuid, 0))
    own = link->obj + 0x34;
  if (link->next) {
    found = link->next(def, link->arg, a3, a4);
    if (found)
      return found;
  }
  return own;
}
