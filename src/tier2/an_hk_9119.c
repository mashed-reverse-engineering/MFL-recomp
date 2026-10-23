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

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "computes the viewport rectangle of the secondary frontend camera from the screen size" */

undefined4 ui_camera_update_view_rect(void)

{
  short sVar1;
  int iVar2;
  int camera;
  int iVar3;
  RwRect *pRVar4;
  RwRect local_10;
  
  sVar1 = screen_width();
  camera = ((int)sVar1 * ((int)g_tutorial_69d2c8 - 0x127)) / 0x280 + 2;
  iVar2 = camera;
  iVar3 = 0;
  if (camera < 0) {
    iVar2 = 0;
    iVar3 = camera;
  }
  local_10.x = iVar2;
  sVar1 = screen_height();
  camera = (sVar1 * 100) / 0x1e0 + 2;
  local_10.y = camera;
  sVar1 = screen_width();
  iVar3 = ((int)sVar1 * (iVar3 + 0x100)) / 0x280 + -4;
  if (iVar3 < 1) {
    return 0;
  }
  local_10.w = iVar3;
  sVar1 = screen_height();
  pRVar4 = &local_10;
  g_ui_camera_8c873c = (sVar1 * 0xdc) / 0x1e0 + -4;
  g_ui_8c8730 = iVar2;
  g_ui_camera_8c8734 = camera;
  g_ui_camera_8c8738 = iVar3;
  local_10.h = g_ui_camera_8c873c;
  camera = get_viewport_camera(0xffffffff);
  RwRasterSubRaster(*(RwRaster **)(g_ui_69d69c + 0x60),*(RwRaster **)(camera + 0x60),pRVar4);
  pRVar4 = &local_10;
  camera = get_viewport_camera(0xffffffff);
  RwRasterSubRaster(*(RwRaster **)(g_ui_69d69c + 100),*(RwRaster **)(camera + 100),pRVar4);
  return 1;
}


