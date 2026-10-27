#include "mfl_tier2.h"
/*004b322b*/ extern __declspec(noreturn) void __cdecl exit(int);
#define POOL_CORRUPT_EXIT_CODE (0xffffffd6)
/* vec3_make_from_param */
/* "(src, x, out): out = {x, src+4, 0}" */
/* pool_list_unlink */
/* "unlinks the node with key param_2 from the pool's list" */
/* pool_push */
/* "(pool, value): appends to the pool, returns 0 when full" */
/* Appends value to the end of the pool. Pool layout (inferred from the accesses): pool[0] is the
   number of entries in use, pool[1] the capacity, pool[2] the pointer to the dword entries.
   Returns 1 when stored, 0 when the pool is full. */
undefined4 pool_push(int *pool,undefined4 value)
{
  int count;

  count = *pool;
  if (!(count < pool[1])) {
    return 0;
  }
  if (count < 0) {
    exit(POOL_CORRUPT_EXIT_CODE);
  }
  *(undefined4 *)(pool[2] + count * 4) = value;
  *pool = *pool + 1;
  return 1;
}
