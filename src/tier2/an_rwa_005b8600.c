/* rwa_stream_create_005b8600 (0x005b8600): RwaStreamCreate */
#include "mfl_tier2.h"

/*005bb0a0*/ extern void *FUN_005bb0a0(void *objdef, int b1, int b2, int b3, int b4, int b5);      /* RwaObjCreate */
/*007dac48*/ extern undefined g_data_7dac48;      /* the class object definition */

/* Thin constructor: the class object definition first, then the caller's arguments in the order the callee wants them. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_stream_create_005b8600(int a1, int a2, int a3, int a4, int a5)
{
  return FUN_005bb0a0(&g_data_7dac48, a1, a3, a2, a4, a5);
}
