/* rwa_param_type_create_by_type_defs_005be1d0 (0x005be1d0): parameter types (_rwaParamTypeCreateByTypeDefs): create the parameter types of a table of type definitions (array, alias, pointer or plain), registered by uuid and name */
#include "mfl_tier2.h"

/*005c4590*/ extern void *rwa_symboltable_005c4590(void *table, const void *key);      /* lookup */
/*005b6730*/ extern RwUInt32 RwaSizeOf(RwUInt32 type);
/*005bdf70*/ extern char *RwaParamTypeCreateArray(RwUInt32 type, void *lo, void *hi, void *a4, void *a5);      /* range data lo .. hi */
/*005be160*/ extern char *RwaParamTypeCreateAlias(void *lo, void *hi, void *type, void *a4, void *a5);
/*005be0b0*/ extern char *RwaParamTypeCreatePointer(void *type, void *a2, void *a3);
/*005bde10*/ extern void FUN_005bde10(void *type, int a2);      /* release a parameter type */
/*005be810*/ extern void *rwa_unique_id_set_uuid_005be810(void *id, void *uuid);
/*005be7d0*/ extern void *rwa_unique_id_set_name_005be7d0(void *id, char *name);
/*00949d50*/ extern undefined g_data_949d50;      /* the parameter type symbol table */

#define PT_ARRAY   0x2000
#define PT_ALIAS   0x4000
#define PT_POINTER 0x1000
#define PT_HAS_ELEMENT 0x4000      /* type flags (+0x14) of an array with a named element type */

typedef struct RwaParamTypeDef {      /* 0x20 */
  char *name;
  RwUInt32 type;           /* +0x04 */
  RwUInt32 size;           /* +0x08 count of an array; range of a plain type: base .. base + size */
  char *base;              /* +0x0c the range data */
  void *uuid;              /* +0x10 */
  const void *element;     /* +0x14 key of the element / target type */
  void *a6, *a7;           /* +0x18 */
} RwaParamTypeDef;

/* Returns defs, NULL when one fails (the types created so far are released again). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaParamTypeDef *rwa_param_type_create_by_type_defs_005be1d0(RwaParamTypeDef *defs, RwUInt32 count)
{
  RwaParamTypeDef *d;
  char *t, *element;
  RwUInt32 i, flags, n[2];      /* n: size and count of an array (in the original's frame order) */

  for (i = 0; i < count; i++) {
    d = &defs[i];
    if (d->type == PT_ARRAY) {
      element = (char *)rwa_symboltable_005c4590(&g_data_949d50, d->element);
      n[0] = d->size;
      n[1] = 0;
      t = RwaParamTypeCreateArray(PT_ARRAY, &n[1], &n[0], d->a6, d->a7);
      if (!t)
        goto fail;
      flags = PT_ARRAY;
      if (element) {
        flags = *(RwUInt32 *)(element + 0x14) | PT_HAS_ELEMENT;
        *(char **)(t + 0x18) = element;
      }
      *(RwUInt32 *)(t + 0x14) |= flags;
    } else if (d->type == PT_ALIAS) {
      element = (char *)rwa_symboltable_005c4590(&g_data_949d50, d->element);
      t = RwaParamTypeCreateAlias(d->base, d->base + RwaSizeOf(*(RwUInt32 *)(element + 0x14)), element, d->a6, d->a7);
    } else if (d->type == PT_POINTER)
      t = RwaParamTypeCreatePointer(d->element ? rwa_symboltable_005c4590(&g_data_949d50, d->element) : 0, d->a6, d->a7);
    else
      t = RwaParamTypeCreateArray(d->type, d->base, d->base + d->size, d->a6, d->a7);
    if (!t)
      goto fail;
    rwa_unique_id_set_uuid_005be810(t, d->uuid);
    rwa_unique_id_set_name_005be7d0(t, d->name);
  }
  return defs;

fail:
  for (; i; i--)
    FUN_005bde10(rwa_symboltable_005c4590(&g_data_949d50, defs[i - 1].uuid), 1);
  return 0;
}
