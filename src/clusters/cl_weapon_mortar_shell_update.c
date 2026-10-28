// cluster-entries: weapon_mortar_shell_update
/* Installed cluster weapon_mortar_shell_update (tools/cluster_install.py from output/cluster_pilot_h2/weapon_mortar_shell_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0047b520*/ extern int car_current_matrix();
/*004c42f0*/ extern void rwframe_look_at(undefined4 p0, undefined4 p1, undefined4 p2);
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

/* "moves one mortar shell along its arc, scales its frame, handles impact and calls the trail
   particles" */

static undefined4 weapon_mortar_shell_impact_check_r(int p0);
extern void weapon_mortar_shell_impact_check(void);
static int weapon_mortar_trail_particles_add_r(int p0);
extern void weapon_mortar_trail_particles_add(void);
static int weapon_pickup_explode_effect_r(int p0);
extern void weapon_pickup_explode_effect(void);
/* shell: [0] owner car, [5..7] position, [8..10] velocity, [0x11] homing-to-owner flag, [0x12] node, [0x13] age (s), [0x14] age/1.3 phase,
   [0x15] base height. Explodes when the phase exceeds 2; while homing steers towards the owner car; height follows sin(phase*pi) then a quadratic fall. */
void weapon_mortar_shell_update_n(undefined4 *shell)

{
  RwV3d *v;
  RwFrame *frame;
  float scratch_f;
  int scratch_i;
  float10 arc;
  struct {
    int car_matrix_ptr;
    float new_x;
    float len_or_new_z;
    float delta_y;
    float delta_z;
    RwV3d to_target;
    float aim_x;
    float aim_y;
    float aim_z;
    RwV3d up_dir;
    float prev_x;
    float prev_y;
    float prev_z;
    RwV3d scale;
  } tmp;
  
  tmp.scale.x = 2.0;
  tmp.scale.y = 2.0;
  tmp.scale.z = 2.0;
  tmp.aim_x = 0.0;
  tmp.aim_y = 0.0;
  tmp.aim_z = 0.0;
  tmp.up_dir.x = 0.0;
  tmp.up_dir.y = 0.0;
  tmp.up_dir.z = 0.0;
  /* (inferred) tmp.prev_* first holds the owner car position (+0x30..0x38 of its matrix) while homing, later the shell's old position; aim_* is the look-at point passed to rwframe_look_at with up_dir. */
  scratch_i = weapon_mortar_shell_impact_check_r((int)shell);
  if (scratch_i == 0) {
    scratch_f = *(float *)&shell[0x13] * k_f_0_7692308;
    *(float *)&shell[0x14] = scratch_f;
    if (k_f_2 < scratch_f) {
      weapon_pickup_explode_effect_r((int)shell);
      return;
    }
    if (shell[0x11] != 0) {
      car_current_matrix(&tmp.car_matrix_ptr,*shell);
      tmp.prev_x = *(float *)(tmp.car_matrix_ptr + 0x30);
      tmp.prev_y = *(float *)(tmp.car_matrix_ptr + 0x34);
      tmp.prev_z = *(float *)(tmp.car_matrix_ptr + 0x38);
      tmp.to_target.x = (*(float *)&shell[2] + tmp.prev_x) - *(float *)&shell[5];
      tmp.to_target.z = (*(float *)&shell[4] + tmp.prev_z) - *(float *)&shell[7];
      tmp.to_target.y = 0.0;
      tmp.len_or_new_z = RwV3dLength(&tmp.to_target);
      RwV3dNormalize(&tmp.to_target,&tmp.to_target);
      scratch_f = tmp.len_or_new_z * *(float *)&shell[0x14] * k_f_0_38;
      tmp.to_target.x = tmp.to_target.x * scratch_f;
      tmp.to_target.y = tmp.to_target.y * scratch_f;
      *(float *)&shell[8] = tmp.to_target.x;
      tmp.to_target.z = scratch_f * tmp.to_target.z;
      *(float *)&shell[9] = tmp.to_target.y;
      scratch_f = k_f_0_58;
      *(float *)&shell[10] = tmp.to_target.z;
      if (scratch_f < *(float *)&shell[0x14]) {
        shell[0x11] = 0;
      }
    }
    if (!(k_f_1_25 > *(float *)&shell[0x14])) {
      arc = ((float10)*(float *)&shell[0x13] - (float10)k_f_1_3) * (float10)k_f_n2_2;
    }
    else {
      arc = (float10)sin((float10)*(float *)&shell[0x14] * (float10)k_f_3_1415927);
    }
    v = (RwV3d *)(shell + 5);
    tmp.prev_x = v->x;
    tmp.prev_y = *(float *)&shell[6];
    tmp.prev_z = *(float *)&shell[7];
    *(float *)&shell[0x13] = g_tick_seconds + *(float *)&shell[0x13];
    *(float *)&shell[0xb] = v->x;
    tmp.new_x = *(float *)&shell[8] + v->x;
    shell[0xc] = shell[6];
    scratch_i = shell[0x12];
    shell[0xd] = shell[7];
    v->x = tmp.new_x;
    tmp.len_or_new_z = *(float *)&shell[10] + *(float *)&shell[7];
    *(float *)&shell[7] = tmp.len_or_new_z;
    arc = arc * (float10)k_f_1_5 + (float10)*(float *)&shell[0x15];
    *(float *)&shell[6] = (float)arc;
    *(float *)&shell[0xe] = tmp.new_x - tmp.prev_x;
    tmp.delta_y = (float)(arc - (float10)tmp.prev_y);
    *(float *)&shell[0xf] = (float)(arc - (float10)tmp.prev_y);
    tmp.delta_z = tmp.len_or_new_z - tmp.prev_z;
    *(float *)&shell[0x10] = tmp.delta_z;
    frame = *(RwFrame **)(scratch_i + 4);
    if (shell[0x11] == 0) {
      tmp.aim_x = (tmp.new_x - tmp.prev_x) + tmp.new_x;
      tmp.aim_y = (float)((float10)tmp.delta_y + arc);
      tmp.aim_z = tmp.delta_z + tmp.len_or_new_z;
    }
    else {
      car_current_matrix(&tmp.car_matrix_ptr,*shell);
      tmp.aim_x = *(float *)(tmp.car_matrix_ptr + 0x30);
      tmp.aim_y = *(float *)(tmp.car_matrix_ptr + 0x34);
      tmp.aim_z = *(float *)(tmp.car_matrix_ptr + 0x38);
    }
    scratch_i = *(int *)(shell[0x12] + 4);
    tmp.up_dir.x = *(RwReal *)(scratch_i + 0x20);
    tmp.up_dir.y = *(RwReal *)(scratch_i + 0x24);
    tmp.up_dir.z = *(RwReal *)(scratch_i + 0x28);
    RwV3dNormalize(&tmp.up_dir,&tmp.up_dir);
    RwFrameTranslate(frame,v,rwCOMBINEREPLACE);
    rwframe_look_at((undefined4)frame,(undefined4)&tmp.aim_x,(undefined4)&tmp.up_dir);
    RwFrameScale(frame,&tmp.scale,rwCOMBINEPRECONCAT);
    weapon_mortar_trail_particles_add_r((int)shell);
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
