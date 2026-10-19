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
/*005ddc28*/ extern float k_f_0;
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

/* WARNING: Unknown calling convention */
/* "per player: updates the launcher model on the car (inferred from weapon_rocket_update)" */

static int rocket_explode_r(int p0);
extern void rocket_explode(void);
static int rocket_fly_homing_r(int p0);
extern void rocket_fly_homing(void);
static int rocket_fly_straight_r(int p0);
extern void rocket_fly_straight(void);
void rocket_update_launcher_n(int in_eax)

{
  int iVar1;
  RwMatrix *pRVar2;
  int iVar3;
  undefined4 uVar4;
  int fx_index;
  int iVar5;
  undefined *puVar6;
  struct {
    undefined1 local_48;
    undefined1 local_47;
    undefined1 local_46;
    undefined1 local_45;
    int local_44;
    undefined1 local_40 [48];
    undefined1 local_10 [16];
  } frame;
  
  frame.local_44 = 0;
  pRVar2 = RwFrameGetLTM(*(RwFrame **)(*(int *)(in_eax + 0x20) + 4));
  iVar1 = *(int *)(in_eax + 0xc);
  if (iVar1 == -1) {
    return;
  }
  iVar3 = weapon_effect_get_slot_value_b(iVar1);
  iVar5 = iVar1 * 0x2c;
  RwFrameTransform(*(RwFrame **)((&g_weapon_6a6008)[iVar1 * 0xb] + 4),
                   *(RwMatrix **)((char *)&g_rocket_state + iVar5),rwCOMBINEREPLACE);
  if (iVar3 == -1) {
    fx_index = iVar1;
    uVar4 = weapon_effect_get_slot_value_a(fx_index);
    fx_index = fx_find_index_by_rwp_id(uVar4,fx_index);
    if (fx_index == -1) {
      *(undefined4 *)((char *)&g_rocket_update_6a6004 + iVar5) = 0x3e99999a;
      *(undefined4 *)((char *)&g_rocket_6a6000 + iVar5) = 0;
      goto LAB_004616d1;
    }
    frame.local_44 = 1;
  }
  *(float *)((char *)&g_rocket_update_6a6004 + iVar5) =
       *(float *)((char *)&g_rocket_update_6a6004 + iVar5) - g_tick_seconds;
LAB_004616d1:
  if (*(float *)((char *)&g_rocket_update_6a6004 + iVar5) < k_f_0 !=
      (*(float *)((char *)&g_rocket_update_6a6004 + iVar5) == k_f_0)) {
    frame.local_47 = 0xff;
    frame.local_45 = 0xff;
    frame.local_48 = 0;
    frame.local_46 = 0;
    if ((*(int *)((char *)&g_rocket_6a6000 + iVar5) == 0) && (0 < *(int *)((char *)&g_weapon_6a5ff8 + iVar5))) {
      if (frame.local_44 == 1) {
        fx_object_get_matrix_ptr(&frame.local_44,0);
        hud_label_add_world_text(0x32b,frame.local_44 + 0x30,&frame.local_48,&frame.local_48,0x41c00000,0);
      }
      else {
        vehicle_get_matrix_local(iVar3,frame.local_40);
        hud_label_add_world_text(0x32b,frame.local_10,&frame.local_48,&frame.local_48,0x41c00000,0);
        sfx_play_voice_at(0x2b,frame.local_10,iVar1);
      }
    }
    *(undefined4 *)((char *)&g_rocket_6a6000 + iVar5) = 1;
    *(undefined4 *)((char *)&g_rocket_update_6a6004 + iVar5) = 0;
  }
  if (*(int *)((char *)&g_rocket_6a6000 + iVar5) == 0) {
    puVar6 = &g_rocket_update_60da40;
  }
  else {
    puVar6 = &g_rocket_update_60da44;
  }
  weapon_find_targets_in_cone(iVar1,&pRVar2->pos,0x41000000,0x41f00000,puVar6);
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
