// cluster-entries: weapon_mortar_shell_update
/* Installed cluster weapon_mortar_shell_update (tools/cluster_install.py from output/cluster_pilot_h2/weapon_mortar_shell_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0047b520*/ extern int car_current_matrix();
/*004c42f0*/ extern void FUN_004c42f0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd40c*/ extern float k_f_1_5;
/*005dd634*/ extern float k_f_2;
/*005de1d0*/ extern float k_f_1_25;
/*005de1f8*/ extern float k_f_3_1415927;
/*005de3a0*/ extern float k_f_1_3;
/*005df718*/ extern float k_f_n2_2;
/*005df71c*/ extern float k_f_0_58;
/*005df720*/ extern float k_f_0_38;
/*005df724*/ extern float k_f_0_7692308;
/*007ef7f4*/ extern float g_tick_seconds;
extern double sin(double);
/* weapon_mortar_shell_update */

/* WARNING: Unknown calling convention */
/* "moves one mortar shell along its arc, scales its frame, handles impact and calls the trail
   particles" */

static undefined4 weapon_mortar_shell_impact_check_r(int p0);
extern void weapon_mortar_shell_impact_check(void);
static int weapon_mortar_trail_particles_add_r(int p0);
extern void weapon_mortar_trail_particles_add(void);
static int weapon_pickup_explode_effect_r(int p0);
extern void weapon_pickup_explode_effect(void);
void weapon_mortar_shell_update_n(undefined4 *in_eax)

{
  RwV3d *v;
  RwFrame *frame;
  float fVar1;
  int iVar2;
  float10 fVar3;
  struct {
    int local_50;
    float local_4c;
    float local_48;
    float local_44;
    float local_40;
    RwV3d local_3c;
    float local_30;
    float local_2c;
    float local_28;
    RwV3d local_24;
    float local_18;
    float local_14;
    float local_10;
    RwV3d local_c;
  } frame_obj;
  
  frame_obj.local_c.x = 2.0;
  frame_obj.local_c.y = 2.0;
  frame_obj.local_c.z = 2.0;
  frame_obj.local_30 = 0.0;
  frame_obj.local_2c = 0.0;
  frame_obj.local_28 = 0.0;
  frame_obj.local_24.x = 0.0;
  frame_obj.local_24.y = 0.0;
  frame_obj.local_24.z = 0.0;
  iVar2 = weapon_mortar_shell_impact_check_r((int)in_eax);
  if (iVar2 == 0) {
    fVar1 = *(float *)&in_eax[0x13] * k_f_0_7692308;
    *(float *)&in_eax[0x14] = fVar1;
    if (k_f_2 < fVar1) {
      weapon_pickup_explode_effect_r((int)in_eax);
      return;
    }
    if (in_eax[0x11] != 0) {
      car_current_matrix(&frame_obj.local_50,*in_eax);
      frame_obj.local_18 = *(float *)(frame_obj.local_50 + 0x30);
      frame_obj.local_14 = *(float *)(frame_obj.local_50 + 0x34);
      frame_obj.local_10 = *(float *)(frame_obj.local_50 + 0x38);
      frame_obj.local_3c.x = (*(float *)&in_eax[2] + frame_obj.local_18) - *(float *)&in_eax[5];
      frame_obj.local_3c.z = (*(float *)&in_eax[4] + frame_obj.local_10) - *(float *)&in_eax[7];
      frame_obj.local_3c.y = 0.0;
      frame_obj.local_48 = RwV3dLength(&frame_obj.local_3c);
      RwV3dNormalize(&frame_obj.local_3c,&frame_obj.local_3c);
      fVar1 = frame_obj.local_48 * *(float *)&in_eax[0x14] * k_f_0_38;
      frame_obj.local_3c.x = frame_obj.local_3c.x * fVar1;
      frame_obj.local_3c.y = frame_obj.local_3c.y * fVar1;
      *(float *)&in_eax[8] = frame_obj.local_3c.x;
      frame_obj.local_3c.z = fVar1 * frame_obj.local_3c.z;
      *(float *)&in_eax[9] = frame_obj.local_3c.y;
      fVar1 = k_f_0_58;
      *(float *)&in_eax[10] = frame_obj.local_3c.z;
      if (fVar1 < *(float *)&in_eax[0x14]) {
        in_eax[0x11] = 0;
      }
    }
    if (k_f_1_25 <= *(float *)&in_eax[0x14]) {
      fVar3 = ((float10)*(float *)&in_eax[0x13] - (float10)k_f_1_3) * (float10)k_f_n2_2;
    }
    else {
      fVar3 = (float10)sin((float10)*(float *)&in_eax[0x14] * (float10)k_f_3_1415927);
    }
    v = (RwV3d *)(in_eax + 5);
    frame_obj.local_18 = v->x;
    frame_obj.local_14 = *(float *)&in_eax[6];
    frame_obj.local_10 = *(float *)&in_eax[7];
    *(float *)&in_eax[0x13] = g_tick_seconds + *(float *)&in_eax[0x13];
    *(float *)&in_eax[0xb] = v->x;
    frame_obj.local_4c = *(float *)&in_eax[8] + v->x;
    in_eax[0xc] = in_eax[6];
    iVar2 = in_eax[0x12];
    in_eax[0xd] = in_eax[7];
    v->x = frame_obj.local_4c;
    frame_obj.local_48 = *(float *)&in_eax[10] + *(float *)&in_eax[7];
    *(float *)&in_eax[7] = frame_obj.local_48;
    fVar3 = fVar3 * (float10)k_f_1_5 + (float10)*(float *)&in_eax[0x15];
    *(float *)&in_eax[6] = (float)fVar3;
    *(float *)&in_eax[0xe] = frame_obj.local_4c - frame_obj.local_18;
    frame_obj.local_44 = (float)(fVar3 - (float10)frame_obj.local_14);
    *(float *)&in_eax[0xf] = (float)(fVar3 - (float10)frame_obj.local_14);
    frame_obj.local_40 = frame_obj.local_48 - frame_obj.local_10;
    *(float *)&in_eax[0x10] = frame_obj.local_40;
    frame = *(RwFrame **)(iVar2 + 4);
    if (in_eax[0x11] == 0) {
      frame_obj.local_30 = (frame_obj.local_4c - frame_obj.local_18) + frame_obj.local_4c;
      frame_obj.local_2c = (float)((float10)frame_obj.local_44 + fVar3);
      frame_obj.local_28 = frame_obj.local_40 + frame_obj.local_48;
    }
    else {
      car_current_matrix(&frame_obj.local_50,*in_eax);
      frame_obj.local_30 = *(float *)(frame_obj.local_50 + 0x30);
      frame_obj.local_2c = *(float *)(frame_obj.local_50 + 0x34);
      frame_obj.local_28 = *(float *)(frame_obj.local_50 + 0x38);
    }
    iVar2 = *(int *)(in_eax[0x12] + 4);
    frame_obj.local_24.x = *(RwReal *)(iVar2 + 0x20);
    frame_obj.local_24.y = *(RwReal *)(iVar2 + 0x24);
    frame_obj.local_24.z = *(RwReal *)(iVar2 + 0x28);
    RwV3dNormalize(&frame_obj.local_24,&frame_obj.local_24);
    RwFrameTranslate(frame,v,rwCOMBINEREPLACE);
    FUN_004c42f0((undefined4)frame,(undefined4)&frame_obj.local_30,(undefined4)&frame_obj.local_24);
    RwFrameScale(frame,&frame_obj.local_c,rwCOMBINEPRECONCAT);
    weapon_mortar_trail_particles_add_r((int)in_eax);
  }
  return;
}

/* weapon_mortar_shell_impact_check_r: C -> original weapon_mortar_shell_impact_check (registers eax) */
__declspec(naked) undefined4 weapon_mortar_shell_impact_check_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_mortar_shell_impact_check
        ret
    }
}

/* weapon_mortar_trail_particles_add_r: C -> original weapon_mortar_trail_particles_add (registers esi) */
__declspec(naked) int weapon_mortar_trail_particles_add_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_mortar_trail_particles_add
        pop esi
        ret
    }
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

/* entry thunk at the original address of weapon_mortar_shell_update: register inputs (eax) -> natural C weapon_mortar_shell_update_n; ret 0 */
__declspec(naked) void weapon_mortar_shell_update(void)
{
    __asm {
        push eax
        call weapon_mortar_shell_update_n
        add esp, 4
        ret
    }
}
