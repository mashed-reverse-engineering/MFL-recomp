// cluster-entries: weapon_rocket_update
/* Installed cluster weapon_rocket_update (tools/cluster_install.py from output/cluster_pilot_uc3/weapon_rocket_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00460a40*/ extern undefined __cdecl weapon_rocket_spin_update(undefined4 p0);
/*00461370*/ extern int rocket_hit_callback();
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(undefined4 p0, undefined4 p1);
/*00469cb0*/ extern int sfx_move_playing_voice();
/*00482010*/ extern int random_int_range();
/*00484680*/ extern int object_set_flag_4();
/*00496850*/ extern int trigger_sphere_register(float *param_1);
/*0049bf00*/ extern int fx_ribbon_add_point();
/*0049fd50*/ extern int fx_smoke_particle_spawn_tinted();
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004c4e20*/ extern int FUN_004c4e20(undefined4 p0, undefined4 p1, undefined4 p2);
/*005dd32c*/ extern float k_f_3;
/*005dd36c*/ extern float k_f_0_4;
/*005dd40c*/ extern float k_f_1_5;
/*005de1b0*/ extern float k_f_n0_05;
/*005de20c*/ extern float k_f_2_5;
/*005de6a8*/ extern float k_f_n0_5;
/*006a5e24*/ extern undefined g_weapon_rocket_6a5e24;
/*006a6040*/ extern undefined g_weapon_rocket_6a6040;
/*006a60cc*/ extern undefined g_weapon_rocket_6a60cc[0x1000];
/*007ef7f4*/ extern float g_tick_seconds;
/*007ef804*/ extern undefined4 g_tick_number;








/* weapon_rocket_update */

static int rocket_explode_r(void * p0);
extern void rocket_explode(void);
static int rocket_fly_homing_r(void * p0);
extern void rocket_fly_homing(void);
static int rocket_fly_straight_r(void * p0);
extern void rocket_fly_straight(void);
static int rocket_update_launcher_r(void * p0);
extern void rocket_update_launcher(void);
void weapon_rocket_update(void)

{
  int *rocket_pos;
  float tmp_f;
  char smoke_jitter;
  undefined4 rw_world;
  int rnd_or_result;
  uint tick_parity;
  int *rocket_cursor;
  int *rocket_prev;
  undefined *launcher_cursor;
  undefined *launcher;
  bool tick_is_even;
  int *sphere_ptr;
  int *rocket;
  undefined1 *hit_buf_ptr;
  struct {
    char smoke_r;
    char smoke_g;
    char smoke_b;
    undefined1 smoke_a;
    undefined *launcher_saved;
    int ray_start_x;
    int ray_start_y;
    int ray_start_z;
    int ray_end_x;
    float ray_end_y;
    int ray_end_z;
    int sphere_x;
    int sphere_y;
    int sphere_z;
    float sphere_radius;
    undefined4 sphere_kind;
    int *sphere_owner;
    undefined4 sphere_unused;
    code *sphere_callback;
  } frame;
  struct {
    undefined1 hit_buf [56];
    float fStack_8;
    float fStack_4;      /* rocket_surface_hit_test writes 64 bytes */
  } frame2;
  
  /* per slot: launcher (spin / aim) update, then the rocket itself (flies straight or homing, 108 byte records) */
  launcher_cursor = g_weapon_rocket_6a60cc;
  rocket_cursor = (int *)&g_weapon_rocket_6a6040;
  do {
    launcher = launcher_cursor + -0x2c;
    rocket_prev = rocket_cursor + -0x1b;
    frame.launcher_saved = launcher;
    object_set_flag_4(*(undefined4 *)(launcher_cursor + -0xc),0);
    if (*(int *)(launcher_cursor + -0x28) != 0) {
      rocket_update_launcher_r((void *)launcher);
      if (*(int *)(launcher_cursor + -0x10) != 0) {
        weapon_rocket_spin_update((undefined4)launcher);
      }
    }
    object_set_flag_4(rocket_cursor[-0x17],0);
    launcher_cursor = launcher;
    if (rocket_cursor[-0x1c] != 0) {
      if ((*rocket_prev == -1) && (rocket_cursor[-0x1a] == -1)) {
        rocket_fly_straight_r((void *)(rocket_cursor + -0x30));
      }
      else {
        rocket_fly_homing_r((void *)(rocket_cursor + -0x30));
      }
      rocket = rocket_cursor + -0x30;
      tmp_f = g_tick_seconds + *(float *)&rocket_cursor[-0x2a];
      *(float *)&rocket_cursor[-0x2a] = tmp_f;
      if (!(tmp_f > k_f_3)) {
        rocket_pos = rocket_cursor + -0x2d;
        /* rocket alive for under 3 s: register its damage trigger sphere (kind 0xb, radius 0.05) */
        frame.sphere_x = *rocket_pos;
        frame.sphere_y = rocket_cursor[-0x2c];
        frame.sphere_z = rocket_cursor[-0x2b];
        frame.sphere_radius = 0.05;
        frame.sphere_callback = rocket_hit_callback;
        frame.sphere_unused = 0;
        frame.sphere_kind = 0xb;
        frame.sphere_owner = rocket;
        trigger_sphere_register((float *)&frame.sphere_x);
        /* every second tick: test the rocket path against the world, explode on a plain hit */
        tick_parity = g_tick_number & 0x80000001;
        tick_is_even = tick_parity == 0;
        if ((int)tick_parity < 0) {
          tick_is_even = (tick_parity - 1 | 0xfffffffe) == 0xffffffff;
        }
        if (tick_is_even) {
          hit_buf_ptr = frame2.hit_buf;
          sphere_ptr = &frame.sphere_x;
          frame.sphere_radius = (*(float *)&rocket_cursor[-0x27] * *(float *)&rocket_cursor[-0x27] +
                      *(float *)&rocket_cursor[-0x28] * *(float *)&rocket_cursor[-0x28] +
                      *(float *)&rocket_cursor[-0x29] * *(float *)&rocket_cursor[-0x29]) * k_f_1_5 + frame.sphere_radius;
          rw_world = weapon_get_rw_801b14();
          rnd_or_result = FUN_004c4e20(rw_world,(undefined4)sphere_ptr,(undefined4)hit_buf_ptr);
          if (rnd_or_result != 0) {
            rnd_or_result = rocket_surface_hit_test((undefined4)frame2.hit_buf,(undefined4)&frame.sphere_x);
            if (rnd_or_result == 0) {
              rocket_explode_r((void *)rocket);
            }
          }
        }
        /* ground probe 3 units below the rocket: sets the smoke / ribbon height value */
        frame.ray_start_x = *rocket_pos;
        frame.ray_start_y = rocket_cursor[-0x2c];
        frame.ray_start_z = rocket_cursor[-0x2b];
        frame.ray_end_x = *rocket_pos;
        frame.ray_end_z = rocket_cursor[-0x2b];
        frame.ray_end_y = *(float *)&rocket_cursor[-0x2c] - k_f_3;
        hit_buf_ptr = frame2.hit_buf;
        rocket = &frame.ray_start_x;
        rw_world = weapon_get_rw_801b14();
        rnd_or_result = world_intersect_line((RpWorld *)rw_world,(undefined4)rocket,(undefined4)hit_buf_ptr);
        if (rnd_or_result == 0) {
          rocket_cursor[-0x26] = -0x45fced91;
        }
        else {
          tmp_f = (frame2.fStack_8 * k_f_3 - k_f_0_4) * k_f_2_5 * k_f_n0_5;
          *(float *)&rocket_cursor[-0x26] = tmp_f;
          if (tmp_f < k_f_n0_05) {
            rocket_cursor[-0x26] = -0x42b33333;
          }
        }
        /* trail: ribbon point, grey smoke about half of the ticks, flight sound follows the rocket */
        fx_ribbon_add_point(rocket_cursor[-0x16],rocket_pos,0,0);
        rnd_or_result = random_int_range(0,10);
        if (5 < rnd_or_result) {
          frame.smoke_r = -0x80;
          frame.smoke_g = -0x80;
          frame.smoke_b = -0x80;
          frame.smoke_a = 0xff;
          smoke_jitter = random_int_range(0xffffffe2,0x1e);
          frame.smoke_r = frame.smoke_r + smoke_jitter;
          frame.smoke_b = frame.smoke_b + smoke_jitter;
          frame.smoke_g = frame.smoke_g + smoke_jitter;
          fx_smoke_particle_spawn_tinted(rocket_pos,0x3e4ccccd,&frame.smoke_r,0x40a00000,0x3fc00000);
        }
        sfx_move_playing_voice(0x14,rocket_pos,rocket_cursor[-0x21]);
        launcher_cursor = frame.launcher_saved;
      }
      else {
        rocket_explode_r((void *)rocket);
      }
    }
    rocket_cursor = rocket_prev;
  } while (rocket_prev != (int *)&g_weapon_rocket_6a5e24);
  return;
}

/* rocket_explode_r: C -> original rocket_explode (registers eax) */
__declspec(naked) int rocket_explode_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call rocket_explode
        ret
    }
}

/* rocket_fly_homing_r: C -> original rocket_fly_homing (registers esi) */
__declspec(naked) int rocket_fly_homing_r(void * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call rocket_fly_homing
        pop esi
        ret
    }
}

/* rocket_fly_straight_r: C -> original rocket_fly_straight (registers esi) */
__declspec(naked) int rocket_fly_straight_r(void * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call rocket_fly_straight
        pop esi
        ret
    }
}

/* rocket_update_launcher_r: C -> original rocket_update_launcher (registers eax) */
__declspec(naked) int rocket_update_launcher_r(void * p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call rocket_update_launcher
        ret
    }
}
