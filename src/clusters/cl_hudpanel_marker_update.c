// cluster-entries: hudpanel_marker_update
/* Installed cluster hudpanel_marker_update (tools/cluster_install.py from output/cluster_pilot_uc3/hudpanel_marker_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004f6240*/ extern struct RwSphere * RpAtomicGetWorldBoundingSphere(struct RpAtomic *p0);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5640*/ extern struct RwV3d * RwV3dTransformPoint(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/* DECLS-END */
extern float rp_random_float_range(float lo, float hi);
extern int vehicle_get_matrix_local(int slot, void *buf);
extern int explosion_queue(RwV3d *pos, float a, float b, float c, int d);
extern int fx_emitter_spawn_b_default(RwV3d *pos, void *axis, float angle, float scale);
extern int effect_sprite_spawn(int a, int *marker, void *colour, float s0, float s1);
extern int particle_pool_find_oldest_or_free(int a, int *marker, int b);
extern int sfx_move_playing_voice(int id, RwV3d *pos, int voice);
extern int sfx_voice_is_active(int id, int voice);
extern int trail_set_param(int slot, int a, int b);
extern int trigger_sphere_register(void *sphere);
extern int weapon_get_rw_801b14(void);
extern void LAB_0041bcb0(void);
extern int game_mode;
extern float g_tick_seconds, float_10_0, float_50_0, k_f_0, k_f_2, k_f_0_5, k_f_0_1, k_f_n0_1, k_f_0_005, k_f_0_75, k_f_0_015, k_f_0_25, k_f_15, k_f_0_125, k_f_n0_125;
extern RwV3d g_car_62ac74;

typedef struct TriggerSphere {
  float x, y, z, radius;
  int kind;
  int *owner;
  int zero;
  void (*callback)(void);
} TriggerSphere;

/* Per frame update of one player marker (esi = the marker record: +0x40 object, +0x44 state 1..4, +0x48 time, +0x4c vehicle, +0x50..+0x5c spin, +0x60 voice): state 1 flies the
   marker frame along the camera direction with a random scatter and spawns the particles / ray test against the world, state 2 explodes, 3 stops the voice, 4 clears the slot. */
static int hudpanel_particle_slot_spawn_r(float * p0);
extern void hudpanel_particle_slot_spawn(void);
static int hudpanel_marker_ray_cast_r(int * p0, float * p1, int p2);
extern void hudpanel_marker_ray_cast(void);
static int hudpanel_slot_clear_r(int * p0);
extern void hudpanel_slot_clear(void);
void hudpanel_marker_update_n(int *m)
{
  RwFrame *frame;
  RwMatrix *fm;
  int obj;
  RwV3d pos;
  float spawn[3];
  RwV3d mover;
  float ray[6];
  float veh[16];
  RwV3d dir;
  RwV3d local_pos;
  RwV3d target;
  RwV3d norm;
  RwMatrix inv;
  TriggerSphere sphere;
  unsigned char col[4];
  RwV3d diff;
  RwV3d expl;
  float len;
  float dist2;
  float factor;
  float speed;
  float spin_a;
  float spin_b;
  float x;
  float scale;
  RwSphere *bound;
  int hit;

  switch (m[0x11]) {
  case 1:
    frame = *(RwFrame **)(m[0x10] + 4);
    if (float_10_0 <= *(float *)&m[0x12]) {
      m[0x11] = 2;
    }
    obj = *(int *)(m[0x10] + 4);
    pos.x = *(float *)(obj + 0x40);
    pos.y = *(float *)(obj + 0x44);
    pos.z = *(float *)(obj + 0x48);
    spawn[0] = RwFrame_modelling(frame).at.x * k_f_n0_125 + pos.x;
    spawn[1] = RwFrame_modelling(frame).at.y * k_f_n0_125 + pos.y;
    spawn[2] = RwFrame_modelling(frame).at.z * k_f_n0_125 + pos.z;
    ray[0] = pos.x;
    ray[1] = pos.y;
    ray[2] = pos.z;
    spawn[0] = (float)(rp_random_float_range(-0.04f, 0.04f) + spawn[0]);
    spawn[1] = (float)(rp_random_float_range(-0.04f, 0.04f) + spawn[1]);
    spawn[2] = (float)(rp_random_float_range(-0.04f, 0.04f) + spawn[2]);
    hudpanel_particle_slot_spawn_r(spawn);
    vehicle_get_matrix_local(m[0x13], veh);
    diff.x = veh[12] - ray[0];
    local_pos.x = veh[12];
    diff.y = veh[13] - ray[1];
    local_pos.y = veh[13];
    local_pos.z = veh[14];
    diff.z = veh[14] - ray[2];
    len = RwV3dLength(&diff);
    dist2 = diff.z * diff.z + diff.x * diff.x + diff.y * diff.y;
    x = len * k_f_0_125;
    speed = x * k_f_0;
    local_pos.x = speed + local_pos.x;
    local_pos.y = x + local_pos.y + k_f_0_125;
    local_pos.z = speed + local_pos.z;
    RwMatrixInvert(&inv, &RwFrame_modelling(frame));
    RwV3dTransformPoint(&target, &local_pos, &inv);
    RwV3dNormalize(&norm, &target);
    if (norm.y > k_f_0_1) {
      m[0x14] = 0xc1000000;
    }
    else if (k_f_n0_1 > norm.y) {
      m[0x14] = 0x41000000;
    }
    else {
      m[0x14] = 0;
    }
    if (norm.x > k_f_0_1) {
      m[0x15] = 0x40800000;
    }
    else if (k_f_n0_1 > norm.x) {
      m[0x15] = 0xc0800000;
    }
    else {
      m[0x15] = 0;
    }
    if (game_mode == GAME_MODE_AIRSTRIKE) {
      speed = 2.0f;
      scale = k_f_2;
    }
    else {
      scale = float_50_0;
      if (!(float_50_0 <= dist2)) {
        scale = (dist2 >= k_f_0) ? dist2 : k_f_0;
      }
      speed = scale * k_f_0_005 + k_f_0_75;
      scale = float_50_0;
      if (!(float_50_0 <= dist2)) {
        scale = (dist2 >= k_f_0) ? dist2 : k_f_0;
      }
      scale = scale * k_f_0_015 + k_f_0_25;
    }
    spin_a = scale * *(float *)&m[0x14];
    *(float *)&m[0x14] = spin_a;
    scale = scale * *(float *)&m[0x15];
    *(float *)&m[0x15] = scale;
    *(float *)&m[0x16] = scale + *(float *)&m[0x16];
    *(float *)&m[0x17] = spin_a + *(float *)&m[0x17];
    RwFrameSetIdentity(frame);
    RwFrameRotate(frame, &g_car_62ac74, *(RwReal *)&m[0x16], rwCOMBINEREPLACE);
    RwFrameRotate(frame, &RwFrame_modelling(frame).right, *(RwReal *)&m[0x17], rwCOMBINEPOSTCONCAT);
    scale = speed * k_f_15;
    spawn[1] = RwFrame_modelling(frame).at.y * scale;
    spawn[2] = RwFrame_modelling(frame).at.z * scale;
    pos.x = scale * RwFrame_modelling(frame).at.x * g_tick_seconds + pos.x;
    pos.y = spawn[1] * g_tick_seconds + pos.y;
    pos.z = spawn[2] * g_tick_seconds + pos.z;
    RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);
    ray[3] = pos.x;
    ray[4] = pos.y;
    ray[5] = pos.z;
    hit = hudpanel_marker_ray_cast_r(m, ray, weapon_get_rw_801b14());
    if (hit != 0) {
      col[0] = 0xff;
      col[1] = 0x80;
      col[2] = 0;
      col[3] = 0x80;
      particle_pool_find_oldest_or_free(0, m, 0);
      effect_sprite_spawn(0, m, col, 2.0f, 4.0f);
      m[0x11] = 2;
    }
    if (m[0x11] != 2) {
      bound = RpAtomicGetWorldBoundingSphere((RpAtomic *)m[0x10]);
      sphere.x = (bound->center).x;
      sphere.y = (bound->center).y;
      sphere.z = (bound->center).z;
      sphere.radius = bound->radius;
      sphere.kind = 5;
      sphere.owner = m;
      sphere.zero = 0;
      sphere.callback = LAB_0041bcb0;
      trigger_sphere_register(&sphere);
      *(float *)&m[0x12] = g_tick_seconds + *(float *)&m[0x12];
    }
    sfx_move_playing_voice(0x14, &pos, m[0x18]);
    trail_set_param(m[0x13], 2, 1);
    return;
  case 2:
    obj = *(int *)(m[0x10] + 4);
    expl.x = *(float *)(obj + 0x40);
    expl.y = *(float *)(obj + 0x44) - k_f_0_5;
    expl.z = *(float *)(obj + 0x48);
    expl.x = (float)(rp_random_float_range(-0.5f, 0.5f) + expl.x);
    expl.z = (float)(rp_random_float_range(-0.5f, 0.5f) + expl.z);
    ray[0] = 2.0f;
    if (game_mode == GAME_MODE_AIRSTRIKE) {
      explosion_queue(&expl, 30.0f, 700.0f, 1500.0f, 1);
    }
    else {
      x = rp_random_float_range(250.0f, 350.0f);
      speed = rp_random_float_range(350.0f, 450.0f);
      scale = rp_random_float_range(25.0f, 35.0f);
      explosion_queue(&expl, scale, speed, x, 1);
    }
    scale = rp_random_float_range(1.5f, 2.0f);
    x = rp_random_float_range(0.0f, 360.0f);
    fx_emitter_spawn_b_default(&expl, &g_car_62ac74, x, scale);
    m[0x11] = 3;
    return;
  case 3:
    m[0x11] = 4;
    sfx_voice_is_active(0x14, m[0x18]);
    return;
  case 4:
    hudpanel_slot_clear_r(m);
  }
}

/* hudpanel_particle_slot_spawn_r: C -> original hudpanel_particle_slot_spawn (registers edi) */
__declspec(naked) int hudpanel_particle_slot_spawn_r(float * p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call hudpanel_particle_slot_spawn
        pop edi
        ret
    }
}

/* hudpanel_marker_ray_cast_r: C -> original hudpanel_marker_ray_cast (registers eax,edx,ecx) */
__declspec(naked) int hudpanel_marker_ray_cast_r(int * p0, float * p1, int p2)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov edx, dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 12]
        call hudpanel_marker_ray_cast
        ret
    }
}

/* hudpanel_slot_clear_r: C -> original hudpanel_slot_clear (registers eax) */
__declspec(naked) int hudpanel_slot_clear_r(int * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_slot_clear
        ret
    }
}

/* entry thunk at the original address of hudpanel_marker_update: register inputs (esi) -> natural C hudpanel_marker_update_n; ret 0 */
__declspec(naked) void hudpanel_marker_update(void)
{
    __asm {
        push esi
        call hudpanel_marker_update_n
        add esp, 4
        ret
    }
}
