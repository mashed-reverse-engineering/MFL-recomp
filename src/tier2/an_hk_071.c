#include "mfl_tier2.h"
/*004b4ba7*/ extern int entry();
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);

/* byte offset in the owner object of the pointer to the clump table (inferred) */
#define OWNER_CLUMP_TABLE_OFFSET (4)
/* debris_model_get_first_clump */

/* Returns the first clump of a debris model, or 0 if it has none.
 * debris_model: the debris model object.
 * The owner object of the model (rwp_owner_object_get) holds, at OWNER_CLUMP_TABLE_OFFSET,
 * a pointer to a pointer to the first clump. Returns **(owner + OWNER_CLUMP_TABLE_OFFSET), or 0 (inferred).
 */

undefined4 debris_model_get_first_clump(undefined4 debris_model)
{
  char *owner_object;
  undefined4 r = 0;
  owner_object = rwp_owner_object_get(debris_model);
  if ((owner_object != 0) && (*(undefined4 **)(owner_object + OWNER_CLUMP_TABLE_OFFSET) != (undefined4 *)0x0)) {
    r = **(undefined4 **)(owner_object + OWNER_CLUMP_TABLE_OFFSET);
  }
  return r;
}


