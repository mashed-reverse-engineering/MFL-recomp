/* rwa_unique_id_set_name_005be7d0 (0x005be7d0): _rwaUniqueIDFreeName, drop the owned name of a unique id and store a new one */
#include "mfl_tier2.h"

/*005bf120*/ extern void FUN_005bf120(void *p);      /* RWA free */

/* Unique id = { uuid, name, flags }; flags bit 0 = the uuid is owned, bit 1 = the name is owned.  The new name is
   borrowed (bit 1 cleared). */
typedef struct RwaUniqueID {
  void *uuid;
  char *name;
  unsigned flags;
} RwaUniqueID;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaUniqueID *rwa_unique_id_set_name_005be7d0(RwaUniqueID *id, char *name)
{
  if (id->name != 0 && (id->flags & 2)) {
    FUN_005bf120(id->name);
    id->flags &= ~2u;
  }
  id->name = name;
  id->flags &= ~2u;
  return id;
}
