#include "mfl_tier2.h"
extern int RwEngineInstance;
extern float float_1_0;
/*004d7770*/ extern void FUN_004d7770();

#define WHITE_RGBA (0xffffffff)
#define ALPHA_OPAQUE (0xff)
#define RWENGINE_SET_RENDER_STATE_FN (0x20) /* vtable slot (inferred) */
#define RWENGINE_DRAW_TRIANGLE_FN (0x2c) /* vtable slot (inferred) */
#define RS_TEXTURE_RASTER (1) /* RwRenderState id (inferred) */
#define RS_SHADE_MODE (7) /* RwRenderState id (inferred) */
#define RS_VERTEX_ALPHA_ENABLE (0xc) /* RwRenderState id (inferred) */
#define QUAD_VERTEX_COUNT (4)

/*004c5860*/
/* TIER2: unvalidated 2026-10-12 authority=diffemu */
/*
 * Draws one textured quad (two triangles) through the RenderWare engine's immediate-mode path.
 *   camera:   RwCamera-like object; 0 uses the engine's default camera (inferred).
 *   rect_min, rect_max: quad corners as x,y fractions of the raster size (0..1) (inferred).
 *   rgba:     colour bytes r,g,b,a; alpha != 0xff turns on vertex alpha (inferred).
 *   texture:  raster bound to the texture stage.
 *   uv_min, uv_max: texture coordinates of the quad corners.
 * Any NULL argument falls back to its default. Render states are set, used, then restored.
 * Returns nothing.
 */
/* TIER2: validated 2026-10-22 authority=diffemu */
void FUN_004c5860(char *camera, float *rect_min, float *rect_max, unsigned char *rgba, int texture, int *uv_min, int *uv_max)
{
  int quad_buf[41];
  float inv_scale;
  char *raster;
  unsigned int packed_rgba;

  quad_buf[0] = (int)WHITE_RGBA;
  quad_buf[5] = 0;
  quad_buf[6] = 0;
  quad_buf[7] = 0;
  quad_buf[8] = 0;
  *(float *)&quad_buf[9] = 1.0f;
  *(float *)&quad_buf[10] = 1.0f;
  *(float *)&quad_buf[11] = 1.0f;
  *(float *)&quad_buf[12] = 1.0f;
  /* Defaults: rect 0..1 in both axes, uv 0..1, colour opaque white. */
  if (camera == 0) {
    camera = *(int *)RwEngineInstance;
  }
  if (rect_min == (float *)0) {
    rect_min = (float *)&quad_buf[5];
  }
  if (rect_max == (float *)0) {
    rect_max = (float *)&quad_buf[11];
  }
  if (rgba == (unsigned char *)0) {
    rgba = (unsigned char *)&quad_buf[0];
  }
  if (uv_min == (int *)0) {
    uv_min = &quad_buf[7];
  }
  if (uv_max == (int *)0) {
    uv_max = &quad_buf[9];
  }
  /* Raster size from the camera's raster; engine call fills quad_buf[3..4] (inferred). */
  raster = *(char **)(camera + 0x60);
  *(float *)&quad_buf[1] = (float)*(int *)(raster + 0xc);
  *(float *)&quad_buf[2] = (float)*(int *)(raster + 0x10);
  FUN_004d7770(raster, &quad_buf[3], &quad_buf[4]);
  inv_scale = float_1_0 / *(float *)(camera + 0x80);
  /* Four vertices, 7 ints each: x, y, z, rhw, colour, u, v. Order: min-min, min-max, max-max, max-min. */
  *(float *)&quad_buf[13] = *(float *)&quad_buf[1] * rect_min[0];
  *(float *)&quad_buf[14] = *(float *)&quad_buf[2] * rect_min[1];
  quad_buf[18] = uv_min[0];
  *(float *)&quad_buf[16] = inv_scale;
  quad_buf[15] = *(int *)(RwEngineInstance + 0x18);
  quad_buf[19] = uv_min[1];
  *(float *)&quad_buf[20] = *(float *)&quad_buf[1] * rect_min[0];
  *(float *)&quad_buf[21] = *(float *)&quad_buf[2] * rect_max[1];
  quad_buf[22] = *(int *)(RwEngineInstance + 0x18);
  *(float *)&quad_buf[23] = inv_scale;
  quad_buf[26] = uv_max[1];
  *(float *)&quad_buf[27] = *(float *)&quad_buf[1] * rect_max[0];
  *(float *)&quad_buf[28] = *(float *)&quad_buf[2] * rect_max[1];
  quad_buf[32] = uv_max[0];
  *(float *)&quad_buf[30] = inv_scale;
  quad_buf[29] = *(int *)(RwEngineInstance + 0x18);
  *(float *)&quad_buf[34] = *(float *)&quad_buf[1] * rect_max[0];
  quad_buf[33] = uv_max[1];
  *(float *)&quad_buf[35] = *(float *)&quad_buf[2] * rect_min[1];
  quad_buf[36] = *(int *)(RwEngineInstance + 0x18);
  *(float *)&quad_buf[37] = inv_scale;
  quad_buf[40] = uv_min[1];
  quad_buf[39] = uv_max[0];
  quad_buf[25] = uv_min[0];
  /* Pack rgba bytes into one 0xAARRGGBB colour, shared by all four vertices. */
  packed_rgba = ((unsigned int)rgba[3] << 24) | ((unsigned int)rgba[0] << 16) | ((unsigned int)rgba[1] << 8) | (unsigned int)rgba[2];
  quad_buf[17] = (int)packed_rgba;
  quad_buf[24] = (int)packed_rgba;
  quad_buf[31] = (int)packed_rgba;
  quad_buf[38] = (int)packed_rgba;
  /* Flat shading and the texture on; vertex alpha only when the colour is not opaque. */
  (*(code **)(RwEngineInstance + RWENGINE_SET_RENDER_STATE_FN))(RS_SHADE_MODE, 1);
  (*(code **)(RwEngineInstance + RWENGINE_SET_RENDER_STATE_FN))(RS_TEXTURE_RASTER, texture);
  if (rgba[3] != ALPHA_OPAQUE) {
    (*(code **)(RwEngineInstance + RWENGINE_SET_RENDER_STATE_FN))(RS_VERTEX_ALPHA_ENABLE, 1);
  }
  /* Draw the quad as two triangles: (0,1,2) and (2,3,0). */
  (*(code **)(RwEngineInstance + RWENGINE_DRAW_TRIANGLE_FN))((char *)&quad_buf[13], QUAD_VERTEX_COUNT, 0, 1, 2);
  (*(code **)(RwEngineInstance + RWENGINE_DRAW_TRIANGLE_FN))((char *)&quad_buf[13], QUAD_VERTEX_COUNT, 2, 3, 0);
  /* Restore vertex alpha off and Gouraud shading (inferred). */
  if (rgba[3] != ALPHA_OPAQUE) {
    (*(code **)(RwEngineInstance + RWENGINE_SET_RENDER_STATE_FN))(RS_VERTEX_ALPHA_ENABLE, 0);
  }
  (*(code **)(RwEngineInstance + RWENGINE_SET_RENDER_STATE_FN))(RS_SHADE_MODE, 2);
}
