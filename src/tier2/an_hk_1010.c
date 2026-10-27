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

/* "per object update using the camera matrix and car matrices (inferred)" */

void cup_scene_update_object(float blend,int car_index)

{
  RwMatrix *cam_mtx;
  char *part_frame;
  uint tick_masked;
  bool on_period;
  double side_offset;
  double cos_part;
  undefined *car_vel_ptr;
  struct {
    RwV3d target_pos;
    float *car_mtx;
    float fwd_offset;
    float car_pos_x;
    float car_pos_y;
    float car_pos_z;
    undefined1 pad_90[4];
    RwV3d to_car;
    RwMatrix look_mtx;
    RwMatrix blend_mtx;
  } frame;
  
  frame.car_mtx = (float *)0x0;
  frame.target_pos.x = 0.0;
  frame.target_pos.y = 0.0;
  frame.target_pos.z = 0.0;
  if (car_index != -1) {
    g_cup_scene_64cb14 = blend;
    g_cup_scene_64cb08 = 1.25;
    g_cup_scene_64cb0c = 2.0;
    cam_mtx = (RwMatrix *)get_viewport_camera_matrix(0);
    vehicle_get_position_158(car_index,&frame.car_pos_x);
    car_current_matrix(&frame.car_mtx,car_index);
    frame.to_car.x = frame.car_mtx[0xc] - (cam_mtx->pos).x;
    frame.to_car.y = frame.car_mtx[0xd] - (cam_mtx->pos).y;
    frame.to_car.z = frame.car_mtx[0xe] - (cam_mtx->pos).z;
    RwV3dNormalize(&frame.to_car,&frame.to_car);
    if ((g_cup_scene_64cb14 < k_f_0_1) &&
       (g_cup_scene_64cb10 = -1.0,
       frame.to_car.z * frame.car_mtx[2] + frame.to_car.x * *frame.car_mtx + frame.to_car.y * frame.car_mtx[1] < k_f_0)) {
      g_cup_scene_64cb10 = 1.0;
    }
    if (g_cup_scene_64cb14 == k_f_0) {
      g_cup_scene_64cb04 = 0.3926991;
    }
    side_offset = (double)sin((double)g_cup_scene_64cb04);
    side_offset = side_offset * (double)g_cup_scene_64cb10 * (double)g_cup_scene_64cb08;
    cos_part = (double)cos((double)g_cup_scene_64cb04);
    frame.fwd_offset = (float)(cos_part * (double)g_cup_scene_64cb08);
    g_cup_scene_64cb04 =
         g_cup_scene_64cb04 - g_tick_seconds * g_cup_scene_64cb0c * g_cup_scene_64cb14 * k_f_0_2;
    frame.target_pos.x = frame.car_mtx[4] * k_f_0_3 +
                 (float)((double)frame.car_pos_x +
                        (double)frame.car_mtx[8] * (double)frame.fwd_offset + side_offset * (double)*frame.car_mtx);
    frame.target_pos.y = frame.car_mtx[5] * k_f_0_3 +
                 frame.car_pos_y + frame.car_mtx[9] * frame.fwd_offset + (float)(side_offset * (double)frame.car_mtx[1]);
    frame.target_pos.z = frame.car_mtx[6] * k_f_0_3 +
                 frame.car_pos_z + frame.car_mtx[10] * frame.fwd_offset + (float)(side_offset * (double)frame.car_mtx[2]);
    frame.car_pos_x = frame.car_mtx[4] * k_f_0_4 + frame.car_pos_x;
    frame.car_pos_y = frame.car_mtx[5] * k_f_0_4 + frame.car_pos_y;
    frame.car_pos_z = frame.car_mtx[6] * k_f_0_4 + frame.car_pos_z;
    RwMatrixTransform(&frame.look_mtx,cam_mtx,rwCOMBINEREPLACE);
    FUN_004c4460(&frame.look_mtx,&frame.car_pos_x,frame.car_mtx + 4);
    frame.look_mtx.pos.x = frame.target_pos.x;
    frame.look_mtx.pos.y = frame.target_pos.y;
    frame.look_mtx.pos.z = frame.target_pos.z;
    RwMatrixUpdate(&frame.look_mtx);
    matrix_interpolate_rotation_translation(&frame.blend_mtx, cam_mtx, &frame.look_mtx, *(int *)&g_cup_scene_64cb14, *(int *)&g_cup_scene_64cb14);
    RwMatrixTransform(cam_mtx,&frame.blend_mtx,rwCOMBINEREPLACE);
    tick_masked = g_tick_number & 0x8000007f;
    on_period = tick_masked == 0;
    if ((int)tick_masked < 0) {
      on_period = (tick_masked - 1 | 0xffffff80) == 0xffffffff;
    }
    if (on_period) {
      cup_scene_draw_team_overlay(*(int *)&blend,car_index);
    }
    part_frame = vehicle_part_frame(0,0x13);
    frame.target_pos.x = 1.0;
    frame.target_pos.y = 1.0;
    frame.target_pos.z = 0.0;
    cam_mtx = RwFrameGetLTM(*(RwFrame **)(part_frame + 4));
    RwFrameRotate((struct RwFrame *)g_cup_64cbd4,(struct RwV3d *)&frame.target_pos,(float)(int)g_camera_7ef7d8 * k_f_0_0075,rwCOMBINEREPLACE);
    RwFrameTranslate((struct RwFrame *)g_cup_64cbd4,(struct RwV3d *)&cam_mtx->pos,rwCOMBINEPOSTCONCAT);
    /* car position + lift, each partial sum rounded to 32 bit like the original's stores */
    frame.target_pos.x = frame.car_mtx[0xc];
    frame.target_pos.y = frame.car_mtx[0xd];
    frame.target_pos.z = frame.car_mtx[0xe];
    frame.target_pos.y = frame.target_pos.y + k_f_4;
    frame.target_pos.x = frame.car_mtx[8] + frame.target_pos.x;
    frame.target_pos.y = frame.car_mtx[9] + frame.target_pos.y;
    frame.target_pos.z = frame.car_mtx[10] + frame.target_pos.z;
    frame.target_pos.x = frame.target_pos.x + *frame.car_mtx;
    frame.target_pos.y = frame.car_mtx[1] + frame.target_pos.y;
    frame.target_pos.z = frame.car_mtx[2] + frame.target_pos.z;
    RwFrameTranslate((struct RwFrame *)g_cup_64cbd8,(struct RwV3d *)&frame.target_pos,rwCOMBINEREPLACE);
    car_vel_ptr = (undefined *)&g_car_62ac74;
    cam_mtx = RwFrameGetLTM((struct RwFrame *)g_cup_64cbd4);
    FUN_004c42f0(g_cup_64cbd8,&cam_mtx->pos,car_vel_ptr);
  }
  return;
}


