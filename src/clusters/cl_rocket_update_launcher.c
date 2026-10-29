// cluster-entries: rocket_update_launcher
/* Installed cluster rocket_update_launcher (tools/cluster_install.py from output/cluster_pilot8/rocket_update_launcher.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00408a80*/ extern int fx_object_get_matrix_ptr();
/*00408aa0*/ extern int __cdecl fx_find_index_by_rwp_id(undefined4 p0, undefined4 p1);
/*00422320*/ extern int vehicle_get_matrix_local();
/*00465470*/ extern int weapon_find_targets_in_cone();
/*00465f20*/ extern int weapon_effect_get_slot_value_a();
/*00465f40*/ extern int weapon_effect_get_slot_value_b();
/*00472890*/ extern int sfx_play_voice_at();
/*00480060*/ extern int hud_label_add_world_text();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*005e64a4*/ extern float k_f_0;
/*0060da40*/ extern undefined g_rocket_update_60da40;
/*0060da44*/ extern undefined g_rocket_update_60da44;
/*006a5ff0*/ extern undefined g_rocket_state;
/*006a5ff8*/ extern undefined g_weapon_6a5ff8;
/*006a6000*/ extern undefined g_rocket_6a6000;
/*006a6004*/ extern undefined g_rocket_update_6a6004;
/*006a6008*/ extern undefined4 g_weapon_6a6008;
/*007ef7f4*/ extern float g_tick_seconds;
/*00460a40*/ extern undefined __cdecl weapon_rocket_spin_update(undefined4 p0);
/*00461370*/ extern int rocket_hit_callback();
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(undefined4 p0, undefined4 p1);
/*00469cb0*/ extern int sfx_move_playing_voice();
/*00482010*/ extern int random_int_range();
/*00484680*/ extern int object_set_flag_4();
/*00496850*/ extern int trigger_sphere_register();
/*0049bf00*/ extern int fx_ribbon_add_point();
/*0049fd50*/ extern int fx_smoke_particle_spawn_tinted();
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004c4e20*/ extern int FUN_004c4e20(undefined4 p0, undefined4 p1, undefined4 p2);
/*005dd32c*/ extern float k_f_3;
/*005dd36c*/ extern float k_f_0_4;
/*005dd40c*/ extern float k_f_1_5;
/*005de1b0*/ extern float k_f_n0_05;
/*005de20c*/ extern float k_f_2_5;
/*005de6a8*/ extern float k_f_n0_5;
/*006a5e24*/ extern undefined g_weapon_rocket_6a5e24;
/*006a6040*/ extern undefined g_weapon_rocket_6a6040;
/*006a60cc*/ extern undefined g_weapon_rocket_6a60cc[0x1000];
/*007ef804*/ extern undefined4 g_tick_number;
/* rocket_update_launcher */

/* "per player: updates the launcher model on the car (inferred from weapon_rocket_update)" */

static int rocket_explode_r(int p0);
extern void rocket_explode(void);
static int rocket_fly_homing_r(int p0);
extern void rocket_fly_homing(void);
static int rocket_fly_straight_r(int p0);
extern void rocket_fly_straight(void);
/* (inferred names) per frame update of one rocket launcher (launcher record, slot at +0xc, frame object at +0x20): follows the launcher frame, counts the slot timer down, and when it
   expires shows the green world label 0x32b above the launching vehicle (or the fx object) once, then searches targets in a cone in front of the launcher. slot_byte_offset = slot * 0x2c into the rocket state table. */
void rocket_update_launcher_n(int launcher)

{
  int slot;
  RwMatrix *launcher_ltm;
  int owner_vehicle;
  undefined4 effect_a_id;
  int fx_index;
  int slot_byte_offset;
  undefined *target_filter;
  struct {
    undefined1 label_r;
    undefined1 label_g;
    undefined1 label_b;
    undefined1 label_a;
    int fx_flag_or_matrix;
    undefined1 vehicle_matrix [48];
    undefined1 label_pos [16];
  } frame;
  
  frame.fx_flag_or_matrix = 0;
  launcher_ltm = RwFrameGetLTM(*(RwFrame **)(*(int *)(launcher + 0x20) + 4));
  slot = *(int *)(launcher + 0xc);
  if (slot == -1) {
    return;
  }
  owner_vehicle = weapon_effect_get_slot_value_b(slot);
  slot_byte_offset = slot * 0x2c;
  RwFrameTransform(*(RwFrame **)((&g_weapon_6a6008)[slot * 0xb] + 4),
                   *(RwMatrix **)((char *)&g_rocket_state + slot_byte_offset),rwCOMBINEREPLACE);
  if (owner_vehicle == -1) {
    fx_index = slot;
    effect_a_id = weapon_effect_get_slot_value_a(fx_index);
    fx_index = fx_find_index_by_rwp_id(effect_a_id,fx_index);
    if (fx_index == -1) {
      *(undefined4 *)((char *)&g_rocket_update_6a6004 + slot_byte_offset) = 0x3e99999a;
      *(undefined4 *)((char *)&g_rocket_6a6000 + slot_byte_offset) = 0;
      goto LAB_004616d1;
    }
    frame.fx_flag_or_matrix = 1;
  }
  *(float *)((char *)&g_rocket_update_6a6004 + slot_byte_offset) =
       *(float *)((char *)&g_rocket_update_6a6004 + slot_byte_offset) - g_tick_seconds;
LAB_004616d1:
  if (*(float *)((char *)&g_rocket_update_6a6004 + slot_byte_offset) <= k_f_0) {
    frame.label_g = 0xff;
    frame.label_a = 0xff;
    frame.label_r = 0;
    frame.label_b = 0;
    if ((*(int *)((char *)&g_rocket_6a6000 + slot_byte_offset) == 0) && (0 < *(int *)((char *)&g_weapon_6a5ff8 + slot_byte_offset))) {
      if (frame.fx_flag_or_matrix == 1) {
        fx_object_get_matrix_ptr(&frame.fx_flag_or_matrix,0);
        hud_label_add_world_text(0x32b,frame.fx_flag_or_matrix + 0x30,&frame.label_r,&frame.label_r,0x41c00000,0);
      }
      else {
        vehicle_get_matrix_local(owner_vehicle,frame.vehicle_matrix);
        hud_label_add_world_text(0x32b,frame.label_pos,&frame.label_r,&frame.label_r,0x41c00000,0);
        sfx_play_voice_at(0x2b,frame.label_pos,slot);
      }
    }
    *(undefined4 *)((char *)&g_rocket_6a6000 + slot_byte_offset) = 1;
    *(undefined4 *)((char *)&g_rocket_update_6a6004 + slot_byte_offset) = 0;
  }
  if (*(int *)((char *)&g_rocket_6a6000 + slot_byte_offset) == 0) {
    target_filter = &g_rocket_update_60da40;
  }
  else {
    target_filter = &g_rocket_update_60da44;
  }
  weapon_find_targets_in_cone(slot,&launcher_ltm->pos,0x41000000,0x41f00000,target_filter);
  return;
}

/* rocket_explode_r: C -> original rocket_explode (registers eax) */
__declspec(naked) int rocket_explode_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call rocket_explode
        ret
    }
}

/* rocket_fly_homing_r: C -> original rocket_fly_homing (registers esi) */
__declspec(naked) int rocket_fly_homing_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call rocket_fly_homing
        pop esi
        ret
    }
}

/* rocket_fly_straight_r: C -> original rocket_fly_straight (registers esi) */
__declspec(naked) int rocket_fly_straight_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call rocket_fly_straight
        pop esi
        ret
    }
}

/* entry thunk at the original address of rocket_update_launcher: register inputs (eax) -> natural C rocket_update_launcher_n; ret 0 */
__declspec(naked) void rocket_update_launcher(void)
{
    __asm {
        push eax
        call rocket_update_launcher_n
        add esp, 4
        ret
    }
}
