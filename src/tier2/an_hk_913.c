#include "mfl_tier2.h"
/*004c65d0*/ extern void __cdecl FUN_004c65d0(void *camera, float *p1, float *p2, undefined4 *p3, void *raster);
/*004d5d90*/ extern struct RwTexture * RwTexDictionaryFindNamedTexture(struct RwTexDictionary *p0, RwChar *p1);
/*005dd624*/ extern float k_f_0_0020833334;
/*005dd7e4*/ extern float k_f_0_03;
/*005de2fc*/ extern float k_f_0_04;
/*005de7c0*/ extern float k_f_0_0015625;
/*005de98c*/ extern undefined2 g_ui_keyboard_5de98c;
/*005de990*/ extern char g_ui_keyboard_5de990[8];
/*005de998*/ extern char s_keyboard_005de998[16];
/*0064d888*/ extern char *g_app_64d888;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007e7740*/ extern struct ControllerCfg g_controller_cfg[4];
/* ui_keyboard_icon_find */
/* compile: /GS */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "RwTexDictionaryFindNamedTexture('keyboard')" */
/* TIER2: validated 2026-10-19 authority=diffemu */
void __cdecl ui_keyboard_icon_find(int controller,float icon_x,float icon_y,undefined4 colour_rgba,int add_suffix)
{
  struct RwTexture *tex;
  void *raster;
  char *p;
  struct {
    float x1;
    float y1;
    float x0;
    float y0;
    char name[16];
  } frame;
  
  frame.name[4] = '\0';
  frame.name[5] = '\0';
  frame.name[6] = '\0';
  frame.name[7] = '\0';
  frame.name[8] = '\0';
  frame.x1 = icon_x * k_f_0_0015625 + k_f_0_04;
  frame.name[9] = '\0';
  frame.name[10] = '\0';
  frame.name[11] = '\0';
  frame.name[12] = '\0';
  raster = (void *)0x0;
  frame.y1 = k_f_0_03 + icon_y * k_f_0_0020833334;
  frame.name[13] = '\0';
  frame.name[14] = '\0';
  frame.name[0] = '\0';
  frame.name[1] = '\0';
  frame.name[2] = '\0';
  frame.name[3] = '\0';
  frame.name[15] = '\0';
  frame.x0 = icon_x * k_f_0_0015625 - k_f_0_04;
  frame.y0 = icon_y * k_f_0_0020833334 - k_f_0_03;
  if (g_controller_cfg[controller].device == 2) {
    *(undefined4 *)(frame.name + 4) = *(undefined4 *)(s_keyboard_005de998 + 4);
    frame.name[8] = s_keyboard_005de998[8];
    *(undefined4 *)frame.name = *(undefined4 *)s_keyboard_005de998;
  }
  else {
    if (g_controller_cfg[controller].device == 1) {
      *(undefined2 *)(frame.name + 4) = *(undefined2 *)(g_ui_keyboard_5de990 + 4);
      frame.name[6] = g_ui_keyboard_5de990[6];
      *(undefined4 *)frame.name = *(undefined4 *)g_ui_keyboard_5de990;
    }
  }
  if (add_suffix != 0) {
    p = frame.name;
    while (*p != '\0') {
      p = p + 1;
    }
    *(undefined2 *)p = g_ui_keyboard_5de98c;
  }
  tex = RwTexDictionaryFindNamedTexture((struct RwTexDictionary *)g_app_64d888,frame.name);
  if (tex != (struct RwTexture *)0x0) {
    raster = *(void **)tex;
  }
  FUN_004c65d0(*(void **)RwEngineInstance,&frame.x0,&frame.x1,&colour_rgba,raster);
  return;
}
