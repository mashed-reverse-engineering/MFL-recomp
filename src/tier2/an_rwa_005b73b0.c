/* rwa_obj_get_param_int32_005b73b0 (0x005b73b0): RwaObjGetParamInt32, read one 32 bit output parameter of an object handle */
#include "mfl_tier2.h"

/* One entry of an interface's parameter table (8 bytes): the getter and the parameter id it is called with. */
typedef struct RwaParamGetter {
  int (*get)(void *obj, int id, void *value);
  unsigned short id;
  unsigned short pad;
} RwaParamGetter;

/* Object handle = { object, interface }; the interface keeps its getter table at +8.  The getter writes the value over
   this function's `handle` argument slot (the original passes that slot, not a local), which is then the return value.
   *ok (optional) = the getter succeeded. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_obj_get_param_int32_005b73b0(int handle, int index, int unused, int *ok)
{
  void **h = (void **)handle;
  RwaParamGetter *e = *(RwaParamGetter **)((char *)h[1] + 8) + index;
  void **res = e->get(h[0], e->id, &handle) ? h : 0;

  if (ok != 0)
    *ok = res != 0;
  return handle;
}
