#include "mfl_tier2.h"


/* pool_clear */

/* "clears the pool header and its entries" */

void __cdecl pool_clear(undefined4 *pool)

{
  int entry_index;
  
  *pool = 0;
  entry_index = 0;
  if (0 < (int)pool[1]) {
    do {
      pool_push(pool,0);
      entry_index = entry_index + 1;
    } while (entry_index < (int)pool[1]);
  }
  *pool = 0;
  return;
}
