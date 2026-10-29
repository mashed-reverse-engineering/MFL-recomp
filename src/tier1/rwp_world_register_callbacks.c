#include "mfl_tier2.h"
/*00494300*/ extern void rwp_world_callback_a(void);
/*00494320*/ extern void rwp_world_register_callbacks_cb_494320(void);
/*00494360*/ extern void rwp_world_register_callbacks_cb_494360(void);
/*005543d0*/ extern void *rw_first_field(int world);
/*0056b550*/ extern void __cdecl FUN_0056b550(void *a, void *cb, void *b);
/*0056b590*/ extern void __cdecl FUN_0056b590(void *a, void *cb, void *b);
/*0056e400*/ extern void __cdecl FUN_0056e400(int world, void *cb, int world2);
/* rwp_world_register_callbacks */

/* Registers the three physics callbacks of a world on its first field. */
void __cdecl rwp_world_register_callbacks(int world)
{
  void *f = rw_first_field(world);
  FUN_0056b590(f, rwp_world_callback_a, f);
  FUN_0056b550(f, rwp_world_register_callbacks_cb_494320, f);
  FUN_0056e400(world, rwp_world_register_callbacks_cb_494360, world);
}
