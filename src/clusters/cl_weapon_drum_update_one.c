// cluster-entries: weapon_drum_update_one
/* Installed cluster weapon_drum_update_one (tools/cluster_install.py from output/cluster_pilot_k/weapon_drum_update_one.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0045f5a0*/ extern int explosion_queue(void *drum, float min_radius, float max_radius, float power, int kind);
/*0045fad0*/ extern int weapon_drum_update_one_cb_45fad0();
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*00485660*/ extern int particle_pool_find_oldest_or_free();
/*004866d0*/ extern int fx_emitter_spawn_b_default();
/*00487670*/ extern int effect_sprite_spawn();
/*00496850*/ extern int trigger_sphere_register(float *drum);
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004c4720*/ extern void FUN_004c4720(void *out, void *a, void *b, unsigned int hit_word);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004c51a0*/ extern void FUN_004c51a0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d3cc0*/ extern int RwMatrixUpdate();
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004f44d0*/ extern struct RpWorld * __cdecl RpWorldRemoveAtomic(struct RpWorld *p0, struct RpAtomic *p1);
/*004f4560*/ extern struct RpWorld * __cdecl RpAtomicGetWorld(struct RpAtomic *p0);
/*004f6240*/ extern struct RwSphere * RpAtomicGetWorldBoundingSphere(struct RpAtomic *p0);
/*005dd41c*/ extern float float_5_0;
/*005ddafc*/ extern float k_f_0_05;
/*005de1ac*/ extern float k_f_0_125;
/*005df72c*/ extern float k_f_0_5df72c;
/*005df730*/ extern float k_f_n5_5df730;
/*005df734*/ extern float k_f_0_5df734;
/*005df738*/ extern float k_f_720;
/*0060da34*/ extern undefined g_weapon_drum_60da34;
/*0062ac74*/ extern float g_car_62ac74;
/*00632ae8*/ extern undefined4 one;
/*007ef7f4*/ extern float g_tick_seconds;
/* weapon_drum_update_one */

/* "updates one depth charge drum: frame transform / spin, trigger sphere, fx emitter, removal from
   the world" */

static int weapon_fx_explosion_slot_clear_r(undefined4 * p0);
extern void weapon_fx_explosion_slot_clear(void);
/* drum fields used: [4..6] position, [7..9] velocity, [0xb] state (0 idle, 1 follow frame, 2 falling, 3 hit or timed out: emitter spawned, atomic removed, 4 explosion_queue, 5 slot clear), [0xc] atomic, [0xd] frame (position frame), [0xe] spin frame, [0xf] age seconds, [0x10] spin angle */
void weapon_drum_update_one(undefined4 *drum)

{
  float fbits1;
  float fbits2;
  float fbits3;
  float far_power;
  float near_radius;
  float near_min;
  float *pos_ptr;
  float vel_x_k;
  RwMatrix *matrix_ptr;
  undefined4 world_ptr;
  int state_or_tmp;
  RwSphere *bound_sphere;
  RpWorld *world;
  RwReal *src_matrix;
  float10 rnd;
  float *seg_ptr;
  float *hit_ptr;
  RwV3d np; /* new position */
  RwUInt32 particle_colour;
  RwMatrix particle_mtx;
  undefined4 particle_kind;
  undefined1 particle_flag;
  struct { /* movement segment: old position -> new position (RwV3d, RwV3d), also the line handed to the world intersection */
    RwV3d from;
    RwV3d to;
  } seg;
  RwV3d scale_vec;
  struct { /* line intersection result record (stack layout of the original) */
    float nx;
    float ny;
    float nz;
    undefined4 pad_c0;
    undefined1 bc [40];
    undefined4 word_38;
    undefined4 word_3c; /* world_intersect_line stores a 64 byte hit record (callee_extents.txt); 60 bytes overwrote the return address */
  } hit;
  struct { /* trigger sphere record handed to trigger_sphere_register */
    RwSphere sphere;
    undefined4 kind;
    undefined4 *owner;
    undefined4 flags;
    code *callback;
  } trigger;
  
  state_or_tmp = drum[0xb];
  if (state_or_tmp != 0) {
    if (state_or_tmp == 1) {
      matrix_ptr = RwFrameGetLTM((RwFrame *)drum[0xd]);
      *(float *)&drum[4] = (matrix_ptr->pos).x;
      *(float *)&drum[5] = (matrix_ptr->pos).y;
      *(float *)&drum[6] = (matrix_ptr->pos).z;
      return;
    }
    if (state_or_tmp == 2) {
      if (!(*(float *)&drum[0xf] > float_5_0)) {      /* NaN takes the branch (original: jne) */
        pos_ptr = (float *)(drum + 4);
        np.x = g_tick_seconds * *(float *)&drum[7] + *pos_ptr;
        seg.from.x = *pos_ptr;
        seg.from.y = *(float *)&drum[5];
        seg.from.z = *(float *)&drum[6];
        np.y = g_tick_seconds * *(float *)&drum[8] + *(float *)&drum[5];
        hit_ptr = (float *)&hit;
        np.z = g_tick_seconds * *(float *)&drum[9] + *(float *)&drum[6];
        seg_ptr = &seg.from.x;
        seg.to.x = np.x;
        seg.to.y = np.y;
        seg.to.z = np.z;
        world_ptr = weapon_get_rw_801b14();
        state_or_tmp = world_intersect_line(world_ptr,seg_ptr,hit_ptr);
        if (state_or_tmp == 0) {
          bound_sphere = RpAtomicGetWorldBoundingSphere((RpAtomic *)drum[0xc]);
          trigger.sphere.center.x = (bound_sphere->center).x;
          trigger.sphere.center.y = (bound_sphere->center).y;
          trigger.sphere.center.z = (bound_sphere->center).z;
          trigger.sphere.radius = bound_sphere->radius;
          trigger.kind = 0x1b;
          trigger.owner = drum;
          trigger.flags = 0;
          trigger.callback = weapon_drum_update_one_cb_45fad0;
          trigger_sphere_register(&trigger);
          src_matrix = (RwReal *)(drum[0xd] + 0x10);
          matrix_ptr = &particle_mtx;
          for (state_or_tmp = 0x10; state_or_tmp != 0; state_or_tmp = state_or_tmp + -1) {
            (matrix_ptr->right).x = *src_matrix;
            src_matrix = src_matrix + 1;
            matrix_ptr = (RwMatrix *)&(matrix_ptr->right).y;
          }
          particle_mtx.pos.x = np.x;
          particle_mtx.pos.y = np.y;
          particle_mtx.pos.z = np.z;
          RwMatrixUpdate(&particle_mtx);
          RwFrameTransform((RwFrame *)drum[0xd],&particle_mtx,rwCOMBINEREPLACE);
          vel_x_k = k_f_0_5df72c;
          *pos_ptr = np.x;
          *(float *)&drum[5] = np.y;
          *(float *)&drum[6] = np.z;
          *(float *)&drum[7] = vel_x_k * g_tick_seconds + *(float *)&drum[7];
          *(float *)&drum[8] = k_f_n5_5df730 * g_tick_seconds + *(float *)&drum[8];
          *(float *)&drum[9] = k_f_0_5df734 * g_tick_seconds + *(float *)&drum[9];
          RwFrameRotate((RwFrame *)drum[0xe],(RwV3d *)&g_weapon_drum_60da34,*(float *)&drum[0x10]
                        ,rwCOMBINEREPLACE);
          *(float *)&drum[0x10] = g_tick_seconds * k_f_720 + *(float *)&drum[0x10];
          *(float *)&drum[0xf] = g_tick_seconds + *(float *)&drum[0xf];
        }
        else {
          particle_colour = 0x400080ff;
          FUN_004c4720(&np,&seg.from,&seg.to,hit.word_38);
          np.x = hit.nx * k_f_0_05 + np.x;
          np.y = hit.ny * k_f_0_05 + np.y;
          np.z = hit.nz * k_f_0_05 + np.z;
          FUN_004c51a0(&particle_mtx,hit.bc,&np);
          effect_sprite_spawn(1,&particle_mtx,&particle_colour,0x40000000,0x40000000);
          effect_sprite_spawn(0,&particle_mtx,&particle_colour,0x40400000,0x40000000);
          scale_vec.y = 2.0;
          scale_vec.x = 2.0;
          scale_vec.z = 1.0;
          RwMatrixScale(&particle_mtx,&scale_vec,rwCOMBINEPRECONCAT);
          particle_pool_find_oldest_or_free(0,&particle_mtx,0);
          drum[0xb] = 3;
        }
        particle_colour = 0x40c8c8c8;
        rnd = (float10)rp_random_float_range(0x3e19999a,0x3e4ccccd);
        state_or_tmp = drum[0xd];
        np.x = *(float *)(state_or_tmp + 0x10) * k_f_0_125 + *(float *)(state_or_tmp + 0x40);
        np.y = *(float *)(state_or_tmp + 0x14) * k_f_0_125 + *(float *)(state_or_tmp + 0x44);
        particle_flag = 6;
        np.z = *(float *)(state_or_tmp + 0x18) * k_f_0_125 + *(float *)(state_or_tmp + 0x48);
        particle_mtx.pos.x = 2.5;
        particle_mtx.flags = particle_colour;
        particle_mtx.up.x = (RwReal)rnd;
        particle_kind = 0x1120;
        particle_mtx.up.y = (RwReal)rnd;
        particle_mtx.pos.z = 1.0;
        particle_mtx.right.x = np.x;
        particle_mtx.right.y = np.y;
        particle_mtx.right.z = np.z;
        rnd = (float10)rp_random_float_range(0xbe19999a,0x3e19999a);
        particle_mtx.up.z = (RwReal)rnd;
        rnd = (float10)rp_random_float_range(0xbe19999a,0x3e19999a);
        fbits3 = (float)rnd;
        particle_mtx.pad1 = *(RwUInt32 *)&fbits3;
        rnd = (float10)rp_random_float_range(0xbe19999a,0x3e19999a);
        particle_mtx.at.x = (RwReal)rnd;
        particle_mtx.at.y = 0.0;
        particle_mtx.at.z = 0.0;
        particle_mtx.pad2 = 0;
        fx_trail_particle_spawn_interpolated(&particle_mtx,0,0,0);
        return;
      }
      drum[0xb] = 3;
      return;
    }
    if (state_or_tmp == 3) {
      drum[0xb] = 4;
      rnd = (float10)rp_random_float_range(0x3fc00000,0x40000000);
      fx_emitter_spawn_b_default(drum + 4,&g_car_62ac74,0x43b40000,(fbits1 = (float)rnd, *(int *)&fbits1));
      *drum = drum[4];
      drum[1] = drum[5];
      drum[2] = drum[6];
      drum[3] = 0x3dcccccd;
      world = RpAtomicGetWorld((RpAtomic *)drum[0xc]);
      if (world != (RpWorld *)0x0) {
        RpWorldRemoveAtomic(world,(RpAtomic *)drum[0xc]);
        return;
      }
    }
    else {
      if (state_or_tmp == 4) {
        drum[3] = 0x40400000;
        far_power = rp_random_float_range(0x43160000,0x437a0000);
        near_radius = rp_random_float_range(0x42c80000,0x43480000);
        near_min = rp_random_float_range(0x420c0000,0x42340000);
        explosion_queue(drum,near_min,near_radius,far_power,2);
        drum[0xb] = 5;
        return;
      }
      if (state_or_tmp == 5) {
        drum[0xb] = 0;
        weapon_fx_explosion_slot_clear_r(drum);
      }
    }
  }
  return;
}

/* weapon_fx_explosion_slot_clear_r: C -> original weapon_fx_explosion_slot_clear (registers esi) */
__declspec(naked) int weapon_fx_explosion_slot_clear_r(undefined4 * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_explosion_slot_clear
        pop esi
        ret
    }
}
