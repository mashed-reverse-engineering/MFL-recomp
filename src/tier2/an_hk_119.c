#include "mfl_tier2.h"

/* offsets into obj / linked object, no struct member known for them (inferred from this function only) */
#define OBJ_VALUE_OFFSET (0x3c)
#define OBJ_FLAGS_OFFSET (0x78)
#define OBJ_LINK_OFFSET (0x11c)
#define LINK_VALUE_OFFSET (0x34)
/* bit in the flag byte: propagate the value to the linked object */
#define OBJ_FLAG_PROPAGATE (8)

/* FUN_005cc2e0 */

/* Stores value in obj's field at OBJ_VALUE_OFFSET. If obj's flag byte at OBJ_FLAGS_OFFSET has
 * OBJ_FLAG_PROPAGATE set, the same value is also stored in the object linked from obj at
 * OBJ_LINK_OFFSET (its field at LINK_VALUE_OFFSET).
 * obj: object to update (char * because the layout has no definition in mfl_tier2.h).
 * value: 32-bit value to store (inferred: a plain 4-byte field, meaning not known). */
void FUN_005cc2e0(char *obj,undefined4 value)

{
  *(undefined4 *)(obj + OBJ_VALUE_OFFSET) = value;
  /* flag set: also propagate value to the linked object */
  if ((*(byte *)(obj + OBJ_FLAGS_OFFSET) & OBJ_FLAG_PROPAGATE) != 0) {
    *(undefined4 *)(*(char **)(obj + OBJ_LINK_OFFSET) + LINK_VALUE_OFFSET) = value;
  }
  return;
}


