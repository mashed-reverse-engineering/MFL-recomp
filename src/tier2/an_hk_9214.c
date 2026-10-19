#include "mfl_tier2.h"
/*004846e0*/ extern int point_lerp_to_aux_camera();
/*004867f0*/ extern int particles_set_position();
/*00486820*/ extern int particles_set_size();
/*00486840*/ extern int particles_set_color();
/*00486880*/ extern int gfx_set_gfx_6c50c8(float v);
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*004b2b48*/ extern int _ftol();
/*005dd330*/ extern float float_1_0;
/*005dda24*/ extern float k_f_0_75;
/*005de1f8*/ extern float k_f_3_1415927;
/*0076be70*/ extern undefined g_particles_76be70;
/*0076beb0*/ extern undefined1 g_particles_76beb0;
/*0076c4e0*/ extern undefined4 g_particles_76c4e0;
/*0076c4e4*/ extern undefined4 g_fx_slots_76c4e4;
/*0076c4e8*/ extern undefined4 g_fx_slots_76c4e8;
/*0076c4fc*/ extern undefined g_effects_update_76c4fc;
/*0076c504*/ extern undefined4 g_fx_slots_76c504;
/*0076c508*/ extern undefined4 g_particles_76c508;
/*0076c50c*/ extern undefined g_fx_systems_76c50c;
/*007ef7f4*/ extern float g_tick_seconds;
extern double sin(double);
/* fx_smoke_puffs_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "iterates the active bit mask of the puff pool and updates each puff" */

void fx_smoke_puffs_update(void)

{
  float fVar1;
  float fVar2;
  byte bVar3;
  char cVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  float10 fVar8;
  struct {
    undefined4 local_18;
    float local_14;
    float local_10;
    float local_c;
    float local_8;
    float local_4;
  } frame;
  
  uVar6 = 0;
LAB_0049d830:
  frame.local_14 = 0.5;
  frame.local_10 = 0.5;
  do {
    while( true ) {
      if (0x7f < (int)uVar6) goto LAB_0049d9f7;
      if (*(int *)((char *)&g_particles_76beb0 + ((int)uVar6 >> 5) * 4) != 0) break;
      uVar6 = (((int)uVar6 >> 5) + 1) * 0x20;
    }
    iVar5 = (int)uVar6 >> 3;
    bVar3 = (&g_particles_76beb0)[iVar5];
    if (bVar3 != 0) {
      switch(uVar6 & 7) {
      case 0:
        uVar6 = uVar6 + 1;
        if ((bVar3 & 1) != 0) goto LAB_0049d8b5;
        break;
      case 1:
        break;
      case 2:
        goto switchD_0049d87d_caseD_2;
      case 3:
        goto switchD_0049d87d_caseD_3;
      case 4:
        goto switchD_0049d87d_caseD_4;
      case 5:
        goto switchD_0049d87d_caseD_5;
      case 6:
        goto switchD_0049d87d_caseD_6;
      case 7:
        goto switchD_0049d87d_caseD_7;
      default:
        goto switchD_0049d87d_default;
      }
      uVar6 = uVar6 + 1;
      if ((bVar3 & 2) != 0) break;
switchD_0049d87d_caseD_2:
      uVar6 = uVar6 + 1;
      if ((bVar3 & 4) != 0) break;
switchD_0049d87d_caseD_3:
      uVar6 = uVar6 + 1;
      if ((bVar3 & 8) != 0) break;
switchD_0049d87d_caseD_4:
      uVar6 = uVar6 + 1;
      if ((bVar3 & 0x10) != 0) break;
switchD_0049d87d_caseD_5:
      uVar6 = uVar6 + 1;
      if ((bVar3 & 0x20) != 0) break;
switchD_0049d87d_caseD_6:
      uVar6 = uVar6 + 1;
      if ((bVar3 & 0x40) != 0) break;
switchD_0049d87d_caseD_7:
      uVar6 = uVar6 + 1;
      if ((char)bVar3 < '\0') break;
    }
switchD_0049d87d_default:
    uVar6 = iVar5 * 8 + 8;
  } while( true );
LAB_0049d8b5:
  if ((int)uVar6 < 0) goto LAB_0049d9f7;
  iVar5 = uVar6 - 1;
  iVar7 = iVar5 * 0x34;
  frame.local_18 = *(undefined4 *)((char *)&g_particles_76c4e0 + iVar7 + 0x18);
  fVar8 = (float10)sin((float10)*(float *)&(&g_fx_slots_76c504)[iVar5 * 0xd] * (float10)k_f_3_1415927);
  fVar1 = g_tick_seconds + *(float *)&(&g_particles_76c508)[iVar5 * 0xd];
  *(float *)&(&g_particles_76c508)[iVar5 * 0xd] = fVar1;
  *(float *)&(&g_fx_slots_76c504)[iVar5 * 0xd] = fVar1;
  frame.local_c = *(float *)((char *)&g_particles_76c4e0 + iVar7 + 0xc) * fVar1 * *(float *)((char *)&g_fx_systems_76c50c + iVar7) +
            *(float *)&(&g_particles_76c4e0)[iVar5 * 0xd];
  frame.local_4 = *(float *)((char *)&g_particles_76c4e0 + iVar7 + 0x14) * fVar1 * *(float *)((char *)&g_fx_systems_76c50c + iVar7) +
            *(float *)&(&g_fx_slots_76c4e8)[iVar5 * 0xd];
  frame.local_8 = (float)(fVar8 * (float10)*(float *)((char *)&g_particles_76c4e0 + iVar7 + 0x30) +
                   (float10)*(float *)&(&g_fx_slots_76c4e4)[iVar5 * 0xd]);
  if (fVar1 <= float_1_0) {
    fVar8 = fVar8 * (float10)*(float *)((char *)&g_particles_76c4e0 + iVar7 + 0x20);
    fVar8 = fVar8 + fVar8 + (float10)*(float *)((char *)&g_particles_76c4e0 + iVar7 + 0x20);
    frame.local_10 = (float)fVar8;
    frame.local_14 = (float)(fVar8 * (float10)k_f_0_75);
    cVar4 = ((int)(((fVar1 * fVar1) * fVar1) * 255.0f));
    frame.local_18 = CONCAT13(-1 - cVar4,(undefined3)frame.local_18);
    point_lerp_to_aux_camera(&frame.local_c,&frame.local_c,0x3d4ccccd);
    fVar1 = *(float *)((char *)&g_effects_update_76c4fc + iVar7);
    fVar2 = *(float *)&(&g_fx_slots_76c504)[iVar5 * 0xd];
    particles_set_position(&frame.local_c);
    particles_set_color(&frame.local_18);
    particles_set_size(&frame.local_14);
    gfx_set_gfx_6c50c8(fVar1 * fVar2);
    particles_add(&g_particles_76be70);
  }
  else {
    (&g_particles_76beb0)[iVar5 >> 3] =
         (&g_particles_76beb0)[iVar5 >> 3] & ~('\x01' << ((byte)iVar5 & 7));
    uVar6 = uVar6 + 1;
  }
  if ((int)uVar6 < 1) {
LAB_0049d9f7:
    particles_flush(&g_particles_76be70);
    return;
  }
  goto LAB_0049d830;
}


