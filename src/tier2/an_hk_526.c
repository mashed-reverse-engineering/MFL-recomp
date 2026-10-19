#include "mfl_tier2.h"
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, int p1, int p2);
/*0056e5b0*/ extern void FUN_0056e5b0(undefined4 p0, undefined4 p1);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005dd364*/ extern float k_f_n1;
/*006fd498*/ extern undefined4 g_rw_6fd498;
/*008b06e0*/ extern struct Car player_car[16];
/* car_set_physics_matrix */

/* "copies the matrix into the physics body and translates by the field_0x138 offset" */

void __cdecl
car_set_physics_matrix
          (int param_1,RwMatrix *matrix_2,char *param_3,char *param_4,char *param_5,
          char *param_6,char *param_7,char *param_8,char *param_9,
          char *param_10,char *param_11,char *param_12,char *param_13,
          char *param_14,char *param_15,char *param_16,char *param_17)

{
  RwMatrix *matrix;
  int iVar1;
  int iVar2;
  RwMatrix *pRVar3;
  RwMatrix *pRVar4;
  RwV3d local_c;
  
  if (((&g_rw_6fd498)[param_1] != 0) && (iVar1 = rwp_owner_object_get((&g_rw_6fd498)[param_1]), iVar1 != 0))
  {
    matrix = *(RwMatrix **)(*(char **)(iVar1 + 0x10) + 8);
    pRVar3 = matrix_2;
    pRVar4 = matrix;
    for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
      (pRVar4->right).x = (pRVar3->right).x;
      pRVar3 = (RwMatrix *)&(pRVar3->right).y;
      pRVar4 = (RwMatrix *)&(pRVar4->right).y;
    }
    local_c.x = player_car[param_1].rwp_body_offset[0] * k_f_n1;
    local_c.y = player_car[param_1].rwp_body_offset[1] * k_f_n1;
    local_c.z = player_car[param_1].rwp_body_offset[2] * k_f_n1;
    RwV3dTransformVectors(&local_c,&local_c,1,matrix_2);
    RwMatrixTranslate(matrix,&local_c,rwCOMBINEPRECONCAT);
    RwFrameUpdateObjects(*(RwFrame **)((&g_rw_6fd498)[param_1] + 4));
    FUN_0056e5b0(iVar1,0);
    rwp_world_set_bit_5c(*(undefined4 *)(iVar1 + 0x24),iVar1,1);
  }
  return;
}


