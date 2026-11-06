/* rwa_param_type_destroy_005bde10 (0x005bde10): parameter types: remove a type and (recurse = 1) its derived types from the symbol table, free its atomics */
#include "mfl_tier2.h"

/*005c45e0*/ extern char *rwa_symboltable_005c45e0(void *table, char *parent);      /* next child of parent, 0: none */
/*005c4550*/ extern void rwa_symboltable_005c4550(void *table, char *entry);      /* remove */
/*005be850*/ extern void _rwaUniqueIDFreeData(void *id);
/*005bdda0*/ extern void RwaAtomicFree(void *atomics, RwUInt32 count);
/*005bed80*/ extern void RwaFreeListFree(void *list, void *item);
/*00949d50*/ extern undefined g_data_949d50;      /* the parameter type symbol table */
/*007dbaf4*/ extern undefined g_data_7dbaf4;      /* type free list */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_param_type_destroy_005bde10(char *type, int recurse)
{
  char *child;

  while ((child = rwa_symboltable_005c45e0(&g_data_949d50, type)) != 0)
    if (recurse == 1)
      rwa_param_type_destroy_005bde10(child, recurse);
  if (!(*(unsigned char *)(type + 0x1c) & 1))
    RwaFreeListFree(&g_data_7dbaf4, type);      /* (as the original: freed before its last uses) */
  rwa_symboltable_005c4550(&g_data_949d50, type);
  _rwaUniqueIDFreeData(type);
  if (!(*(unsigned char *)(type + 0x1c) & 2)) {
    if (*(void **)(type + 0xc))
      RwaAtomicFree(*(void **)(type + 0xc), *(RwUInt32 *)(type + 0x14));
    if (*(void **)(type + 0x10))
      RwaAtomicFree(*(void **)(type + 0x10), *(RwUInt32 *)(type + 0x14));
  }
}
