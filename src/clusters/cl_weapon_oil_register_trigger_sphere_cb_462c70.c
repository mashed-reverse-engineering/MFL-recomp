// cluster-entries: weapon_oil_register_trigger_sphere_cb_462c70
/* Installed cluster weapon_oil_register_trigger_sphere_cb_462c70 (tools/cluster_install.py from output/cluster_pilot_k/weapon_oil_register_trigger_sphere_cb_462c70.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00434bb0*/ extern int get_event_type();
/*00437f60*/ extern int screen_shake_add();
/*005dd330*/ extern float float_1_0;
/*005dd370*/ extern float k_f_0_3;
/*005ddea0*/ extern float k_f_20;
/*005dec28*/ extern float k_f_220;
/* weapon_oil_register_trigger_sphere_cb_462c70 */

/* function pointer pushed/stored by weapon_oil_register_trigger_sphere (MOV dword ptr [ESP +
   0x20],0x462c70) */

static int weapon_oil_car_slide_random_r(int p0);
extern void weapon_oil_car_slide_random(void);
void weapon_oil_register_trigger_sphere_cb_462c70(char *param_1, char *param_2)

{
  float *pfVar1;
  int iVar2;
  int event_type;
  
  pfVar1 = *(float **)(param_1 + 0x14);
  if (pfVar1 != (float *)0x0) {
    iVar2 = *(int *)(param_2 + 0x10);
    if ((0xfeff < iVar2) && (iVar2 < 0xff08)) {
      if ((k_f_0_3 < *pfVar1) && (k_f_20 < (float_1_0 - pfVar1[1]) * k_f_220)) {
        weapon_oil_car_slide_random_r(iVar2 + -0xff00);
      }
      event_type = get_event_type();
      if ((event_type == EVENT_TRAINING) && (iVar2 == 0xff01)) {
        screen_shake_add(1);
      }
    }
    if ((*(int *)&pfVar1[0x16] == 0) &&
       ((*(int *)(param_2 + 0x10) == 0x10 || (*(int *)(param_2 + 0x10) == 0x11)))) {
      pfVar1[0x17] = 0.1;
      *(int *)&pfVar1[0x16] = 1;
    }
  }
  return;
}

/* weapon_oil_car_slide_random_r: C -> original weapon_oil_car_slide_random (registers esi) */
__declspec(naked) int weapon_oil_car_slide_random_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_oil_car_slide_random
        pop esi
        ret
    }
}
