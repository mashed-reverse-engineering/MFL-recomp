// cluster-entries: explosion_spawn_fx
/* Installed cluster explosion_spawn_fx (tools/cluster_install.py from output/cluster_pilot_hb1/explosion_spawn_fx.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00472340*/ extern int sfx_play_player_set_at();
/*00481fd0*/ extern double random_range();
/*004866b0*/ extern int fx_emitter_spawn_a_default();
/*0048e4f0*/ extern int rwp_object_set_enabled();
/*0048e6f0*/ extern int rwp_object_get_matrix();
/*00498f40*/ extern int fx_ring_add_point();
/*00499b40*/ extern int explosion_decal();
/*0049fd50*/ extern int fx_smoke_particle_spawn_tinted();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, int p1, int p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd340*/ extern float k_f_0_5;
/*005dd374*/ extern float k_f_0_2;
/*005dd620*/ extern float float_10_0;
/*005dd62c*/ extern float k_f_100;
/*005dd7e8*/ extern float k_f_60;
/*005ddb18*/ extern float float_0_8;
/*005ddb1c*/ extern float k_f_0_96;
/*005ddb20*/ extern float k_f_0_9;
/*0062ac74*/ extern float g_car_62ac74;
/*00651aa4*/ extern undefined4 g_explosion_651aa4;
/*007ef7f4*/ extern float g_tick_seconds;
/* explosion_spawn_fx */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "random colour / size, explosion_decal and sfx_play_player_set_at at the effect position" */

float10 explosion_spawn_fx_n(char *in_eax)

{
  undefined4 *puVar1;
  int *piVar2;
  float fVar3;
  int rwp_matrix;
  RwMatrix *pRVar4;
  int iVar5;
  RwV3d *pRVar6;
  float *pfVar7;
  float10 fVar8;
  RwV3d *pRVar9;
  RwV3d *pRVar10;
  struct {
    float local_1c;
    undefined1 local_18;
    undefined1 local_17;
    undefined1 local_16;
    undefined1 local_15;
    float local_14;
    float local_10;
    float local_c;
    float local_8;
    float local_4;
  } frame;
  
  rwp_matrix = rwp_object_get_matrix(*(undefined4 *)(in_eax + 0x44));
  frame.local_1c = (k_f_100 - *(float *)(in_eax + 0x5c)) * k_f_0_01;
  pfVar7 = (float *)(rwp_matrix + 0x30);
  fVar8 = (float10)random_range(0,0x3f800000);
  frame.local_14 = (float)fVar8;
  fVar8 = (float10)random_range(0x3f4ccccd,0x3f99999a);
  frame.local_10 = (float)fVar8;
  pRVar4 = RwFrameGetLTM(*(RwFrame **)(*(int *)(in_eax + 0x3c) + 4));
  pRVar6 = &pRVar4->pos;
  pRVar4 = RwFrameGetLTM(*(RwFrame **)(*(int *)(in_eax + 0x40) + 4));
  fVar3 = g_tick_seconds + *(float *)(in_eax + 0x6c);
  pRVar9 = &pRVar4->pos;
  *(float *)(in_eax + 0x6c) = fVar3;
  frame.local_15 = 0x80;
  if (k_f_3 <= fVar3) {
    *(undefined4 *)(in_eax + 0x6c) = 0;
    *(undefined4 *)(in_eax + 0x68) = *(undefined4 *)(in_eax + 0x60);
  }
  if (float_1_0 < frame.local_1c) {
    frame.local_1c = 1.0;
  }
  frame.local_18 = ((int)(frame.local_10 * 128.0f));
  frame.local_17 = frame.local_18;
  frame.local_16 = ((int)(frame.local_10 * 150.0f));
  if (*(int *)(in_eax + 0x48) == 0) {
    if (float_1_0 <= frame.local_1c) {
      g_explosion_651aa4 = g_explosion_651aa4 + 1;
      *(undefined4 *)(in_eax + 0x48) = 1;
      *(undefined4 *)(in_eax + 0xe0) = 0;
      fx_emitter_spawn_a_default(pfVar7,&g_car_62ac74,0x43b40000);
      sfx_play_player_set_at(pfVar7);
      if (*(int *)(in_eax + 0x18) != 0) {
        rwp_object_set_enabled(*(undefined4 *)(in_eax + 0x44),1);
        iVar5 = rwp_owner_object_get(*(undefined4 *)(in_eax + 0x18));
        rwp_world_set_bit_5c(*(undefined4 *)(iVar5 + 0x24),iVar5,1);
        piVar2 = *(int **)(*(int *)(iVar5 + 0x10) + 4);
        if (piVar2 != (int *)0x0) {
          *(float *)&piVar2[6] = (float_1_0 / (g_tick_seconds * float_10_0));
          puVar1 = (undefined4 *)(*(int *)(*(int *)(*piVar2 + 0x10) + 8) + 0x10 + piVar2[1] * 0x20);
          *puVar1 = 0;
          puVar1[1] = 0x41a00000;
          puVar1[2] = 0;
          *(float *)&piVar2[5] = (float_1_0 / (g_tick_seconds * k_f_60));
        }
      }
      iVar5 = 6;
      do {
        frame.local_c = *pfVar7;
        frame.local_8 = *(float *)(rwp_matrix + 0x34);
        frame.local_4 = *(float *)(rwp_matrix + 0x38);
        fVar8 = (float10)random_range(0xbf666666,0x3f666666);
        frame.local_c = (float)(fVar8 + (float10)frame.local_c);
        fVar8 = (float10)random_range(0xbf666666,0x3f666666);
        frame.local_8 = (float)(fVar8 + (float10)frame.local_8);
        fVar8 = (float10)random_range(0xbf666666,0x3f666666);
        frame.local_4 = (float)(fVar8 + (float10)frame.local_4);
        fx_emitter_spawn_a_default(&frame.local_c,&g_car_62ac74,0x43b40000);
        explosion_decal(&frame.local_c,0x3f99999a);
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
      return (float10)frame.local_1c;
    }
    if (frame.local_14 + frame.local_14 < frame.local_1c) {
      fVar8 = (float10)random_range(0,0x3f800000);
      pRVar10 = pRVar9;
      if ((float10)k_f_0_5 < fVar8) {
        pRVar10 = pRVar6;
      }
      fx_smoke_particle_spawn_tinted(pRVar10,0x3f333333,&frame.local_18,0x40a00000,0x3f000000);
    }
    if ((k_f_0_5 < frame.local_1c) && (*(int *)(in_eax + 100) == 0)) {
      fx_ring_add_point(pRVar6,0x3f800000);
      fx_ring_add_point(pRVar6,0x3f800000);
      fx_ring_add_point(pRVar6,0x3f800000);
      fx_ring_add_point(pRVar9,0x3f800000);
      fx_ring_add_point(pRVar9,0x3f800000);
      fx_ring_add_point(pRVar9,0x3f800000);
      *(undefined4 *)(in_eax + 100) = 1;
    }
  }
  else {
    if (k_f_0_9 < frame.local_14) {
      fVar8 = (float10)random_range(0,0x3f800000);
      if ((float10)k_f_0_5 < fVar8) {
        pRVar9 = pRVar6;
      }
      fx_smoke_particle_spawn_tinted(pRVar9,0x3f333333,&frame.local_18,0x40a00000,0x3f000000);
    }
    if (k_f_0_96 < frame.local_14) {
      frame.local_c = *pfVar7;
      frame.local_8 = *(float *)(rwp_matrix + 0x34);
      frame.local_4 = *(float *)(rwp_matrix + 0x38);
      fVar8 = (float10)random_range(0xbf666666,0x3f666666);
      frame.local_c = (float)(fVar8 + (float10)frame.local_c);
      fVar8 = (float10)random_range(0xbf666666,0x3f666666);
      frame.local_8 = (float)(fVar8 * (float10)k_f_0_2 + (float10)frame.local_8);
      fVar8 = (float10)random_range(0xbf666666,0x3f666666);
      frame.local_4 = (float)(fVar8 + (float10)frame.local_4);
      fx_emitter_spawn_a_default(&frame.local_c,&g_car_62ac74,0x43b40000);
      fVar8 = (float10)random_range(0,0x3f800000);
      if ((float10)float_0_8 < fVar8) {
        explosion_decal(&frame.local_c,0x3f99999a);
      }
      sfx_play_player_set_at(&frame.local_c);
      return (float10)frame.local_1c;
    }
  }
  return (float10)frame.local_1c;
}

/* entry thunk at the original address of explosion_spawn_fx: register inputs (eax) -> natural C explosion_spawn_fx_n; ret 0 */
__declspec(naked) void explosion_spawn_fx(void)
{
    __asm {
        push eax
        call explosion_spawn_fx_n
        add esp, 4
        ret
    }
}
