#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*00545d00*/ extern RwBool RpAtomicDestroy_00545d00(struct RpAtomic *p0);
/*0076df18*/ extern RpAtomic *g_fx_ptank_76df18;
/*0076df2c*/ extern undefined4 g_fx_ptank_76df2c;
/* fx_ptank_atomic_destroy */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "destroys the atomic and frame of _g_data_76df18" */
/* Destroys the particle-tank atomic g_fx_ptank_76df18 and its parent frame, then clears
   g_fx_ptank_76df18 and g_fx_ptank_76df2c. Takes no parameters, returns nothing.
   Does nothing if the atomic is already NULL. */
void fx_ptank_atomic_destroy(void)
{
  RwFrame *frame;

  /* Fetch the frame before destroying the atomic (it is owned by the atomic's object). (inferred) */
  if (g_fx_ptank_76df18 != (RpAtomic *)0x0) {
    frame = RpAtomic_object_object_parent(g_fx_ptank_76df18);
    RpAtomicDestroy_00545d00(g_fx_ptank_76df18);
    RwFrameDestroy(frame);
    g_fx_ptank_76df18 = (RpAtomic *)0x0;
    g_fx_ptank_76df2c = 0;
  }
  return;
}
