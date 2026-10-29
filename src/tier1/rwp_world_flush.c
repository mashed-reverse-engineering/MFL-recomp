#include "mfl_tier2.h"
/*00490db0*/ extern void rwp_objects_destroy_all(void);
/*00490e80*/ extern void rwp_atomics_destroy(void);
/*00494300*/ extern void rwp_world_callback_a(void);
/*00494380*/ extern void __cdecl rwp_world_register_callbacks(int world);
/*005543d0*/ extern void *rw_first_field(int world);
/*0056b590*/ extern void __cdecl FUN_0056b590(void *a, void *cb, void *b);
/*00701c9c*/ extern int g_rw_701c9c;
/* Flushes the physics world: re-registers its callbacks and rebuilds the object table. */
void rwp_world_flush(void)
{
  if (g_rw_701c9c) {
    void *f = rw_first_field(g_rw_701c9c);
    FUN_0056b590(f, rwp_world_callback_a, f);
    rwp_objects_destroy_all();
    rwp_world_register_callbacks(g_rw_701c9c);
    rwp_atomics_destroy();
  }
}
