#include "mfl_tier2.h"
/*0040d0d0*/ extern char *__cdecl find_texture_dict_b(RwChar *p0);
/*0042f780*/ extern char *__cdecl clump_load_by_name(char *p0, undefined4 p1, undefined4 p2);
/*004661f0*/ extern int weapon_fx_grid_clear();
/*0049b780*/ extern int __cdecl particles_alloc_system(struct RwTexture *p0, int p1, int p2, uint p3);
/*0049b860*/ extern int fx_set_blend_modes();
/*004c3f90*/ extern int clump_get_atomics(RpClump * p0, RpAtomic ** p1);
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004f6bf0*/ extern int FUN_004f6bf0(void *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a9588*/ extern undefined g_weapon_6a9588;
/*006a9728*/ extern undefined4 g_weapon_6a9728[0x1000];
/*006aab78*/ extern char *g_weapon_6aab78;
/* flamethrower_load_assets */

/* "FlameThrower.dff and its particle systems" */

void flamethrower_load_assets(void)

{
  RpClump *clump;
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  RwMatrix *pRVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  RwReal *pRVar7;
  int aiStack_28 [10];
  
  clump = (RpClump *)clump_load_by_name("FlameThrower.dff", 0x0, 0x0);
  g_weapon_6aab78 = (char *)((RwTexture *)find_texture_dict_b("exp_cloud2"));
  puVar5 = g_weapon_6a9728;
  do {
    puVar6 = puVar5 + -0x1a;
    uVar1 = FUN_004f6bf0(clump);
    puVar5[-0x12] = uVar1;
    iVar2 = clump_get_atomics((RpClump *)uVar1,(RpAtomic **)aiStack_28);
    while (iVar2 != 0) {
      iVar3 = iVar2 + -1;
      iVar2 = iVar2 + -1;
      iVar3 = FUN_004c52b0(aiStack_28[iVar3],0,0);
      if (iVar3 == 1) {
        iVar3 = aiStack_28[iVar2];
        puVar5[-0x11] = iVar3;
        pRVar4 = RwFrameGetLTM(*(RwFrame **)(iVar3 + 4));
        pRVar7 = (RwReal *)(puVar5 + -0x10);
        for (iVar3 = 0x10; iVar3 != 0; iVar3 = iVar3 + -1) {
          *pRVar7 = (pRVar4->right).x;
          pRVar4 = (RwMatrix *)&(pRVar4->right).y;
          pRVar7 = pRVar7 + 1;
        }
      }
    }
    weapon_fx_grid_clear(puVar6);
    uVar1 = particles_alloc_system((struct RwTexture *)g_weapon_6aab78,0x1e,6,0x37);
    puVar5[-0x16] = uVar1;
    fx_set_blend_modes(uVar1,5,2);
    puVar5 = puVar6;
  } while (puVar6 != (undefined4 *)&g_weapon_6a9588);
  RpClumpDestroy(clump);
  return;
}


