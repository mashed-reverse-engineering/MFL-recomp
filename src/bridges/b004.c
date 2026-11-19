#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*004b322b*/ extern int FUN_004b322b();
/*004c40c0*/ extern int FUN_004c40c0();
/*004c4280*/ extern int FUN_004c4280();
/*004c45e0*/ extern double FUN_004c45e0();
/*004c5360*/ extern int FUN_004c5360();
/*004c66a0*/ extern int FUN_004c66a0();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d3cc0*/ extern int FUN_004d3cc0();
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f4920*/ extern int FUN_004f4920();
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*00545d00*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*00562d20*/ extern int Rt2dCTMSetIdentity();
/*00562e10*/ extern int Rt2dCTMRotate();
/*005632e0*/ extern int Rt2dCTMPush();
/*00563340*/ extern int FUN_00563340();
/*005633c0*/ extern int Rt2dCTMTranslate();
/*00564b10*/ extern int Rt2dBrushSetRGBA();
/*00565f80*/ extern double Rt2dFontGetStringWidth();
/*00567760*/ extern int FUN_00567760();
/*00567780*/ extern int FUN_00567780();
/*005dd32c*/ extern float k_f_3;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd624*/ extern float k_f_0_0020833334;
/*005dd628*/ extern float k_f_0_25;
/*005ddaf0*/ extern float k_f_1_0en5;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb00*/ extern float k_f_0_025;
/*005ddb34*/ extern float k_f_0_95;
/*005e64a4*/ extern float k_f_0;
/*005de258*/ extern float k_f_0_003;
/*005de47c*/ extern float k_f_n0_25;
/*005df2d4*/ extern float k_f_1_333;
/*005dfde4*/ extern float k_f_1_33;
/*005e06e8*/ extern undefined g_smplfzx_retire_5e06e8;
/*005e20fc*/ extern undefined4 g_app_5e20fc;
/*005e215c*/ extern undefined4 g_app_5e215c;
/*005e218c*/ extern undefined4 g_app_5e218c;
/*005e64a4*/ extern undefined4 k_f_0;
/*0060dc20*/ extern undefined4 weapon_descriptors;
/*0060de60*/ extern undefined4 weapon_descriptor_count;
/*00617948*/ extern undefined4 g_sound_master_fade;
/*006c2688*/ extern undefined4 g_car_6c2688;
/*006c28c0*/ extern undefined4 g_car_6c28c0;
/*006c2ed8*/ extern undefined4 g_text_state;
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/*006c5e20*/ extern undefined g_gfx_6c5e20;
/*006f1e20*/ extern undefined1 g_gfx_6f1e20;
/*00702268*/ extern undefined4 g_sector_table_702268;
/*00702668*/ extern undefined g_sector_table_702668;
/*0071abf0*/ extern undefined4 g_smplfzx_71abf0;
/*0071abf4*/ extern undefined4 g_particles_71abf4;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007e8b64*/ extern undefined4 g_car_7e8b64;
/*007e8b68*/ extern undefined4 g_car_7e8b68;
/*007e8b6c*/ extern undefined4 g_car_7e8b6c;
/*007e8b70*/ extern undefined4 g_car_7e8b70;
/*007e8b74*/ extern undefined4 g_car_7e8b74;
/*007e8b78*/ extern undefined4 g_car_7e8b78;
/*007e8b7c*/ extern undefined4 g_car_7e8b7c;
/*007e8b80*/ extern undefined4 g_car_7e8b80;
/*007eab64*/ extern undefined4 g_car_7eab64;
/*007ef7f4*/ extern undefined4 g_tick_seconds;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/*00843ea0*/ extern undefined g_collision_triangles;
/*008bd740*/ extern undefined g_car_8bd740;
extern undefined4 bbox_plane_corner_test(undefined4 param_1, undefined4 * param_2);
extern void car_points_vs_dynamic_objects(float * param_1);
extern void car_points_vs_triangles(int * param_1, int param_2);
extern void debug_printf(undefined4 param_1, ...);
extern float10 distance_between_points(float * param_1, float * param_2, float * param_3);
extern undefined4 get_viewport_camera(int param_1);
extern undefined4 pool_list_unlink(short * param_1, int param_2);
extern undefined4 pool_push(int * param_1, undefined4 param_2);
extern undefined4 race_get_phase(void);
extern undefined4 smplfzx_get_object(uint param_1);
extern undefined4 sphere_plane_side(undefined4 param_1, int param_2);
static void effect_sprite_reset_r(undefined4 * reg_eax);
static float10 powerup_direction_and_distance_r(int * reg_eax, RwV3d * reg_esi);
extern float10 powerup_direction_and_distance_c(int * reg_eax, RwV3d * reg_esi);
static void rt2d_text_pixel_to_uv_r(float * reg_eax, float param_2, float param_3);
#include "bridge_sel.h"

/* effect_sprite_reset_r */
extern void effect_sprite_reset(void);
__declspec(naked) void effect_sprite_reset_r(undefined4 * reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call effect_sprite_reset
        ret
    }
}

/* powerup_direction_and_distance_r */
extern void powerup_direction_and_distance(void);
__declspec(naked) float10 powerup_direction_and_distance_r(int * reg_eax, RwV3d * reg_esi)
{
    __asm {
        push esi
        mov eax, dword ptr [esp + 8]
        mov esi, dword ptr [esp + 12]
        call powerup_direction_and_distance
        pop esi
        ret
    }
}

/* rt2d_text_pixel_to_uv_r */
extern void rt2d_text_pixel_to_uv(void);
__declspec(naked) void rt2d_text_pixel_to_uv_r(float * reg_eax, float param_2, float param_3)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov eax, dword ptr [esp + 12]
        call rt2d_text_pixel_to_uv
        add esp, 8
        ret
    }
}

/* powerup_move_and_spin */
void powerup_move_and_spin_c(int * reg_edi);
void powerup_move_and_spin_c(int * reg_edi)
{
  float fVar1;
  RwFrame *frame;
  float10 fVar2;
  float local_c;
  float local_8;
  float local_4;
  
  frame = *(RwFrame **)(*reg_edi + 4);
  ((RwV3d *)(reg_edi + 0xe))->x = (float)reg_edi[0x11] + (float)reg_edi[0xe];
  reg_edi[0xf] = (int)((float)reg_edi[0x12] + (float)reg_edi[0xf]);
  reg_edi[0x10] = (int)((float)reg_edi[0x13] + (float)reg_edi[0x10]);
  reg_edi[0x11] = (int)((float)reg_edi[0x11] * k_f_0_95);
  reg_edi[0x12] = (int)((float)reg_edi[0x12] * k_f_0_95);
  reg_edi[0x13] = (int)((float)reg_edi[0x13] * k_f_0_95);
  RwFrameTranslate(frame,(RwV3d *)(reg_edi + 0xe),rwCOMBINEREPLACE);
  fVar1 = (float)reg_edi[1];
  reg_edi[1] = (int)((float)reg_edi[2] + fVar1);
  RwFrameRotate(frame,(RwV3d *)(reg_edi + 3),(float)reg_edi[2] + fVar1,rwCOMBINEPRECONCAT);
  if ((float)reg_edi[0x13] * (float)reg_edi[0x13] +
      (float)reg_edi[0x11] * (float)reg_edi[0x11] + (float)reg_edi[0x12] * (float)reg_edi[0x12] <
      k_f_1_0en5) {
    fVar2 = (float10)BR_powerup_direction_and_distance(reg_edi,&local_c);
    fVar2 = fVar2 * (float10)k_f_0_05;
    reg_edi[0x11] = (int)(float)((float10)local_c * fVar2);
    reg_edi[0x12] = (int)(float)((float10)local_8 * fVar2);
    reg_edi[0x13] = (int)(float)(fVar2 * (float10)local_4);
  }
  return;
}


__declspec(naked) void powerup_move_and_spin(void)
{
    __asm {
        push edi
        call powerup_move_and_spin_c
        add esp, 4
        ret
    }
}

/* weapon_effect_scale_by_camera_distance */
void weapon_effect_scale_by_camera_distance_c(int reg_edi);
void weapon_effect_scale_by_camera_distance_c(int reg_edi)
{
  int camera;
  RwMatrix *matrix;
  float10 fVar1;
  RwV3d local_c;
  
  local_c.z = *(float *)(reg_edi + 0x1c) * k_f_4;
  local_c.x = 4.0;
  local_c.y = 4.0;
  camera = get_viewport_camera(0);
  fVar1 = (float10)distance_between_points
                             (reg_edi + 0x48,reg_edi + 0x3c,*(int *)(camera + 4) + 0x40);
  if ((float10)k_f_0_003 <= fVar1) {
    if ((float10)k_f_3 < fVar1) {
      fVar1 = (float10)k_f_3;
    }
  }
  else {
    fVar1 = (float10)k_f_0_003;
  }
  local_c.x = (RwReal)((float10)local_c.x * fVar1 * (float10)k_f_0_5);
  local_c.y = (RwReal)(fVar1 * (float10)k_f_0_5 * (float10)local_c.y);
  matrix = (RwMatrix *)(*(int *)(*(int *)(reg_edi + 0x54) + 4) + 0x10);
  RwMatrixScale(matrix,&local_c,rwCOMBINEPRECONCAT);
  FUN_004d3cc0(matrix);
  RwFrameUpdateObjects(*(RwFrame **)(*(int *)(reg_edi + 0x54) + 4));
  return;
}


__declspec(naked) void weapon_effect_scale_by_camera_distance(void)
{
    __asm {
        push edi
        call weapon_effect_scale_by_camera_distance_c
        add esp, 4
        ret
    }
}

/* weapon_fx_clump_remove_c */
void weapon_fx_clump_remove_c_c(int reg_esi);
void weapon_fx_clump_remove_c_c(int reg_esi)
{
  RpWorld *world;
  
  *(undefined4 *)(reg_esi + 0xc) = 0;
  *(undefined4 *)(reg_esi + 0x10) = 0;
  *(undefined4 *)(reg_esi + 0x14) = 0;
  *(undefined4 *)(reg_esi + 0x18) = 0;
  world = (RpWorld *)FUN_004f4920(*(undefined4 *)(reg_esi + 0x1c));
  if (world != (RpWorld *)0x0) {
    RpWorldRemoveClump(world,*(RpClump **)(reg_esi + 0x1c));
  }
  return;
}


__declspec(naked) void weapon_fx_clump_remove_c(void)
{
    __asm {
        push esi
        call weapon_fx_clump_remove_c_c
        add esp, 4
        ret
    }
}

/* weapon_fx_state_clear */
void weapon_fx_state_clear_c(undefined4 * reg_esi);
void weapon_fx_state_clear_c(undefined4 * reg_esi)
{
  undefined4 uVar1;
  
  uVar1 = reg_esi[0x2c];
  FUN_004c66a0(reg_esi,0xb4);
  reg_esi[4] = 0;
  reg_esi[2] = 0;
  reg_esi[1] = 0;
  reg_esi[9] = 0;
  reg_esi[8] = 0;
  reg_esi[6] = 0;
  reg_esi[0xe] = 0;
  reg_esi[0xd] = 0;
  reg_esi[0xc] = 0;
  reg_esi[0x14] = 0;
  reg_esi[0x12] = 0;
  reg_esi[0x11] = 0;
  reg_esi[0x19] = 0;
  reg_esi[0x18] = 0;
  reg_esi[0x16] = 0;
  reg_esi[0x1e] = 0;
  reg_esi[0x1d] = 0;
  reg_esi[0x1c] = 0;
  reg_esi[0x2c] = uVar1;
  reg_esi[3] = reg_esi[3] | 0x20003;
  reg_esi[10] = 0x3f800000;
  reg_esi[5] = 0x3f800000;
  *reg_esi = 0x3f800000;
  reg_esi[0x1a] = 0x3f800000;
  reg_esi[0x15] = 0x3f800000;
  reg_esi[0x10] = 0x3f800000;
  reg_esi[0x13] = reg_esi[0x13] | 0x20003;
  reg_esi[0x23] = 0x3e000000;
  return;
}


__declspec(naked) void weapon_fx_state_clear(void)
{
    __asm {
        push esi
        call weapon_fx_state_clear_c
        add esp, 4
        ret
    }
}

/* weapon_find_descriptor */
int * weapon_find_descriptor_c(int reg_esi);
int * weapon_find_descriptor_c(int reg_esi)
{
  int *piVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < (int)weapon_descriptor_count) {
    piVar1 = &weapon_descriptors;
    do {
      if (reg_esi == *piVar1) {
        return piVar1;
      }
      iVar2 = iVar2 + 1;
      piVar1 = piVar1 + 0x10;
    } while (iVar2 < (int)weapon_descriptor_count);
  }
  return (int *)0x0;
}


__declspec(naked) void weapon_find_descriptor(void)
{
    __asm {
        push esi
        call weapon_find_descriptor_c
        add esp, 4
        ret
    }
}

/* player_unequip_weapon */
void player_unequip_weapon_c(int reg_esi);
void player_unequip_weapon_c(int reg_esi)
{
  (**(code **)(*(int *)(reg_esi + 0xa8) + 0x10))(reg_esi);
  *(undefined4 *)(reg_esi + 0xa8) = 0;
  *(undefined4 *)(reg_esi + 0xac) = 0;
  return;
}


__declspec(naked) void player_unequip_weapon(void)
{
    __asm {
        push esi
        call player_unequip_weapon_c
        add esp, 4
        ret
    }
}

/* sound_master_fade_towards */
void sound_master_fade_towards_c(undefined4 reg_eax, float param_2);
void sound_master_fade_towards_c(undefined4 reg_eax, float param_2)
{
  if (g_sound_master_fade < param_2) {
    g_sound_master_fade = g_sound_master_fade + k_f_0_025;
  }
  if (param_2 < g_sound_master_fade) {
    g_sound_master_fade = g_sound_master_fade - k_f_0_025;
  }
  return;
}


__declspec(naked) void sound_master_fade_towards(void)
{
    __asm {
        push ecx
        push edx
        push dword ptr [esp + 12]
        push eax
        call sound_master_fade_towards_c
        add esp, 8
        pop edx
        pop ecx
        ret
    }
}

/* course_object_id_register */
int course_object_id_register_c(int reg_edx);
int course_object_id_register_c(int reg_edx)
{
  int iVar1;
  int iVar2;
  
  iVar1 = g_car_6c28c0;
  iVar2 = 0;
  if (0 < (int)g_car_6c28c0) {
    do {
      if ((&g_car_6c2688)[iVar2] == reg_edx) {
        return iVar2;
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)g_car_6c28c0);
  }
  (&g_car_6c2688)[g_car_6c28c0] = reg_edx;
  g_car_6c28c0 = g_car_6c28c0 + 1;
  return iVar1;
}


__declspec(naked) void course_object_id_register(void)
{
    __asm {
        push edx
        push edx
        call course_object_id_register_c
        add esp, 4
        pop edx
        ret
    }
}

/* car_world_collision_test */
int car_world_collision_test_c(int * reg_esi);
int car_world_collision_test_c(int * reg_esi)
{
  int *piVar1;
  undefined4 uVar2;
  int *piVar3;
  int race_phase;
  int iVar4;
  int iVar5;
  int *piVar6;
  int local_54;
  RwV3d local_4c;
  /* the original's locals at -0x40..-0x1c are one contiguous block and the event record copies 16 words from -0x40 (w00..w3c):
     w00..w0c / w10..w18 / w1c..w24 below; w28..w3c are whatever follows on the original's stack (not read for event type 3) */
  struct {
    int local_40[4];
    float local_30;
    float local_2c;
    float local_28;
    int local_24;
    int local_20;
    int local_1c;
    int tail[6];
  } ev;
  
  race_phase = reg_esi[0x26b];
  local_4c.x = 0.0;
  local_4c.y = 0.1;
  local_4c.z = 0.0;
  piVar3 = reg_esi + 0x133;
  iVar5 = 2;
  do {
    piVar3[1] = *piVar3;
    *piVar3 = 0;
    iVar4 = piVar3[0x10];
    piVar3[0x10] = 0;
    piVar3[0x11] = iVar4;
    iVar4 = piVar3[0x20];
    piVar3[0x20] = 0;
    piVar3[0x21] = iVar4;
    iVar4 = piVar3[0x30];
    piVar3[0x30] = 0;
    piVar3[0x31] = iVar4;
    iVar4 = piVar3[0x40];
    piVar3[-8] = -1;
    piVar3[0x40] = 0;
    piVar3[8] = -1;
    piVar3[0x41] = iVar4;
    iVar4 = piVar3[0x50];
    piVar3[0x18] = -1;
    piVar3[0x28] = -1;
    piVar3[0x50] = 0;
    piVar3[0x51] = iVar4;
    iVar4 = piVar3[0x60];
    piVar3[0x38] = -1;
    piVar3[0x48] = -1;
    piVar3[0x60] = 0;
    piVar3[0x58] = -1;
    piVar3[0x61] = iVar4;
    iVar4 = piVar3[0x70];
    piVar3[0x68] = -1;
    piVar3[0x70] = 0;
    piVar3[0x78] = -1;
    piVar3[0x71] = iVar4;
    piVar3[0x81] = piVar3[0x80];
    piVar3[0x80] = 0;
    piVar3 = piVar3 + 0x90;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  RwV3dTransformPoints
            ((RwV3d *)(reg_esi + 0x27e),(RwV3d *)(reg_esi + 0x18),0x12,
             (RwMatrix *)(reg_esi + race_phase * 0x10 + 0x24a));
  RwV3dTransformPoints
            ((RwV3d *)&g_car_8bd740,&local_4c,1,(RwMatrix *)(reg_esi + race_phase * 0x10 + 0x24a));
  car_points_vs_triangles(reg_esi,&g_collision_triangles);
  car_points_vs_dynamic_objects(reg_esi);
  iVar5 = 0;
  race_phase = 0;
  local_54 = 0;
  piVar3 = reg_esi + 0x27f;
  piVar6 = reg_esi + 299;
  do {
    iVar4 = *piVar6;
    if (((iVar4 != -1) && (local_54 = local_54 + 1, race_phase == 0)) &&
       (*(float *)&piVar6[9] <= *(float *)&piVar6[8])) {
      ev.local_40[0] = reg_esi[0x279];
      ev.local_30 = *(float *)&piVar6[5] * *(float *)&piVar6[0xd];
      ev.local_40[1] = piVar3[-1];
      ev.local_40[2] = *piVar3;
      ev.local_40[3] = piVar3[1];
      ev.local_2c = *(float *)&piVar6[6] * *(float *)&piVar6[0xd];
      ev.local_24 = piVar3[-0x267];
      ev.local_20 = piVar3[-0x266];
      ev.local_1c = piVar3[-0x265];
      ev.local_28 = *(float *)&piVar6[7] * *(float *)&piVar6[0xd];
      race_phase = race_get_phase();
      uVar2 = g_last_frame_ticks;
      if (((race_phase != 1) && (race_phase != 2)) &&
         ((race_phase != 3 && ((race_phase != 4 && (race_phase != 5)))))) {
        (&g_event_log_type)[g_event_log_head] = 3;
        (&g_event_log_time)[g_event_log_head] = uVar2;
        (&g_event_log_param)[g_event_log_head] = *reg_esi + 0xff00;
        (&g_car_7e8b64)[g_event_log_head * 8] = iVar5;
        (&g_car_7e8b68)[g_event_log_head * 8] = iVar4;
        (&g_car_7e8b6c)[g_event_log_head * 8] = ev.local_40[2];
        (&g_car_7e8b70)[g_event_log_head * 8] = ev.local_40[3];
        *(float *)&(&g_car_7e8b74)[g_event_log_head * 8] = ev.local_30;
        *(float *)&(&g_car_7e8b78)[g_event_log_head * 8] = ev.local_2c;
        *(float *)&(&g_car_7e8b7c)[g_event_log_head * 8] = ev.local_28;
        (&g_car_7e8b80)[g_event_log_head * 8] = ev.local_24;
        race_phase = 0;
        do {
          piVar1 = ev.local_40 + race_phase;
          iVar4 = g_event_log_head * 0x10 + race_phase;
          race_phase = race_phase + 1;
          (&g_car_7eab64)[iVar4] = *piVar1;
        } while (race_phase < 0x10);
        g_event_log_head = g_event_log_head + 1;
        if (0xff < g_event_log_head) {
          g_event_log_head = 0;
        }
      }
      race_phase = 1;
    }
    iVar5 = iVar5 + 1;
    piVar6 = piVar6 + 0x10;
    piVar3 = piVar3 + 3;
  } while (iVar5 < 0x12);
  reg_esi[0x27b] = local_54;
  return race_phase;
}


__declspec(naked) void car_world_collision_test(void)
{
    __asm {
        push esi
        call car_world_collision_test_c
        add esp, 4
        ret
    }
}

/* hud_label_draw_text */
void hud_label_draw_text_c(undefined4 reg_edi, float param_2, float param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, float param_8, int param_9);
void hud_label_draw_text_c(undefined4 reg_edi, float param_2, float param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, float param_8, int param_9)
{
  float fVar1;
  float fVar2;
  undefined4 *puVar3;
  float10 fVar4;
  float fStack_10;
  float fStack_c;
  undefined4 local_8;
  undefined4 local_4;
  
  local_8 = 0;
  local_4 = 0;
  puVar3 = (undefined4 *)FUN_004c4280(*(undefined4 *)(g_text_state + 4));
  FUN_00567780(*(undefined4 *)(g_text_state + 4));
  RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)*puVar3);
  RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,(void *)0x2);
  fVar2 = param_8 * k_f_0_0020833334;
  BR_rt2d_text_pixel_to_uv(&fStack_10,param_2,param_3);
  Rt2dCTMPush();
  Rt2dCTMSetIdentity();
  Rt2dCTMTranslate(fStack_10,fStack_c);
  Rt2dCTMRotate(0);
  Rt2dCTMTranslate(-fStack_10,-fStack_c);
  Rt2dBrushSetRGBA(*(undefined4 *)(g_text_state + 0xc),param_4,param_5,param_6,param_7);
  fVar4 = (float10)Rt2dFontGetStringWidth(*(undefined4 *)(g_text_state + 8),reg_edi,fVar2);
  fVar1 = (float)fVar4;
  if (param_9 == 1) {
    fStack_10 = fStack_10 - fVar1 * k_f_0_5;
    fStack_c = fStack_c - fVar2 * k_f_0_5;
    if (fStack_10 < k_f_0) {
      fStack_10 = 0.0;
    }
    if (k_f_1_333 < fVar1 + fStack_10) {
      fStack_10 = k_f_1_33 - fVar1;
    }
  }
  else if (param_9 == 2) {
    fStack_10 = fVar1 + fStack_10;
  }
  Rt2dCTMTranslate(fStack_10,fStack_c);
  FUN_00567760(*(undefined4 *)(g_text_state + 8),reg_edi,fVar2,&local_8,
               *(undefined4 *)(g_text_state + 0xc));
  FUN_00563340();
  return;
}


__declspec(naked) void hud_label_draw_text(void)
{
    __asm {
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push edi
        call hud_label_draw_text_c
        add esp, 36
        ret
    }
}

/* fatal_assert_loop */
void fatal_assert_loop_c(undefined4 reg_esi, undefined4 param_2);
void fatal_assert_loop_c(undefined4 reg_esi, undefined4 param_2)
{
  do {
    FID_conflict__wprintf("%s\n",param_2,reg_esi);
  } while( true );
}


__declspec(naked) void fatal_assert_loop(void)
{
    __asm {
        push dword ptr [esp + 4]
        push esi
        call fatal_assert_loop_c
        add esp, 8
        ret
    }
}

/* fx_emitter_init */
void fx_emitter_init_c(int reg_edi, float * param_2, undefined4 param_3, undefined4 param_4, float param_5, int param_6, undefined4 param_7);
void fx_emitter_init_c(int reg_edi, float * param_2, undefined4 param_3, undefined4 param_4, float param_5, int param_6, undefined4 param_7)
{
  float *pfVar1;
  float *pfVar2;
  RwV3d *out;
  RwV3d *out_00;
  float fVar3;
  float *pfVar4;
  float10 fVar5;
  
  fVar3 = k_f_0;
  *(float *)(reg_edi + 0xc0) = *param_2;
  *(float *)(reg_edi + 0xc4) = param_2[1];
  *(float *)(reg_edi + 200) = param_2[2];
  *(undefined4 *)(reg_edi + 0xd0) = 0;
  *(undefined4 *)(reg_edi + 0xd4) = 0;
  *(int *)(reg_edi + 0xdc) = param_6;
  *(int *)(reg_edi + 0xe0) = param_6;
  *(undefined4 *)(reg_edi + 0xe4) = param_7;
  *(undefined4 *)(reg_edi + 0xd8) = 1;
  *(undefined4 *)(reg_edi + 0xe8) = 1;
  if (fVar3 < param_5) {
    *(float *)(reg_edi + 0xd0) = -param_5;
  }
  pfVar4 = (float *)(reg_edi + 8);
  param_6 = 4;
  do {
    pfVar1 = pfVar4 + -2;
    *pfVar1 = *param_2;
    pfVar4[-1] = param_2[1];
    *pfVar4 = param_2[2];
    pfVar2 = pfVar4 + 0x16;
    *pfVar2 = *param_2;
    pfVar4[0x17] = param_2[1];
    pfVar4[0x18] = param_2[2];
    fVar5 = (float10)FUN_004c45e0(*(float *)(reg_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(reg_edi + 0xdc) * k_f_0_25);
    *pfVar1 = (float)(fVar5 + (float10)*pfVar1);
    fVar5 = (float10)FUN_004c45e0(*(float *)(reg_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(reg_edi + 0xdc) * k_f_0_25);
    pfVar4[-1] = (float)(fVar5 + (float10)pfVar4[-1]);
    fVar5 = (float10)FUN_004c45e0(*(float *)(reg_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(reg_edi + 0xdc) * k_f_0_25);
    *pfVar4 = (float)(fVar5 + (float10)*pfVar4);
    fVar5 = (float10)FUN_004c45e0(*(float *)(reg_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(reg_edi + 0xdc) * k_f_0_25);
    *pfVar2 = (float)(fVar5 + (float10)*pfVar2);
    fVar5 = (float10)FUN_004c45e0(*(float *)(reg_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(reg_edi + 0xdc) * k_f_0_25);
    pfVar4[0x17] = (float)(fVar5 + (float10)pfVar4[0x17]);
    fVar5 = (float10)FUN_004c45e0(*(float *)(reg_edi + 0xdc) * k_f_n0_25,
                                  *(float *)(reg_edi + 0xdc) * k_f_0_25);
    out = (RwV3d *)(pfVar4 + 10);
    pfVar4[0x18] = (float)(fVar5 + (float10)pfVar4[0x18]);
    fVar5 = (float10)FUN_004c45e0(0xbf800000,0x3f800000);
    out->x = (float)fVar5;
    fVar5 = (float10)FUN_004c45e0(0xbf800000,0x3f800000);
    pfVar4[0xb] = (float)fVar5;
    fVar5 = (float10)FUN_004c45e0(0xbf800000,0x3f800000);
    pfVar4[0xc] = (float)fVar5;
    out_00 = (RwV3d *)(pfVar4 + 0x22);
    fVar5 = (float10)FUN_004c45e0(0xbf800000,0x3f800000);
    out_00->x = (float)fVar5;
    fVar5 = (float10)FUN_004c45e0(0xbf800000,0x3f800000);
    pfVar4[0x23] = (float)fVar5;
    fVar5 = (float10)FUN_004c45e0(0xbf800000,0x3f800000);
    pfVar4[0x24] = (float)fVar5;
    RwV3dNormalize(out,out);
    RwV3dNormalize(out_00,out_00);
    pfVar4 = pfVar4 + 3;
    param_6 = param_6 + -1;
  } while (param_6 != 0);
  return;
}


__declspec(naked) void fx_emitter_init(void)
{
    __asm {
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push dword ptr [esp + 24]
        push edi
        call fx_emitter_init_c
        add esp, 28
        ret
    }
}

/* effect_sprite_reset */
void effect_sprite_reset_c(undefined4 * reg_eax);
void effect_sprite_reset_c(undefined4 * reg_eax)
{
  byte *pbVar1;
  
  *(undefined1 *)(reg_eax + 0x10) = 0xff;
  *(undefined1 *)((int)reg_eax + 0x41) = 0xff;
  *(undefined1 *)((int)reg_eax + 0x42) = 0xff;
  *(undefined1 *)((int)reg_eax + 0x43) = 0xff;
  reg_eax[10] = 0x3f800000;
  reg_eax[5] = 0x3f800000;
  *reg_eax = 0x3f800000;
  reg_eax[3] = reg_eax[3] | 0x20003;
  reg_eax[0x12] = 0;
  reg_eax[0x13] = 0;
  reg_eax[0x14] = 0;
  reg_eax[0x15] = 0;
  reg_eax[4] = 0;
  reg_eax[2] = 0;
  reg_eax[1] = 0;
  reg_eax[9] = 0;
  reg_eax[8] = 0;
  reg_eax[6] = 0;
  reg_eax[0xe] = 0;
  reg_eax[0xd] = 0;
  reg_eax[0xc] = 0;
  if (reg_eax[0x17] == 0) {
    pbVar1 = (byte *)((int)&g_gfx_6c57b0 + ((int)reg_eax[0x16] >> 3));
    *pbVar1 = *pbVar1 & ~('\x01' << ((byte)reg_eax[0x16] & 7));
    return;
  }
  if (reg_eax[0x17] == 1) {
    pbVar1 = (byte *)((int)&g_gfx_6c51a8 + ((int)reg_eax[0x16] >> 3));
    *pbVar1 = *pbVar1 & ~('\x01' << ((byte)reg_eax[0x16] & 7));
  }
  return;
}


__declspec(naked) void effect_sprite_reset(void)
{
    __asm {
        push eax
        call effect_sprite_reset_c
        add esp, 4
        ret
    }
}

/* effect_sprite_init */
void effect_sprite_init_c(undefined4 * reg_eax, undefined4 * param_2, undefined4 * param_3, undefined4 param_4, undefined4 param_5);
void effect_sprite_init_c(undefined4 * reg_eax, undefined4 * param_2, undefined4 * param_3, undefined4 param_4, undefined4 param_5)
{
  byte *pbVar1;
  int iVar2;
  undefined4 *puVar3;
  
  reg_eax[0x13] = param_4;
  reg_eax[0x14] = param_5;
  reg_eax[0x15] = 0;
  reg_eax[0x12] = 0;
  puVar3 = reg_eax;
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *param_2;
    param_2 = param_2 + 1;
    puVar3 = puVar3 + 1;
  }
  reg_eax[0x10] = *param_3;
  if (reg_eax[0x17] == 0) {
    iVar2 = (int)reg_eax[0x16] >> 3;
    *(byte *)((int)&g_gfx_6c57b0 + iVar2) =
         *(byte *)((int)&g_gfx_6c57b0 + iVar2) | '\x01' << ((byte)reg_eax[0x16] & 7);
    return;
  }
  if (reg_eax[0x17] == 1) {
    pbVar1 = (byte *)((int)&g_gfx_6c51a8 + ((int)reg_eax[0x16] >> 3));
    *pbVar1 = *pbVar1 | '\x01' << ((byte)reg_eax[0x16] & 7);
  }
  return;
}


__declspec(naked) void effect_sprite_init(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push eax
        call effect_sprite_init_c
        add esp, 20
        ret
    }
}

/* effect_sprite_update */
void effect_sprite_update_c(RwMatrix * reg_esi, undefined4 reg_eax);
void effect_sprite_update_c(RwMatrix * reg_esi, undefined4 reg_eax)
{
  float fVar1;
  RwV3d local_c;
  
  reg_esi[1].up.y = g_tick_seconds + reg_esi[1].up.y;
  local_c.x = g_tick_seconds * reg_esi[1].up.x + reg_esi[1].right.z;
  fVar1 = (float)reg_esi[1].flags;
  reg_esi[1].right.z = local_c.x;
  if (fVar1 <= local_c.x) {
    BR_effect_sprite_reset(reg_esi);
    return;
  }
  local_c.y = local_c.x;
  local_c.z = local_c.x;
  RwFrameTransform(*(RwFrame **)((int)reg_esi[1].right.y + 4),reg_esi,rwCOMBINEREPLACE);
  RwFrameScale(*(RwFrame **)((int)reg_esi[1].right.y + 4),&local_c,rwCOMBINEPRECONCAT);
  return;
}


__declspec(naked) void effect_sprite_update(void)
{
    __asm {
        push eax
        push esi
        call effect_sprite_update_c
        add esp, 8
        ret
    }
}

/* sprite_slot_destroy_atomic_44 */
void sprite_slot_destroy_atomic_44_c(int reg_esi);
void sprite_slot_destroy_atomic_44_c(int reg_esi)
{
  RwFrameDestroy(*(RwFrame **)(*(int *)(reg_esi + 0x44) + 4));
  RpAtomicSetFrame(*(RpAtomic **)(reg_esi + 0x44),(RwFrame *)0x0);
  RpAtomicDestroy(*(RpAtomic **)(reg_esi + 0x44));
  *(undefined4 *)(reg_esi + 0x44) = 0;
  return;
}


__declspec(naked) void sprite_slot_destroy_atomic_44(void)
{
    __asm {
        push esi
        call sprite_slot_destroy_atomic_44_c
        add esp, 4
        ret
    }
}

/* effect_slot_free */
void effect_slot_free_c(int reg_esi);
void effect_slot_free_c(int reg_esi)
{
  FUN_004c66a0(&g_gfx_6c5e20 + reg_esi * 0x2c0,0x2c0);
  (&g_gfx_6f1e20)[reg_esi >> 3] = (&g_gfx_6f1e20)[reg_esi >> 3] & ~('\x01' << ((byte)reg_esi & 7));
  return;
}


__declspec(naked) void effect_slot_free(void)
{
    __asm {
        push esi
        call effect_slot_free_c
        add esp, 4
        ret
    }
}

/* triangle_normal_cross_array */
void triangle_normal_cross_array_c(float * reg_eax, RwV3d * reg_ecx, int param_3);
void triangle_normal_cross_array_c(float * reg_eax, RwV3d * reg_ecx, int param_3)
{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  
  fVar1 = reg_eax[3];
  fVar2 = *reg_eax;
  fVar3 = reg_eax[4];
  fVar4 = reg_eax[1];
  fVar5 = reg_eax[5];
  fVar6 = reg_eax[2];
  fVar7 = reg_eax[6];
  fVar8 = *reg_eax;
  fVar9 = reg_eax[7];
  fVar10 = reg_eax[1];
  fVar11 = reg_eax[8];
  fVar12 = reg_eax[2];
  reg_ecx->x = (fVar11 - fVar12) * (fVar3 - fVar4) - (fVar9 - fVar10) * (fVar5 - fVar6);
  reg_ecx->y = (fVar7 - fVar8) * (fVar5 - fVar6) - (fVar11 - fVar12) * (fVar1 - fVar2);
  reg_ecx->z = (fVar9 - fVar10) * (fVar1 - fVar2) - (fVar7 - fVar8) * (fVar3 - fVar4);
  if (param_3 != 0) {
    RwV3dNormalize(reg_ecx,reg_ecx);
  }
  return;
}


__declspec(naked) void triangle_normal_cross_array(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ecx
        push eax
        call triangle_normal_cross_array_c
        add esp, 12
        ret
    }
}

/* collision_edge_shares_vertex_pair */
undefined4 collision_edge_shares_vertex_pair_c(int reg_esi, int reg_edx);
undefined4 collision_edge_shares_vertex_pair_c(int reg_esi, int reg_edx)
{
  if (((*(int *)(reg_esi + 0x10) == *(int *)(reg_edx + 0x10)) ||
      (*(int *)(reg_esi + 0x10) == *(int *)(reg_edx + 0x14))) &&
     ((*(int *)(reg_esi + 0x14) == *(int *)(reg_edx + 0x10) ||
      (*(int *)(reg_esi + 0x14) == *(int *)(reg_edx + 0x14))))) {
    return 1;
  }
  return 0;
}


__declspec(naked) void collision_edge_shares_vertex_pair(void)
{
    __asm {
        push edx
        push edx
        push esi
        call collision_edge_shares_vertex_pair_c
        add esp, 8
        pop edx
        ret
    }
}

/* course_mesh_triangle_normals_build */
void course_mesh_triangle_normals_build_c(int reg_edx, RwV3d * reg_ecx, int reg_ebx, int param_4);
void course_mesh_triangle_normals_build_c(int reg_edx, RwV3d * reg_ecx, int reg_ebx, int param_4)
{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  ushort *puVar15;
  
  if (0 < param_4) {
    puVar15 = (ushort *)(reg_edx + 4);
    do {
      pfVar1 = (float *)(reg_ebx + (uint)*puVar15 * 0xc);
      fVar3 = *pfVar1;
      pfVar2 = (float *)(reg_ebx + (uint)puVar15[-1] * 0xc);
      fVar4 = *pfVar2;
      fVar5 = pfVar1[1];
      fVar6 = pfVar2[1];
      fVar7 = pfVar1[2];
      pfVar1 = (float *)(reg_ebx + (uint)puVar15[-2] * 0xc);
      fVar8 = pfVar2[2];
      fVar9 = *pfVar1;
      fVar10 = *pfVar2;
      fVar11 = pfVar1[1];
      fVar12 = pfVar2[1];
      fVar13 = pfVar1[2];
      fVar14 = pfVar2[2];
      reg_ecx->x = (fVar13 - fVar14) * (fVar5 - fVar6) - (fVar11 - fVar12) * (fVar7 - fVar8);
      reg_ecx->y = (fVar9 - fVar10) * (fVar7 - fVar8) - (fVar13 - fVar14) * (fVar3 - fVar4);
      reg_ecx->z = (fVar11 - fVar12) * (fVar3 - fVar4) - (fVar9 - fVar10) * (fVar5 - fVar6);
      RwV3dNormalize(reg_ecx,reg_ecx);
      puVar15 = puVar15 + 4;
      reg_ecx = reg_ecx + 1;
      param_4 = param_4 + -1;
    } while (param_4 != 0);
  }
  return;
}


__declspec(naked) void course_mesh_triangle_normals_build(void)
{
    __asm {
        push dword ptr [esp + 4]
        push ebx
        push ecx
        push edx
        call course_mesh_triangle_normals_build_c
        add esp, 16
        ret
    }
}

/* planes_test_bbox */
undefined4 planes_test_bbox_c(int reg_ebx, int reg_eax, undefined4 param_3);
undefined4 planes_test_bbox_c(int reg_ebx, int reg_eax, undefined4 param_3)
{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < reg_ebx) {
    do {
      iVar1 = bbox_plane_corner_test(reg_eax,param_3);
      if (iVar1 == 0) {
        return 0;
      }
      iVar2 = iVar2 + 1;
      reg_eax = reg_eax + 0x10;
    } while (iVar2 < reg_ebx);
  }
  return 1;
}


__declspec(naked) void planes_test_bbox(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        push ebx
        call planes_test_bbox_c
        add esp, 12
        ret
    }
}

/* planes_test_sphere */
undefined4 planes_test_sphere_c(int reg_ebx, int reg_eax, undefined4 param_3);
undefined4 planes_test_sphere_c(int reg_ebx, int reg_eax, undefined4 param_3)
{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < reg_ebx) {
    do {
      iVar1 = sphere_plane_side(reg_eax,param_3);
      if (iVar1 != 2) {
        return 0;
      }
      iVar2 = iVar2 + 1;
      reg_eax = reg_eax + 0x10;
    } while (iVar2 < reg_ebx);
  }
  return 1;
}


__declspec(naked) void planes_test_sphere(void)
{
    __asm {
        push dword ptr [esp + 4]
        push eax
        push ebx
        call planes_test_sphere_c
        add esp, 12
        ret
    }
}

/* sector_table_count_entries */
void sector_table_count_entries_c(int reg_edx, char * reg_eax);
void sector_table_count_entries_c(int reg_edx, char * reg_eax)
{
  char *pcVar1;
  char *pcVar2;
  char *pcVar3;
  int *piVar4;
  char *pcVar5;
  
  piVar4 = &g_sector_table_702268 + reg_edx;
  *(char **)(&g_sector_table_702668 + reg_edx * 4) = reg_eax;
  *piVar4 = 0;
  if (((reg_eax[3] != '\0' || reg_eax[2] != '\0') || reg_eax[1] != '\0') || *reg_eax != '\0') {
    do {
      pcVar5 = reg_eax + 4;
      *piVar4 = *piVar4 + 1;
      pcVar1 = reg_eax + 6;
      pcVar2 = reg_eax + 7;
      pcVar3 = reg_eax + 5;
      reg_eax = pcVar5;
    } while (((*pcVar2 != '\0' || *pcVar1 != '\0') || *pcVar3 != '\0') || *pcVar5 != '\0');
  }
  return;
}


__declspec(naked) void sector_table_count_entries(void)
{
    __asm {
        push eax
        push edx
        call sector_table_count_entries_c
        add esp, 8
        ret
    }
}

/* smplfzx_retire */
void smplfzx_retire_c(int reg_esi);
void smplfzx_retire_c(int reg_esi)
{
  int iVar1;
  
  if (reg_esi == 0) {
    debug_printf("ERROR: SmplFzx. Retiring NULL object!!\n");
    return;
  }
  debug_printf("SmplFzx : Retiring %d\n",*(undefined4 *)(reg_esi + 0xc));
  iVar1 = pool_list_unlink(g_particles_71abf4,reg_esi);
  if (iVar1 == 0) {
    debug_printf(&g_smplfzx_retire_5e06e8,*(undefined4 *)(reg_esi + 0xc));
    return;
  }
  pool_push(g_smplfzx_71abf0,reg_esi);
  return;
}


__declspec(naked) void smplfzx_retire(void)
{
    __asm {
        push esi
        call smplfzx_retire_c
        add esp, 4
        ret
    }
}

/* smplfzx_current_object_get_field */
undefined4 smplfzx_current_object_get_field_c(int reg_esi);
undefined4 smplfzx_current_object_get_field_c(int reg_esi)
{
  int smplfzx_object;
  
  smplfzx_object = smplfzx_get_object(reg_esi);
  if (smplfzx_object == 0) {
                    /* WARNING: Subroutine does not return */
    FUN_004b322b(0xfffffff5);
  }
  return **(undefined4 **)(*(int *)(g_particles_71abf4 + 0xc) + reg_esi * 0x14);
}


__declspec(naked) void smplfzx_current_object_get_field(void)
{
    __asm {
        push esi
        call smplfzx_current_object_get_field_c
        add esp, 4
        ret
    }
}

/* fx_object_apply_transform */
void fx_object_apply_transform_c(int * reg_esi);
void fx_object_apply_transform_c(int * reg_esi)
{
  int iVar1;
  
  if (*reg_esi != 0) {
    RwFrameTransform(*(RwFrame **)(*reg_esi + 4),(RwMatrix *)(reg_esi + 4),rwCOMBINEREPLACE);
    iVar1 = FUN_004c40c0(*reg_esi);
    if (iVar1 != 0) {
      *(undefined4 *)(iVar1 + 4) = 0xffffffff;
    }
    FUN_004c5360(*reg_esi,0);
  }
  return;
}


__declspec(naked) void fx_object_apply_transform(void)
{
    __asm {
        push esi
        call fx_object_apply_transform_c
        add esp, 4
        ret
    }
}

/* sphere_vs_box_test */
undefined4 sphere_vs_box_test_c(undefined4 reg_eax, float * param_2, float * param_3);
undefined4 sphere_vs_box_test_c(undefined4 reg_eax, float * param_2, float * param_3)
{
  if (((((*param_3 < param_2[3]) || (*param_2 <= *param_3)) || (param_3[1] < param_2[4])) ||
      (((param_2[1] <= param_3[1] || (param_3[2] < param_2[5])) || (param_2[2] <= param_3[2])))) &&
     (((param_3[3] + *param_3 < param_2[3] ||
       (*param_3 - param_3[3] < *param_2 == (*param_3 - param_3[3] == *param_2))) ||
      ((param_3[3] + param_3[1] < param_2[4] ||
       (((param_3[1] - param_3[3] < param_2[1] == (param_3[1] - param_3[3] == param_2[1]) ||
         (param_3[3] + param_3[2] < param_2[5])) ||
        (param_3[2] - param_3[3] < param_2[2] == (param_3[2] - param_3[3] == param_2[2]))))))))) {
    return 0;
  }
  return 1;
}


__declspec(naked) void sphere_vs_box_test(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push eax
        call sphere_vs_box_test_c
        add esp, 12
        ret
    }
}

/* movie_guid_compare */
undefined4 movie_guid_compare_c(int * reg_edx, undefined4 reg_eax);
undefined4 movie_guid_compare_c(int * reg_edx, undefined4 reg_eax)
{
  int iVar1;
  int *piVar2;
  int *piVar3;
  bool bVar4;
  
  iVar1 = 4;
  bVar4 = true;
  piVar2 = reg_edx + 0xb;
  piVar3 = &g_app_5e20fc;
  do {
    if (iVar1 == 0) break;
    iVar1 = iVar1 + -1;
    bVar4 = *piVar2 == *piVar3;
    piVar2 = piVar2 + 1;
    piVar3 = piVar3 + 1;
  } while (bVar4);
  if (!bVar4) {
    return 0x80070057;
  }
  iVar1 = 4;
  bVar4 = true;
  piVar2 = reg_edx;
  piVar3 = &g_app_5e218c;
  do {
    if (iVar1 == 0) break;
    iVar1 = iVar1 + -1;
    bVar4 = *piVar2 == *piVar3;
    piVar2 = piVar2 + 1;
    piVar3 = piVar3 + 1;
  } while (bVar4);
  if (bVar4) {
    iVar1 = 4;
    bVar4 = true;
    piVar2 = reg_edx + 4;
    piVar3 = &g_app_5e215c;
    do {
      if (iVar1 == 0) break;
      iVar1 = iVar1 + -1;
      bVar4 = *piVar2 == *piVar3;
      piVar2 = piVar2 + 1;
      piVar3 = piVar3 + 1;
    } while (bVar4);
    if (bVar4) {
      reg_eax = 0;
    }
  }
  return reg_eax;
}


__declspec(naked) void movie_guid_compare(void)
{
    __asm {
        push eax
        push edx
        call movie_guid_compare_c
        add esp, 8
        ret 4
    }
}

/* video_state_clear */
void video_state_clear_c(undefined4 * reg_ecx);
void video_state_clear_c(undefined4 * reg_ecx)
{
  *reg_ecx = 0;
  return;
}


__declspec(naked) void video_state_clear(void)
{
    __asm {
        push ecx
        push edx
        push ecx
        call video_state_clear_c
        add esp, 4
        pop edx
        pop ecx
        ret
    }
}

