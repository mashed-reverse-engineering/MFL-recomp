/* rwa_unique_id_set_uuid_005be810 (0x005be810): _rwaUniqueIDFreeUUID, drop the owned uuid of a unique id and store a new one */
#include "mfl_tier2.h"

/*005bed80*/ extern void FUN_005bed80(void *freelist, void *p);      /* RwaFreeListFree */
/*007dbb68*/ extern undefined g_data_7dbb68;      /* free list of the 16 byte uuids */

/* Unique id = { uuid, name, flags }; flags bit 0 = the uuid is owned.  The new uuid is borrowed (bit 0 cleared). */
typedef struct RwaUniqueID {
  void *uuid;
  char *name;
  unsigned flags;
} RwaUniqueID;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaUniqueID *rwa_unique_id_set_uuid_005be810(RwaUniqueID *id, void *uuid)
{
  if (id->uuid != 0 && (id->flags & 1)) {
    FUN_005bed80(&g_data_7dbb68, id->uuid);
    id->flags &= ~1u;
  }
  id->uuid = uuid;
  id->flags &= ~1u;
  return id;
}
