// cluster-entries: fx_emitter_init
/* Installed cluster fx_emitter_init (tools/cluster_install.py from output/cluster_pilot11/fx_emitter_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c45e0*/ extern float rp_random_float_range(float p0, float p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd628*/ extern float k_f_0_25;
/*005e64a4*/ extern float k_f_0;
/*005de47c*/ extern float k_f_n0_25;
/* emitter slot, FX_EMITTER_SLOT_SIZE bytes: four corner points with two random directions each, then the emitter state */
typedef struct FxEmitterSlot {
    RwV3d pos_a[4];       /* 0x00: corner start points (pos + jitter of +-size/4) */
    RwV3d dir_a[4];       /* 0x30: normalised random directions */
    RwV3d pos_b[4];       /* 0x60 */
    RwV3d dir_b[4];       /* 0x90 */
    RwV3d pos;            /* 0xc0: emitter position */
    undefined4 unknown_cc;
    undefined4 rise;      /* 0xd0: float, -rise when the argument is > 0 */
    undefined4 rise_b;    /* 0xd4 */
    int one_d8;           /* 0xd8: set to 1 */
    int size;             /* 0xdc: float bits */
    int size_b;           /* 0xe0: float bits */
    undefined4 flags;     /* 0xe4 */
    int one_e8;           /* 0xe8: set to 1 */
} FxEmitterSlot;
/* fx_emitter_init */

/* "initialises one 0x1D8 byte emitter slot from position, size and flags" */

void fx_emitter_init_n(FxEmitterSlot *emitter, float *pos, undefined4 unused_3, undefined4 unused_4, float rise, int size_or_count, undefined4 flags)

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
  emitter->pos.x = *pos;
  emitter->pos.y = pos[1];
  emitter->pos.z = pos[2];
  emitter->rise = 0;
  emitter->rise_b = 0;
  emitter->size = size_or_count;
  emitter->size_b = size_or_count;
  emitter->flags = flags;
  emitter->one_d8 = 1;
  emitter->one_e8 = 1;
  if (zero < rise) {
    *(float *)&emitter->rise = -rise;
  }
  corner = (float *)&emitter->pos_a[0].z; /* walks the four corners, 12 bytes (one RwV3d) per step */
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
    rnd = rp_random_float_range(*(float *)&emitter->size * k_f_n0_25,
                                  *(float *)&emitter->size * k_f_0_25);
    *point_a = rnd + *point_a;
    rnd = rp_random_float_range(*(float *)&emitter->size * k_f_n0_25,
                                  *(float *)&emitter->size * k_f_0_25);
    corner[-1] = rnd + corner[-1];
    rnd = rp_random_float_range(*(float *)&emitter->size * k_f_n0_25,
                                  *(float *)&emitter->size * k_f_0_25);
    *corner = rnd + *corner;
    rnd = rp_random_float_range(*(float *)&emitter->size * k_f_n0_25,
                                  *(float *)&emitter->size * k_f_0_25);
    *point_b = rnd + *point_b;
    rnd = rp_random_float_range(*(float *)&emitter->size * k_f_n0_25,
                                  *(float *)&emitter->size * k_f_0_25);
    corner[0x17] = rnd + corner[0x17];
    rnd = rp_random_float_range(*(float *)&emitter->size * k_f_n0_25,
                                  *(float *)&emitter->size * k_f_0_25);
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
