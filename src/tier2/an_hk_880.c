#include "mfl_tier2.h"
/*0048e6d0*/ extern int rwp_object_get_handle();
/*0056c140*/ extern int __cdecl FUN_0056c140(int a1, int a2, void *a3);
/*0056c3b0*/ extern int __cdecl FUN_0056c3b0(int a1, int a2, int a3, int a4, void *a5);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);

/* Byte offsets into the owner object and into the body-flag holder it points to. */
#define OWNER_BLOCK_OFFSET (0x10)
#define OWNER_BITSET_HOLDER_OFFSET (0x24)
#define OWNER_BIT_INDEX_OFFSET (0x20)
#define BITSET_ARRAY_OFFSET (100)
/* Bit set is an array of 32-bit words: word = index >> 5, bit = index & 0x1f. */
#define BITSET_WORD_SHIFT (5)
#define BITSET_BIT_MASK (0x1f)
#define BITSET_WORD_BYTES (4)
/* rwp_object_query_ray_hit */

/* "queries the body data of a physics object if it is in the world" */

/* Ray test against the physics body of the object identified by object_ref (inferred).
 * ray_origin and ray_direction are passed on to FUN_0056c3b0 unchanged (inferred).
 * On a hit, hit_point and hit_normal each receive 3 words and 1 is returned; otherwise 0.
 * Locals are reused: query_handle holds the rwp handle, then word 0 of the owner block;
 * owner_or_hit holds the owner object, then the FUN_0056c3b0 result.
 */
undefined4 __cdecl rwp_object_query_ray_hit(undefined4 object_ref,undefined4 ray_origin,undefined4 ray_direction,undefined4 *hit_point,
          undefined4 *hit_normal)

{
  undefined4 owner_block_word2;
  undefined4 query_handle;
  int owner_or_hit;
  struct {
    undefined1 query_setup [16];
    undefined1 hit_header [4];
    undefined4 hit_point_x;
    undefined4 hit_point_y;
    undefined4 hit_point_z;
    undefined4 hit_normal_x;
    undefined4 hit_normal_y;
    undefined4 hit_normal_z;
    undefined4 pad_top;
  } hit_buffer;

  /* Resolve the object's handle and its owner object, then read the owner's block:
     word 0 becomes query_handle, word 2 becomes owner_block_word2 (inferred). */
  query_handle = rwp_object_get_handle(object_ref);
  owner_or_hit = rwp_owner_object_get(query_handle);
  query_handle = **(undefined4 **)(owner_or_hit + OWNER_BLOCK_OFFSET);
  owner_block_word2 = (*(undefined4 **)(owner_or_hit + OWNER_BLOCK_OFFSET))[2];
  /* Only test rays for an owner that has a body and whose body-flag bit is set,
     i.e. the object is in the world (inferred). */
  if ((*(int *)(owner_or_hit + OWNER_BITSET_HOLDER_OFFSET) != 0) &&
     ((*(uint *)(*(char **)(*(char **)(owner_or_hit + OWNER_BITSET_HOLDER_OFFSET) + BITSET_ARRAY_OFFSET) + (uint)(*(ushort *)(owner_or_hit + OWNER_BIT_INDEX_OFFSET) >> BITSET_WORD_SHIFT) * BITSET_WORD_BYTES
                ) & 1 << ((byte)*(ushort *)(owner_or_hit + OWNER_BIT_INDEX_OFFSET) & BITSET_BIT_MASK)) != 0)) {
    /* Per-query setup into hit_buffer, then the ray test itself (both inferred). */
    FUN_0056c140(query_handle,owner_block_word2,hit_buffer.query_setup);
    owner_or_hit = FUN_0056c3b0(query_handle,owner_block_word2,ray_origin,ray_direction,hit_buffer.hit_header);
    /* Hit: copy the 6 result words out, 3 to hit_point and 3 to hit_normal (inferred). */
    if (owner_or_hit != 0) {
      *hit_point = hit_buffer.hit_point_x;
      hit_point[1] = hit_buffer.hit_point_y;
      hit_point[2] = hit_buffer.hit_point_z;
      *hit_normal = hit_buffer.hit_normal_x;
      hit_normal[1] = hit_buffer.hit_normal_y;
      hit_normal[2] = hit_buffer.hit_normal_z;
      return 1;
    }
  }
  return 0;
}


