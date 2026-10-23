#include "mfl_tier2.h"
/*004015b0*/ extern undefined __cdecl cup_scene_draw_team_overlay(undefined4 p0, undefined4 p1);
/*004220f0*/ extern int vehicle_get_position_158();
/*00422280*/ extern char *vehicle_part_frame();
/*004757a0*/ extern char *get_viewport_camera_matrix();
/*0047b520*/ extern int car_current_matrix();
/*00485350*/ extern undefined __cdecl matrix_interpolate_rotation_translation(void *p0, void *p1, void *p2, undefined4 p3, undefined4 p4);
/*004c42f0*/ extern void FUN_004c42f0(undefined4 p0, void *p1, void *p2);
/*004c4460*/ extern void FUN_004c4460(void *p0, void *p1, void *p2);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd33c*/ extern float k_f_4;
/*005dd368*/ extern float k_f_0_0075;
/*005dd36c*/ extern float k_f_0_4;
/*005dd370*/ extern float k_f_0_3;
/*005dd374*/ extern float k_f_0_2;
/*005dd378*/ extern float k_f_0_1;
/*005ddc28*/ extern float k_f_0;
/*0062ac74*/ extern float g_car_62ac74;
/*0064cb04*/ extern float g_cup_scene_64cb04;
/*0064cb08*/ extern float g_cup_scene_64cb08;
/*0064cb0c*/ extern float g_cup_scene_64cb0c;
/*0064cb10*/ extern float g_cup_scene_64cb10;
/*0064cb14*/ extern float g_cup_scene_64cb14;
/*0064cbd4*/ extern char *g_cup_64cbd4;
/*0064cbd8*/ extern undefined4 g_cup_64cbd8;
/*007ef7d8*/ extern undefined4 g_camera_7ef7d8;
/*007ef7f4*/ extern float g_tick_seconds;
/*007ef804*/ extern undefined4 g_tick_number;
extern double cos(double);
extern double sin(double);
/* cup_scene_update_object */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per object update using the camera matrix and car matrices (inferred)" */

void cup_scene_update_object(float param_1,int param_2)

{
  RwMatrix *pRVar1;
  char *iVar2;
  uint uVar3;
  bool bVar4;
  double fVar5;
  double fVar6;
  undefined *puVar7;
  struct {
    RwV3d local_b0;
    float *local_a4;
    float local_a0;
    float local_9c;
    float local_98;
    float local_94;
    undefined1 pad_90[4];
    RwV3d local_8c;
    RwMatrix local_80;
    RwMatrix local_40;
  } frame;
  
  frame.local_a4 = (float *)0x0;
  frame.local_b0.x = 0.0;
  frame.local_b0.y = 0.0;
  frame.local_b0.z = 0.0;
  if (param_2 != -1) {
    g_cup_scene_64cb14 = param_1;
    g_cup_scene_64cb08 = 1.25;
    g_cup_scene_64cb0c = 2.0;
    pRVar1 = (RwMatrix *)get_viewport_camera_matrix(0);
    vehicle_get_position_158(param_2,&frame.local_9c);
    car_current_matrix(&frame.local_a4,param_2);
    frame.local_8c.x = frame.local_a4[0xc] - (pRVar1->pos).x;
    frame.local_8c.y = frame.local_a4[0xd] - (pRVar1->pos).y;
    frame.local_8c.z = frame.local_a4[0xe] - (pRVar1->pos).z;
    RwV3dNormalize(&frame.local_8c,&frame.local_8c);
    if ((g_cup_scene_64cb14 < k_f_0_1) &&
       (g_cup_scene_64cb10 = -1.0,
       frame.local_8c.z * frame.local_a4[2] + frame.local_8c.x * *frame.local_a4 + frame.local_8c.y * frame.local_a4[1] < k_f_0)) {
      g_cup_scene_64cb10 = 1.0;
    }
    if (g_cup_scene_64cb14 == k_f_0) {
      g_cup_scene_64cb04 = 0.3926991;
    }
    fVar5 = (double)sin((double)g_cup_scene_64cb04);
    fVar5 = fVar5 * (double)g_cup_scene_64cb10 * (double)g_cup_scene_64cb08;
    fVar6 = (double)cos((double)g_cup_scene_64cb04);
    frame.local_a0 = (float)(fVar6 * (double)g_cup_scene_64cb08);
    g_cup_scene_64cb04 =
         g_cup_scene_64cb04 - g_tick_seconds * g_cup_scene_64cb0c * g_cup_scene_64cb14 * k_f_0_2;
    frame.local_b0.x = frame.local_a4[4] * k_f_0_3 +
                 (float)((double)frame.local_9c +
                        (double)frame.local_a4[8] * (double)frame.local_a0 + fVar5 * (double)*frame.local_a4);
    frame.local_b0.y = frame.local_a4[5] * k_f_0_3 +
                 frame.local_98 + frame.local_a4[9] * frame.local_a0 + (float)(fVar5 * (double)frame.local_a4[1]);
    frame.local_b0.z = frame.local_a4[6] * k_f_0_3 +
                 frame.local_94 + frame.local_a4[10] * frame.local_a0 + (float)(fVar5 * (double)frame.local_a4[2]);
    frame.local_9c = frame.local_a4[4] * k_f_0_4 + frame.local_9c;
    frame.local_98 = frame.local_a4[5] * k_f_0_4 + frame.local_98;
    frame.local_94 = frame.local_a4[6] * k_f_0_4 + frame.local_94;
    RwMatrixTransform(&frame.local_80,pRVar1,rwCOMBINEREPLACE);
    FUN_004c4460(&frame.local_80,&frame.local_9c,frame.local_a4 + 4);
    frame.local_80.pos.x = frame.local_b0.x;
    frame.local_80.pos.y = frame.local_b0.y;
    frame.local_80.pos.z = frame.local_b0.z;
    RwMatrixUpdate(&frame.local_80);
    matrix_interpolate_rotation_translation(&frame.local_40, pRVar1, &frame.local_80, *(int *)&g_cup_scene_64cb14, *(int *)&g_cup_scene_64cb14);
    RwMatrixTransform(pRVar1,&frame.local_40,rwCOMBINEREPLACE);
    uVar3 = g_tick_number & 0x8000007f;
    bVar4 = uVar3 == 0;
    if ((int)uVar3 < 0) {
      bVar4 = (uVar3 - 1 | 0xffffff80) == 0xffffffff;
    }
    if (bVar4) {
      cup_scene_draw_team_overlay(*(int *)&param_1,param_2);
    }
    iVar2 = vehicle_part_frame(0,0x13);
    frame.local_b0.x = 1.0;
    frame.local_b0.y = 1.0;
    frame.local_b0.z = 0.0;
    pRVar1 = RwFrameGetLTM(*(RwFrame **)(iVar2 + 4));
    RwFrameRotate((struct RwFrame *)g_cup_64cbd4,(struct RwV3d *)&frame.local_b0,(float)(int)g_camera_7ef7d8 * k_f_0_0075,rwCOMBINEREPLACE);
    RwFrameTranslate((struct RwFrame *)g_cup_64cbd4,(struct RwV3d *)&pRVar1->pos,rwCOMBINEPOSTCONCAT);
    /* car position + lift, each partial sum rounded to 32 bit like the original's stores */
    frame.local_b0.x = frame.local_a4[0xc];
    frame.local_b0.y = frame.local_a4[0xd];
    frame.local_b0.z = frame.local_a4[0xe];
    frame.local_b0.y = frame.local_b0.y + k_f_4;
    frame.local_b0.x = frame.local_a4[8] + frame.local_b0.x;
    frame.local_b0.y = frame.local_a4[9] + frame.local_b0.y;
    frame.local_b0.z = frame.local_a4[10] + frame.local_b0.z;
    frame.local_b0.x = frame.local_b0.x + *frame.local_a4;
    frame.local_b0.y = frame.local_a4[1] + frame.local_b0.y;
    frame.local_b0.z = frame.local_a4[2] + frame.local_b0.z;
    RwFrameTranslate((struct RwFrame *)g_cup_64cbd8,(struct RwV3d *)&frame.local_b0,rwCOMBINEREPLACE);
    puVar7 = (undefined *)&g_car_62ac74;
    pRVar1 = RwFrameGetLTM((struct RwFrame *)g_cup_64cbd4);
    FUN_004c42f0(g_cup_64cbd8,&pRVar1->pos,puVar7);
  }
  return;
}


