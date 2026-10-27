// cluster-entries: course_city_cb1_sub_455ee0
/* Installed cluster course_city_cb1_sub_455ee0 (tools/cluster_install.py from output/cluster_pilot_k/course_city_cb1_sub_455ee0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();
/*004c45c0*/ extern int rp_random_int_range(int p0, int p1);
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*005df53c*/ extern float k_f_1_95;
/* course_city_cb1_sub_455ee0 */

/* single caller course_city_cb1 */

void course_city_cb1_sub_455ee0_n(undefined4 *position)

{
  char jitter;
  int count_left;
  float10 rand_val;
  struct {
    undefined4 fx_pos_x;
    undefined4 fx_pos_y;
    undefined4 fx_pos_z;
    char fx_col_r;
    char fx_col_g;
    undefined1 fx_col_b;
    undefined1 fx_col_a;
    undefined4 fx_size_start;
    undefined4 fx_size_end;
    float fx_vel_x;
    float fx_vel_y;
    float fx_vel_z;
    undefined4 fx_accel_x;
    undefined4 fx_accel_y;
    undefined4 fx_accel_z;
    undefined4 fx_life_scale;
    undefined1 fx_field_34[4];
    undefined4 fx_field_38;
    undefined1 fx_field_3c[4];
    undefined4 fx_flags;
  } frame;   /* fx_pos_x .. fx_flags: the particle record of fx_trail_particle_spawn_interpolated (layout as FxParticle in cl_fx_dust_particle_spawn.c, without the trailing kind byte) */
  
  count_left = 3;
  do {
    jitter = rp_random_int_range(0,100);
    frame.fx_pos_x = *position;
    frame.fx_pos_y = position[1];
    frame.fx_pos_z = position[2];
    rand_val = (float10)rp_random_float_range(0xbecccccd,0x3ecccccd);
    frame.fx_vel_x = (float)rand_val;
    rand_val = (float10)random_range(0x3f4ccccd,0x3f8ccccd);
    frame.fx_vel_y = (float)(rand_val * (float10)k_f_1_95);
    rand_val = (float10)rp_random_float_range(0xbecccccd,0x3ecccccd);
    frame.fx_vel_z = (float)rand_val;
    frame.fx_col_r = jitter + '@';
    frame.fx_col_g = jitter + -0x80;
    frame.fx_col_b = 0xff;
    frame.fx_col_a = 0xff;
    frame.fx_life_scale = 0x3f86bca2;
    frame.fx_field_38 = 0x3f800000;
    frame.fx_flags = 0xa0;
    frame.fx_size_start = 0x3e800000;
    frame.fx_size_end = 0x3e800000;
    frame.fx_accel_x = 0;
    frame.fx_accel_y = 0xc0bccccd;
    frame.fx_accel_z = 0;
    fx_trail_particle_spawn_interpolated(&frame.fx_pos_x,0,0,0);
    count_left = count_left + -1;
  } while (count_left != 0);
  return;
}

/* entry thunk at the original address of course_city_cb1_sub_455ee0: register inputs (edi) -> natural C course_city_cb1_sub_455ee0_n; ret 0 */
__declspec(naked) void course_city_cb1_sub_455ee0(void)
{
    __asm {
        push edi
        call course_city_cb1_sub_455ee0_n
        add esp, 4
        ret
    }
}
