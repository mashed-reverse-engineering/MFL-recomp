#include "mfl_tier2.h"
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*0056b100*/ extern void FUN_0056b100(undefined4 p0, void *p1, undefined4 p2);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, void *p1, int p2);
/*0056b3b0*/ extern void FUN_0056b3b0(void *p0, void *p1);
/*0056e5b0*/ extern void FUN_0056e5b0(void *p0, undefined4 p1);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*0058cab0*/ extern int FUN_0058cab0(void *p0, void *p1);
/*006fab78*/ extern undefined4 g_rw_6fab78;
/* offsets into the owner object (inferred from use) */
#define OWNER_FRAME_OFFSET (0x10)
#define OWNER_WORLD_OFFSET (0x24)
#define OWNER_BITSET_INDEX_OFFSET (0x20)
/* offset of the bitset table inside the world (inferred) */
#define WORLD_BITSET_OFFSET (0x60)
/* bit index is split into a word index (>> 5) and a bit within the word (& 0x1f) */
#define BITSET_WORD_SHIFT (5)
#define BITSET_BIT_MASK (0x1f)
/* rwp_object_set_matrix */
/* "(index, matrix): transforms the physics object's frame and updates its world position" */
void rwp_object_set_matrix(int object_index,RwMatrix *transform)
{
  char *world;
  ushort bit_index;
  RwMatrix *matrix;
  char *owner_object;
  undefined4 aux_result;
  undefined1 aux_buffer [32];

  owner_object = rwp_owner_object_get((&g_rw_6fab78)[object_index]);
  world = *(char **)(owner_object + OWNER_WORLD_OFFSET);
  /* if the world has a bit set for this object, notify it before the update */
  if ((world != 0) && (bit_index = *(ushort *)(owner_object + OWNER_BITSET_INDEX_OFFSET),
     ((*(uint *)(*(char **)(world + WORLD_BITSET_OFFSET) + (uint)(bit_index >> BITSET_WORD_SHIFT) * 4) &
      1 << (bit_index & BITSET_BIT_MASK)) != 0))) {
    FUN_0056b3b0(world,owner_object);
  }
  matrix = *(RwMatrix **)(*(char **)(owner_object + OWNER_FRAME_OFFSET) + 8);
  RwMatrixTransform(matrix,transform,rwCOMBINEREPLACE);
  RwMatrixUpdate(matrix);
  RwFrameUpdateObjects(*(RwFrame **)((&g_rw_6fab78)[object_index] + 4));
  FUN_0056e5b0(owner_object,0);
  /* if the object is in a world, report the update to it (inferred) */
  if (*(int *)(owner_object + OWNER_WORLD_OFFSET) != 0) {
    aux_result = FUN_0058cab0(owner_object,aux_buffer);
    FUN_0056b100(*(undefined4 *)(owner_object + OWNER_WORLD_OFFSET),owner_object,aux_result);
    rwp_world_set_bit_5c(*(undefined4 *)(owner_object + OWNER_WORLD_OFFSET),owner_object,0);
  }
  return;
}
