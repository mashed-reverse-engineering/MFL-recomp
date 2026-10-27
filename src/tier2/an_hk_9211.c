#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*007d2128*/ extern char *RwEngineInstance;
/* video_draw_textured_quad */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "screen quad with uv rect and colour using the movie/video texture in the viewport" */

/* (inferred) rect_min / rect_max are fractions of the raster (NULL = whole raster 0,0..1,1), color_rgba NULL = opaque white, u_max / v_max are float bits */
void video_draw_textured_quad(char *viewport,float *rect_min,float *rect_max,undefined1 *color_rgba,void *texture_raster,
               undefined4 u_max,undefined4 v_max)
{
  char *raster;
  undefined1 default_color[4];
  short offset_y_s [2];
  short offset_x_s [2];
  int offset_y_i;
  float raster_w;
  float raster_h;
  float offset_x;
  float offset_y;
  float default_rect [4];
  RwIm2DVertex v[4];
  
  default_rect[0] = 0.0;
  default_rect[1] = 0.0;
  default_rect[2] = 1.0;
  default_rect[3] = 1.0;
  default_color[0] = 0xff;
  default_color[1] = 0xff;
  default_color[2] = 0xff;
  default_color[3] = 0xff;
  if (rect_min == (float *)0x0) {
    rect_min = default_rect;
  }
  if (rect_max == (float *)0x0) {
    rect_max = default_rect + 2;
  }
  if (color_rgba == (undefined1 *)0x0) {
    color_rgba = default_color;
  }
  raster = *(char **)(viewport + 0x60);
  raster_w = (float)*(int *)(raster + 0xc);
  raster_h = (float)*(int *)(raster + 0x10);
  RwRasterGetOffset(raster,offset_x_s,offset_y_s);
  offset_x = (float)(int)offset_x_s[0];
  offset_y_i = (int)offset_y_s[0];
  offset_y = (float)offset_y_i;
  v[0].rhw = float_1_0 / *(float *)(viewport + 0x80);
  v[0].x = raster_w * *rect_min + offset_x;
  v[0].y = raster_h * rect_min[1] + offset_y;
  v[0].v = *(RwReal *)&v_max;
  v[0].z = RwIm2DGetNearScreenZ();
  v[0].u = 0.0;
  v[1].x = v[0].x;
  v[1].y = raster_h * rect_max[1] + offset_y;
  v[1].z = RwIm2DGetNearScreenZ();
  v[1].rhw = v[0].rhw;
  v[1].u = 0.0;
  v[1].v = 0.0;
  v[2].x = raster_w * *rect_max + offset_x;
  v[2].y = v[1].y;
  v[2].z = RwIm2DGetNearScreenZ();
  v[2].rhw = v[0].rhw;
  v[2].u = *(RwReal *)&u_max;
  v[2].v = 0.0;
  v[3].x = v[2].x;
  v[3].y = v[0].y;
  v[3].z = RwIm2DGetNearScreenZ();
  v[3].rhw = v[0].rhw;
  v[3].v = *(RwReal *)&v_max;
  v[3].u = *(RwReal *)&u_max;
  v[0].emissiveColor = CONCAT31(CONCAT21(CONCAT11(color_rgba[3],*color_rgba),color_rgba[1]),color_rgba[2]);
  v[1].emissiveColor = v[0].emissiveColor;
  v[2].emissiveColor = v[0].emissiveColor;
  v[3].emissiveColor = v[0].emissiveColor;
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER,texture_raster);
  if (color_rgba[3] != 0xff) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  }
  RwIm2DRenderTriangle(v,4,0,1,2);
  RwIm2DRenderTriangle(v,4,2,3,0);
  if (color_rgba[3] != 0xff) {
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x0);
  }
  RwRenderStateSet(rwRENDERSTATESHADEMODE,(void *)0x2);
  return;
}
