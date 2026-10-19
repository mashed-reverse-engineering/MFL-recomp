#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1e80*/ extern void FUN_004d1e80(undefined4 p0);
/*004d59f0*/ extern struct RwTexture * RwTextureSetRaster(struct RwTexture *p0, struct RwRaster *p1);
/*004d5bf0*/ extern RwBool RwTextureDestroy(struct RwTexture *p0);
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);

/* byte offsets inside the camera object (inferred from usage) */
#define CAMERA_FRAME_OFFSET (4)
#define CAMERA_RASTER_A_OFFSET (0x60)
#define CAMERA_RASTER_B_OFFSET (100)

/* rwp_camera_destroy_frame */

/*
 * Destroys the render resources owned by a camera object: detaches its frame
 * and destroys it, destroys the two rasters stored in the camera, then calls
 * FUN_004d1e80 on the camera itself (inferred: final camera cleanup).
 * Also destroys `texture` if it is non-NULL, after clearing its raster.
 * Both pointers may be NULL. Always returns 0.
 * "detaches and destroys the frame of the camera object"
 */

undefined4 __cdecl rwp_camera_destroy_frame(void *camera,RwTexture *texture)

{
  RwFrame *frame;

  if (camera != (void *)0x0) {
    /* camera frame: detach from the camera, then destroy it */
    frame = *(RwFrame **)((char *)camera + CAMERA_FRAME_OFFSET);
    if (frame != (RwFrame *)0x0) {
      _rwObjectHasFrameSetFrame(camera,(RwFrame *)0x0);
      RwFrameDestroy(frame);
    }
    /* the two raster slots of the camera (purpose unknown, not in dossier) */
    if (*(RwRaster **)((char *)camera + CAMERA_RASTER_A_OFFSET) != (RwRaster *)0x0) {
      RwRasterDestroy(*(RwRaster **)((char *)camera + CAMERA_RASTER_A_OFFSET));
    }
    if (*(RwRaster **)((char *)camera + CAMERA_RASTER_B_OFFSET) != (RwRaster *)0x0) {
      RwRasterDestroy(*(RwRaster **)((char *)camera + CAMERA_RASTER_B_OFFSET));
    }
    FUN_004d1e80(camera);
  }
  /* texture: drop its raster reference, then destroy the texture */
  if (texture != (RwTexture *)0x0) {
    RwTextureSetRaster(texture,(RwRaster *)0x0);
    RwTextureDestroy(texture);
  }
  return 0;
}


