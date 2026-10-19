#include "mfl_tier2.h"
typedef int CourseId;
typedef int WeaponId;
typedef signed char sbyte;
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
/*0089de60*/ extern undefined4 g_gfx_89de60;
/*008b06e0*/ extern char player_car[];
#define RWP_BLOCK_STRIDE (0x944)
#define RWP_BLOCK_FLAG (-0x10)
#define RWP_BLOCK_ENTRY_BASE (-8)
#define RWP_BLOCK_RESOLVED_COUNT (0x41)
#define RWP_BLOCK_INDEX_OFFSET (0x21)
#define RWP_BLOCK_GROUP2_COUNT (0x8fa)
#define RWP_BLOCK_GROUP2_BASE (0x4d)
#define RWP_BLOCK_FINAL_COUNT (0x933)
#define RWP_BLOCK_FINAL_BASE (0x901)
#define RWP_GROUP2_STRIDE (0x117)
#define RWP_GROUP2_COUNT_OFFSET (0x103)
#define RWP_GROUP2_OBJ_FROM_COUNT (0x10b)
#define RWP_SLOT_INDICES_STRIDE (0x20)
#define RWP_FINAL_STRIDE (7)

/* rwp_trigger_groups_resolve_indices */

/* "for each trigger group resolves the rwp object base_label + n to object indices (g_rwp_objects)"
    */

/* Walks the trigger-group blocks in g_gfx_89de60 (one every RWP_BLOCK_STRIDE ints, up to player_car).
   For each block whose enable byte (RWP_BLOCK_FLAG) is non-zero it writes two resolved tables:
   the ids (base_label + k) and their object indices (rwp_object_find_index_by_id) for every object
   the block lists, and the same for the group-2 records. Layout offsets are inferred from the code.
   Takes no parameters and returns nothing. */

void rwp_trigger_groups_resolve_indices(void)

{
  int tmp;
  int *entry;
  int object_index;
  int *dst;
  int k;
  int *count_ptr;
  int entry_index;
  int *slot_indices;
  int slot;
  int *block;

  block = &g_gfx_89de60;
  do {
    entry_index = 0;
    block[RWP_BLOCK_RESOLVED_COUNT] = 0;
    if ((char)block[RWP_BLOCK_FLAG] != '\0') {
      /* group 1: for each listed object, append its ids and their indices after the resolved count */
      if (0 < *block) {
        entry = block + RWP_BLOCK_ENTRY_BASE;
        do {
          object_index = *entry;
          k = 0;
          if (0 < *(int *)&g_rwp_objects[object_index].field_0x10c) {
            do {
              tmp = g_rwp_objects[object_index].base_label + k;
              block[block[RWP_BLOCK_RESOLVED_COUNT] + k + 1] = tmp;
              tmp = rwp_object_find_index_by_id(tmp);
              block[block[RWP_BLOCK_RESOLVED_COUNT] + k + RWP_BLOCK_INDEX_OFFSET] = tmp;
              k = k + 1;
            } while (k < *(int *)&g_rwp_objects[object_index].field_0x10c);
          }
          block[RWP_BLOCK_RESOLVED_COUNT] =
               block[RWP_BLOCK_RESOLVED_COUNT] + *(int *)&g_rwp_objects[object_index].field_0x10c;
          entry_index = entry_index + 1;
          entry = entry + 1;
        } while (entry_index < *block);
      }
      entry_index = 0;
      /* group 2: for each record, resolve every object id into its slot_indices and count them
         (tmp is the id counter here, k the object's id count, then the index) */
      if (0 < block[RWP_BLOCK_GROUP2_COUNT]) {
        entry = block + RWP_BLOCK_GROUP2_BASE;
        do {
          slot = 0;
          if (0 < *entry) {
            slot_indices = entry + 3;
            count_ptr = entry + RWP_GROUP2_COUNT_OFFSET;
            do {
              object_index = count_ptr[-RWP_GROUP2_OBJ_FROM_COUNT];
              k = *(int *)&g_rwp_objects[object_index].field_0x10c;
              tmp = 0;
              *count_ptr = 0;
              dst = slot_indices;
              if (0 < k) {
                do {
                  k = rwp_object_find_index_by_id(g_rwp_objects[object_index].base_label + tmp);
                  *dst = k;
                  *count_ptr = *count_ptr + 1;
                  tmp = tmp + 1;
                  dst = dst + 1;
                } while (tmp < *(int *)&g_rwp_objects[object_index].field_0x10c);
              }
              slot = slot + 1;
              slot_indices = slot_indices + RWP_SLOT_INDICES_STRIDE;
              count_ptr = count_ptr + 1;
            } while (slot < *entry);
          }
          entry_index = entry_index + 1;
          entry = entry + RWP_GROUP2_STRIDE;
        } while (entry_index < block[RWP_BLOCK_GROUP2_COUNT]);
      }
      object_index = 0;
      /* final records: reset their flags to 1 / 0 / 1 (inferred meaning) */
      if (0 < block[RWP_BLOCK_FINAL_COUNT]) {
        entry = block + RWP_BLOCK_FINAL_BASE;
        do {
          entry[-1] = 1;
          *entry = 0;
          entry[-4] = 1;
          object_index = object_index + 1;
          entry = entry + RWP_FINAL_STRIDE;
        } while (object_index < block[RWP_BLOCK_FINAL_COUNT]);
      }
    }
    block = block + RWP_BLOCK_STRIDE;
  } while (block < (int *)&player_car);
  return;
}
