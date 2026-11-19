#include "mfl_tier2.h"
/*004c3f90*/ extern int FUN_004c3f90();
/*004c4720*/ extern int FUN_004c4720();
/*004c4de0*/ extern int FUN_004c4de0();
/*004c51a0*/ extern int FUN_004c51a0();
/*004c52b0*/ extern int FUN_004c52b0();
/*004c5380*/ extern int FUN_004c5380();
/*004c53e0*/ extern int FUN_004c53e0();
/*004c66a0*/ extern int FUN_004c66a0();
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*005698c0*/ extern int RwCameraCalcPixelRay();
/*005dd340*/ extern float k_f_0_5;
/*005dd378*/ extern float k_f_0_1;
/*005ddafc*/ extern float k_f_0_05;
/*005e64a4*/ extern float k_f_0;
/*005ddf54*/ extern float k_f_1_86;
/*005de1bc*/ extern float k_f_0_25_5de1bc;
/*005e64a4*/ extern undefined4 k_f_0;
/*006037c0*/ extern undefined4 g_phase_timer;
/*00603e80*/ extern undefined g_hudpanel_603e80;
/*00603e84*/ extern undefined g_hudpanel_603e84;
/*00603ed4*/ extern undefined g_score_panel_603ed4;
/*00603ed8*/ extern undefined g_score_panel_603ed8;
/*00603edc*/ extern undefined g_score_panel_603edc;
/*00603f1c*/ extern undefined4 g_race_start_603f1c;
/*00603f20*/ extern undefined4 g_race_start_603f20;
/*00603f24*/ extern undefined g_race_start_603f24;
/*00651250*/ extern undefined g_scene_651250;
/*00651960*/ extern undefined4 g_fx_651960;
/*00651964*/ extern undefined4 g_fx_651964;
/*00651968*/ extern undefined4 g_fx_651968;
/*0065196c*/ extern undefined4 g_fx_65196c;
/*00651970*/ extern undefined4 g_fx_651970;
/*00651974*/ extern undefined4 g_fx_651974;
/*00651978*/ extern undefined4 g_fx_651978;
/*0065197c*/ extern undefined4 g_fx_65197c;
/*00651aa0*/ extern undefined4 g_fx_object_count;
/*00651aac*/ extern undefined4 g_fx_spawn_651aac;
/*00658dbc*/ extern undefined4 g_score_panel_658dbc;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007eeb88*/ extern undefined4 g_race_7eeb88;
/*007ef7dc*/ extern undefined4 g_race_clock_a;
/*007ef7fc*/ extern undefined4 g_race_7ef7fc;
/*007ef800*/ extern undefined4 g_time_limit_ticks;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008d8b14*/ extern undefined4 g_race_setup;
/*008d98b0*/ extern undefined4 g_fx_8d98b0;
extern void btb_panel_slot_set_value(int param_1, float param_2);
extern undefined4 camera_get_car_8c7460(void);
extern undefined4 camera_path_update(int param_1);
extern void camera_records_warmup(undefined4 param_1, undefined4 param_2);
extern int car_get_field_bf4(int param_1);
extern int car_get_field_bf8(int param_1);
extern uint car_is_active(uint param_1);
extern undefined4 car_reset_state(uint param_1);
extern undefined4 car_turbo_charge_update(int param_1, int param_2);
extern undefined4 car_turbo_spend(int param_1);
extern undefined4 course_get_course_8c879c(void);
extern void __cdecl course_hook11(char *param_1);
extern void endpoint_panel_get_value(undefined4 * param_1);
extern void fx_system_reset_positions(int * param_1, undefined4 * param_2);
extern undefined4 get_event_type(void);
extern void ghost_update(void);
extern void intro_banner_set_active(int param_1);
extern void menu_set_car_8c7460_and_reset(int param_1);
extern void race_finish(void);
extern undefined4 race_get_setup_index(void);
extern void race_mark_ai_slots(void);
extern void race_set_phase(int param_1);
extern void race_spawn_cars(void);
extern void race_update_cameras(undefined4 param_1);
extern void sfx_play_voice_at(int param_1, undefined4 * param_2, int param_3);
extern void sfx_voice_is_active(int param_1, int param_2);
extern void time_trial_record_step(undefined4 param_1, uint param_2);
extern void trail_set_param(int param_1, int param_2, undefined4 param_3);
extern undefined4 ui_get_menu_69d2d0(void);
extern void vehicle_get_matrix_local(int param_1, undefined4 * param_2);
#include "bridge_sel.h"

/* fx_spawn */
void fx_spawn_c(RwMatrix * reg_eax, undefined4 * reg_edi);
void fx_spawn_c(RwMatrix * reg_eax, undefined4 * reg_edi)
{
  int iVar1;
  uint uVar2;
  int *piVar3;
  
  uVar2 = g_fx_spawn_651aac;
  piVar3 = &g_fx_651960 + g_fx_spawn_651aac * 8;
  iVar1 = *piVar3;
  (&g_fx_651968)[g_fx_spawn_651aac * 8] = 0;
  (&g_fx_65196c)[uVar2 * 8] = 1;
  (&g_fx_651964)[uVar2 * 8] = 0x40c00000;
  RwFrameTransform(*(RwFrame **)(iVar1 + 4),reg_eax,rwCOMBINEREPLACE);
  (&g_fx_651974)[uVar2 * 8] = *reg_edi;
  (&g_fx_651978)[uVar2 * 8] = reg_edi[1];
  iVar1 = *piVar3;
  (&g_fx_65197c)[uVar2 * 8] = reg_edi[2];
  fx_system_reset_positions((&g_fx_651970)[uVar2 * 8],*(int *)(iVar1 + 4) + 0x40);
  g_fx_8d98b0 = g_fx_spawn_651aac;
  if (3 < (int)g_fx_spawn_651aac) {
    g_fx_8d98b0 = g_fx_spawn_651aac - (g_fx_spawn_651aac & 0xfffffffc);
  }
  sfx_play_voice_at(0x15,*(int *)(*piVar3 + 4) + 0x40,g_fx_8d98b0);
  sfx_play_voice_at(0x14,*(int *)(*piVar3 + 4) + 0x40,g_fx_8d98b0);
  g_fx_spawn_651aac = g_fx_spawn_651aac + 1;
  if (9 < (int)g_fx_spawn_651aac) {
    g_fx_spawn_651aac = 0;
  }
  return;
}


__declspec(naked) void fx_spawn(void)
{
    __asm {
        push edi
        push eax
        call fx_spawn_c
        add esp, 8
        ret
    }
}

/* fx_find_object_by_owner */
undefined * fx_find_object_by_owner_c(int reg_esi);
undefined * fx_find_object_by_owner_c(int reg_esi)
{
  undefined *puVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)g_fx_object_count) {
    puVar1 = &g_scene_651250;
    do {
      if (*(int *)(puVar1 + 0x44) == reg_esi) {
        return puVar1;
      }
      iVar2 = iVar2 + 1;
      puVar1 = puVar1 + 0xec;
    } while (iVar2 < (int)g_fx_object_count);
  }
  return (undefined *)0x0;
}


__declspec(naked) void fx_find_object_by_owner(void)
{
    __asm {
        push esi
        call fx_find_object_by_owner_c
        add esp, 4
        ret
    }
}

/* race_phase_3_update */
void race_phase_3_update_c(int reg_ebx);
void race_phase_3_update_c(int reg_ebx)
{
  int car_active;
  int event_type;
  int iVar1;
  
  if (g_phase_timer == 100000) {
    camera_records_warmup(0,0);
    menu_set_car_8c7460_and_reset(1);
    race_spawn_cars();
    race_update_cameras(1);
  }
  iVar1 = 0;
  event_type = 0xb8;
  do {
    if (*(int *)(event_type + g_race_setup) != 0) {
      car_active = car_is_active(iVar1);
      if (car_active == 1) {
        car_reset_state(iVar1);
      }
    }
    event_type = event_type + 4;
    iVar1 = iVar1 + 1;
  } while (event_type < 200);
  g_phase_timer = g_phase_timer - (uint)(reg_ebx * 0x44c) / 0x3c;
  event_type = camera_path_update(reg_ebx);
  if (event_type == 0) {
    event_type = camera_get_car_8c7460();
    if (event_type == 0) {
      event_type = get_event_type();
      if (event_type == 0x11) {
        race_set_phase(6);
        g_phase_timer = 0;
        return;
      }
      race_set_phase(4);
      g_phase_timer = 12000;
    }
  }
  return;
}


__declspec(naked) void race_phase_3_update(void)
{
    __asm {
        push ebx
        call race_phase_3_update_c
        add esp, 4
        ret
    }
}

/* race_phase_5_update */
void race_phase_5_update_c(int reg_eax);
void race_phase_5_update_c(int reg_eax)
{
  int event_type;
  int car_active;
  undefined4 uVar1;
  undefined4 *puVar2;
  int iVar3;
  float local_4;
  
  local_4 = 0.0;
  event_type = course_get_course_8c879c();
  if (((event_type != 0) && (event_type = ui_get_menu_69d2d0(), event_type == 0)) &&
     ((event_type = get_event_type(), event_type == 0 ||
      ((event_type = get_event_type(), event_type == 1 ||
       (event_type = get_event_type(), event_type == 2)))))) {
    race_finish();
  }
  g_phase_timer = g_phase_timer - reg_eax;
  g_race_7ef7fc = (uint)(g_time_limit_ticks < g_race_clock_a);
  event_type = race_get_setup_index();
  course_hook11(**(undefined4 **)(g_race_setup + 4 + event_type * 4));
  race_mark_ai_slots();
  event_type = 0;
  iVar3 = 0xb8;
  do {
    if ((*(int *)(iVar3 + g_race_setup) != 0) &&
       (car_active = car_is_active(event_type), car_active == 1)) {
      car_reset_state(event_type);
      car_turbo_charge_update(event_type,reg_eax);
    }
    iVar3 = iVar3 + 4;
    event_type = event_type + 1;
  } while (iVar3 < 200);
  endpoint_panel_get_value(&local_4);
  if (k_f_1_86 <= local_4) {
    event_type = 0;
    puVar2 = &g_race_7eeb88;
    iVar3 = 0xb8;
    do {
      if ((*(int *)(iVar3 + g_race_setup) != 0) &&
         (car_active = car_is_active(event_type), car_active == 1)) {
        car_turbo_spend(event_type);
        uVar1 = car_get_field_bf4(event_type);
        puVar2[-1] = uVar1;
        uVar1 = car_get_field_bf8(event_type);
        *puVar2 = uVar1;
      }
      iVar3 = iVar3 + 4;
      event_type = event_type + 1;
      puVar2 = puVar2 + 3;
    } while (iVar3 < 200);
    g_phase_timer = 0;
    event_type = get_event_type();
    if (((event_type == 0) || (event_type = get_event_type(), event_type == 1)) ||
       (event_type = get_event_type(), event_type == 2)) {
      intro_banner_set_active(1);
      ghost_update();
    }
    race_set_phase(6);
  }
  event_type = get_event_type();
  if (((event_type != 0) && (event_type = get_event_type(), event_type != 1)) &&
     (event_type = get_event_type(), event_type != 2)) {
    return;
  }
  time_trial_record_step(0,0);
  return;
}


__declspec(naked) void race_phase_5_update(void)
{
    __asm {
        push eax
        call race_phase_5_update_c
        add esp, 4
        ret
    }
}

/* hud_icon_uv_rect_get */
void hud_icon_uv_rect_get_c(uint reg_esi, float * reg_ecx);
void hud_icon_uv_rect_get_c(uint reg_esi, float * reg_ecx)
{
  if (((int)reg_esi < 0) || (4 < (int)reg_esi)) {
    *reg_ecx = 0.0;
    reg_ecx[1] = 0.0;
    reg_ecx[2] = 1.0;
    reg_ecx[3] = 1.0;
  }
  else {
    *reg_ecx = (float)(reg_esi & 3) * k_f_0_25_5de1bc;
    reg_ecx[1] = (float)((int)(reg_esi + ((int)reg_esi >> 0x1f & 3U)) >> 2) * k_f_0_25_5de1bc;
    reg_ecx[2] = (float)((reg_esi & 3) + 1) * k_f_0_25_5de1bc;
    reg_ecx[3] = (float)((int)(reg_esi + 4 + ((int)(reg_esi + 4) >> 0x1f & 3U)) >> 2) *
                 k_f_0_25_5de1bc;
    if (reg_esi == 3) {
      *reg_ecx = 0.255;
      reg_ecx[1] = 0.255;
      reg_ecx[2] = 0.93;
      reg_ecx[3] = 0.97;
      return;
    }
  }
  return;
}


__declspec(naked) void hud_icon_uv_rect_get(void)
{
    __asm {
        push ecx
        push esi
        call hud_icon_uv_rect_get_c
        add esp, 8
        ret
    }
}

/* hudpanel_slot_init */
void hudpanel_slot_init_c(int reg_esi);
void hudpanel_slot_init_c(int reg_esi)
{
  FUN_004c66a0(reg_esi,0x6c);
  *(undefined4 *)(reg_esi + 8) = 0x3d4ccccd;
  *(undefined4 *)(reg_esi + 0xc) = 0x3d4ccccd;
  *(undefined1 *)(reg_esi + 0x10) = 0xff;
  *(undefined1 *)(reg_esi + 0x11) = 0xff;
  *(undefined1 *)(reg_esi + 0x12) = 0xff;
  *(undefined1 *)(reg_esi + 0x13) = 0xff;
  return;
}


__declspec(naked) void hudpanel_slot_init(void)
{
    __asm {
        push esi
        call hudpanel_slot_init_c
        add esp, 4
        ret
    }
}

/* hudpanel_slot_reset */
void hudpanel_slot_reset_c(undefined4 * reg_eax);
void hudpanel_slot_reset_c(undefined4 * reg_eax)
{
  *reg_eax = *(undefined4 *)(&g_hudpanel_603e80 + reg_eax[0x14] * 8);
  reg_eax[1] = *(undefined4 *)(&g_hudpanel_603e84 + reg_eax[0x14] * 8);
  reg_eax[0xd] = 0;
  reg_eax[0x19] = 0xffffffff;
  reg_eax[0xe] = 0;
  reg_eax[0xf] = 0;
  reg_eax[0x10] = 0;
  reg_eax[0x11] = 0;
  reg_eax[0x13] = 0;
  return;
}


__declspec(naked) void hudpanel_slot_reset(void)
{
    __asm {
        push eax
        push eax
        call hudpanel_slot_reset_c
        add esp, 4
        pop eax
        ret
    }
}

/* hudpanel_marker_calc_pixel_ray */
void hudpanel_marker_calc_pixel_ray_c(int reg_eax);
void hudpanel_marker_calc_pixel_ray_c(int reg_eax)
{
  if (*(int *)(reg_eax + 0x34) == 2) {
    RwCameraCalcPixelRay
              (RwCameraGetCurrentCamera(),reg_eax + 0x1c,reg_eax + 0x14,
               *(undefined4 *)(reg_eax + 0x10));
  }
  return;
}


__declspec(naked) void hudpanel_marker_calc_pixel_ray(void)
{
    __asm {
        push eax
        call hudpanel_marker_calc_pixel_ray_c
        add esp, 4
        ret
    }
}

/* hudpanel_marker_ray_cast */
undefined4 hudpanel_marker_ray_cast_c(undefined4 * reg_eax, int reg_edx, undefined4 reg_ecx);
undefined4 hudpanel_marker_ray_cast_c(undefined4 * reg_eax, int reg_edx, undefined4 reg_ecx)
{
  int iVar1;
  undefined4 *puVar2;
  RwV3d local_8c;       /* 12 byte vector: passed by address */
  struct { float x, y, z; int i74; undefined1 b70[36]; int i4c; undefined4 d48; int i44; } hit;  /* one 64 byte result object written by FUN_004c4de0 (the C reads its members afterwards) */
  undefined4 local_40 [16];
  
  iVar1 = FUN_004c4de0(reg_ecx,reg_edx,&hit);
  if ((iVar1 != 0) &&
     (*(int *)(*(int *)(*(int *)(hit.i44 + 0x10) +
                       ((uint)*(ushort *)(*(int *)(hit.i4c + 4) + 6 + hit.i74 * 8) +
                       (uint)*(ushort *)(hit.i4c + 0x80)) * 4) + 4) != -0xfefeff)) {
    FUN_004c4720(&local_8c,reg_edx,reg_edx + 0xc,hit.d48);
    local_8c.x = hit.x * k_f_0_05 + local_8c.x;
    local_8c.y = hit.y * k_f_0_05 + local_8c.y;
    local_8c.z = hit.z * k_f_0_05 + local_8c.z;
    FUN_004c51a0(local_40,hit.b70,&local_8c);
    if (reg_eax != (undefined4 *)0x0) {
      puVar2 = local_40;
      for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
        *reg_eax = *puVar2;
        puVar2 = puVar2 + 1;
        reg_eax = reg_eax + 1;
      }
    }
    return 1;
  }
  return 0;
}


__declspec(naked) void hudpanel_marker_ray_cast(void)
{
    __asm {
        push ecx
        push edx
        push eax
        call hudpanel_marker_ray_cast_c
        add esp, 12
        ret
    }
}

/* hudpanel_slot_clear */
void hudpanel_slot_clear_c(int reg_eax);
void hudpanel_slot_clear_c(int reg_eax)
{
  *(undefined4 *)(reg_eax + 0x44) = 0;
  *(undefined4 *)(reg_eax + 0x48) = 0;
  *(undefined4 *)(reg_eax + 0x58) = 0;
  *(undefined4 *)(reg_eax + 0x5c) = 0;
  *(undefined4 *)(reg_eax + 0x50) = 0;
  *(undefined4 *)(reg_eax + 0x54) = 0;
  trail_set_param(*(undefined4 *)(reg_eax + 0x4c),2,0);
  sfx_voice_is_active(0x14,0);
  sfx_voice_is_active(0x14,1);
  sfx_voice_is_active(0x14,2);
  sfx_voice_is_active(0x14,3);
  return;
}


__declspec(naked) void hudpanel_slot_clear(void)
{
    __asm {
        push eax
        call hudpanel_slot_clear_c
        add esp, 4
        ret
    }
}

/* hudpanel_slot_attach_clump */
void hudpanel_slot_attach_clump_c(int reg_eax, int reg_ebx, undefined4 param_3);
void hudpanel_slot_attach_clump_c(int reg_eax, int reg_ebx, undefined4 param_3)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int local_18 [6];
  
  FUN_004c3f90(reg_eax,local_18);
  *(undefined4 *)(reg_ebx + 0x48) = param_3;
  *(int *)(reg_ebx + 0x40) = reg_eax;
  *(undefined4 *)(reg_ebx + 0x44) = *(undefined4 *)(reg_eax + 4);
  FUN_004c66a0(reg_ebx + 0x20,0x20);
  iVar3 = 0;
  do {
    iVar1 = local_18[iVar3];
    iVar2 = FUN_004c52b0(iVar1,0,0);
    *(int *)(reg_ebx + 0x20 + iVar2 * 4) = iVar1;
    FUN_004c53e0(*(undefined4 *)(iVar1 + 0x18),0x40,1);
    iVar3 = iVar3 + 1;
  } while (iVar3 < 6);
  return;
}


__declspec(naked) void hudpanel_slot_attach_clump(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ebx
        push eax
        call hudpanel_slot_attach_clump_c
        add esp, 12
        ret
    }
}

/* hudpanel_counters_clear */
void hudpanel_counters_clear_c(int reg_eax);
void hudpanel_counters_clear_c(int reg_eax)
{
  *(undefined4 *)(reg_eax + 0x4c) = 0;
  *(undefined4 *)(reg_eax + 0x50) = 0;
  return;
}


__declspec(naked) void hudpanel_counters_clear(void)
{
    __asm {
        push eax
        push edx
        push eax
        call hudpanel_counters_clear_c
        add esp, 4
        pop edx
        pop eax
        ret
    }
}

/* hudpanel_render_parts_if_active */
void hudpanel_render_parts_if_active_c(int * reg_eax);
void hudpanel_render_parts_if_active_c(int * reg_eax)
{
  if (reg_eax[0x14] != 0) {
    if (*reg_eax != 0) {
      (**(code **)(reg_eax[8] + 0x48))(reg_eax[8]);
    }
    if (reg_eax[1] != 0) {
      (**(code **)(reg_eax[9] + 0x48))(reg_eax[9]);
    }
    if (reg_eax[2] != 0) {
      (**(code **)(reg_eax[10] + 0x48))(reg_eax[10]);
    }
    if (reg_eax[3] != 0) {
      (**(code **)(reg_eax[0xb] + 0x48))(reg_eax[0xb]);
    }
    if (reg_eax[4] != 0) {
      (**(code **)(reg_eax[0xc] + 0x48))(reg_eax[0xc]);
    }
    if (reg_eax[5] != 0) {
      (**(code **)(reg_eax[0xd] + 0x48))(reg_eax[0xd]);
    }
  }
  return;
}


__declspec(naked) void hudpanel_render_parts_if_active(void)
{
    __asm {
        push eax
        call hudpanel_render_parts_if_active_c
        add esp, 4
        ret
    }
}

/* time_trial_panel_step_part_b */
void time_trial_panel_step_part_b_c(int reg_edi);
void time_trial_panel_step_part_b_c(int reg_edi)
{
  int iVar1;
  int iVar2;
  RwReal RVar3;
  float local_50;
  RwV3d local_4c;
  RwMatrix local_40;       /* the callee writes the whole 64 byte matrix; the code reads its position (+0x30) behind the 48 byte part */
  
  vehicle_get_matrix_local(0,(undefined4 *)&local_40);
  iVar2 = 0;
  do {
    iVar1 = *(int *)(reg_edi + iVar2 * 4);
    local_4c.x = local_40.pos.x - *(float *)(iVar1 + 0x30);
    local_4c.y = local_40.pos.y - *(float *)(iVar1 + 0x34);
    local_4c.z = local_40.pos.z - *(float *)(iVar1 + 0x38);
    RVar3 = RwV3dLength(&local_4c);
    local_50 = RVar3 * k_f_0_1;
    if (k_f_0_5 < local_50 == (k_f_0_5 == local_50)) {
      if (local_50 < k_f_0) {
        local_50 = 0.0;
      }
    }
    else {
      local_50 = 0.5;
    }
    btb_panel_slot_set_value(iVar2,local_50);
    iVar2 = iVar2 + 1;
  } while (iVar2 < 2);
  return;
}


__declspec(naked) void time_trial_panel_step_part_b(void)
{
    __asm {
        push edi
        call time_trial_panel_step_part_b_c
        add esp, 4
        ret
    }
}

/* score_panel_attach_clump */
void score_panel_attach_clump_c(int reg_eax, int reg_ebx);
void score_panel_attach_clump_c(int reg_eax, int reg_ebx)
{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 local_44 [17];
  
  FUN_004c3f90(reg_eax,local_44);
  *(int *)(reg_ebx + 0x5c) = reg_eax;
  *(undefined4 *)(reg_ebx + 0x60) = *(undefined4 *)(reg_eax + 4);
  FUN_004c66a0(reg_ebx,0x50);
  iVar3 = 0;
  do {
    uVar1 = local_44[iVar3];
    iVar2 = FUN_004c52b0(uVar1,0,0);
    iVar3 = iVar3 + 1;
    *(undefined4 *)(reg_ebx + iVar2 * 4) = uVar1;
  } while (iVar3 < 0x11);
  return;
}


__declspec(naked) void score_panel_attach_clump(void)
{
    __asm {
        push ebx
        push eax
        call score_panel_attach_clump_c
        add esp, 8
        ret
    }
}

/* score_panel_entry_init */
void score_panel_entry_init_c(int reg_esi);
void score_panel_entry_init_c(int reg_esi)
{
  undefined4 local_4;
  
  *(undefined4 *)(reg_esi + 0x68) = 0;
  *(undefined4 *)(reg_esi + 0x6c) = 0;
  local_4 = 0xff323232;
  *(int *)(reg_esi + 0x68) = 1 << ((byte)g_player_slots[*(int *)(reg_esi + 100)].team & 0x1f);
  FUN_004c5380(*(undefined4 *)(reg_esi + 0x14),&local_4);
  FUN_004c5380(*(undefined4 *)(reg_esi + 8),&local_4);
  if (*(int *)(reg_esi + 100) < g_score_panel_658dbc) {
    *(undefined4 *)(reg_esi + 0x6c) = 1;
  }
  return;
}


__declspec(naked) void score_panel_entry_init(void)
{
    __asm {
        push esi
        call score_panel_entry_init_c
        add esp, 4
        ret
    }
}

/* score_panel_entry_reset */
void score_panel_entry_reset_c(int reg_ecx);
void score_panel_entry_reset_c(int reg_ecx)
{
  float *pfVar1;
  int iVar2;
  
  pfVar1 = (float *)(reg_ecx + 0x50);
  iVar2 = *(int *)(reg_ecx + 100) * 0xc;
  *pfVar1 = *(float *)(&g_score_panel_603ed4 + iVar2);
  *(undefined4 *)(reg_ecx + 0x54) = *(undefined4 *)(&g_score_panel_603ed8 + iVar2);
  *(undefined4 *)(reg_ecx + 0x58) = *(undefined4 *)(&g_score_panel_603edc + iVar2);
  *(undefined4 *)(reg_ecx + 0x70) = 0;
  *pfVar1 = *pfVar1 + *pfVar1;
  return;
}


__declspec(naked) void score_panel_entry_reset(void)
{
    __asm {
        push ecx
        push ecx
        call score_panel_entry_reset_c
        add esp, 4
        pop ecx
        ret
    }
}

/* score_panel_entry_scale_parts_low */
void score_panel_entry_scale_parts_low_c(int reg_esi, RwV3d * reg_edi);
void score_panel_entry_scale_parts_low_c(int reg_esi, RwV3d * reg_edi)
{
  if ((*(byte *)(reg_esi + 0x68) & 1) != 0) {
    RwFrameScale(*(RwFrame **)(*(int *)(reg_esi + 0x1c) + 4),reg_edi,rwCOMBINEREPLACE);
  }
  if ((*(byte *)(reg_esi + 0x68) & 2) != 0) {
    RwFrameScale(*(RwFrame **)(*(int *)(reg_esi + 0x20) + 4),reg_edi,rwCOMBINEREPLACE);
  }
  if ((*(byte *)(reg_esi + 0x68) & 4) != 0) {
    RwFrameScale(*(RwFrame **)(*(int *)(reg_esi + 0x24) + 4),reg_edi,rwCOMBINEREPLACE);
  }
  if ((*(byte *)(reg_esi + 0x68) & 8) != 0) {
    RwFrameScale(*(RwFrame **)(*(int *)(reg_esi + 0x28) + 4),reg_edi,rwCOMBINEREPLACE);
  }
  if ((*(byte *)(reg_esi + 0x68) & 0x10) != 0) {
    RwFrameScale(*(RwFrame **)(*(int *)(reg_esi + 0x2c) + 4),reg_edi,rwCOMBINEREPLACE);
  }
  if ((*(byte *)(reg_esi + 0x68) & 0x20) != 0) {
    RwFrameScale(*(RwFrame **)(*(int *)(reg_esi + 0x30) + 4),reg_edi,rwCOMBINEREPLACE);
  }
  return;
}


__declspec(naked) void score_panel_entry_scale_parts_low(void)
{
    __asm {
        push edi
        push esi
        call score_panel_entry_scale_parts_low_c
        add esp, 8
        ret
    }
}

/* score_panel_entry_scale_parts */
void score_panel_entry_scale_parts_c(int reg_esi, RwV3d * reg_edi);
void score_panel_entry_scale_parts_c(int reg_esi, RwV3d * reg_edi)
{
  if ((*(uint *)(reg_esi + 0x68) & 0x2000) != 0) {
    RwFrameScale(*(RwFrame **)(*(int *)(reg_esi + 0x34) + 4),reg_edi,rwCOMBINEREPLACE);
  }
  if ((*(uint *)(reg_esi + 0x68) & 0x4000) != 0) {
    RwFrameScale(*(RwFrame **)(*(int *)(reg_esi + 0x38) + 4),reg_edi,rwCOMBINEREPLACE);
  }
  if ((char)((uint)*(undefined4 *)(reg_esi + 0x68) >> 8) < '\0') {
    RwFrameScale(*(RwFrame **)(*(int *)(reg_esi + 0x3c) + 4),reg_edi,rwCOMBINEREPLACE);
  }
  return;
}


__declspec(naked) void score_panel_entry_scale_parts(void)
{
    __asm {
        push edi
        push esi
        call score_panel_entry_scale_parts_c
        add esp, 8
        ret
    }
}

/* score_panel_entry_destroy */
void score_panel_entry_destroy_c(int reg_eax);
void score_panel_entry_destroy_c(int reg_eax)
{
  RpClumpDestroy(*(RpClump **)(reg_eax + 0x5c));
  return;
}


__declspec(naked) void score_panel_entry_destroy(void)
{
    __asm {
        push eax
        call score_panel_entry_destroy_c
        add esp, 4
        ret
    }
}

/* team_panel_load_parts */
void team_panel_load_parts_c(int reg_eax, int reg_ebx);
void team_panel_load_parts_c(int reg_eax, int reg_ebx)
{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined1 local_78;
  undefined1 local_77;
  undefined1 local_76;
  undefined1 local_75;
  undefined4 local_74 [29];
  
  local_78 = 0x32;
  local_77 = 0x32;
  local_76 = 0x32;
  local_75 = 0xff;
  FUN_004c3f90(reg_eax,local_74);
  *(int *)(reg_ebx + 0x15c) = reg_eax;
  *(undefined4 *)(reg_ebx + 0x160) = *(undefined4 *)(reg_eax + 4);
  FUN_004c66a0(reg_ebx + 0xb0,0xa0);
  iVar3 = 0;
  do {
    uVar1 = local_74[iVar3];
    iVar2 = FUN_004c52b0(uVar1,0,0);
    iVar3 = iVar3 + 1;
    *(undefined4 *)(reg_ebx + 0xb0 + iVar2 * 4) = uVar1;
  } while (iVar3 < 0x1d);
  FUN_004c5380(*(undefined4 *)(reg_ebx + 0xb8),&local_78);
  return;
}


__declspec(naked) void team_panel_load_parts(void)
{
    __asm {
        push ebx
        push eax
        call team_panel_load_parts_c
        add esp, 8
        ret
    }
}

/* team_panel_build_team_mask */
void team_panel_build_team_mask_c(int reg_eax);
void team_panel_build_team_mask_c(int reg_eax)
{
  int iVar1;
  byte bVar2;
  int iVar3;
  char cVar4;
  
  iVar1 = *(int *)(reg_eax + 0xc);
  *(int *)(reg_eax + 0x168) = 0x800000 << ((char)iVar1 - 1U & 0x1f);
  iVar3 = 0;
  if (0 < iVar1) {
    cVar4 = '\0';
    do {
      bVar2 = (char)g_player_slots[*(int *)(reg_eax + iVar3 * 4)].team + cVar4;
      cVar4 = cVar4 + '\x06';
      iVar3 = iVar3 + 1;
      *(uint *)(reg_eax + 0x168) = *(uint *)(reg_eax + 0x168) | 1 << (bVar2 & 0x1f);
    } while (iVar3 < iVar1);
  }
  return;
}


__declspec(naked) void team_panel_build_team_mask(void)
{
    __asm {
        push eax
        push eax
        call team_panel_build_team_mask_c
        add esp, 4
        pop eax
        ret
    }
}

/* team_panel_load_team_colour */
void team_panel_load_team_colour_c(int reg_ecx);
void team_panel_load_team_colour_c(int reg_ecx)
{
  int iVar1;
  
  iVar1 = *(int *)(reg_ecx + 0x164);
  *(undefined4 *)(reg_ecx + 0x150) = (&g_race_start_603f1c)[iVar1 * 3];
  *(undefined4 *)(reg_ecx + 0x154) = (&g_race_start_603f20)[iVar1 * 3];
  *(undefined4 *)(reg_ecx + 0x158) = *(undefined4 *)(&g_race_start_603f24 + iVar1 * 0xc);
  return;
}


__declspec(naked) void team_panel_load_team_colour(void)
{
    __asm {
        push ecx
        call team_panel_load_team_colour_c
        add esp, 4
        ret
    }
}

/* team_panel_entry_render */
void team_panel_entry_render_c(int reg_eax);
void team_panel_entry_render_c(int reg_eax)
{
  if (*(int *)(reg_eax + 0x1c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xbc) + 0x48))(*(int *)(reg_eax + 0xbc));
  }
  if (*(int *)(reg_eax + 0x18) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xb8) + 0x48))(*(int *)(reg_eax + 0xb8));
  }
  if (*(int *)(reg_eax + 0x10) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xb0) + 0x48))(*(int *)(reg_eax + 0xb0));
  }
  if (*(int *)(reg_eax + 0x14) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xb4) + 0x48))(*(int *)(reg_eax + 0xb4));
  }
  if (*(int *)(reg_eax + 0x20) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xc0) + 0x48))(*(int *)(reg_eax + 0xc0));
  }
  if (*(int *)(reg_eax + 0x80) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x120) + 0x48))(*(int *)(reg_eax + 0x120));
  }
  if (*(int *)(reg_eax + 0x84) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x124) + 0x48))(*(int *)(reg_eax + 0x124));
  }
  if (*(int *)(reg_eax + 0x88) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x128) + 0x48))(*(int *)(reg_eax + 0x128));
  }
  if (*(int *)(reg_eax + 0x8c) != 0) {
    (**(code **)(*(int *)(reg_eax + 300) + 0x48))(*(int *)(reg_eax + 300));
  }
  if (*(int *)(reg_eax + 0x90) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x130) + 0x48))(*(int *)(reg_eax + 0x130));
  }
  if (*(int *)(reg_eax + 0x94) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x134) + 0x48))(*(int *)(reg_eax + 0x134));
  }
  if (*(int *)(reg_eax + 0x68) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x108) + 0x48))(*(int *)(reg_eax + 0x108));
  }
  if (*(int *)(reg_eax + 0x6c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x10c) + 0x48))(*(int *)(reg_eax + 0x10c));
  }
  if (*(int *)(reg_eax + 0x70) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x110) + 0x48))(*(int *)(reg_eax + 0x110));
  }
  if (*(int *)(reg_eax + 0x74) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x114) + 0x48))(*(int *)(reg_eax + 0x114));
  }
  if (*(int *)(reg_eax + 0x78) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x118) + 0x48))(*(int *)(reg_eax + 0x118));
  }
  if (*(int *)(reg_eax + 0x7c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x11c) + 0x48))(*(int *)(reg_eax + 0x11c));
  }
  if (*(int *)(reg_eax + 0x3c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xdc) + 0x48))(*(int *)(reg_eax + 0xdc));
  }
  if (*(int *)(reg_eax + 0x40) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xe0) + 0x48))(*(int *)(reg_eax + 0xe0));
  }
  if (*(int *)(reg_eax + 0x44) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xe4) + 0x48))(*(int *)(reg_eax + 0xe4));
  }
  if (*(int *)(reg_eax + 0x48) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xe8) + 0x48))(*(int *)(reg_eax + 0xe8));
  }
  if (*(int *)(reg_eax + 0x4c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xec) + 0x48))(*(int *)(reg_eax + 0xec));
  }
  if (*(int *)(reg_eax + 0x50) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xf0) + 0x48))(*(int *)(reg_eax + 0xf0));
  }
  if (*(int *)(reg_eax + 0x24) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xc4) + 0x48))(*(int *)(reg_eax + 0xc4));
  }
  if (*(int *)(reg_eax + 0x28) != 0) {
    (**(code **)(*(int *)(reg_eax + 200) + 0x48))(*(int *)(reg_eax + 200));
  }
  if (*(int *)(reg_eax + 0x2c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xcc) + 0x48))(*(int *)(reg_eax + 0xcc));
  }
  if (*(int *)(reg_eax + 0x30) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xd0) + 0x48))(*(int *)(reg_eax + 0xd0));
  }
  if (*(int *)(reg_eax + 0x34) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xd4) + 0x48))(*(int *)(reg_eax + 0xd4));
  }
  if (*(int *)(reg_eax + 0x38) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xd8) + 0x48))(*(int *)(reg_eax + 0xd8));
  }
  return;
}


__declspec(naked) void team_panel_entry_render(void)
{
    __asm {
        push eax
        call team_panel_entry_render_c
        add esp, 4
        ret
    }
}

/* team_panel_entry_destroy */
void team_panel_entry_destroy_c(int reg_eax);
void team_panel_entry_destroy_c(int reg_eax)
{
  RpClumpDestroy(*(RpClump **)(reg_eax + 0x15c));
  return;
}


__declspec(naked) void team_panel_entry_destroy(void)
{
    __asm {
        push eax
        call team_panel_entry_destroy_c
        add esp, 4
        ret
    }
}

/* hudpanel_render_group */
void hudpanel_render_group_c(int * reg_eax);
void hudpanel_render_group_c(int * reg_eax)
{
  (**(code **)(reg_eax[3] + 0x48))(reg_eax[3]);
  (**(code **)(reg_eax[1] + 0x48))(reg_eax[1]);
  (**(code **)(*reg_eax + 0x48))(*reg_eax);
  (**(code **)(reg_eax[2] + 0x48))(reg_eax[2]);
  return;
}


__declspec(naked) void hudpanel_render_group(void)
{
    __asm {
        push eax
        call hudpanel_render_group_c
        add esp, 4
        ret
    }
}

/* endpoint_panel_attach_clump */
void endpoint_panel_attach_clump_c(int reg_eax, int reg_ebx);
void endpoint_panel_attach_clump_c(int reg_eax, int reg_ebx)
{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 local_60 [24];
  
  FUN_004c3f90(reg_eax,local_60);
  *(int *)(reg_ebx + 0x100) = reg_eax;
  *(undefined4 *)(reg_ebx + 0x104) = *(undefined4 *)(reg_eax + 4);
  FUN_004c66a0(reg_ebx + 0x80,0x80);
  iVar3 = 0;
  do {
    uVar1 = local_60[iVar3];
    iVar2 = FUN_004c52b0(uVar1,0,0);
    iVar3 = iVar3 + 1;
    *(undefined4 *)(reg_ebx + 0x80 + iVar2 * 4) = uVar1;
  } while (iVar3 < 0x18);
  return;
}


__declspec(naked) void endpoint_panel_attach_clump(void)
{
    __asm {
        push ebx
        push eax
        call endpoint_panel_attach_clump_c
        add esp, 8
        ret
    }
}

/* team_endpoint_panel_load_parts */
void team_endpoint_panel_load_parts_c(int reg_eax, int reg_ebx);
void team_endpoint_panel_load_parts_c(int reg_eax, int reg_ebx)
{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined1 local_8c;
  undefined1 local_8b;
  undefined1 local_8a;
  undefined1 local_89;
  undefined4 local_88 [34];
  
  local_8c = 0x32;
  local_8b = 0x32;
  local_8a = 0x32;
  local_89 = 0xff;
  FUN_004c3f90(reg_eax,local_88);
  *(int *)(reg_ebx + 0x150) = reg_eax;
  *(undefined4 *)(reg_ebx + 0x154) = *(undefined4 *)(reg_eax + 4);
  FUN_004c66a0(reg_ebx + 0xb0,0xa0);
  iVar3 = 0;
  do {
    uVar1 = local_88[iVar3];
    iVar2 = FUN_004c52b0(uVar1,0,0);
    iVar3 = iVar3 + 1;
    *(undefined4 *)(reg_ebx + 0xb0 + iVar2 * 4) = uVar1;
  } while (iVar3 < 0x22);
  FUN_004c5380(*(undefined4 *)(reg_ebx + 0xb8),&local_8c);
  return;
}


__declspec(naked) void team_endpoint_panel_load_parts(void)
{
    __asm {
        push ebx
        push eax
        call team_endpoint_panel_load_parts_c
        add esp, 8
        ret
    }
}

/* endpoint_panel_build_team_mask */
void endpoint_panel_build_team_mask_c(int reg_eax);
void endpoint_panel_build_team_mask_c(int reg_eax)
{
  int iVar1;
  byte bVar2;
  int iVar3;
  char cVar4;
  
  iVar1 = *(int *)(reg_eax + 0xc);
  *(int *)(reg_eax + 0x15c) = 0x800000 << ((char)iVar1 - 1U & 0x1f);
  iVar3 = 0;
  if (0 < iVar1) {
    cVar4 = '\0';
    do {
      bVar2 = (char)g_player_slots[*(int *)(reg_eax + iVar3 * 4)].team + cVar4;
      cVar4 = cVar4 + '\x06';
      iVar3 = iVar3 + 1;
      *(uint *)(reg_eax + 0x15c) = *(uint *)(reg_eax + 0x15c) | 1 << (bVar2 & 0x1f);
    } while (iVar3 < iVar1);
  }
  return;
}


__declspec(naked) void endpoint_panel_build_team_mask(void)
{
    __asm {
        push eax
        push eax
        call endpoint_panel_build_team_mask_c
        add esp, 4
        pop eax
        ret
    }
}

/* endpoint_panel_entry_render */
void endpoint_panel_entry_render_c(int reg_eax);
void endpoint_panel_entry_render_c(int reg_eax)
{
  if (*(int *)(reg_eax + 0x1c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xbc) + 0x48))(*(int *)(reg_eax + 0xbc));
  }
  if (*(int *)(reg_eax + 0x18) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xb8) + 0x48))(*(int *)(reg_eax + 0xb8));
  }
  if (*(int *)(reg_eax + 0x10) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xb0) + 0x48))(*(int *)(reg_eax + 0xb0));
  }
  if (*(int *)(reg_eax + 0x14) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xb4) + 0x48))(*(int *)(reg_eax + 0xb4));
  }
  if (*(int *)(reg_eax + 0x20) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xc0) + 0x48))(*(int *)(reg_eax + 0xc0));
  }
  if (*(int *)(reg_eax + 0x54) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xf4) + 0x48))(*(int *)(reg_eax + 0xf4));
  }
  if (*(int *)(reg_eax + 0x58) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xf8) + 0x48))(*(int *)(reg_eax + 0xf8));
  }
  if (*(int *)(reg_eax + 0x5c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xfc) + 0x48))(*(int *)(reg_eax + 0xfc));
  }
  if (*(int *)(reg_eax + 0x60) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x100) + 0x48))(*(int *)(reg_eax + 0x100));
  }
  if (*(int *)(reg_eax + 100) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x104) + 0x48))(*(int *)(reg_eax + 0x104));
  }
  if (*(int *)(reg_eax + 0x80) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x120) + 0x48))(*(int *)(reg_eax + 0x120));
  }
  if (*(int *)(reg_eax + 0x84) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x124) + 0x48))(*(int *)(reg_eax + 0x124));
  }
  if (*(int *)(reg_eax + 0x88) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x128) + 0x48))(*(int *)(reg_eax + 0x128));
  }
  if (*(int *)(reg_eax + 0x8c) != 0) {
    (**(code **)(*(int *)(reg_eax + 300) + 0x48))(*(int *)(reg_eax + 300));
  }
  if (*(int *)(reg_eax + 0x90) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x130) + 0x48))(*(int *)(reg_eax + 0x130));
  }
  if (*(int *)(reg_eax + 0x94) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x134) + 0x48))(*(int *)(reg_eax + 0x134));
  }
  if (*(int *)(reg_eax + 0x68) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x108) + 0x48))(*(int *)(reg_eax + 0x108));
  }
  if (*(int *)(reg_eax + 0x6c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x10c) + 0x48))(*(int *)(reg_eax + 0x10c));
  }
  if (*(int *)(reg_eax + 0x70) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x110) + 0x48))(*(int *)(reg_eax + 0x110));
  }
  if (*(int *)(reg_eax + 0x74) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x114) + 0x48))(*(int *)(reg_eax + 0x114));
  }
  if (*(int *)(reg_eax + 0x78) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x118) + 0x48))(*(int *)(reg_eax + 0x118));
  }
  if (*(int *)(reg_eax + 0x7c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0x11c) + 0x48))(*(int *)(reg_eax + 0x11c));
  }
  if (*(int *)(reg_eax + 0x3c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xdc) + 0x48))(*(int *)(reg_eax + 0xdc));
  }
  if (*(int *)(reg_eax + 0x40) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xe0) + 0x48))(*(int *)(reg_eax + 0xe0));
  }
  if (*(int *)(reg_eax + 0x44) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xe4) + 0x48))(*(int *)(reg_eax + 0xe4));
  }
  if (*(int *)(reg_eax + 0x48) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xe8) + 0x48))(*(int *)(reg_eax + 0xe8));
  }
  if (*(int *)(reg_eax + 0x4c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xec) + 0x48))(*(int *)(reg_eax + 0xec));
  }
  if (*(int *)(reg_eax + 0x50) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xf0) + 0x48))(*(int *)(reg_eax + 0xf0));
  }
  if (*(int *)(reg_eax + 0x24) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xc4) + 0x48))(*(int *)(reg_eax + 0xc4));
  }
  if (*(int *)(reg_eax + 0x28) != 0) {
    (**(code **)(*(int *)(reg_eax + 200) + 0x48))(*(int *)(reg_eax + 200));
  }
  if (*(int *)(reg_eax + 0x2c) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xcc) + 0x48))(*(int *)(reg_eax + 0xcc));
  }
  if (*(int *)(reg_eax + 0x30) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xd0) + 0x48))(*(int *)(reg_eax + 0xd0));
  }
  if (*(int *)(reg_eax + 0x34) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xd4) + 0x48))(*(int *)(reg_eax + 0xd4));
  }
  if (*(int *)(reg_eax + 0x38) != 0) {
    (**(code **)(*(int *)(reg_eax + 0xd8) + 0x48))(*(int *)(reg_eax + 0xd8));
  }
  return;
}


__declspec(naked) void endpoint_panel_entry_render(void)
{
    __asm {
        push eax
        call endpoint_panel_entry_render_c
        add esp, 4
        ret
    }
}

