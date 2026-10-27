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

/* "spawns the explosion fx, emitter and light flash at the pickup position" */

void weapon_pickup_explode_effect_n(char *pickup)

{
  RwV3d *translation;
  RpClump *clump;
  struct {
    undefined1 flash_r;
    undefined1 flash_g;
    undefined1 flash_b;
    undefined1 flash_a;
    undefined1 cam_lerp_pos [12];
    RwReal explosion_x;
    undefined4 explosion_y;
    undefined4 explosion_z;
    undefined4 explosion_w;
    RwMatrix sprite_matrix;
  } frame;
  
  translation = (RwV3d *)(pickup + 0x14);
  frame.flash_r = 0xff;
  frame.flash_g = 0x80;
  frame.flash_b = 0;
  frame.flash_a = 100;
  *(undefined4 *)(pickup + 4) = 0;
  point_lerp_to_aux_camera(translation,frame.cam_lerp_pos,0.3);
  clump = *(RpClump **)(pickup + 0x48);
  clump_set_flag_on_atomics(clump, 0x0);
  fx_emitter_spawn_b_default(translation,&g_car_62ac74,360.0,rp_random_float_range(1.75,2.25));
  frame.explosion_x = translation->x;
  frame.explosion_y = *(undefined4 *)(pickup + 0x18);
  frame.explosion_z = *(undefined4 *)(pickup + 0x1c);
  frame.explosion_w = 0x40000000;
  explosion_queue(&frame.explosion_x,rp_random_float_range(25.0,35.0),rp_random_float_range(100.0,200.0),
                  rp_random_float_range(100.0,200.0),2);
  frame.sprite_matrix.flags = frame.sprite_matrix.flags | 0x20003;
  frame.sprite_matrix.at.z = 1.0;
  frame.sprite_matrix.up.y = 1.0;
  frame.sprite_matrix.right.x = 1.0;
  frame.sprite_matrix.up.x = 0.0;
  frame.sprite_matrix.right.z = 0.0;
  frame.sprite_matrix.right.y = 0.0;
  frame.sprite_matrix.at.y = 0.0;
  frame.sprite_matrix.at.x = 0.0;
  frame.sprite_matrix.up.z = 0.0;
  frame.sprite_matrix.pos.z = 0.0;
  frame.sprite_matrix.pos.y = 0.0;
  frame.sprite_matrix.pos.x = 0.0;
  RwMatrixTranslate(&frame.sprite_matrix,translation,rwCOMBINEREPLACE);
  RwMatrixRotate(&frame.sprite_matrix,(RwV3d *)&g_car_62ac68,90.0,rwCOMBINEPRECONCAT);
  effect_sprite_spawn(1,&frame.sprite_matrix,&frame.flash_r,2.0,4.0);
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
