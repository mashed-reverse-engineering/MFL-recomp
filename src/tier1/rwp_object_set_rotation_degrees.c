#include "mfl_tier2.h"
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*0056b100*/ extern void FUN_0056b100(undefined4 p0, void *p1, undefined4 p2);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, void *p1, int p2);
/*0056b3b0*/ extern void FUN_0056b3b0(void *p0, void *p1);
/*0056e5b0*/ extern void FUN_0056e5b0(void *p0, undefined4 p1);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*0058cab0*/ extern int FUN_0058cab0(void *p0, void *p1);
/*005ddad4*/ extern float k_f_90;
/*006fd498*/ extern undefined4 g_rw_6fd498;
/* rwp_object_set_rotation_degrees */
/* "sets the body matrix rotation (angle*90 about z) and position" */
void rwp_object_set_rotation_degrees(int param_1,int param_2,RwReal param_3,RwReal param_4,RwReal param_5)
{
  ushort bit_index;
  char *iVar1;
  RwMatrix *matrix;
  char *iVar2;
  undefined4 uVar3;
  struct {
    RwV3d local_2c;
    undefined1 local_20 [32];
  } frame;
  
  frame.local_2c.x = 0.0;
  frame.local_2c.y = 0.0;
  frame.local_2c.z = 1.0;
  iVar2 = rwp_owner_object_get((&g_rw_6fd498)[param_1]);
  iVar1 = *(char **)(iVar2 + 0x24);
  if (iVar1 != 0) {
    if ((*(uint *)(*(char **)(iVar1 + 0x60) + (uint)((bit_index = *(ushort *)(iVar2 + 0x20)) >> 5) * 4) & 1 << (bit_index & 0x1f)) != 0) {
      FUN_0056b3b0(iVar1,iVar2);
    }
  }
  matrix = *(RwMatrix **)(*(char **)(iVar2 + 0x10) + 8);
  RwMatrixRotate(matrix,&frame.local_2c,(float)param_2 * k_f_90,rwCOMBINEREPLACE);
  (matrix->pos).x = param_3;
  (matrix->pos).y = param_4;
  (matrix->pos).z = param_5;
  RwMatrixUpdate(matrix);
  RwFrameUpdateObjects(*(RwFrame **)((&g_rw_6fd498)[param_1] + 4));
  FUN_0056e5b0(iVar2,0);
  if (*(int *)(iVar2 + 0x24) != 0) {
    uVar3 = FUN_0058cab0(iVar2,frame.local_20);
    FUN_0056b100(*(undefined4 *)(iVar2 + 0x24),iVar2,uVar3);
    rwp_world_set_bit_5c(*(undefined4 *)(iVar2 + 0x24),iVar2,0);
  }
  return;
}
