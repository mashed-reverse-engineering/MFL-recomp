#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define POOL_HEADER_SIZE (12)
#define POOL_SLOT_SIZE (4)
#define POOL_CAPACITY_MASK (0xffff)
/* pool_create */
/* Allocates a pool header and a slot array of capacity entries (capacity is masked to 16 bits).
 * capacity: number of slots wanted. Returns the new pool, or NULL if either allocation fails.
 * Pool header layout (inferred): [0] = used count, set to 0; [1] = capacity; [2] = slots pointer.
 * The header is then passed to pool_clear before it is returned.
 */
/* "(capacity): allocates a 0xC byte pool header" */
undefined4 * __cdecl pool_create(unsigned short capacity)
{
  undefined4 *pool;
  void *slots;

  pool = RwMalloc(POOL_HEADER_SIZE,0);
  if (pool == (undefined4 *)0x0) {
    return (undefined4 *)0x0;
  }
  slots = RwMalloc((capacity & POOL_CAPACITY_MASK) * POOL_SLOT_SIZE,0);
  pool[2] = slots;
  if (slots == (void *)0x0) {
    return (undefined4 *)0x0;
  }
  pool[1] = capacity & POOL_CAPACITY_MASK;
  *pool = 0;
  pool_clear(pool);
  return pool;
}
