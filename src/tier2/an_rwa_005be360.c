/* rwa_param_type_open_module_005be360 (0x005be360): parameters (_rwaParamTypeOpenModule): open the five fixed-size parameter block lists, the parameter type list and the built-in parameter types */
#include "mfl_tier2.h"

typedef struct RwaBlockListDef {
  void *list;      /* the free list's own storage */
  RwUInt32 size;   /* block size */
} RwaBlockListDef;

/*005c44f0*/ extern void rwa_symboltable_005c44f0(void *table);
/*005bef40*/ extern void *RwaFreeListCreate(RwUInt32 size, RwUInt32 count, RwUInt32 align, RwUInt32 a4, void *a5, RwUInt32 hint);
/*005beec0*/ extern void RwaFreeListDestroy(void *list);
/*005be1d0*/ extern int rwa_param_type_create_by_type_defs_005be1d0(void *defs, int count);
/*00649c90*/ extern RwaBlockListDef g_rwa_block_lists[5];
/*00649cb8*/ extern char g_rwa_param_type_defs[];
/*00949d50*/ extern char g_data_949d50[];
/*007dbaf4*/ extern char g_data_7dbaf4[];      /* parameter type free list */
/*007dbae8*/ extern int g_data_7dbae8;
/*007dbb60*/ extern int g_data_7dbb60;      /* open flag */

#define RWA_HEAP_HINT 0x40803

/* Returns 1, or 0 with everything destroyed again. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_param_type_open_module_005be360(void)
{
  RwUInt32 i;

  rwa_symboltable_005c44f0(g_data_949d50);
  for (i = 0; i < 5; i++) {
    if (!RwaFreeListCreate(g_rwa_block_lists[i].size, 8, 4, 0, g_rwa_block_lists[i].list, RWA_HEAP_HINT)) {
      /* as the original: the lists made so far are destroyed, then the loop goes on from list 1 */
      for (; i > 0; i--)
        RwaFreeListDestroy(g_rwa_block_lists[i - 1].list);
    }
  }
  g_data_7dbae8 = 0;
  g_data_7dbb60 = 1;
  if (RwaFreeListCreate(0x28, 0x10, 4, 0, g_data_7dbaf4, RWA_HEAP_HINT)) {
    if (rwa_param_type_create_by_type_defs_005be1d0(g_rwa_param_type_defs, 0xf))
      return 1;
    RwaFreeListDestroy(g_data_7dbaf4);
  }
  for (; i > 0; i--)
    RwaFreeListDestroy(g_rwa_block_lists[i - 1].list);
  g_data_7dbb60 = 0;
  return 0;
}
