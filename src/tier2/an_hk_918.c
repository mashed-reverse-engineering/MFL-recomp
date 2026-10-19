#include "mfl_tier2.h"
/*00430a90*/ extern int ui_set_ui_69cc2c(undefined4 p0);
/*00430aa0*/ extern int ui_set_ui_69cc2e(undefined4 p0);
/*00432ee0*/ extern int ui_camera_create_main();
/*00434b70*/ extern int ui_camera_create_secondary();
/*004756a0*/ extern int aux_camera_setup(undefined4 p0, undefined4 p1);
/*004a8830*/ extern int app_get_app_62c5c8();
/*004a8840*/ extern int app_get_app_62c5cc();
/*004a9420*/ extern int resource_load(int p0, char *p1, undefined4 *p2, undefined4 *p3);
/*004c3eb0*/ extern RwTexDictionary * FUN_004c3eb0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004d3060*/ extern struct RwVideoMode * RwEngineGetVideoModeInfo(struct RwVideoMode *p0, RwInt32 p1);
/*004d3090*/ extern RwInt32 __cdecl RwEngineGetCurrentVideoMode(void);
/*004d5d90*/ extern struct RwTexture * RwTexDictionaryFindNamedTexture(struct RwTexDictionary *p0, RwChar *p1);
/*0076e7e4*/ extern RwTexDictionary *g_app_76e7e4;
/*0076e7ec*/ extern undefined4 g_car_76e7ec;
/* video_init_main_camera */

/* WARNING: Enum "RwVideoModeFlag": Some values do not have unique names */
/* "'RWTEXDICTIONARY': video mode info and aux camera setup" */

/* TIER2: validated 2026-10-19 authority=diffemu */
void video_init_main_camera(void)
{
  undefined4 uVar2;
  undefined4 uVar3;
  struct {
    undefined4 size;
    undefined4 data;
  } res;
  RwVideoMode local_18;
  
  RwEngineGetVideoModeInfo(&local_18,(int)(short)RwEngineGetCurrentVideoMode());
  uVar2 = app_get_app_62c5c8();
  uVar3 = app_get_app_62c5cc();
  aux_camera_setup(uVar2,uVar3);
  ui_set_ui_69cc2c(uVar2);
  ui_set_ui_69cc2e(uVar3);
  ui_camera_create_main();
  ui_camera_create_secondary();
  res.size = 0;
  res.data = 0;
  resource_load(0x194,"RWTEXDICTIONARY",&res.data,&res.size);
  g_app_76e7e4 = FUN_004c3eb0(res.data,res.size,0,0);
  if (g_app_76e7e4 != (RwTexDictionary *)0x0) {
    g_car_76e7ec = (undefined4)RwTexDictionaryFindNamedTexture(g_app_76e7e4,"LoadIcon_fully_loaded");
  }
  return;
}
