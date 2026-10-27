#include "mfl_tier2.h"
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1bd0*/ extern struct RwCamera * RwCameraSetNearClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1da0*/ extern struct RwCamera * RwCameraSetProjection(struct RwCamera *p0, enum RwCameraProjection p1);
/*004d1ec0*/ extern struct RwCamera * RwCameraCreate();
/*004d5b90*/ extern struct RwTexture * RwTextureCreate(struct RwRaster *p0);
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);
/*004d7950*/ extern struct RwRaster * RwRasterCreate(RwInt32 p0, RwInt32 p1, RwInt32 p2, RwInt32 p3);
#define RASTER_TYPE_CAMERA_TEXTURE (5) /* RwRasterCreate type, OR'd with the caller's flags (inferred) */
#define TEXTURE_FILTER_LINEAR (2) /* low byte of filterAddressing (inferred) */
#define CAMERA_NEAR_PLANE (0.5)
#define CAMERA_FAR_PLANE (300.0)
/* rwp_raster_pair_create */
/*
 * Creates a perspective camera whose frame buffer is a colour raster that is
 * also usable as a texture, plus a Z-buffer raster, both of the same size.
 *   width, height : size in pixels of both rasters
 *   raster_flags  : extra flags OR'd into the colour raster's type
 *   texture_out   : receives the texture wrapping the colour raster
 * Returns the camera, or NULL on failure (every partial allocation is freed).
 */
RwCamera * __cdecl rwp_raster_pair_create(undefined4 *texture_out,RwInt32 width,RwInt32 height,uint raster_flags)
{
  RwRaster *color_raster;
  RwRaster *depth_raster;
  RwFrame *frame;
  RwCamera *camera;
  RwTexture *texture;

  /* colour raster first; the rest only runs if each allocation succeeded */
  color_raster = RwRasterCreate(width,height,0,raster_flags | RASTER_TYPE_CAMERA_TEXTURE);
  if (color_raster != (RwRaster *)0x0) {
    /* Z-buffer raster (type 1) */
    depth_raster = RwRasterCreate(width,height,0,1);
    if (depth_raster != (RwRaster *)0x0) {
      frame = RwFrameCreate();
      if (frame != (RwFrame *)0x0) {
        camera = RwCameraCreate();
        if (camera != (RwCamera *)0x0) {
          /* attach rasters and frame, then set projection and clip planes */
          RwCameraSetProjection(camera,rwPERSPECTIVE);
          RwCamera_frameBuffer(camera) = color_raster;
          RwCamera_zBuffer(camera) = depth_raster;
          _rwObjectHasFrameSetFrame(camera,frame);
          RwFrameSetIdentity(frame);
          RwCameraSetNearClipPlane(camera,CAMERA_NEAR_PLANE);
          RwCameraSetFarClipPlane(camera,CAMERA_FAR_PLANE);
          /* wrap the colour raster as a texture for the caller */
          texture = RwTextureCreate(color_raster);
          *(undefined1 *)&RwTexture_filterAddressing(texture) = TEXTURE_FILTER_LINEAR;
          *texture_out = texture;
          return camera;
        }
        /* failure path: unwind allocations in reverse order */
        RwFrameDestroy(frame);
      }
      RwRasterDestroy(depth_raster);
    }
    RwRasterDestroy(color_raster);
  }
  return (RwCamera *)0x0;
}
