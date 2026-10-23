#include "mfl_tier2.h"
/*00490db0*/ extern int rwp_objects_destroy_all();
/*00490e80*/ extern int rwp_atomics_destroy();
/*00494300*/ extern int rwp_world_callback_a();
/*00494380*/ extern undefined __cdecl rwp_world_register_callbacks(undefined4 p0, undefined4 p1, void *p2, undefined4 p3, undefined4 p4);
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056b590*/ extern void FUN_0056b590();
/*00701c9c*/ extern undefined4 g_rw_701c9c;
#define RWP_WORLD_CALLBACK_A_ADDR (0x494300) /* address of rwp_world_callback_a (inferred) */
/* rwp_world_flush */

/* "flushes the physics world (callbacks) and rebuilds the object table" */

void rwp_world_flush(void)

{
  undefined4 first_field;
  code *callback_a;
  undefined4 first_field_copy;
  int world;

  /* nothing to flush unless the world global is set */
  if (g_rw_701c9c != 0) {
    world = g_rw_701c9c;
    first_field = rw_first_field(g_rw_701c9c);
    callback_a = rwp_world_callback_a;
    first_field_copy = first_field;
    /* first_field is the first field of the world (inferred from rw_first_field) */
    FUN_0056b590(first_field, RWP_WORLD_CALLBACK_A_ADDR, first_field);
    /* drop the old object table */
    rwp_objects_destroy_all();
    /* re-register the physics callbacks on the world (inferred) */
    rwp_world_register_callbacks(g_rw_701c9c,first_field,callback_a,first_field_copy,world);
    rwp_atomics_destroy();
    return;
  }
  return;
}


