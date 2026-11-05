/* rwa_obj_def_open_module_005ba9f0 (0x005ba9f0): object definitions (_rwaObjDefOpenModule): the module's three free lists and the NULL object definition */
#include "mfl_tier2.h"

/*005bef40*/ extern void *RwaFreeListCreate(RwUInt32 size, RwUInt32 count, RwUInt32 align, RwUInt32 a4, void *a5, RwUInt32 hint);
/*005beec0*/ extern void RwaFreeListDestroy(void *list);
/*005ba8d0*/ extern char *RwaObjDefCreate(void *a0, void *a1, void *funcs, void *a3, void *a4, RwUInt32 hint);
/*005be7d0*/ extern void *rwa_unique_id_set_name_005be7d0(void *id, char *name);
/*005be810*/ extern void *rwa_unique_id_set_uuid_005be810(void *id, void *uuid);

/*007dad30*/ extern undefined g_data_7dad30;      /* object definitions free list (0x5c) */
/*007dadd8*/ extern undefined g_data_7dadd8;      /* free list of 0x18 byte items */
/*007dadb4*/ extern undefined g_data_7dadb4;      /* free list of 8 byte items */
/*007dad58*/ extern undefined g_data_7dad58;      /* the NULL definition's function table */
/*006497cc*/ extern undefined g_data_6497cc;      /* the NULL definition's UUID */
/*007dadfc*/ extern void *g_data_7dadfc;          /* (registered definitions list head) */
/*007dae00*/ extern int g_data_7dae00;            /* module open */
/*007dae04*/ extern int g_data_7dae04;            /* module opening */
/*00949d5c*/ extern char *g_data_949d5c;          /* the NULL object definition */

#define RWA_HINT 0x40809

/* Returns 1, or 0 with everything created so far destroyed again. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_obj_def_open_module_005ba9f0(void)
{
  g_data_7dadfc = 0;
  g_data_7dae04 = 1;
  if (!RwaFreeListCreate(0x5c, 8, 4, 0, &g_data_7dad30, RWA_HINT))
    goto fail;
  if (!RwaFreeListCreate(0x18, 8, 4, 0, &g_data_7dadd8, RWA_HINT))
    goto fail_defs;
  if (!RwaFreeListCreate(8, 8, 4, 0, &g_data_7dadb4, RWA_HINT))
    goto fail_items;
  g_data_7dae00 = 1;
  g_data_949d5c = RwaObjDefCreate(0, 0, &g_data_7dad58, 0, 0, RWA_HINT);
  if (!g_data_949d5c)
    goto fail_small;
  rwa_unique_id_set_name_005be7d0(g_data_949d5c, 0);
  rwa_unique_id_set_uuid_005be810(g_data_949d5c, &g_data_6497cc);
  *(short *)(g_data_949d5c + 0x46) = 1;
  g_data_7dae04 = 0;
  return 1;

fail_small:
  RwaFreeListDestroy(&g_data_7dadb4);
fail_items:
  RwaFreeListDestroy(&g_data_7dadd8);
fail_defs:
  RwaFreeListDestroy(&g_data_7dad30);
fail:
  g_data_7dae00 = 0;
  g_data_7dae04 = 0;
  return 0;
}
