/* rwa_soft_mixer_obj_create_005d8110 (0x005d8110): software mixer object create (object definition 0x7dc2d0), third argument 0 */
#include "mfl_tier2.h"

/*005bb0a0*/ extern void *FUN_005bb0a0(void *objdef, int b1, int b2, int b3, int b4, int b5);      /* RwaObjCreate */
/*007dc2d0*/ extern undefined g_data_7dc2d0;      /* the class object definition */

/* Thin constructor: the class object definition first, then the caller's arguments in the order the callee wants them. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_mixer_obj_create_005d8110(int a1, int a2, int a3, int a4)
{
  return FUN_005bb0a0(&g_data_7dc2d0, a1, a2, 0, a3, a4);
}
