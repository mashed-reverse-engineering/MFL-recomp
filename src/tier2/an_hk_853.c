#include "mfl_tier2.h"
/*0048e650*/ extern int rwp_object_set_in_world();
/*00492d10*/ extern int rwp_object_set_active();
/*00499dc0*/ extern int __cdecl shadow_project_to_ground_00499dc0(int p0, RwMatrix *p1, float *p2, RwMatrix *p3, float *p4);
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*0056b100*/ extern void FUN_0056b100(undefined4 p0, void *p1, undefined4 p2);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, void *p1, int p2);
/*0056b3b0*/ extern void FUN_0056b3b0(undefined4 p0, void *p1);
/*0056c350*/ extern void FUN_0056c350(undefined4 p0, void *p1, undefined4 p2, void *p3);
/*0056e5b0*/ extern void FUN_0056e5b0(void *p0, undefined4 p1);
/*0058c8a0*/ extern char *rwp_owner_object_get(int p0);
/*0058cab0*/ extern int FUN_0058cab0(void *p0, void *p1);
/*006f6950*/ extern undefined4 g_rw_6f6950;
/*006f6c70*/ extern undefined4 g_rw_6f6c70;
/*006f6f90*/ extern undefined g_rw_6f6f90;
/*006fa858*/ extern undefined4 g_rw_6fa858;
/*006fab78*/ extern undefined4 g_rw_6fab78;
/*006fae98*/ extern undefined g_rw_6fae98;
/*006fcdd8*/ extern undefined4 g_rw_6fcdd8;
/*006fd4a8*/ extern undefined4 g_rw_6fd4a8;
/*006fea90*/ extern undefined4 g_rw_6fea90;
/*00801b14*/ extern undefined4 g_rw_801b14;

#define MATRIX_WORD_COUNT (0x10) /* 16 words = one 4x4 RwMatrix */
#define MATRIX_SLOT_BYTES (0x40) /* per-object stride in the matrix tables (g_rw_6fea90, g_rw_6f6f90) */
#define MATRIX_FLAGS_SET (0x20003) /* bits OR-ed into matrix_flags (inferred) */
#define FLOAT_ONE_BITS (0x3f800000) /* IEEE 754 bit pattern of 1.0f */
#define SHADOW_RESULT_NONE (0xffffffff) /* marker stored in shadow_result[6] before the probe (inferred) */
#define SHADOW_POINTS_STRIDE (6) /* floats per object in shadow_points */
#define SHADOW_RESULT_STRIDE (10) /* floats per object in shadow_result (0x28 bytes) */
#define SHADOW_RESULT_TABLE_BYTES (0x1f40) /* end of the shadow_result table: 200 slots of 0x28 (inferred) */
#define BITSET_WORD_SHIFT (5) /* 32 bits per word of the bitset at object world +0x60 */
#define BITSET_BIT_MASK (0x1f)

/* rwp_objects_reset_all */

/* "resets every dynamic object (matrix from the saved one, flags, world and active state)" */

/* Resets every dynamic object slot. No parameters, no return value.
 * Slot i (obj_index) is reset when it is in use (g_rw_6fa858[i]) and has an owner object (g_rw_6fab78[i]):
 *   - sets the slot in world and active (rwp_object_set_in_world / rwp_object_set_active)
 *   - restores the object's matrix from the saved table g_rw_6fea90 (MATRIX_SLOT_BYTES per slot)
 *   - updates the matrix and the owner frame, then re-links the object to its world (inferred)
 *   - projects the ground shadow with g_rw_801b14 as the caster, if one is set
 * Slots are walked in order; the shadow pointers advance with them. */
void rwp_objects_reset_all(void)

{
  undefined4 *object_matrix;
  char *object;
  undefined4 slot;
  int world;
  int obj_index;
  undefined4 *saved_matrix_src;
  undefined4 *matrix_dst;
  /* scratch frame: identity matrix at right_x..pos_z (inferred), then the output vectors and a 32 byte buffer */
  struct {
    float *shadow_points;
    int matrix_offset;
    float *shadow_result;
    undefined4 right_x;
    undefined4 right_y;
    undefined4 right_z;
    uint matrix_flags;
    undefined4 up_x;
    undefined4 up_y;
    undefined4 up_z;
    undefined1 pad_64[4];
    undefined4 at_x;
    undefined4 at_y;
    undefined4 at_z;
    undefined1 pad_54[4];
    undefined4 pos_x;
    undefined4 pos_y;
    undefined4 pos_z;
    undefined1 pad_44[4];
    float out_pos_x;
    float out_pos_y;
    float out_pos_z;
    undefined1 pad_34[4];
    float aux_x;
    float aux_y;
    float aux_z;
    undefined1 pad_24[4];
    undefined1 scratch [32];
  } frame;

  /* identity matrix in frame: right = (1,0,0), up = (0,1,0), at = (0,0,1), pos = 0 */
  frame.matrix_flags = frame.matrix_flags | MATRIX_FLAGS_SET;
  frame.at_z = FLOAT_ONE_BITS;
  frame.up_y = FLOAT_ONE_BITS;
  frame.right_x = FLOAT_ONE_BITS;
  frame.up_x = 0;
  frame.right_z = 0;
  frame.right_y = 0;
  frame.at_y = 0;
  frame.at_x = 0;
  frame.up_z = 0;
  frame.pos_z = 0;
  frame.pos_y = 0;
  frame.pos_x = 0;
  obj_index = 0;
  frame.shadow_points = (float *)&g_rw_6fd4a8;
  frame.shadow_result = (float *)&g_rw_6fae98;
  frame.matrix_offset = 0;
  /* per-slot loop; the local `world` is reused as the matrix word countdown further down (inferred) */
  do {
    *(undefined4 *)&frame.shadow_result[6] = SHADOW_RESULT_NONE;
    (&g_rw_6f6950)[obj_index] = 0;
    (&g_rw_6f6c70)[obj_index] = 0;
    /* slot in use (g_rw_6fa858[i]): g_rw_6fcdd8[i] is cleared, then the owner must be set */
    if (((&g_rw_6fa858)[obj_index] != 0) && ((&g_rw_6fcdd8)[obj_index] = 0, (&g_rw_6fab78)[obj_index] != 0)) {
      rwp_object_set_in_world(obj_index,1);
      rwp_object_set_active(obj_index,1);
      object = rwp_owner_object_get((&g_rw_6fab78)[obj_index]);
      world = *(int *)(object + 0x24);
      /* object's bit set in the world bitset (+0x60, bit id at object +0x20): detach it (FUN_0056b3b0, inferred) */
      if ((world != 0) &&
         ((*(uint *)(*(char **)(world + 0x60) + (uint)(*(ushort *)(object + 0x20) >> BITSET_WORD_SHIFT) * 4) &
          1 << ((byte)*(ushort *)(object + 0x20) & BITSET_BIT_MASK)) != 0)) {
        FUN_0056b3b0(world,object);
      }
      object_matrix = *(undefined4 **)(*(char **)(object + 0x10) + 8);
      saved_matrix_src = (undefined4 *)((char *)&g_rw_6fea90 + frame.matrix_offset);
      matrix_dst = object_matrix;
      /* copy the saved matrix (MATRIX_WORD_COUNT words) over the object's matrix */
      for (world = MATRIX_WORD_COUNT; world != 0; world = world + -1) {
        *matrix_dst = *saved_matrix_src;
        saved_matrix_src = saved_matrix_src + 1;
        matrix_dst = matrix_dst + 1;
      }
      RwMatrixUpdate(object_matrix);
      RwFrameUpdateObjects(*(RwFrame **)((&g_rw_6fab78)[obj_index] + 4));
      FUN_0056e5b0(object,0);
      /* owner has a world: re-link the object in it (FUN_0056b100 / rwp_world_set_bit_5c, inferred) */
      if (*(int *)(object + 0x24) != 0) {
        slot = FUN_0058cab0(object,frame.scratch);
        FUN_0056b100(*(undefined4 *)(object + 0x24),object,slot);
        rwp_world_set_bit_5c(*(undefined4 *)(object + 0x24),object,0);
      }
      FUN_0056c350(**(undefined4 **)(object + 0x10),&frame.right_x,0,&frame.out_pos_x);
      frame.shadow_points[3] = frame.aux_x;
      frame.shadow_points[4] = frame.aux_y;
      frame.shadow_points[5] = frame.aux_z;
      *frame.shadow_points = frame.out_pos_x;
      frame.shadow_points[1] = frame.out_pos_y;
      frame.shadow_points[2] = frame.out_pos_z;
      /* ground shadow: points from FUN_0056c350 output, projected only when a caster (g_rw_801b14) is set */
      if (g_rw_801b14 != 0) {
        shadow_project_to_ground_00499dc0
                  (g_rw_801b14,(RwMatrix *)((char *)&g_rw_6fea90 + frame.matrix_offset),frame.shadow_points,
                   (RwMatrix *)((char *)&g_rw_6f6f90 + frame.matrix_offset),frame.shadow_result);
      }
    }
    frame.shadow_result = frame.shadow_result + SHADOW_RESULT_STRIDE;
    frame.shadow_points = frame.shadow_points + SHADOW_POINTS_STRIDE;
    frame.matrix_offset = frame.matrix_offset + MATRIX_SLOT_BYTES;
    obj_index = obj_index + 1;
  } while ((int)frame.shadow_result < (int)&g_rw_6fae98 + SHADOW_RESULT_TABLE_BYTES);
  return;
}


