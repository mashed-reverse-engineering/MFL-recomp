/* rwa_param_type_create_pointer_005be0b0 (0x005be0b0): parameters (RwaParamTypeCreatePointer): a pointer parameter type, to an existing base type (an array type over the base's range) or a plain pointer type */
#include "mfl_tier2.h"

/*005bdf70*/ extern char *RwaParamTypeCreateArray(RwUInt32 type, void *lo, void *hi, void *block, void *name);      /* range data lo .. hi */
/*005b6730*/ extern RwUInt32 RwaSizeOf(RwUInt32 type);
/*005bec50*/ extern void *rwa_unit243670_005bec50(void *list, RwUInt32 hint);      /* free list: allocate */
/*005be6b0*/ extern void _rwaUniqueIDInitialize(void *obj);
/*005c4510*/ extern void rwa_symboltable_005c4510(void *table, void *entry);      /* add */
/*007dbaf4*/ extern char g_data_7dbaf4[];      /* parameter type free list */
/*00949d50*/ extern char g_data_949d50[];      /* parameter type table */

#define RWA_TYPE_POINTER 0x1000
#define RWA_HINT_PARAM   0x30803
/* type+0x1c flags */
#define PT_INBLOCK 0x1      /* lives in the caller's block */
#define PT_NAMED   0x2

typedef struct RwaParamType {      /* 0x28 */
  char uid[0xc];
  void *lo, *hi;      /* +0x0c / +0x10 range data */
  RwUInt32 type;      /* +0x14 */
  struct RwaParamType *base;      /* +0x18 */
  RwUInt32 flags;     /* +0x1c */
  RwUInt32 w20, w24;
} RwaParamType;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaParamType *rwa_param_type_create_pointer_005be0b0(RwaParamType *base, RwaParamType *block, void *name)
{
  RwaParamType *t;

  if (base) {
    t = (RwaParamType *)RwaParamTypeCreateArray(RWA_TYPE_POINTER, base->lo, base->hi, block, name);
    if (t)
      t->base = base;
    return t;
  }
  RwaSizeOf(RWA_TYPE_POINTER);      /* result unused, as in the original */
  if (!block) {
    t = (RwaParamType *)rwa_unit243670_005bec50(g_data_7dbaf4, RWA_HINT_PARAM);
    t->flags = 0;
  } else {
    t = block;
    t->flags = PT_INBLOCK;
  }
  t->lo = 0;
  t->hi = 0;
  if (name)
    t->flags |= PT_NAMED;
  _rwaUniqueIDInitialize(t);
  t->type = RWA_TYPE_POINTER;
  t->base = 0;
  rwa_symboltable_005c4510(g_data_949d50, t);
  return t;
}
