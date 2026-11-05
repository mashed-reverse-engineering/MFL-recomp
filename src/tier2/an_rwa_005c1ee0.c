/* rwa_obj_int_get_param_data_005c1ee0 (0x005c1ee0): RwaObjIntGetParamData, read one output parameter through an interface table */
#include "mfl_tier2.h"

/* One entry of an interface's parameter table (8 bytes): the getter and the parameter id it is called with. */
typedef struct RwaParamGetter {
  void (*get)(void *obj, int id, void *value);
  unsigned short id;
  unsigned short pad;
} RwaParamGetter;

/* The interface keeps its getter table at +8.  The getter writes the value over this function's own `intf` argument slot,
   which is then the return value (the original passes &intf, not a local). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_obj_int_get_param_data_005c1ee0(void *obj, int intf, int index)
{
  RwaParamGetter *e = *(RwaParamGetter **)(intf + 8) + index;

  e->get(obj, e->id, &intf);
  return intf;
}
