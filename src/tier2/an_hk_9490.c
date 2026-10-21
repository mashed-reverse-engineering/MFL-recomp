#include "mfl_tier2.h"
extern char UNK_0045d5b0[];
extern char UNK_005df51c[];
/*0045c6b0*/ extern int course_atomics_clone_3();
/*0045ca00*/ extern int anim_set_callback();
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e6f0*/ extern int rwp_object_get_matrix();
/*00492d10*/ extern int rwp_object_set_active();
/*004a5d10*/ extern int object_frames_create_with_matrices();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5d90*/ extern struct RwTexture * RwTexDictionaryFindNamedTexture(struct RwTexDictionary *p0, RwChar *p1);
/*005dda24*/ extern float k_f_0_75;
/*0060d9c8*/ extern undefined4 g_course_60d9c8;
/*0060d9cc*/ extern undefined4 g_course_60d9cc;
/*006a27a0*/ extern undefined4 g_course_warzone_6a27a0;
/*006a2888*/ extern undefined g_course_6a2888;
/*006a2998*/ extern undefined4 g_course_warzone_6a2998;
/*006a299c*/ extern undefined4 g_course_warzone_6a299c;
/*006a29a0*/ extern undefined4 g_course_warzone_6a29a0;
/*006a29a4*/ extern undefined4 g_course_warzone_6a29a4;
/*008bf040*/ extern undefined4 g_course_warzone_8bf040;
/*008bf044*/ extern undefined4 g_course_warzone_8bf044;
/*008bf060*/ extern undefined4 g_course_warzone_8bf060;
/*006a2950*/ extern char g_course_get_6a2950[];
/* course_warzone_cb0 */


/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void course_warzone_cb0(int param_1)

{
  undefined4 uVar1;
  RwTexture *pRVar2;
  undefined4 *puVar3;
  RwV3d *pRVar4;
  int iVar5;
  undefined4 *puVar6;
  RwReal RVar7;
  struct {
    RwReal RStack_c;
    float fStack_8;
    RwReal RStack_4;
  } v;
  
  course_atomics_clone_3(param_1);
  uVar1 = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc11c))));
  g_course_warzone_6a2998 =
       object_frames_create_with_matrices(uVar1,*(undefined4 *)(param_1 + 0xc008),param_1 + 0x1800);
  g_course_warzone_6a299c =
       object_frames_create_with_matrices(uVar1,*(undefined4 *)(param_1 + 0xc00c),param_1 + 0x2400);
  uVar1 = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc118))));
  g_course_warzone_6a29a0 =
       object_frames_create_with_matrices(uVar1,*(undefined4 *)(param_1 + 0xc000),param_1);
  uVar1 = (undefined4)(clump_first_atomic((RpClump *)(*(undefined4 *)(param_1 + 0xc168))));
  g_course_warzone_6a29a4 =
       object_frames_create_with_matrices(uVar1,*(undefined4 *)(param_1 + 0xc018),param_1 + 0x4800);
  puVar3 = (undefined4 *)&g_course_warzone_8bf060;
  do {
    uVar1 = func_0x0045d750(*(undefined4 *)(param_1 + 0xc154),0x10);
    *puVar3 = uVar1;
    puVar3 = puVar3 + 1;
  } while ((int)puVar3 < (int)&g_course_warzone_8bf060 + 0x20);
  g_course_warzone_8bf044 = func_0x0045d6b0(*(undefined4 *)(param_1 + 0xc010),param_1 + 0x3000);
  func_0x0045dc10(param_1);
  v.RStack_c = *(RwReal *)(param_1 + 0x30);
  v.RStack_4 = *(RwReal *)(param_1 + 0x38);
  v.fStack_8 = *(float *)(param_1 + 0x34) + k_f_0_75;
  pRVar2 = RwTexDictionaryFindNamedTexture(*(RwTexDictionary **)(param_1 + 0xc9d0),"OakTreeSprite");
  func_0x0048d900(pRVar2);
  iVar5 = 0;
  if (0 < *(int *)(param_1 + 0xc008)) {
    pRVar4 = (RwV3d *)(param_1 + 0x1820);
    do {
      v.RStack_c = pRVar4[1].y;
      v.fStack_8 = pRVar4[1].z;
      v.RStack_4 = pRVar4[2].x;
      RVar7 = RwV3dLength(pRVar4);
      func_0x0048db30(&v,*(undefined4 *)(param_1 + 0xc004),param_1 + 0xc00,RVar7);
      g_course_warzone_8bf040 = g_course_warzone_8bf040 + 1;
      iVar5 = iVar5 + 1;
      pRVar4 = (RwV3d *)&pRVar4[5].y;
    } while (iVar5 < *(int *)(param_1 + 0xc008));
  }
  iVar5 = 0;
  if (0 < *(int *)(param_1 + 0xc00c)) {
    pRVar4 = (RwV3d *)(param_1 + 0x2420);
    do {
      v.RStack_c = pRVar4[1].y;
      v.fStack_8 = pRVar4[1].z;
      v.RStack_4 = pRVar4[2].x;
      RVar7 = RwV3dLength(pRVar4);
      func_0x0048db30(&v,*(undefined4 *)(param_1 + 0xc004),param_1 + 0xc00,RVar7);
      g_course_warzone_8bf040 = g_course_warzone_8bf040 + 1;
      iVar5 = iVar5 + 1;
      pRVar4 = (RwV3d *)&pRVar4[5].y;
    } while (iVar5 < *(int *)(param_1 + 0xc00c));
  }
  FID_conflict__wprintf("Created %d trees\n",g_course_warzone_8bf040);
  g_course_60d9c8 = rwp_object_find_index_by_id(1000);
  g_course_60d9cc = rwp_object_find_index_by_id(2000);
  func_0x0045ca50(&g_course_6a2888,*(undefined4 *)(param_1 + 0xc114),
                  *(undefined4 *)(param_1 + 0xc9b0));
  anim_set_callback(&g_course_6a2888,&UNK_0045d5b0);
  puVar3 = (undefined4 *)rwp_object_get_matrix(g_course_60d9c8);
  uVar1 = g_course_60d9cc;
  puVar6 = (undefined4 *)((int)&g_course_get_6a2950 + 0x8);
  for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
    *puVar6 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar6 = puVar6 + 1;
  }
  puVar3 = (undefined4 *)rwp_object_get_matrix(uVar1);
  puVar6 = (undefined4 *)((int)&g_course_warzone_6a27a0 + 0x8);
  for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
    *puVar6 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar6 = puVar6 + 1;
  }
  rwp_object_set_active(g_course_60d9c8,0);
  rwp_object_set_active(g_course_60d9cc,0);
  g_course_warzone_6a27a0 = func_0x004849e0(&UNK_005df51c,*(undefined4 *)(param_1 + 0xc9d0));
  return;
}





