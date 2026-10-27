#include "mfl_tier2.h"
/*004978b0*/ extern int fx_pool_destroy();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*00545d00*/ extern RwBool RpAtomicDestroy_00545d00(struct RpAtomic *p0);
/*007519a4*/ extern struct RpAtomic *g_fx_atomic_a;
/*007519a8*/ extern struct RpAtomic *g_fx_atomic_b;
/* fx_atomics_destroy */
/* "destroys the two fx atomics and their frames" */
/*
 * Tears down the two global fx atomics (g_fx_atomic_a, g_fx_atomic_b) and
 * the frame each one is attached to, then releases the fx pool.
 * Takes no parameters and returns nothing.
 */
void fx_atomics_destroy(void)
{
  RwFrame *atomic_a_frame;
  RwFrame *atomic_b_frame;

  /* Grab both parent frames before the atomics are destroyed (inferred:
     the frames are reached through the atomics' parent links). */
  atomic_a_frame = RpAtomic_object_object_parent(g_fx_atomic_a);
  atomic_b_frame = RpAtomic_object_object_parent(g_fx_atomic_b);
  RpAtomicDestroy_00545d00(g_fx_atomic_a);
  RpAtomicDestroy_00545d00(g_fx_atomic_b);
  RwFrameDestroy(atomic_a_frame);
  RwFrameDestroy(atomic_b_frame);
  fx_pool_destroy();
  return;
}
