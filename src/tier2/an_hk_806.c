#include "mfl_tier2.h"
/*00495bc0*/ extern int viewport_raster_destroy();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*00545d00*/ extern RwBool RpAtomicDestroy_00545d00(struct RpAtomic *p0);
/*006a06e0*/ extern struct RpAtomic *g_sky_atomic;
/* course_superg_cb5 */

/*0044a020*/ extern int course_sky_atomic_destroy();
/*
 * Course cleanup for the SuperGrid course (cb5 = callback 5, inferred).
 * Takes no parameters and returns nothing.
 * Destroys the viewport raster, then destroys the sky atomic (g_sky_atomic)
 * through course_sky_atomic_destroy(). The parent frame of the sky atomic is
 * read into sky_frame first, but nothing uses it (inferred: the read is
 * left over from an earlier version of the cleanup).
 */
void course_superg_cb5(void)

{
  RwFrame *sky_frame;

  viewport_raster_destroy(0);
  sky_frame = RpAtomic_object_object_parent(g_sky_atomic);
  course_sky_atomic_destroy();
  return;
}


