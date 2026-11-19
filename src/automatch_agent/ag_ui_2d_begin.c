#include "mfl_tier2.h"
/*00562d20*/ extern int Rt2dCTMSetIdentity();
/*00562ef0*/ extern int Rt2dDeviceGetClippath();
/*00563030*/ extern int Rt2dDeviceSetCamera();
/*005632e0*/ extern int Rt2dCTMPush();
/*005642c0*/ extern int Rt2dPathGetBBox();
/*00564450*/ extern int Rt2dPathDestroy();
/*005644c0*/ extern int Rt2dPathCreate();
/*0069b9f8*/ extern undefined4 g_course_69b9f8;


/* ui_2d_begin */

/* "Rt2dDeviceSetCamera + CTM push / identity" */

void ui_2d_begin(undefined4 camera)

{
  undefined4 clip_path;
  
  Rt2dDeviceSetCamera(camera);
  Rt2dCTMPush();
  Rt2dCTMSetIdentity();
  clip_path = Rt2dPathCreate();
  Rt2dDeviceGetClippath(clip_path);
  Rt2dPathGetBBox(clip_path,&g_course_69b9f8);
  Rt2dPathDestroy(clip_path);
  Rt2dCTMPop();
  ui_2d_layout_rects_init();
  return;
}
