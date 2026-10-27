#include "mfl_tier2.h"
/*005c4340*/ extern int __cdecl FUN_005c4340(void *a1, int a2, int a3);
/* FUN_005c9380
 * Initialises the record at obj: stores id at offset 0, kind (16-bit,
 * inferred) at offset 4, zeroes the 16-bit fields at offsets 6, 8 and 10, then
 * calls FUN_005c4340 on the sub-block at offset 12 (inferred to be an init call).
 * Returns obj on success, NULL if that call returned 0 (inferred). */
uint __cdecl FUN_005c9380(undefined4 *obj,undefined4 id,undefined4 kind)
{
  int init_result;

  *obj = id;
  *(undefined2 *)(obj + 1) = (undefined2)kind;
  *(undefined2 *)((char *)obj + 6) = 0;
  *(undefined2 *)(obj + 2) = 0;
  *(undefined2 *)((char *)obj + 10) = 0;
  init_result = FUN_005c4340(obj + 3,1,1);
  return init_result != 0 ? (uint)obj : 0;
}
