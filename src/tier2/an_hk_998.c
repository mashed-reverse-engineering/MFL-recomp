#include "mfl_tier2.h"
extern struct TrailSlot g_trails[6];
/*004146f0*/ extern int race_event_has_panel();
/*004223e0*/ extern int vehicle_get_field_18c();
/*00465250*/ extern int material_uv_scroll_update();
/*00475730*/ extern int get_viewport_camera();
/*00484770*/ extern double distance_between_points();
/*0048e3d0*/ extern int rwp_object_get_user_data();
/*004c42f0*/ extern void FUN_004c42f0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd41c*/ extern float float_5_0;
/*005dd61c*/ extern float k_f_0_001;
/*005dd62c*/ extern float k_f_100;
/*005ddafc*/ extern float k_f_0_05;
/*005de198*/ extern float k_f_n100;
/*005de19c*/ extern float k_f_1_0en4;
/*00603920*/ extern undefined4 g_trail_603920;
/*00603924*/ extern undefined4 g_trail_render_603924;
/*00603928*/ extern undefined4 g_trail_render_603928;
/*007ef804*/ extern undefined4 g_tick_number;
extern double cos(double);
extern double sin(double);
/* trail_render */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "renders the trail records (inferred)" */

void trail_render(void)

{
  RwFrame *frame;
  float fVar1;
  TrailSlot *pTVar2;
  int iVar3;
  uint user_data;
  int camera;
  float *pfVar4;
  TrailSlot *pTVar5;
  float *v;
  float10 fVar6;
  struct {
    int local_44;
    undefined1 pad_40[4];
    RwV3d local_3c;
    float local_30;
    float local_2c;
    undefined4 local_28;
    undefined1 pad_24[8];
    float local_1c;
    float local_18;
    float local_14;
    float local_10;
    float local_c;
    float local_8;
    float local_4;
  } frame_obj;
  
  frame_obj.local_44 = 0;
  vehicle_get_field_18c(0);
  iVar3 = race_event_has_panel();
  if (0 < iVar3) {
    pTVar5 = g_trails;
    pfVar4 = &g_trails[0].w14;
    pTVar2 = g_trails;
    do {
      v = &pTVar2->w08;
      frame_obj.local_3c.x = g_trail_603920;
      frame_obj.local_30 = 0.0;
      frame_obj.local_2c = 1.0;
      frame_obj.local_28 = 0;
      frame_obj.local_3c.y = g_trail_render_603924;
      frame_obj.local_3c.z = g_trail_render_603928;
      frame = *(RwFrame **)(pTVar5->w00 + 4);
      if ((pTVar5->active != 0) &&
         (!(*(float *)&pTVar5->w20 <= k_f_1_0en4))) {
        user_data = rwp_object_get_user_data(pTVar5->object_a);
        if (((user_data & 8) == 0) &&
           (((user_data = rwp_object_get_user_data(pTVar5->object_b), (user_data & 8) == 0 &&
             (user_data = rwp_object_get_user_data(pTVar5->object_a), (user_data & 2) == 0)) &&
            (user_data = rwp_object_get_user_data(pTVar5->object_b), (user_data & 2) == 0)))) {
          fVar1 = pTVar5->w0c - pTVar5->w18;
          frame_obj.local_1c = pTVar5->w10 - pTVar5->w1c;
          frame_obj.local_18 = (*v - *pfVar4) * k_f_100 + *v;
          frame_obj.local_14 = k_f_100 * fVar1 + v[1];
          frame_obj.local_3c.x = 4.0;
          frame_obj.local_3c.y = 4.0;
          frame_obj.local_10 = frame_obj.local_1c * k_f_100 + v[2];
          frame_obj.local_c = (*v - *pfVar4) * k_f_n100 + *pfVar4;
          frame_obj.local_8 = fVar1 * k_f_n100 + pfVar4[1];
          frame_obj.local_4 = frame_obj.local_1c * k_f_n100 + pfVar4[2];
          frame_obj.local_3c.z = *(float *)&pTVar5->w20 * k_f_4;
          camera = get_viewport_camera(0);
          fVar6 = (float10)distance_between_points(&frame_obj.local_18,&frame_obj.local_c,*(int *)(camera + 4) + 0x40);
          if ((float10)k_f_0_001 <= fVar6) {
            if ((float10)float_5_0 < fVar6) {
              fVar6 = (float10)float_5_0;
            }
          }
          else {
            fVar6 = (float10)k_f_0_001;
          }
          frame_obj.local_3c.x = (RwReal)((float10)frame_obj.local_3c.x * fVar6 * (float10)k_f_0_5);
          frame_obj.local_3c.y = (RwReal)(fVar6 * (float10)k_f_0_5 * (float10)frame_obj.local_3c.y);
          material_uv_scroll_update(pTVar5->w00,0);
          fVar6 = (float10)cos((float10)(int)g_tick_number * (float10)k_f_0_05);
          frame_obj.local_30 = (float)fVar6;
          fVar6 = (float10)sin((float10)(int)g_tick_number * (float10)k_f_0_05);
          frame_obj.local_2c = (float)fVar6;
          RwFrameTranslate(frame,(RwV3d *)v,rwCOMBINEREPLACE);
          FUN_004c42f0((undefined4)frame,(undefined4)pfVar4,(undefined4)&frame_obj.local_30);
          RwFrameScale(frame,&frame_obj.local_3c,rwCOMBINEPRECONCAT);
        }
        else {
          pTVar5->active = 0;
        }
      }
      frame_obj.local_44 = frame_obj.local_44 + 1;
      pTVar5 = pTVar5 + 1;
      pTVar2 = (TrailSlot *)(v + 9);
      pfVar4 = pfVar4 + 0xb;
    } while (frame_obj.local_44 < iVar3);
  }
  return;
}


