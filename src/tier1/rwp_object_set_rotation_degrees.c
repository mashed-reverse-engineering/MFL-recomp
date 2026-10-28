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
void rwp_object_set_rotation_degrees(int slot,int quarter_turns,RwReal pos_x,RwReal pos_y,RwReal pos_z)
{
  ushort bit_index;
  char *owner_world;
  RwMatrix *matrix;
  char *owner_object;
  undefined4 pose_id;
  struct {
    RwV3d axis;
    undefined1 pose_scratch [32];
  } frame;
  
  /* rotation axis = +Z (inferred: "angle*90 about z" per old summary) */
  frame.axis.x = 0.0;
  frame.axis.y = 0.0;
  frame.axis.z = 1.0;
  owner_object = rwp_owner_object_get((&g_rw_6fd498)[slot]);
  /* owner_object+0x24 is its world/container pointer (inferred); if the object's bit in the world's bitset (+0x60, index = owner_object+0x20) is set, FUN_0056b3b0 is called first */
  owner_world = *(char **)(owner_object + 0x24);
  if (owner_world != 0) {
    if ((*(uint *)(*(char **)(owner_world + 0x60) + (uint)((bit_index = *(ushort *)(owner_object + 0x20)) >> 5) * 4) & 1 << (bit_index & 0x1f)) != 0) {
      FUN_0056b3b0(owner_world,owner_object);
    }
  }
  /* rotate the frame matrix by quarter_turns*90 degrees (k_f_90) about Z, replacing it, then set the position */
  matrix = *(RwMatrix **)(*(char **)(owner_object + 0x10) + 8);
  RwMatrixRotate(matrix,&frame.axis,(float)quarter_turns * k_f_90,rwCOMBINEREPLACE);
  (matrix->pos).x = pos_x;
  (matrix->pos).y = pos_y;
  (matrix->pos).z = pos_z;
  RwMatrixUpdate(matrix);
  RwFrameUpdateObjects(*(RwFrame **)((&g_rw_6fd498)[slot] + 4));
  /* refresh the object's pose/world registration if it is in a world (inferred) */
  FUN_0056e5b0(owner_object,0);
  if (*(int *)(owner_object + 0x24) != 0) {
    pose_id = FUN_0058cab0(owner_object,frame.pose_scratch);
    FUN_0056b100(*(undefined4 *)(owner_object + 0x24),owner_object,pose_id);
    rwp_world_set_bit_5c(*(undefined4 *)(owner_object + 0x24),owner_object,0);
  }
  return;
}
