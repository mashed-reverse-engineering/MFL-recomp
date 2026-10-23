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
  int *piVar1;
  float fVar2;
  char random_int_2;
  undefined4 uVar3;
  int random_int;
  uint uVar4;
  int *piVar5;
  int *piVar6;
  undefined *puVar7;
  undefined *reg_eax;
  bool bVar8;
  int *piVar9;
  int *piVar10;
  undefined1 *puVar11;
  struct {
    char cStack_ec;
    char cStack_eb;
    char cStack_ea;
    undefined1 uStack_e9;
    undefined *puStack_e8;
    int iStack_e4;
    int iStack_e0;
    int iStack_dc;
    int iStack_d8;
    float fStack_d4;
    int iStack_d0;
    int iStack_cc;
    int iStack_c8;
    int iStack_c4;
    float fStack_c0;
    undefined4 uStack_bc;
    int *piStack_b8;
    undefined4 uStack_b4;
    code *pcStack_b0;
  } frame;
  struct {
    undefined1 auStack_40 [56];
    float fStack_8;
    float fStack_4;      /* rocket_surface_hit_test writes 64 bytes */
  } frame2;
  
  puVar7 = g_weapon_rocket_6a60cc;
  piVar5 = (int *)&g_weapon_rocket_6a6040;
  do {
    reg_eax = puVar7 + -0x2c;
    piVar6 = piVar5 + -0x1b;
    frame.puStack_e8 = reg_eax;
    object_set_flag_4(*(undefined4 *)(puVar7 + -0xc),0);
    if (*(int *)(puVar7 + -0x28) != 0) {
      rocket_update_launcher_r((void *)reg_eax);
      if (*(int *)(puVar7 + -0x10) != 0) {
        weapon_rocket_spin_update((undefined4)reg_eax);
      }
    }
    object_set_flag_4(piVar5[-0x17],0);
    puVar7 = reg_eax;
    if (piVar5[-0x1c] != 0) {
      if ((*piVar6 == -1) && (piVar5[-0x1a] == -1)) {
        rocket_fly_straight_r((void *)(piVar5 + -0x30));
      }
      else {
        rocket_fly_homing_r((void *)(piVar5 + -0x30));
      }
      piVar10 = piVar5 + -0x30;
      fVar2 = g_tick_seconds + *(float *)&piVar5[-0x2a];
      *(float *)&piVar5[-0x2a] = fVar2;
      if (!(fVar2 > k_f_3)) {
        piVar1 = piVar5 + -0x2d;
        frame.iStack_cc = *piVar1;
        frame.iStack_c8 = piVar5[-0x2c];
        frame.iStack_c4 = piVar5[-0x2b];
        frame.fStack_c0 = 0.05;
        frame.pcStack_b0 = rocket_hit_callback;
        frame.uStack_b4 = 0;
        frame.uStack_bc = 0xb;
        frame.piStack_b8 = piVar10;
        trigger_sphere_register((float *)&frame.iStack_cc);
        uVar4 = g_tick_number & 0x80000001;
        bVar8 = uVar4 == 0;
        if ((int)uVar4 < 0) {
          bVar8 = (uVar4 - 1 | 0xfffffffe) == 0xffffffff;
        }
        if (bVar8) {
          puVar11 = frame2.auStack_40;
          piVar9 = &frame.iStack_cc;
          frame.fStack_c0 = (*(float *)&piVar5[-0x27] * *(float *)&piVar5[-0x27] +
                      *(float *)&piVar5[-0x28] * *(float *)&piVar5[-0x28] +
                      *(float *)&piVar5[-0x29] * *(float *)&piVar5[-0x29]) * k_f_1_5 + frame.fStack_c0;
          uVar3 = weapon_get_rw_801b14();
          random_int = FUN_004c4e20(uVar3,(undefined4)piVar9,(undefined4)puVar11);
          if (random_int != 0) {
            random_int = rocket_surface_hit_test((undefined4)frame2.auStack_40,(undefined4)&frame.iStack_cc);
            if (random_int == 0) {
              rocket_explode_r((void *)piVar10);
            }
          }
        }
        frame.iStack_e4 = *piVar1;
        frame.iStack_e0 = piVar5[-0x2c];
        frame.iStack_dc = piVar5[-0x2b];
        frame.iStack_d8 = *piVar1;
        frame.iStack_d0 = piVar5[-0x2b];
        frame.fStack_d4 = *(float *)&piVar5[-0x2c] - k_f_3;
        puVar11 = frame2.auStack_40;
        piVar10 = &frame.iStack_e4;
        uVar3 = weapon_get_rw_801b14();
        random_int = world_intersect_line((RpWorld *)uVar3,(undefined4)piVar10,(undefined4)puVar11);
        if (random_int == 0) {
          piVar5[-0x26] = -0x45fced91;
        }
        else {
          fVar2 = (frame2.fStack_8 * k_f_3 - k_f_0_4) * k_f_2_5 * k_f_n0_5;
          *(float *)&piVar5[-0x26] = fVar2;
          if (fVar2 < k_f_n0_05) {
            piVar5[-0x26] = -0x42b33333;
          }
        }
        fx_ribbon_add_point(piVar5[-0x16],piVar1,0,0);
        random_int = random_int_range(0,10);
        if (5 < random_int) {
          frame.cStack_ec = -0x80;
          frame.cStack_eb = -0x80;
          frame.cStack_ea = -0x80;
          frame.uStack_e9 = 0xff;
          random_int_2 = random_int_range(0xffffffe2,0x1e);
          frame.cStack_ec = frame.cStack_ec + random_int_2;
          frame.cStack_ea = frame.cStack_ea + random_int_2;
          frame.cStack_eb = frame.cStack_eb + random_int_2;
          fx_smoke_particle_spawn_tinted(piVar1,0x3e4ccccd,&frame.cStack_ec,0x40a00000,0x3fc00000);
        }
        sfx_move_playing_voice(0x14,piVar1,piVar5[-0x21]);
        puVar7 = frame.puStack_e8;
      }
      else {
        rocket_explode_r((void *)piVar10);
      }
    }
    piVar5 = piVar6;
  } while (piVar6 != (int *)&g_weapon_rocket_6a5e24);
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
