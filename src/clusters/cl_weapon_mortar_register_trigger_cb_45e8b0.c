// cluster-entries: weapon_mortar_register_trigger_cb_45e8b0
/* Installed cluster weapon_mortar_register_trigger_cb_45e8b0 (tools/cluster_install.py from output/cluster_pilot_kb/weapon_mortar_register_trigger_cb_45e8b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0049cb60*/ extern int sphere_vs_box_test(float *hit, float *param_3);
/*005dd36c*/ extern float k_f_0_4;
/* weapon_mortar_register_trigger_cb_45e8b0 */

/* function pointer pushed/stored by weapon_mortar_register_trigger (MOV dword ptr [ESP +
   0x20],0x45e8b0) */

static int weapon_pickup_explode_effect_r(int p0);
extern void weapon_pickup_explode_effect(void);
void weapon_mortar_register_trigger_cb_45e8b0(int trigger, char *hit)

{
  int reg_edi;
  int overlap;
  
  if ((*(int *)(trigger + 0x10) == 0x13) &&
     (((reg_edi = *(int *)(trigger + 0x14), *(int *)(hit + 0x10) == 0x15 &&
       (overlap = sphere_vs_box_test((float *)(*(char **)(hit + 0x14) + 0x68),
                                   (float *)trigger), overlap != 0)) ||
      ((k_f_0_4 <= *(float *)(reg_edi + 0x50) &&
       ((0xfeff < *(int *)(hit + 0x10) && (*(int *)(hit + 0x10) < 0xff08)))))))) {
    weapon_pickup_explode_effect_r(reg_edi);
  }
  return;
}

/* weapon_pickup_explode_effect_r: C -> original weapon_pickup_explode_effect (registers edi) */
__declspec(naked) int weapon_pickup_explode_effect_r(int p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call weapon_pickup_explode_effect
        pop edi
        ret
    }
}
