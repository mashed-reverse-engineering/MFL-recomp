#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*00759698*/ extern struct RpAtomic *g_fx_ptank_atomic_a;
/*0075969c*/ extern struct RpAtomic *g_fx_ptank_atomic_b;
/* fx_ptank_destroy */
/* "destroys both PTank atomics and frames"
 * Destroys the two PTank atomics (g_fx_ptank_atomic_a, g_fx_ptank_atomic_b) and the
 * frame each one is attached to (its parent frame). Takes no parameters, returns nothing.
 * Each pair is freed as frame first, then atomic, so the frame is read before the atomic goes.
 */
void fx_ptank_destroy(void)
{
  RwFrameDestroy(RpAtomic_object_object_parent(g_fx_ptank_atomic_a));
  RpAtomicDestroy(g_fx_ptank_atomic_a);
  RwFrameDestroy(RpAtomic_object_object_parent(g_fx_ptank_atomic_b));
  RpAtomicDestroy(g_fx_ptank_atomic_b);
  return;
}
