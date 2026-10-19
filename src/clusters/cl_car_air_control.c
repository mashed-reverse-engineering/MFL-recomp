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

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "when the car is airborne (+0x9E0 == 0, mode 0) levels pitch and roll with RwMatrixRotate;
   otherwise damps the angular part (inferred)" */

void car_air_control_n(Car *in_ecx, RwMatrix *in_esi, float param_4)

{
  float fVar1;
  double fVar2;
  double fVar3;
  float10 fVar4;
  struct {
    RwV3d local_18;
    RwV3d local_c;
  } frame;
  
  if (in_ecx->wheels_in_contact == k_f_0) {
    if (in_ecx->mode == 0) {
      fVar1 = param_4 * k_f_0_001;
      fVar2 = (in_esi->at).z * g_car_62ac7c +
              (in_esi->at).x * g_car_62ac74 + (in_esi->at).y * g_car_62ac78;
      fVar3 = k_f_n1;
      if ((k_f_n1 <= fVar2) && (fVar3 = fVar2, float_1_0 < fVar2)) {
        fVar3 = float_1_0;
      }
      fVar4 = (float10)FUN_004b3284((double)fVar3);
      RwMatrixRotate(in_esi,(RwV3d *)&g_car_62ac68,
                     (float)(-(fVar4 * (float10)(57.295799255371094) - (float10)(90.0)) * (float10)fVar1),
                     rwCOMBINEPRECONCAT);
      fVar2 = (in_esi->right).z * g_car_62ac7c +
              g_car_62ac74 * (in_esi->right).x + (in_esi->right).y * g_car_62ac78;
      fVar3 = k_f_n1;
      if ((k_f_n1 <= fVar2) && (fVar3 = fVar2, float_1_0 < fVar2)) {
        fVar3 = float_1_0;
      }
      fVar4 = (float10)FUN_004b3284((double)fVar3);
      RwMatrixRotate(in_esi,(RwV3d *)&g_weapon_62ac80,
                     (float)((fVar4 * (float10)(57.295799255371094) - (float10)k_f_90) * (float10)fVar1),
                     rwCOMBINEPRECONCAT);
      return;
    }
    in_ecx->angular_velocity[2] = 0.0;
    in_ecx->angular_velocity[1] = 0.0;
    in_ecx->angular_velocity[0] = 0.0;
    frame.local_18.x = in_ecx->velocity[0];
    frame.local_18.y = in_ecx->velocity[1];
    frame.local_18.z = in_ecx->velocity[2];
    RwV3dNormalize(&frame.local_18,&frame.local_18);
    frame.local_c.x = frame.local_18.z;
    frame.local_c.y = frame.local_18.y;
    frame.local_c.z = frame.local_18.x;
    RwMatrixRotate(in_esi,&frame.local_c,param_4 * k_f_0_05,rwCOMBINEPRECONCAT);
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
