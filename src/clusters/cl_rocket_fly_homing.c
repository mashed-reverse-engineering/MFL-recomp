// cluster-entries: rocket_fly_homing
/* Installed cluster rocket_fly_homing (tools/cluster_install.py from output/cluster_pilot10/rocket_fly_homing.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00408a80*/ extern int fx_object_get_matrix_ptr();
/*0047b520*/ extern int car_current_matrix();
/*0047b750*/ extern int car_get_speed();
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd374*/ extern float k_f_0_2;
/*005dd61c*/ extern float k_f_0_001;
/*005e64a4*/ extern float k_f_0;
/*005de19c*/ extern float k_f_1_0en4;
/*005de25c*/ extern float k_f_0_005;
/*005de490*/ extern float k_f_0_02;
/*005debe8*/ extern float k_f_0_075;
/*005df774*/ extern float k_f_4_8780486en5;
/*005df7b8*/ extern float k_f_n0_2;
/*0062ac74*/ extern float g_car_62ac74;
#define ROCKET_STEER_PITCH_POS (0x41a00000) /* float +20.0: pitch_rate when the target is below -0.2 in the rocket frame (inferred) */
#define ROCKET_STEER_PITCH_NEG (0xc1a00000) /* float -20.0: pitch_rate when the target direction y > 0.2 */
#define ROCKET_STEER_YAW_POS (0x40000000) /* float +2.0: yaw_rate when the target direction x > 0.2 */
#define ROCKET_STEER_YAW_NEG (0xc0000000) /* float -2.0: yaw_rate when the target direction x < -0.2 */
#define ROCKET_ACCEL_MAX (0x3ba3d70a) /* float 0.005 */
#define ROCKET_SPEED_MIN (0x3d99999a) /* float 0.075 */
/* rocket_fly_homing */

/* One flight step of a homing rocket: steers yaw/pitch towards the target by fixed rates, adapts speed to the target car's speed (accel clamped 0..0.005, speed >= 0.075), then moves the rocket frame along its 'at' axis */

/* target_world = target position (+0.2 up) from the target fx object (+0x58) or car (+0x54); transformed into the rocket frame
   (inverse matrix) and normalised to target_dir, whose x/y components steer the rocket within +-0.2. */
void rocket_fly_homing_n(RocketRecord *rocket)

{
  RwV3d *v;
  RwFrame *frame;
  float scratch;
  struct {
    int target_matrix;
    float dt_scaled;
    RwV3d target_world;
    RwV3d target_dir;
    RwV3d target_local;
    RwMatrix inv_matrix;
  } frame_obj;
  
  frame_obj.dt_scaled = 0.0;
  frame = *(RwFrame **)(rocket->frame_holder + 4);
  RwMatrixInvert(&frame_obj.inv_matrix,&RwFrame_modelling(frame));
  if (rocket->target_car == -1) {
    fx_object_get_matrix_ptr(&frame_obj.target_matrix,rocket->target_fx_object);
  }
  else {
    car_current_matrix(&frame_obj.target_matrix,rocket->target_car);
  }
  frame_obj.target_world.x = *(RwReal *)(frame_obj.target_matrix + 0x30);
  frame_obj.target_world.z = *(RwReal *)(frame_obj.target_matrix + 0x38);
  frame_obj.target_world.y = *(float *)(frame_obj.target_matrix + 0x34) + k_f_0_2;
  RwV3dTransformPoints(&frame_obj.target_local,&frame_obj.target_world,1,&frame_obj.inv_matrix);
  RwV3dNormalize(&frame_obj.target_dir,&frame_obj.target_local);
  if (!(frame_obj.target_dir.y > k_f_0_2)) {
    if (!(k_f_n0_2 > frame_obj.target_dir.y)) {
      *(undefined4 *)&rocket->pitch_rate = 0;
    }
    else {
      *(undefined4 *)&rocket->pitch_rate = ROCKET_STEER_PITCH_POS;
    }
  }
  else {
    *(undefined4 *)&rocket->pitch_rate = ROCKET_STEER_PITCH_NEG;
  }
  if (!(frame_obj.target_dir.x > k_f_0_2)) {
    if (!(k_f_n0_2 > frame_obj.target_dir.x)) {
      *(undefined4 *)&rocket->yaw_rate = 0;
    }
    else {
      *(undefined4 *)&rocket->yaw_rate = ROCKET_STEER_YAW_NEG;
    }
  }
  else {
    *(undefined4 *)&rocket->yaw_rate = ROCKET_STEER_YAW_POS;
  }
  RwV3dLength(&frame_obj.target_local);
  rocket->yaw = rocket->yaw_rate + rocket->yaw;
  rocket->pitch = rocket->pitch + rocket->pitch_rate;
  RwFrameSetIdentity(frame);
  RwFrameRotate(frame,(RwV3d *)&g_car_62ac74,rocket->yaw,rwCOMBINEREPLACE);
  RwFrameRotate(frame,&(RwFrame_modelling(frame)).right,rocket->pitch,rwCOMBINEPOSTCONCAT);
  car_get_speed(&frame_obj.dt_scaled,rocket->target_car);
  scratch = k_f_4_8780486en5 * frame_obj.dt_scaled;
  rocket->target_speed_a = scratch;
  scratch = scratch - rocket->speed;
  rocket->target_speed_b = scratch;
  if (!(scratch < k_f_0_02)) {      /* NaN takes this branch (original: fcomp, test ah,5, jp) */
    scratch = rocket->accel - k_f_0_001;
  }
  else {
    scratch = rocket->accel + k_f_1_0en4;
  }
  rocket->accel = scratch;
  if (rocket->accel < k_f_0) {
    *(undefined4 *)&rocket->accel = 0;
  }
  if (k_f_0_005 < rocket->accel) {
    *(undefined4 *)&rocket->accel = ROCKET_ACCEL_MAX;
  }
  scratch = rocket->accel + rocket->speed;
  rocket->speed = scratch;
  if (scratch < k_f_0_075) {
    *(undefined4 *)&rocket->speed = ROCKET_SPEED_MIN;
  }
  v = &rocket->pos;
  rocket->vel.x = rocket->speed * (RwFrame_modelling(frame)).at.x;
  rocket->vel.y = (RwFrame_modelling(frame)).at.y * rocket->speed;
  rocket->vel.z = (RwFrame_modelling(frame)).at.z * rocket->speed;
  scratch = rocket->lift + rocket->vel.y;
  rocket->vel.y = scratch;
  v->x = v->x + rocket->vel.x;
  rocket->pos.y = scratch + rocket->pos.y;
  rocket->pos.z = rocket->vel.z + rocket->pos.z;
  RwFrameTranslate(frame,v,rwCOMBINEPOSTCONCAT);
  return;
}

/* entry thunk at the original address of rocket_fly_homing: register inputs (esi) -> natural C rocket_fly_homing_n; ret 0 */
__declspec(naked) void rocket_fly_homing(void)
{
    __asm {
        push esi
        call rocket_fly_homing_n
        add esp, 4
        ret
    }
}
