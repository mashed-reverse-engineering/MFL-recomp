#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*004a0600*/ extern int rain_buffers_init_colours();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd364*/ extern float k_f_n1;
/*0062ac2c*/ extern float g_app_62ac2c;
/*0062ac3c*/ extern undefined4 g_app_62ac3c;
/*0062ac40*/ extern undefined4 g_app_62ac40;
/*0062ac44*/ extern undefined4 g_app_62ac44;
/*0062ac48*/ extern undefined4 g_app_62ac48;
/*0062ac4c*/ extern undefined4 g_app_62ac4c;
/*0062ac50*/ extern undefined4 g_app_62ac50;
/*0062ac54*/ extern undefined4 g_app_62ac54;
/*0062ac58*/ extern undefined4 g_app_62ac58;
/*0062ac5c*/ extern undefined4 g_app_62ac5c;
/*0062ac60*/ extern undefined4 g_app_62ac60;
/*0076e3b8*/ extern undefined4 g_app_76e3b8;
/*0076e3bc*/ extern undefined4 g_app_76e3bc;
/*0076e3c4*/ extern undefined4 g_app_76e3c4;
/*0076e3c8*/ extern undefined4 g_app_76e3c8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00885840*/ extern undefined4 g_fx_buffers_885840;
/*00886000*/ extern undefined4 g_fx_buffers_886000;
/*00886004*/ extern undefined4 g_fx_buffers_886004;
/*00886008*/ extern undefined4 g_fx_buffers_886008;
/*0088600c*/ extern undefined4 g_fx_buffers_88600c;
/*00886010*/ extern undefined4 g_fx_buffers_886010;
/*00886014*/ extern undefined4 g_fx_buffers_886014;
#define PARTICLE_BUF_BYTES (0x7000)
#define STATE_BUF_BYTES (0x2a00)
#define STATE_ENTRY_BYTES (0xc)
#define PARTICLE_DIR_OFFSET (0x10)
#define PARTICLE_INDEX_MASK (0x1f)
#define SPREAD_MAX_BITS (0x41600000) /* float 14.0 */
#define FILL_MIN_BITS (0xc1000000) /* float -8.0 */
#define FILL_MAX_BITS (0x41000000) /* float 8.0 */
/* fx_buffers_alloc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "allocates the 0x7000 / 0x2A00 byte buffers" */

/*
 * Allocates and seeds the fx particle buffers. Takes no parameters. Returns 1 on success, 0 if the particle buffer allocation failed.
 * (inferred from the code; the buffer contents are not documented elsewhere)
 *   g_app_76e3bc: PARTICLE_BUF_BYTES, 896 records of 32 bytes. +0x00 float, +0x04/+0x08 x/y spawn values, +0x0c ushort index
 *                 (particle_index & PARTICLE_INDEX_MASK), +0x10..+0x18 random unit direction scaled by g_app_62ac2c.
 *   g_app_76e3b8: STATE_BUF_BYTES, 896 entries of STATE_ENTRY_BYTES. [0] spread in 0..14, [1] and [2] jitter in [k_f_n1 * spread, spread].
 *   g_fx_buffers_885840..886000: 496 floats in [-8, 8].
 * Then sets g_fx_buffers_886000..886014 to 0/1/0/2/0/3 (meaning unknown) and calls rain_buffers_init_colours().
 */

undefined4 fx_buffers_alloc(void)

{
  float jitter_lo_1;
  float jitter_lo_2;
  float scratch_float; /* speed scale, later reused as the spread value */
  void *state_buffer;
  ushort particle_index;
  RwV3d *dir;
  float *cursor;
  int state_offset;
  float10 rand_value;

  g_app_76e3bc = (undefined4)(RwMalloc(PARTICLE_BUF_BYTES,0));
  g_app_76e3b8 = (undefined4)(RwMalloc(STATE_BUF_BYTES,0));
  /* particle buffer allocation failed: nothing to seed */
  if (g_app_76e3bc == 0) {
    return 0;
  }
  particle_index = 0;
  state_offset = 0;
  dir = (RwV3d *)((int)g_app_76e3bc + PARTICLE_DIR_OFFSET);
  /* one particle per pass: fill its 32-byte record and its 12-byte state entry */
  do {
    rand_value = (float10)random_range(g_app_62ac3c,g_app_62ac40);
    dir[-2].z = (float)rand_value;
    rand_value = (float10)random_range(g_app_76e3c4,g_app_76e3c8);
    dir[-1].x = (float)rand_value;
    rand_value = (float10)random_range(g_app_62ac44,g_app_62ac48);
    dir[-1].y = (float)rand_value;
    rand_value = (float10)random_range(g_app_62ac4c,g_app_62ac50);
    dir->x = (float)rand_value;
    rand_value = (float10)random_range(g_app_62ac54,g_app_62ac58);
    dir->y = (float)rand_value;
    rand_value = (float10)random_range(g_app_62ac5c,g_app_62ac60);
    dir->z = (float)rand_value;
    RwV3dNormalize(dir,dir);
    dir->x = g_app_62ac2c * dir->x;
    dir->y = g_app_62ac2c * dir->y;
    scratch_float = g_app_62ac2c;
    *(ushort *)&dir[-1].z = particle_index & PARTICLE_INDEX_MASK;
    dir->z = scratch_float * dir->z;
    rand_value = (float10)random_range(0,SPREAD_MAX_BITS);
    state_buffer = (void *)g_app_76e3b8;
    scratch_float = (float)rand_value;
    *(float *)(state_offset + (int)g_app_76e3b8) = scratch_float;
    cursor = (float *)(state_offset + (int)state_buffer);
    rand_value = (float10)random_range((jitter_lo_1 = scratch_float * k_f_n1, *(int *)&jitter_lo_1),*(int *)&scratch_float);
    cursor[1] = (float)rand_value;
    rand_value = (float10)random_range((jitter_lo_2 = *cursor * k_f_n1, *(int *)&jitter_lo_2),*(int *)&*cursor);
    *(float *)(state_offset + 8 + (int)g_app_76e3b8) = (float)rand_value;
    dir = (RwV3d *)&dir[2].z;
    state_offset = state_offset + STATE_ENTRY_BYTES;
    particle_index = particle_index + 1;
  } while (state_offset < STATE_BUF_BYTES);
  /* fill the g_fx_buffers_885840 table with random floats in [-8, 8] (inferred) */
  cursor = (float *)&g_fx_buffers_885840;
  do {
    rand_value = (float10)random_range(FILL_MIN_BITS,FILL_MAX_BITS);
    *cursor = (float)rand_value;
    cursor = cursor + 1;
  } while ((int)cursor < ((int)&g_fx_buffers_886000));
  g_fx_buffers_886000 = 0;
  g_fx_buffers_88600c = 1;
  g_fx_buffers_886004 = 0;
  g_fx_buffers_886010 = 2;
  g_fx_buffers_886008 = 0;
  g_fx_buffers_886014 = 3;
  rain_buffers_init_colours();
  return 1;
}


