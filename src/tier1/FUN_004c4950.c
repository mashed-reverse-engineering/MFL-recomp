#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1e80*/ extern void FUN_004d1e80(void *p0);
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);
/* FUN_004c4950
 * Teardown for an RW object (inferred: destructor-style).
 * Detaches and destroys the frame stored at object+0x4, then clears and destroys
 * the rasters at object+0x60 and object+0x64 (inferred), then calls FUN_004d1e80(object).
 * param object: the RW object being torn down (inferred). Returns nothing.
 */
void FUN_004c4950(void *object)
{
  RwFrame *frame;
  RwRaster *raster;

  /* detach and destroy the frame at +0x4 (inferred) */
  frame = *(RwFrame **)((char *)object + 4);
  if (frame != (RwFrame *)0x0) {
    _rwObjectHasFrameSetFrame(object,(RwFrame *)0x0);
    RwFrameDestroy(frame);
  }
  /* clear and destroy the raster at +0x60 (inferred) */
  raster = *(RwRaster **)((char *)object + 0x60);
  if (raster != (RwRaster *)0x0) {
    *(undefined4 *)((char *)object + 0x60) = 0;
    RwRasterDestroy(raster);
  }
  /* clear and destroy the raster at +0x64 (inferred) */
  raster = *(RwRaster **)((char *)object + 100);
  if (raster != (RwRaster *)0x0) {
    *(undefined4 *)((char *)object + 100) = 0;
    RwRasterDestroy(raster);
  }
  FUN_004d1e80(object);
  return;
}
