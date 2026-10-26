// cluster-entries: fx_emitter_init
/* Installed cluster fx_emitter_init (tools/cluster_install.py from output/cluster_pilot11/fx_emitter_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c45e0*/ extern float rp_random_float_range(float p0, float p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd628*/ extern float k_f_0_25;
/*005ddc28*/ extern float k_f_0;
/*005de47c*/ extern float k_f_n0_25;
/* fx_emitter_init */

/* WARNING: Unknown calling convention */
/* "initialises one 0x1D8 byte emitter slot from position, size and flags" */

void fx_emitter_init_n(int emitter, float *pos, undefined4 unused_3, undefined4 unused_4, float rise, int size_or_count, undefined4 flags)

{
  float *point_a;
  float *point_b;
  RwV3d *dir_a;
  RwV3d *dir_b;
  float zero;
  float *corner;
  float rnd;
  
  /* emitter slot (0x1D8 bytes, edi): +0xc0 position, +0xd0 rise (negative when > 0), +0xdc/+0xe0 size, +0xe4 flags; four corner records are jittered by +-size/4 around pos and get two random unit directions each */
  zero = k_f_0;
  *(float *)(emitter + 0xc0) = *pos;
  *(float *)(emitter + 0xc4) = pos[1];
  *(float *)(emitter + 200) = pos[2];
  *(undefined4 *)(emitter + 0xd0) = 0;
  *(undefined4 *)(emitter + 0xd4) = 0;
  *(int *)(emitter + 0xdc) = size_or_count;
  *(int *)(emitter + 0xe0) = size_or_count;
  *(undefined4 *)(emitter + 0xe4) = flags;
  *(undefined4 *)(emitter + 0xd8) = 1;
  *(undefined4 *)(emitter + 0xe8) = 1;
  if (zero < rise) {
    *(float *)(emitter + 0xd0) = -rise;
  }
  corner = (float *)(emitter + 8);
  size_or_count = 4;
  do {
    point_a = corner + -2;
    *point_a = *pos;
    corner[-1] = pos[1];
    *corner = pos[2];
    point_b = corner + 0x16;
    *point_b = *pos;
    corner[0x17] = pos[1];
    corner[0x18] = pos[2];
    rnd = rp_random_float_range(*(float *)(emitter + 0xdc) * k_f_n0_25,
                                  *(float *)(emitter + 0xdc) * k_f_0_25);
    *point_a = rnd + *point_a;
    rnd = rp_random_float_range(*(float *)(emitter + 0xdc) * k_f_n0_25,
                                  *(float *)(emitter + 0xdc) * k_f_0_25);
    corner[-1] = rnd + corner[-1];
    rnd = rp_random_float_range(*(float *)(emitter + 0xdc) * k_f_n0_25,
                                  *(float *)(emitter + 0xdc) * k_f_0_25);
    *corner = rnd + *corner;
    rnd = rp_random_float_range(*(float *)(emitter + 0xdc) * k_f_n0_25,
                                  *(float *)(emitter + 0xdc) * k_f_0_25);
    *point_b = rnd + *point_b;
    rnd = rp_random_float_range(*(float *)(emitter + 0xdc) * k_f_n0_25,
                                  *(float *)(emitter + 0xdc) * k_f_0_25);
    corner[0x17] = rnd + corner[0x17];
    rnd = rp_random_float_range(*(float *)(emitter + 0xdc) * k_f_n0_25,
                                  *(float *)(emitter + 0xdc) * k_f_0_25);
    dir_a = (RwV3d *)(corner + 10);
    corner[0x18] = rnd + corner[0x18];
    rnd = rp_random_float_range(-1.0f,1.0f);
    dir_a->x = rnd;
    rnd = rp_random_float_range(-1.0f,1.0f);
    corner[0xb] = rnd;
    rnd = rp_random_float_range(-1.0f,1.0f);
    corner[0xc] = rnd;
    dir_b = (RwV3d *)(corner + 0x22);
    rnd = rp_random_float_range(-1.0f,1.0f);
    dir_b->x = rnd;
    rnd = rp_random_float_range(-1.0f,1.0f);
    corner[0x23] = rnd;
    rnd = rp_random_float_range(-1.0f,1.0f);
    corner[0x24] = rnd;
    RwV3dNormalize(dir_a,dir_a);
    RwV3dNormalize(dir_b,dir_b);
    corner = corner + 3;
    size_or_count = size_or_count + -1;
  } while (size_or_count != 0);
  return;
}

/* entry thunk at the original address of fx_emitter_init: register inputs (edi,s0,s1,s2,s3,s4,s5) -> natural C fx_emitter_init_n; ret 0 */
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
        call fx_emitter_init_n
        add esp, 28
        ret
    }
}
