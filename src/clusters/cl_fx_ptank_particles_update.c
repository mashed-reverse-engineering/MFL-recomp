// cluster-entries: fx_ptank_particles_update
/* Installed cluster fx_ptank_particles_update (tools/cluster_install.py from output/cluster_pilot_j1/fx_ptank_particles_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)
typedef signed char sbyte;
/*00475100*/ extern int course_height_grid_sample_with_normal(float *query);
/*00497770*/ extern int pool_push(int pool, int index);
/*00545d10*/ extern int RpPTankAtomicLock_00545d10(int atomic, int *lock, int flags, int bound);
/*00545f20*/ extern int RpPTankAtomicUnlock_00545f20(int atomic);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd410*/ extern float k_f_n2;
/*005dd61c*/ extern float k_f_0_001;
/*005ddb20*/ extern float k_f_0_9;
/*005ddc28*/ extern float k_f_0;
/*0072dff0*/ extern undefined g_fx_ptank_72dff0;
/*00745bd8*/ extern undefined4 g_particles_745bd8;
/*00745bdc*/ extern undefined4 g_particles_745bdc;
/*00745be0*/ extern undefined4 g_particles_745be0;
/*00745be4*/ extern undefined4 g_particles_745be4;
/*00745c48*/ extern undefined1 g_particles_745c48;
/*00745dc0*/ extern undefined4 g_particles_745dc0;
/*00745dce*/ extern undefined1 g_fx_ptank_745dce;
/*00745dd0*/ extern undefined4 g_fx_ptank_745dd0;
/*00745dd4*/ extern undefined4 g_fx_ptank_745dd4;
/*00745dd8*/ extern undefined4 g_fx_ptank_745dd8;
/*00745ddc*/ extern undefined4 g_fx_ptank_745ddc;
/*007519a4*/ extern undefined4 g_fx_atomic_a;
/*007519a8*/ extern undefined4 g_fx_atomic_b;
/*007519ac*/ extern float g_particles_7519ac;
/*007519b4*/ extern undefined4 g_particles_7519b4;
/*007519b8*/ extern undefined4 g_particles_7519b8;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*007ef798*/ extern float g_fx_ptank_7ef798;
/*007ef79c*/ extern float g_fx_ptank_7ef79c;
/*007ef804*/ extern undefined4 g_tick_number;
/* rounding point: the original stores the value to a stack float and reloads it */
static float float_barrier(float v) { return v; }

/* Pool of 1280 particles (40 blocks of 32, 0x4c bytes each); `p` points 0x18 bytes into a record:
   p[-6..-4] position, p[-3] colour (alpha in its top byte), p[-2], p[-1] size, p[0..2] velocity, p[3..5] acceleration,
   p[6] age rate, p[7] age (1.0 = dead), p[8] size growth, p[9] time of the last update, p[10] flags (0x80 = free sprite buffer,
   0x20 no ground test, 8 bounce, 2 die on ground, 0x1000 fade alpha), byte p+0x2c = PTank layer (0/1), byte p+0x2d = texture row,
   p[12] update callback.  Each live particle is stepped, written into the locked PTank buffers and dropped (pool_push) when it dies. */

/* "locks the PTank, steps the pool particles and unlocks" */

void fx_ptank_particles_update(float dt)

{
  uint *flags_ptr;
  byte *active_byte;
  float step;
  undefined4 tex_w4;
  float vx;
  float bounce_ny;
  int tmp;
  int layer;
  sbyte active_bit;
  int tex_off;
  uint index;
  float *p;
  double dvy, dvz;
  struct {
    int *block_flags;
    undefined1 pad_b4[8];
    int slot_in_block;
    float *block_particles;
    uint block_first_index;
    float *out_pos;
    float *out_uv;
    float *out_colour;
    undefined4 *out_aux;
    float *out_size;
    float hit_nx;
    float hit_ny;
    float hit_nz;
    uint index_copy;
    int emitted [2];
    float old_vx;
    float old_vy;
    float old_vz;
    float query_x;
    float query_y;
    float query_z;
    float *normal_out;
    float sum_vx;
    float sum_vy;
    undefined1 pad_50[4];
    int lock_uv_a [2];
    undefined1 lock_uv_b [8];
    int lock_colour_a [2];
    undefined1 lock_colour_b [8];
    int lock_aux_a [2];
    undefined1 lock_aux_b [8];
    int lock_pos_a [2];
    undefined1 lock_pos_b [8];
    float bounce_x;
  } frame;
  
  g_particles_7519ac = g_particles_7519ac + dt;
  frame.emitted[0] = 0;
  frame.emitted[1] = 0;
  g_particles_7519b4 = 0;
  RpPTankAtomicLock_00545d10(g_fx_atomic_a,frame.lock_pos_a,1,0x40000000);
  RpPTankAtomicLock_00545d10(g_fx_atomic_a,frame.lock_colour_a,2,0x40000000);
  RpPTankAtomicLock_00545d10(g_fx_atomic_a,frame.lock_uv_a,4,0x40000000);
  RpPTankAtomicLock_00545d10(g_fx_atomic_a,frame.lock_aux_a,0x80,0x40000000);
  RpPTankAtomicLock_00545d10(g_fx_atomic_b,frame.lock_pos_b,1,0x40000000);
  RpPTankAtomicLock_00545d10(g_fx_atomic_b,frame.lock_colour_b,2,0x40000000);
  RpPTankAtomicLock_00545d10(g_fx_atomic_b,frame.lock_uv_b,4,0x40000000);
  RpPTankAtomicLock_00545d10(g_fx_atomic_b,frame.lock_aux_b,0x80,0x40000000);
  frame.block_first_index = 0;
  frame.block_flags = (int *)&g_particles_745c48;
  frame.block_particles = (float *)&g_fx_ptank_72dff0;
  do {
    if (*frame.block_flags != 0) {
      frame.slot_in_block = 0;
      index = frame.block_first_index;
      p = frame.block_particles;
      frame.index_copy = frame.block_first_index;
      do {
        active_bit = (sbyte)(index & 7);
        active_byte = &g_particles_745c48 + ((int)index >> 3);
        if (((&g_particles_745c48)[(int)index >> 3] & (byte)(1 << active_bit)) == 0) {
LAB_00497ead:
          frame.slot_in_block = frame.slot_in_block + 1;
          index = index + 1;
          p = p + 0x13;
          frame.index_copy = index;
        }
        else {
          step = g_particles_7519ac - p[9];
          p[9] = g_particles_7519ac;
          vx = step * p[6] + p[7];
          p[7] = vx;
          if (!(vx >= float_1_0)) {      /* the original takes this branch for NaN too (C0 set by an unordered compare) */
            if (((((index & 7) == (g_tick_number & 7)) && (p[1] < k_f_0)) &&
                ((*(uint *)&p[10] & 0x20) == 0)) && (p[1] < k_f_0)) {
              frame.query_z = p[-4];
              frame.query_x = p[-6];
              frame.hit_nx = 0.0;
              frame.hit_ny = 1.0;
              frame.hit_nz = 0.0;
              frame.query_y = 0.0;
              frame.normal_out = (float *)0x0;
              if ((*(uint *)&p[10] & 8) != 0) {
                frame.normal_out = &frame.hit_nx;
              }
              tmp = course_height_grid_sample_with_normal(&frame.query_x);
              if (tmp != 2) {
                pool_push(g_particles_7519b8,index);
                *active_byte = *active_byte & ~('\x01' << active_bit);
                goto LAB_00497ebe;
              }
              if (p[-5] - frame.query_y < k_f_0_001 != (p[-5] - frame.query_y == k_f_0_001)) {
                if ((*(uint *)&p[10] & 2) != 0) {
                  pool_push(g_particles_7519b8,index);
                  *active_byte = *active_byte & ~('\x01' << active_bit);
                  goto LAB_00497ebe;
                }
                if ((*(uint *)&p[10] & 8) != 0) {
                  bounce_ny = frame.hit_ny * k_f_0_9;
                  frame.hit_nz = frame.hit_nz * k_f_0_9;
                  vx = (frame.hit_nz * p[2] +
                          frame.hit_nx * k_f_0_9 * *p + bounce_ny * p[1]) * k_f_n2;
                  frame.bounce_x = vx * frame.hit_nx * k_f_0_9;
                  *p = frame.bounce_x + *p;
                  p[1] = vx * bounce_ny + p[1];
                  p[2] = vx * frame.hit_nz + p[2];
                }
              }
            }
            frame.old_vx = *p;
            frame.old_vy = p[1];
            frame.old_vz = p[2];
            vx = float_barrier(p[3] * step + *p);      /* stored to a stack float in the original, reloaded for the products below */
            *p = vx;
            dvy = p[4] * step + p[1];      /* y and z velocity stay unrounded on the x87 stack for the position update */
            p[1] = dvy;
            dvz = p[5] * step + p[2];
            p[2] = dvz;
            frame.sum_vx = vx + frame.old_vx;
            frame.sum_vy = dvy + frame.old_vy;
            step = step * k_f_0_5;
            p[-6] = frame.sum_vx * step + p[-6];
            p[-5] = frame.sum_vy * step + p[-5];
            p[-4] = step * (dvz + frame.old_vz) + p[-4];
            tmp = g_particles_7519b4;
            if (*(char *)(p + 10) < '\0') {
              step = p[-2];
              frame.out_pos = (float *)((char *)&g_particles_745dc0 + g_particles_7519b4 * 0x20);
              frame.out_size = (float *)((char *)&g_fx_ptank_745dd0 + g_particles_7519b4 * 0x20);
              frame.out_colour = (float *)((char *)&g_fx_ptank_745ddc + g_particles_7519b4 * 0x20);
              (&g_fx_ptank_745dce)[g_particles_7519b4 * 0x20] = 0;
              *frame.out_size = vx * step;
              *(float *)&(&g_fx_ptank_745dd4)[tmp * 8] = p[-2] * p[1];
              g_particles_7519b4 = g_particles_7519b4 + 1;
              *(float *)&(&g_fx_ptank_745dd8)[tmp * 8] = p[-2] * p[2];
            }
            else {
              layer = (int)*(char *)(p + 0xb);
              tmp = frame.emitted[layer];
              frame.out_pos = (float *)(frame.lock_pos_a[layer * 2 + 1] * tmp + frame.lock_pos_a[layer * 2]);
              frame.out_uv = (float *)(frame.lock_uv_a[layer * 2 + 1] * tmp + frame.lock_uv_a[layer * 2]);
              frame.out_colour = (float *)(frame.lock_colour_a[layer * 2 + 1] * tmp + frame.lock_colour_a[layer * 2]);
              frame.out_aux = (undefined4 *)(frame.lock_aux_a[layer * 2 + 1] * tmp + frame.lock_aux_a[layer * 2]);
              *frame.out_uv = (p[8] * p[7] + float_1_0) * p[-2];
              frame.out_uv[1] = (p[8] * p[7] + float_1_0) * p[-1];
              tex_off = (uint)*(byte *)((int)p + 0x2d) * 0x10;
              *frame.out_aux = *(undefined4 *)((char *)((undefined *)&g_particles_745bd8) + tex_off);
              frame.out_aux[1] = *(undefined4 *)((char *)((undefined *)&g_particles_745bdc) + tex_off);
              tex_off = (uint)*(byte *)((int)p + 0x2d) * 0x10;
              frame.out_aux[2] = *(undefined4 *)((char *)((undefined *)&g_particles_745be0) + tex_off);
              tex_w4 = *(undefined4 *)((char *)((undefined *)&g_particles_745be4) + tex_off);
              frame.emitted[layer] = tmp + 1;
              frame.out_aux[3] = tex_w4;
              index = frame.index_copy;
            }
            *frame.out_pos = p[-6];
            frame.out_pos[1] = p[-5];
            frame.out_pos[2] = p[-4];
            *frame.out_colour = p[-3];
            if ((*(uint *)&p[10] & 0x1000) != 0) {
              *(undefined1 *)((int)frame.out_colour + 3) = (undefined1)(int)((float)*((byte *)p - 9) * (float_1_0 - p[7]));      /* alpha fades out with the age */
            }
            if (*(code **)&p[0xc] != (code *)0x0) {
              (**(code **)&p[0xc])(p + -6,&frame.out_pos);
            }
            if (7 < *(byte *)((int)frame.out_colour + 3)) goto LAB_00497ead;
            pool_push(g_particles_7519b8,index);
            *active_byte = *active_byte & ~('\x01' << active_bit);
          }
          else {
            pool_push(g_particles_7519b8,index);
            *active_byte = *active_byte & ~('\x01' << active_bit);
          }
        }
LAB_00497ebe:;
      } while (frame.slot_in_block < 0x20);
    }
    frame.block_particles = frame.block_particles + 0x260;
    frame.block_first_index = frame.block_first_index + 0x20;
    frame.block_flags = frame.block_flags + 1;
    if ((int)&g_particles_745be4 + 0xb < (int)frame.block_particles) {
      RpPTankAtomicUnlock_00545f20(g_fx_atomic_a);
      RpPTankAtomicUnlock_00545f20(g_fx_atomic_b);
      tmp = g_fx_atomic_a;
      *(uint *)(*(int *)(g_ptank_plugin_offset + g_fx_atomic_a) + 0x40) =
           *(uint *)(*(int *)(g_ptank_plugin_offset + g_fx_atomic_a) + 0x40) | 0x800000;
      *(int *)(*(int *)(g_ptank_plugin_offset + tmp) + 4) = frame.emitted[0];
      tmp = g_fx_atomic_b;
      flags_ptr = (uint *)(*(int *)(g_ptank_plugin_offset + g_fx_atomic_b) + 0x40);
      *flags_ptr = *flags_ptr | 0x800000;
      *(int *)(*(int *)(g_ptank_plugin_offset + tmp) + 4) = frame.emitted[1];
      g_fx_ptank_7ef798 = (float)(frame.emitted[1] + frame.emitted[0]);
      if (g_fx_ptank_7ef79c < (float)(frame.emitted[1] + frame.emitted[0])) {
        g_fx_ptank_7ef79c = g_fx_ptank_7ef798;
      }
      return;
    }
  } while( true );
}

