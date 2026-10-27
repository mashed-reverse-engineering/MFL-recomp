#include "mfl_tier2.h"
/*004a5d70*/ extern int object_render();
/*006fa7d8*/ extern undefined4 g_rw_6fa7d8;
/*00701c94*/ extern int g_rw_701c94;


/* rwp_atomics_render */

/* Render all atomics in the global list. */

void rwp_atomics_render(void)
{
  int atomic_index;
  
  atomic_index = 0;
  if (0 < g_rw_701c94) {
    do {
      object_render((&g_rw_6fa7d8)[atomic_index]);
      atomic_index = atomic_index + 1;
    } while (atomic_index < g_rw_701c94);
  }
  return;
}
