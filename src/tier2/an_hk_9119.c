#include "mfl_tier2.h"
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*00475730*/ extern int get_viewport_camera();
/*004d78f0*/ extern struct RwRaster * RwRasterSubRaster(struct RwRaster *p0, struct RwRaster *p1, struct RwRect *p2);
/*0069d2c8*/ extern undefined4 g_tutorial_69d2c8;
/*0069d69c*/ extern char *g_ui_69d69c;
/*008c8730*/ extern undefined4 g_ui_8c8730;
/*008c8734*/ extern undefined4 g_ui_camera_8c8734;
/*008c8738*/ extern undefined4 g_ui_camera_8c8738;
/*008c873c*/ extern undefined4 g_ui_camera_8c873c;
/* ui_camera_update_view_rect */

/* "computes the viewport rectangle of the secondary frontend raw_val from the screen size" */

/* Computes the 640x480-relative viewport sub-rect of the secondary frontend camera from the screen size and sub-rasters its two rasters (+0x60, +100); returns 0 if the width is non-positive, else 1 (inferred). */
undefined4 ui_camera_update_view_rect(void)

{
  short screen_dim;
  int clamped_x;
  int raw_val;
  int view_w;
  RwRect *rect_ptr;
  RwRect view_rect;
  
  screen_dim = screen_width();
  raw_val = ((int)screen_dim * ((int)g_tutorial_69d2c8 - 0x127)) / 0x280 + 2;
  clamped_x = raw_val;
  view_w = 0;
  if (raw_val < 0) {
    clamped_x = 0;
    view_w = raw_val;
  }
  view_rect.x = clamped_x;
  screen_dim = screen_height();
  raw_val = (screen_dim * 100) / 0x1e0 + 2;
  view_rect.y = raw_val;
  screen_dim = screen_width();
  view_w = ((int)screen_dim * (view_w + 0x100)) / 0x280 + -4;
  if (view_w < 1) {
    return 0;
  }
  view_rect.w = view_w;
  screen_dim = screen_height();
  rect_ptr = &view_rect;
  g_ui_camera_8c873c = (screen_dim * 0xdc) / 0x1e0 + -4;
  g_ui_8c8730 = clamped_x;
  g_ui_camera_8c8734 = raw_val;
  g_ui_camera_8c8738 = view_w;
  view_rect.h = g_ui_camera_8c873c;
  raw_val = get_viewport_camera(0xffffffff);
  RwRasterSubRaster(*(RwRaster **)(g_ui_69d69c + 0x60),*(RwRaster **)(raw_val + 0x60),rect_ptr);
  rect_ptr = &view_rect;
  raw_val = get_viewport_camera(0xffffffff);
  RwRasterSubRaster(*(RwRaster **)(g_ui_69d69c + 100),*(RwRaster **)(raw_val + 100),rect_ptr);
  return 1;
}


