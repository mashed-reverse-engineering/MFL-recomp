/* rwa_obj_open_module_005bb2e0 (0x005bb2e0): objects (_rwaObjOpenModule): the object list, the 0x1c byte item free list and the root object */
#include "mfl_tier2.h"

/*005bef40*/ extern void *RwaFreeListCreate(RwUInt32 size, RwUInt32 count, RwUInt32 align, RwUInt32 a4, void *a5, RwUInt32 hint);
/*005beec0*/ extern void RwaFreeListDestroy(void *list);
/*005bb0a0*/ extern void *FUN_005bb0a0(void *objdef, int b1, int b2, void *params, void (*callback)(void), int b5);      /* RwaObjCreate */
/*005bab00*/ extern void gap_5bab00(void);      /* the root object's callback */

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/*007dae30*/ extern RwaLink g_data_7dae30;        /* the object list */
/*007dae38*/ extern RwUInt32 g_data_7dae38;
/*007dae3c*/ extern undefined g_data_7dae3c;      /* free list of 0x1c byte items */
/*007dae60*/ extern void *g_data_7dae60;          /* the root object */
/*007dae64*/ extern int g_data_7dae64;            /* module open */
/*00949d5c*/ extern void *g_data_949d5c;          /* the NULL object definition */

#define RWA_HINT 0x4080b

/* Returns 1, 0 when the free list or the root object cannot be created. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_obj_open_module_005bb2e0(void)
{
  g_data_7dae30.next = &g_data_7dae30;
  g_data_7dae30.prev = &g_data_7dae30;
  g_data_7dae38 = 0;
  if (RwaFreeListCreate(0x1c, 0x10, 4, 0, &g_data_7dae3c, RWA_HINT)) {
    g_data_7dae64 = 1;
    g_data_7dae60 = FUN_005bb0a0(g_data_949d5c, 0, 0, 0, gap_5bab00, 0);
    if (g_data_7dae60)
      return 1;
    RwaFreeListDestroy(&g_data_7dae3c);
  }
  g_data_7dae64 = 0;
  return 0;
}
