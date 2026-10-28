#include "mfl_tier2.h"
/*0042d9e0*/ extern double hud_get_8c8798(void);
/*004c42f0*/ extern void rwframe_look_at(void *p0, void *p1, void *p2);
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*005dd324*/ extern float k_f_1_7;
/*005dd328*/ extern float k_f_0_6;
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd334*/ extern float k_f_0_033333335;
/*0062ac74*/ extern struct RwV3d g_car_62ac74;
/*0064cbc0*/ extern char *g_cup_64cbc0;
/*0064cbc4*/ extern char *g_load_64cbc4;
/*0064cbc8*/ extern char *g_load_64cbc8;
/*0064cbd0*/ extern float g_cup_64cbd0;
/* cup_scene_setup_frames */
/* "positions the cup scene clumps: the trophy frame and a second frame next to it" */
void cup_scene_setup_frames(void)
{
  struct RwFrame *frame;
  struct RwFrame *frame2;
  RwMatrix *parent;
  int t;
  RwV3d pos;
  RwV3d pos2;
  RwV3d scale;

  frame = *(struct RwFrame **)(g_cup_64cbc0 + 4);
  pos.x = -1.5f;
  pos.y = -1.2f;
  pos.z = 3.0f;
  scale.x = 1.5f;
  scale.y = 1.5f;
  scale.z = 1.5f;
  frame2 = *(struct RwFrame **)(g_load_64cbc8 + 4);
  parent = (RwMatrix *)(*(char **)(g_load_64cbc4 + 4) + 0x10);
  t = (int)hud_get_8c8798();
  if (t > 30) {
    t = 30;
  }
  pos.z = (float_1_0 - (float)t * k_f_0_033333335) * k_f_3;
  pos.z = pos.z + k_f_3;
  RwFrameSetIdentity(frame);
  RwFrameTranslate(frame, &pos, rwCOMBINEREPLACE);
  RwFrameRotate(frame, &g_car_62ac74, g_cup_64cbd0, rwCOMBINEPRECONCAT);
  RwFrameScale(frame, &scale, rwCOMBINEPRECONCAT);
  RwFrameTransform(frame, parent, rwCOMBINEPOSTCONCAT);
  RwFrameUpdateObjects(frame);
  pos2 = pos;
  pos2.x = pos.x - float_1_0;
  pos2.y = pos2.y + 0.6f;
  pos2.z = pos2.z - k_f_1_7;
  RwFrameTranslate(frame2, &pos2, rwCOMBINEREPLACE);
  RwFrameTransform(frame2, parent, rwCOMBINEPOSTCONCAT);
  rwframe_look_at(frame2, &RwFrame_modelling(frame).pos, &g_car_62ac74);
}
