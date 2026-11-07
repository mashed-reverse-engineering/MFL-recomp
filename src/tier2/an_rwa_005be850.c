/* rwa_unique_id_free_data_005be850 (0x005be850): unique ids (_rwaUniqueIDFreeData): release the owned uuid (free list, flag 1) and the owned name (heap, flag 2); returns ID */
#include "mfl_tier2.h"

typedef struct RwaUniqueID {
  void *uuid;
  void *name;
  RwUInt32 flags;      /* 1: uuid from the uuid free list, 2: name allocated */
} RwaUniqueID;

/*007dbb68*/ extern undefined g_data_7dbb68;      /* free list of the 16 byte uuids */
/*005bed80*/ extern void FUN_005bed80(void *freelist, void *p);      /* RwaFreeListFree */
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
RwaUniqueID *rwa_unique_id_free_data_005be850(RwaUniqueID *id)
{
  if (id->uuid && (id->flags & 1)) {
    FUN_005bed80(&g_data_7dbb68, id->uuid);
    id->flags &= ~1u;
  }
  if (id->name && (id->flags & 2)) {
    FUN_005bf120(id->name);
    id->flags &= ~2u;
  }
  return id;
}
