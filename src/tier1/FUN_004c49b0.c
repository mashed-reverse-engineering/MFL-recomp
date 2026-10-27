#include "mfl_tier2.h"
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1da0*/ extern struct RwCamera * RwCameraSetProjection(struct RwCamera *p0, enum RwCameraProjection p1);
/*004d1ec0*/ extern struct RwCamera * RwCameraCreate();
/*004d5b90*/ extern struct RwTexture * RwTextureCreate(struct RwRaster *p0);
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);
/*004d7950*/ extern struct RwRaster * RwRasterCreate(RwInt32 p0, RwInt32 p1, RwInt32 p2, RwInt32 p3);
#define FRAMEBUFFER_RASTER_FLAGS (0x505)
#define TEXTURE_FILTER_LINEAR (2)
/* FUN_004c49b0
 * Creates a camera with a colour buffer and a z buffer, plus a texture over the colour buffer
 * (inferred: render target setup). Both buffers are width x height.
 * out_camera, out_texture: receive the new camera and texture (set to 0 first).
 * Returns 1 on success. On failure, everything created so far is destroyed and 0 is returned.
 */
undefined4 FUN_004c49b0(RwInt32 width,RwInt32 height,undefined4 *out_camera,undefined4 *out_texture)
{
  RwRaster *frame_buffer;
  RwRaster *z_buffer;
  RwFrame *frame;
  RwCamera *camera;
  RwTexture *texture;

  *out_camera = 0;
  *out_texture = 0;
  /* Colour buffer first, then depth buffer, frame and camera. Each failure falls through to free the earlier ones. */
  frame_buffer = RwRasterCreate(width,height,0,FRAMEBUFFER_RASTER_FLAGS);
  if (frame_buffer != (RwRaster *)0x0) {
    z_buffer = RwRasterCreate(width,height,0,1);
    if (z_buffer != (RwRaster *)0x0) {
      frame = RwFrameCreate();
      if (frame != (RwFrame *)0x0) {
        camera = RwCameraCreate();
        if (camera != (RwCamera *)0x0) {
          RwCameraSetProjection(camera,rwPERSPECTIVE);
          RwCamera_frameBuffer(camera) = frame_buffer;
          RwCamera_zBuffer(camera) = z_buffer;
          _rwObjectHasFrameSetFrame(camera,frame);
          RwFrameSetIdentity(frame);
          /* Texture over the colour buffer; filter byte set to linear (inferred from the constant name). */
          texture = RwTextureCreate(frame_buffer);
          *(undefined1 *)&RwTexture_filterAddressing(texture) = TEXTURE_FILTER_LINEAR;
          *out_camera = camera;
          *out_texture = texture;
          return 1;
        }
        RwFrameDestroy(frame);
      }
      RwRasterDestroy(z_buffer);
    }
    RwRasterDestroy(frame_buffer);
  }
  return 0;
}
