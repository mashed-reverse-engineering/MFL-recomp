#include "mfl_tier2.h"
#define OBJ_VALUE_OFFSET (8)
/* FUN_005bf930
 * Returns the 32-bit value stored at byte offset OBJ_VALUE_OFFSET of `obj`
 * (inferred: the field's meaning is unknown). `obj` is a raw byte pointer to
 * the object. */
undefined4 __cdecl FUN_005bf930(char *obj)
{
  return *(undefined4 *)(obj + OBJ_VALUE_OFFSET);
}
