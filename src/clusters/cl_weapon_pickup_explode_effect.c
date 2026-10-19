// cluster-entries: weapon_pickup_explode_effect
/* Installed cluster weapon_pickup_explode_effect (tools/cluster_install.py from output/cluster_pilot_hb1/weapon_pickup_explode_effect.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0045f5a0*/ extern int explosion_queue(float *position, float a, float b, float c, int kind);
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*004846e0*/ extern int point_lerp_to_aux_camera(RwV3d *position, void *out, float t);
/*004866d0*/ extern int fx_emitter_spawn_b_default(RwV3d *position, void *direction, float angle, float scale);
/*00487670*/ extern int effect_sprite_spawn(int kind, RwMatrix *matrix, void *color, float a, float b);
/*004c45e0*/ extern float rp_random_float_range(float lo, float hi);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac74*/ extern float g_car_62ac74;
/* weapon_pickup_explode_effect */

/* WARNING: Unknown calling convention */
/* "spawns the explosion fx, emitter and light flash at the pickup position" */

void weapon_pickup_explode_effect_n(char *in_edi)

{
  RwV3d *translation;
  RpClump *clump;
  struct {
    undefined1 local_60;
    undefined1 local_5f;
    undefined1 local_5e;
    undefined1 local_5d;
    undefined1 local_5c [12];
    RwReal local_50;
    undefined4 local_4c;
    undefined4 local_48;
    undefined4 local_44;
    RwMatrix local_40;
  } frame;
  
  translation = (RwV3d *)(in_edi + 0x14);
  frame.local_60 = 0xff;
  frame.local_5f = 0x80;
  frame.local_5e = 0;
  frame.local_5d = 100;
  *(undefined4 *)(in_edi + 4) = 0;
  point_lerp_to_aux_camera(translation,frame.local_5c,0.3);
  clump = *(RpClump **)(in_edi + 0x48);
  clump_set_flag_on_atomics(clump, 0x0);
  fx_emitter_spawn_b_default(translation,&g_car_62ac74,360.0,rp_random_float_range(1.75,2.25));
  frame.local_50 = translation->x;
  frame.local_4c = *(undefined4 *)(in_edi + 0x18);
  frame.local_48 = *(undefined4 *)(in_edi + 0x1c);
  frame.local_44 = 0x40000000;
  explosion_queue(&frame.local_50,rp_random_float_range(25.0,35.0),rp_random_float_range(100.0,200.0),
                  rp_random_float_range(100.0,200.0),2);
  frame.local_40.flags = frame.local_40.flags | 0x20003;
  frame.local_40.at.z = 1.0;
  frame.local_40.up.y = 1.0;
  frame.local_40.right.x = 1.0;
  frame.local_40.up.x = 0.0;
  frame.local_40.right.z = 0.0;
  frame.local_40.right.y = 0.0;
  frame.local_40.at.y = 0.0;
  frame.local_40.at.x = 0.0;
  frame.local_40.up.z = 0.0;
  frame.local_40.pos.z = 0.0;
  frame.local_40.pos.y = 0.0;
  frame.local_40.pos.x = 0.0;
  RwMatrixTranslate(&frame.local_40,translation,rwCOMBINEREPLACE);
  RwMatrixRotate(&frame.local_40,(RwV3d *)&g_car_62ac68,90.0,rwCOMBINEPRECONCAT);
  effect_sprite_spawn(1,&frame.local_40,&frame.local_60,2.0,4.0);
  return;
}

/* entry thunk at the original address of weapon_pickup_explode_effect: register inputs (edi) -> natural C weapon_pickup_explode_effect_n; ret 0 */
__declspec(naked) void weapon_pickup_explode_effect(void)
{
    __asm {
        push edi
        call weapon_pickup_explode_effect_n
        add esp, 4
        ret
    }
}
