// cluster-entries: car_air_control
/* Installed cluster car_air_control (tools/cluster_install.py from output/cluster_pilot9/car_air_control.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004b3284*/ extern double FUN_004b3284(double p0);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005dd61c*/ extern float k_f_0_001;
/*005ddad4*/ extern float k_f_90;
/*005ddafc*/ extern float k_f_0_05;
/*005ddc28*/ extern float k_f_0;
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac74*/ extern float g_car_62ac74;
/*0062ac78*/ extern float g_car_62ac78;
/*0062ac7c*/ extern float g_car_62ac7c;
/*0062ac80*/ extern undefined g_weapon_62ac80;
/* car_air_control */

/* "when the car is airborne (+0x9E0 == 0, mode 0) levels pitch and roll with RwMatrixRotate;
   otherwise damps the angular part (inferred)" */

void car_air_control_n(Car *car, RwMatrix *matrix, float dt)

{
  float step;
  double dot_a;
  double dot_clamped;
  float10 angle;
  struct {
    RwV3d velocity_dir;
    RwV3d swirl_axis;
  } frame;
  
  if (car->wheels_in_contact == k_f_0) {
    if (car->mode == 0) {
      step = dt * k_f_0_001;
      dot_a = (matrix->at).z * g_car_62ac7c +
              (matrix->at).x * g_car_62ac74 + (matrix->at).y * g_car_62ac78;
      dot_clamped = dot_a;      /* clamp to -1..1; NaN passes unchanged (original: fcom / test ah,5 / jp, fcom / test ah,0x41 / jne) */
      if (dot_a < k_f_n1) {
        dot_clamped = k_f_n1;
      }
      else if (dot_a > float_1_0) {
        dot_clamped = float_1_0;
      }
      angle = (float10)FUN_004b3284((double)dot_clamped);
      RwMatrixRotate(matrix,(RwV3d *)&g_car_62ac68,
                     (float)(-(angle * (float10)(57.295799255371094) - (float10)(90.0)) * (float10)step),
                     rwCOMBINEPRECONCAT);
      dot_a = (matrix->right).z * g_car_62ac7c +
              g_car_62ac74 * (matrix->right).x + (matrix->right).y * g_car_62ac78;
      dot_clamped = dot_a;      /* clamp to -1..1; NaN passes unchanged (original: fcom / test ah,5 / jp, fcom / test ah,0x41 / jne) */
      if (dot_a < k_f_n1) {
        dot_clamped = k_f_n1;
      }
      else if (dot_a > float_1_0) {
        dot_clamped = float_1_0;
      }
      angle = (float10)FUN_004b3284((double)dot_clamped);
      RwMatrixRotate(matrix,(RwV3d *)&g_weapon_62ac80,
                     (float)((angle * (float10)(57.295799255371094) - (float10)k_f_90) * (float10)step),
                     rwCOMBINEPRECONCAT);
      return;
    }
    car->angular_velocity[2] = 0.0;
    car->angular_velocity[1] = 0.0;
    car->angular_velocity[0] = 0.0;
    frame.velocity_dir.x = car->velocity[0];
    frame.velocity_dir.y = car->velocity[1];
    frame.velocity_dir.z = car->velocity[2];
    RwV3dNormalize(&frame.velocity_dir,&frame.velocity_dir);
    frame.swirl_axis.x = frame.velocity_dir.z;
    frame.swirl_axis.y = frame.velocity_dir.y;
    frame.swirl_axis.z = frame.velocity_dir.x;
    RwMatrixRotate(matrix,&frame.swirl_axis,dt * k_f_0_05,rwCOMBINEPRECONCAT);
  }
  return;
}

/* entry thunk at the original address of car_air_control: register inputs (ecx,esi,s0) -> natural C car_air_control_n; ret 0 */
__declspec(naked) void car_air_control(void)
{
    __asm {
        push dword ptr [esp + 4]
        push esi
        push ecx
        call car_air_control_n
        add esp, 12
        ret
    }
}
