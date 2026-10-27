#include "mfl_tier2.h"

#define RASTER_DEFAULT_DEPTH 0
#define RASTER_TYPE_ZBUFFER 1
#define RASTER_TYPE_CAMERA 2

/*004755b0*/ extern int aux_frame_destroy();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1ec0*/ extern struct RwCamera * RwCameraCreate();
/*004d7950*/ extern struct RwRaster * RwRasterCreate(RwInt32 p0, RwInt32 p1, RwInt32 p2, RwInt32 p3);


/* ui_camera_create */

/* "(width, height, ...): RwCamera with frame and raster for the frontend" */

/* Creates an RwCamera with an attached frame, framebuffer raster, and optional z-buffer raster. */
RwCamera * ui_camera_create(RwInt32 width,RwInt32 height,int create_z_buffer)
{
  RwCamera *camera;
  RwFrame *frame;
  RwRaster *raster;
  
  /* Create camera object */
  camera = RwCameraCreate();
  if (camera != (RwCamera *)0x0) {
    /* Create and attach frame to camera */
    frame = RwFrameCreate();
    _rwObjectHasFrameSetFrame(camera,frame);
    /* Create framebuffer raster */
    raster = RwRasterCreate(width,height,RASTER_DEFAULT_DEPTH,RASTER_TYPE_CAMERA);
    RwCamera_frameBuffer(camera) = raster;
    if (create_z_buffer != 0) {
      /* Create z-buffer raster */
      raster = RwRasterCreate(width,height,RASTER_DEFAULT_DEPTH,RASTER_TYPE_ZBUFFER);
      RwCamera_zBuffer(camera) = raster;
    }
    /* Verify frame attachment and required rasters were allocated */
    if (((RwCamera_object_object_parent(camera) != (void *)0x0) && (RwCamera_frameBuffer(camera) != (RwRaster *)0x0)
        ) && ((create_z_buffer == 0 || (RwCamera_zBuffer(camera) != (RwRaster *)0x0)))) {
      return camera;
    }
  }
  /* Clean up on failure */
  aux_frame_destroy(camera);
  return (RwCamera *)0x0;
}

