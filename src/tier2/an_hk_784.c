#include "mfl_tier2.h"
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056b510*/ extern void FUN_0056b510(undefined4 p0, undefined4 p1);
/*0056e250*/ extern int FUN_0056e250(void *p0, undefined4 p1, void *p2);
/*00573f10*/ extern int FUN_00573f10(undefined4 p0, undefined4 p1);
/*00573f70*/ extern void FUN_00573f70(void *p0);
/*00573fd0*/ extern void FUN_00573fd0(void *p0, undefined4 p1);
/*0058cb40*/ extern void FUN_0058cb40(void *p0, undefined4 p1, undefined4 p2);
/*006fa858*/ extern undefined4 g_rw_6fa858[0x1000];
/*0088a840*/ extern undefined4 g_rwp_world_88a840;

#define FLOAT_1000_BITS (0x447a0000) /* float bits of +1000.0f */
#define FLOAT_NEG_1000_BITS (0xc47a0000) /* float bits of -1000.0f */
#define WORLD_MAX_OBJECTS (200)
#define WORLD_MAX_CONTACTS (100)
#define WORLD_BITS_OFFSET (0xc) /* byte offset of the bit matrix inside the world */
#define WORLD_DIM_OFFSET (4) /* int at +4, n in the index math (inferred) */
#define BIT_WORD_SHIFT (5) /* bit index -> 32-bit word index */
#define BIT_INDEX_MASK (0x1f) /* bit index -> bit within the word */

/* rwp_world_create */

/* "creates the Rwp world (+-1000 box), 200 objects, 100 contacts, collision masks via the bit
   matrix"
   Builds the collision world: a +-1000 box, WORLD_MAX_OBJECTS objects and WORLD_MAX_CONTACTS
   contacts, then sets collision bits in the world's bit matrix.
   parent: passed to FUN_0056e250 together with the box (inferred: the parent frame).
   Returns the handle FUN_0056e250 created (inferred). */

undefined4 rwp_world_create(undefined4 parent)

{
  uint *matrix_word;
  undefined4 rwp_handle;
  undefined4 first_field;
  uint bit_index;
  int remaining;
  uint word_index;
  undefined4 *slot_ptr;
  struct {
    undefined4 bounds_max_x;
    undefined4 bounds_max_y;
    undefined4 bounds_max_z;
    undefined4 bounds_max_w;
    undefined4 bounds_min_x;
    undefined4 bounds_min_y;
    undefined4 bounds_min_z;
    undefined4 bounds_min_w;
    undefined1 world_buf [124];
  } frame;

  /* bounds box: two vectors in frame, +1000 corner then -1000 corner, w = 0 (inferred) */
  frame.bounds_max_w = 0;
  frame.bounds_min_w = 0;
  frame.bounds_max_x = FLOAT_1000_BITS;
  frame.bounds_max_y = FLOAT_1000_BITS;
  frame.bounds_max_z = FLOAT_1000_BITS;
  frame.bounds_min_x = FLOAT_NEG_1000_BITS;
  frame.bounds_min_y = FLOAT_NEG_1000_BITS;
  frame.bounds_min_z = FLOAT_NEG_1000_BITS;
  FUN_00573f70(frame.world_buf);
  FUN_0058cb40(frame.world_buf,WORLD_MAX_OBJECTS,WORLD_MAX_CONTACTS);
  FUN_00573fd0(frame.world_buf,0x20);
  rwp_handle = FUN_0056e250(frame.world_buf,parent,&frame.bounds_max_x);
  g_rwp_world_88a840 = FUN_00573f10(10,10);
  /* collision bits: set bit (n+1)*k for k = 1..8, plus bit 7n+1, where n = world +WORLD_DIM_OFFSET
     (inferred). Each bit lives in the 32-bit word at WORLD_BITS_OFFSET + (bit >> 5) * 4. */
  bit_index = *(int *)(g_rwp_world_88a840 + WORLD_DIM_OFFSET) + 1;
  matrix_word = (uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET + (bit_index >> BIT_WORD_SHIFT) * 4);
  *matrix_word = *matrix_word | 1 << ((byte)bit_index & BIT_INDEX_MASK);
  bit_index = *(int *)(g_rwp_world_88a840 + WORLD_DIM_OFFSET) * 2 + 2;
  word_index = bit_index >> BIT_WORD_SHIFT;
  *(uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET +word_index * 4) =
       *(uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET +word_index * 4) | 1 << ((byte)bit_index & BIT_INDEX_MASK);
  bit_index = (*(int *)(g_rwp_world_88a840 + WORLD_DIM_OFFSET) + 1) * 3;
  matrix_word = (uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET + (bit_index >> BIT_WORD_SHIFT) * 4);
  *matrix_word = *matrix_word | 1 << ((byte)bit_index & BIT_INDEX_MASK);
  bit_index = *(int *)(g_rwp_world_88a840 + WORLD_DIM_OFFSET) * 4 + 4;
  matrix_word = (uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET + (bit_index >> BIT_WORD_SHIFT) * 4);
  *matrix_word = *matrix_word | 1 << ((byte)bit_index & BIT_INDEX_MASK);
  bit_index = (*(int *)(g_rwp_world_88a840 + WORLD_DIM_OFFSET) + 1) * 5;
  word_index = bit_index >> BIT_WORD_SHIFT;
  *(uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET +word_index * 4) =
       *(uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET +word_index * 4) | 1 << ((byte)bit_index & BIT_INDEX_MASK);
  /* the compiler reused the two temporaries here: word_index holds the bit index and
     bit_index the word index (names follow the first use, not this one) */
  word_index = (*(int *)(g_rwp_world_88a840 + WORLD_DIM_OFFSET) + 1) * 6;
  bit_index = word_index >> BIT_WORD_SHIFT;
  *(uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET +bit_index * 4) =
       *(uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET +bit_index * 4) | 1 << ((byte)word_index & BIT_INDEX_MASK);
  bit_index = (*(int *)(g_rwp_world_88a840 + WORLD_DIM_OFFSET) + 1) * 7;
  matrix_word = (uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET + (bit_index >> BIT_WORD_SHIFT) * 4);
  *matrix_word = *matrix_word | 1 << ((byte)bit_index & BIT_INDEX_MASK);
  bit_index = *(int *)(g_rwp_world_88a840 + WORLD_DIM_OFFSET) * 7 + 1;
  matrix_word = (uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET + (bit_index >> BIT_WORD_SHIFT) * 4);
  *matrix_word = *matrix_word | 1 << ((byte)bit_index & BIT_INDEX_MASK);
  bit_index = *(int *)(g_rwp_world_88a840 + WORLD_DIM_OFFSET) * 8 + 8;
  word_index = bit_index >> BIT_WORD_SHIFT;
  *(uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET +word_index * 4) =
       *(uint *)(g_rwp_world_88a840 + WORLD_BITS_OFFSET +word_index * 4) | 1 << ((byte)bit_index & BIT_INDEX_MASK);
  first_field = rw_first_field((int *)rwp_handle);
  /* attach the world to the first field of the handle (inferred) */
  FUN_0056b510(first_field, g_rwp_world_88a840);
  /* set the first WORLD_MAX_OBJECTS entries of g_rw_6fa858 to 1 (inferred: one flag per object) */
  slot_ptr = g_rw_6fa858;
  for (remaining = WORLD_MAX_OBJECTS; remaining != 0; remaining = remaining + -1) {
    *slot_ptr = 1;
    slot_ptr = slot_ptr + 1;
  }
  return rwp_handle;
}


