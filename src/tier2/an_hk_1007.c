#include "mfl_tier2.h"
/*0040d0f0*/ extern int __cdecl find_texture_dict_c(RwChar *p0);
/*00422350*/ extern int vehicle_part_ltm();
/*00435040*/ extern int team_color_names();
/*00481fd0*/ extern double random_range();
/*00482010*/ extern int random_int_range();
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*00886020*/ extern undefined4 g_app_886020[0x1000];
/*0088602c*/ extern undefined4 g_particles_88602c;
/*00886030*/ extern undefined4 g_effects_update_886030;
/*00886034*/ extern undefined4 g_effects_update_886034;
/*00886038*/ extern undefined4 g_effects_update_886038;
/*0088603c*/ extern undefined4 g_effects_update_88603c;
/*00886044*/ extern undefined4 g_effects_update_886044;
/*00886048*/ extern undefined g_effects_update_886048;
/*00886054*/ extern undefined4 g_effects_update_886054;
/*00886058*/ extern undefined4 g_effects_update_886058;
/*0088605c*/ extern undefined4 g_effects_update_88605c;
/*00886060*/ extern undefined g_effects_update_886060;
/*0088606c*/ extern undefined4 g_effects_update_88606c;
/* particle_emit_textured */

/* "tritex particle emitter" */

void __cdecl particle_emit_textured(float *param_1,undefined4 *param_2,int param_3,int param_4,RwTexture *texture_2,
          float param_6,float *param_7)

{
  PlayerSlot *pPVar1;
  int *piVar2;
  undefined4 random_int;
  RwTexture *texture;
  int iVar3;
  int iVar4;
  float fVar5;
  float10 fVar6;
  struct {
    undefined1 local_40;
    undefined1 local_3f;
    undefined1 local_3e;
    undefined1 local_3d;
    undefined1 local_3c;
    undefined1 local_3b;
    undefined1 local_3a;
    undefined1 local_39;
    undefined1 local_38;
    undefined1 local_37;
    undefined1 local_36;
    undefined1 local_35;
    undefined1 local_34;
    undefined1 local_33;
    undefined1 local_32;
    undefined1 local_31;
    undefined1 local_30;
    undefined1 local_2f;
    undefined1 local_2e;
    undefined1 local_2d;
    undefined1 local_2c;
    undefined1 local_2b;
    undefined1 local_2a;
    undefined1 local_29;
    undefined4 local_28;
    float local_24;
    float local_20;
    float local_1c;
    undefined1 *local_18 [4];
    undefined1 *local_8;
    undefined1 *local_4;
  } frame;
  
  frame.local_38 = 0x3c;
  frame.local_32 = 0x3c;
  frame.local_18[0] = &frame.local_40;
  frame.local_18[2] = &frame.local_38;
  frame.local_8 = &frame.local_30;
  frame.local_3d = 0xff;
  frame.local_3b = 0x54;
  frame.local_39 = 0xff;
  frame.local_35 = 0xff;
  frame.local_31 = 0xff;
  frame.local_2f = 0x54;
  frame.local_2d = 0xff;
  frame.local_29 = 0xff;
  frame.local_28 = 0xffffffff;
  frame.local_18[1] = &frame.local_3c;
  frame.local_18[3] = &frame.local_34;
  pPVar1 = g_player_slots + param_3;
  iVar3 = pPVar1->team;
  frame.local_4 = &frame.local_2c;
  frame.local_40 = 0x78;
  frame.local_3f = 0x19;
  frame.local_3e = 0x1e;
  frame.local_3c = 4;
  frame.local_3a = 0xa4;
  frame.local_37 = 0x7c;
  frame.local_36 = 0x4c;
  frame.local_34 = 0xe4;
  frame.local_33 = 0xc4;
  frame.local_30 = 0xd4;
  frame.local_2e = 0x9c;
  frame.local_2c = 0;
  frame.local_2b = 0;
  frame.local_2a = 0;
  if (param_4 == 1) {
    random_int = 0x33;
  }
  else {
    if (param_4 != 2) {
      frame.local_24 = *param_1;
      fVar5 = param_1[1];
      frame.local_1c = param_1[2];
      param_3 = *param_2;
      goto LAB_004a0037;
    }
    random_int = 0x34;
  }
  iVar4 = vehicle_part_ltm(param_3,random_int);
  frame.local_24 = *(float *)(iVar4 + 0x30);
  fVar5 = *(float *)(iVar4 + 0x34);
  frame.local_1c = *(float *)(iVar4 + 0x38);
  param_3 = *(int *)frame.local_18[iVar3];
LAB_004a0037:
  iVar3 = 0;
  piVar2 = g_app_886020;
  while (*piVar2 != 0) {
    piVar2 = piVar2 + 0x14;
    iVar3 = iVar3 + 1;
    if ((int)&g_effects_update_88606c + 0x913 < (int)piVar2) {
      return;
    }
  }
  fVar6 = (float10)random_range(0xba83126f,0x3a83126f);
  frame.local_24 = (float)(fVar6 + (float10)frame.local_24);
  fVar6 = (float10)random_range(0xba83126f,0x3a83126f);
  iVar4 = iVar3 * 0x50;
  frame.local_1c = (float)(fVar6 + (float10)frame.local_1c);
  *(float *)&(&g_effects_update_886030)[iVar3 * 0x14] = frame.local_24;
  *(float *)&(&g_effects_update_886034)[iVar3 * 0x14] = fVar5;
  *(float *)&(&g_effects_update_886038)[iVar3 * 0x14] = frame.local_1c;
  (&g_effects_update_88605c)[iVar3 * 0x14] = 0;
  random_int = random_int_range(0x14,0x23);
  (&g_effects_update_886058)[iVar3 * 0x14] = random_int;
  fVar6 = (float10)random_range(0,0x3f800000);
  *(float *)((char *)&g_effects_update_886060 + iVar4) = (float)fVar6;
  fVar6 = (float10)random_range(0,0x3f800000);
  *(float *)(iVar4 + ((int)&g_effects_update_886060 + 0x4)) = (float)fVar6;
  fVar6 = (float10)random_range(0,0x3f800000);
  *(float *)(iVar4 + ((int)&g_effects_update_886060 + 0x8)) = (float)fVar6;
  fVar6 = (float10)random_range(0xbcf5c28f,0x3cf5c28f);
  *(float *)&(&g_effects_update_88603c)[iVar3 * 0x14] = (float)fVar6;
  fVar6 = (float10)random_range(0xbcf5c28f,0x3cf5c28f);
  *(float *)&(&g_effects_update_886044)[iVar3 * 0x14] = (float)fVar6;
  fVar6 = (float10)random_range(0x3c23d70a,0x3d23d70a);
  *(float *)(iVar4 + ((int)&g_effects_update_88603c + 0x4)) = (float)fVar6;
  if (param_7 != (float *)0x0) {
    *(float *)&(&g_effects_update_88603c)[iVar3 * 0x14] =
         *(float *)&(&g_effects_update_88603c)[iVar3 * 0x14] + *param_7;
    *(float *)(iVar4 + ((int)&g_effects_update_88603c + 0x4)) = (float)((float10)param_7[1] + fVar6);
    *(float *)&(&g_effects_update_886044)[iVar3 * 0x14] =
         param_7[2] + *(float *)&(&g_effects_update_886044)[iVar3 * 0x14];
  }
  (&g_effects_update_88606c)[iVar3 * 0x14] = 0;
  (&g_effects_update_886054)[iVar3 * 0x14] = 0;
  fVar6 = (float10)random_range(0x3dcccccd,0x3f19999a);
  frame.local_24 = (float)(fVar6 * (float10)param_6);
  fVar6 = (float10)random_range(0x3dcccccd,0x3f19999a);
  frame.local_20 = (float)(fVar6 * (float10)param_6);
  fVar6 = (float10)random_range(0x3dcccccd,0x3f19999a);
  *(float *)((char *)&g_effects_update_886048 + iVar4) = frame.local_24;
  frame.local_1c = (float)(fVar6 * (float10)param_6);
  *(float *)(iVar4 + ((int)&g_effects_update_886048 + 0x4)) = frame.local_20;
  *(float *)(iVar4 + ((int)&g_effects_update_886048 + 0x8)) = frame.local_1c;
  (g_app_886020)[iVar3 * 0x14] = 1;
  if (texture_2 == (RwTexture *)0x0) {
    if ((param_4 == 1) && (iVar4 = random_int_range(0,3), iVar4 == 3)) {
      texture = (RwTexture *)team_color_names(pPVar1->team);
    }
    else {
      texture = (RwTexture *)find_texture_dict_c("tritex");
    }
    RpMaterialSetTexture((RpMaterial *)(&g_particles_88602c)[iVar3 * 0x14],texture);
    *(int *)((&g_particles_88602c)[iVar3 * 0x14] + 4) = param_3;
    return;
  }
  RpMaterialSetTexture((RpMaterial *)(&g_particles_88602c)[iVar3 * 0x14],texture_2);
  *(undefined4 *)((&g_particles_88602c)[iVar3 * 0x14] + 4) = frame.local_28;
  return;
}


