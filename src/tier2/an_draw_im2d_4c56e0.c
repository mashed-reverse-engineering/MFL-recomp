#include "mfl_tier2.h"
/* The game's 2D debug drawing helper over the RenderWare Im2D layer: a screen-space line.
   Positions are given in unit screen coordinates (0..1) and scaled to the frame buffer size; the camera defaults to
   the current camera, the colour to opaque white.  0x4c56e0..0x4c5b10. */
/*004d7770*/ extern int RwRasterGetOffset(struct RwRaster *raster, short *xOffset, short *yOffset);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
extern float float_1_0;

#define RW_CAMERA_FRAMEBUFFER(camera) (*(char **)((char *)(camera) + 0x60))
#define RW_CAMERA_NEARPLANE(camera) (*(float *)((char *)(camera) + 0x80))   /* 1/near is the Im2D rhw */
#define RW_RASTER_WIDTH(raster) (*(int *)((raster) + 0xc))
#define RW_RASTER_HEIGHT(raster) (*(int *)((raster) + 0x10))
/* A colour given as four bytes r,g,b,a packed to the 0xAARRGGBB vertex colour. */
#define PACK_ARGB(c) ((((((c)[3] << 8) | (c)[0]) << 8 | (c)[1]) << 8) | (c)[2])
#define COLOR_OPAQUE 0xff       /* alpha value that needs no alpha blending */

/* Draws a line from `from` to `to` (unit screen coordinates; defaults (0,0) and (1,1)) in the colour rgba (default white). */
/* TIER2: unvalidated 2026-10-31 authority=static */
void im2d_draw_line(void *camera, float *from, float *to, unsigned char *rgba)
{
  float defaultPoints[4];
  unsigned char white[4];
  float width, height;
  short xOffset, yOffset;
  RwIm2DVertex v[2];

  defaultPoints[0] = 0.0f;
  defaultPoints[1] = 0.0f;
  defaultPoints[2] = 1.0f;
  defaultPoints[3] = 1.0f;
  white[0] = 0xff;
  white[1] = 0xff;
  white[2] = 0xff;
  white[3] = 0xff;
  if (camera == 0) {
    camera = RwCameraGetCurrentCamera();
  }
  if (from == 0) {
    from = &defaultPoints[0];
  }
  if (to == 0) {
    to = &defaultPoints[2];
  }
  if (rgba == 0) {
    rgba = white;
  }
  width = (float)RW_RASTER_WIDTH(RW_CAMERA_FRAMEBUFFER(camera));
  height = (float)RW_RASTER_HEIGHT(RW_CAMERA_FRAMEBUFFER(camera));
  RwRasterGetOffset((struct RwRaster *)RW_CAMERA_FRAMEBUFFER(camera), &xOffset, &yOffset);
  v[0].rhw = float_1_0 / RW_CAMERA_NEARPLANE(camera);
  v[0].x = width * from[0];
  v[0].y = height * from[1];
  v[0].z = RwIm2DGetNearScreenZ();
  v[1].x = width * to[0];
  v[1].y = height * to[1];
  v[1].z = RwIm2DGetNearScreenZ();
  v[1].rhw = v[0].rhw;
  v[0].emissiveColor = PACK_ARGB(rgba);
  v[1].emissiveColor = v[0].emissiveColor;
  RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
  if (rgba[3] != COLOR_OPAQUE) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)1);
  }
  RwIm2DRenderLine(v, 2, 0, 1);
  if (rgba[3] != COLOR_OPAQUE) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)0);
  }
  RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);
}
