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
/* Oil trigger-sphere callback: for trigger ids 0xff01..0xff07 may start a random slide (weapon_oil_car_slide_random) and shake the screen in training; ids 0x10/0x11 arm a one-shot flag on the car data (inferred). */
void weapon_oil_register_trigger_sphere_cb_462c70(char *trigger_owner, char *trigger_other)

{
  float *car_data;
  int trigger_id;
  int event_type;
  
  car_data = *(float **)(trigger_owner + 0x14);
  if (car_data != (float *)0x0) {
    trigger_id = *(int *)(trigger_other + 0x10);
    if ((0xfeff < trigger_id) && (trigger_id < 0xff08)) {
      if ((k_f_0_3 < *car_data) && (k_f_20 < (float_1_0 - car_data[1]) * k_f_220)) {
        weapon_oil_car_slide_random_r(trigger_id + -0xff00);
      }
      event_type = get_event_type();
      if ((event_type == EVENT_TRAINING) && (trigger_id == 0xff01)) {
        screen_shake_add(1);
      }
    }
    if ((*(int *)&car_data[0x16] == 0) &&
       ((*(int *)(trigger_other + 0x10) == 0x10 || (*(int *)(trigger_other + 0x10) == 0x11)))) {
      car_data[0x17] = 0.1;
      *(int *)&car_data[0x16] = 1;
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
