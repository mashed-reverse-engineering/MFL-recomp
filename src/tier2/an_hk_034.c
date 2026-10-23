#include "mfl_tier2.h"
typedef int CourseId;
typedef int WeaponId;
typedef signed char sbyte;
/*0056b220*/ extern void rwp_world_set_bit_60_64(void *p0, void *p1, int p2);
/*0056b300*/ extern void FUN_0056b300(undefined4 p0, void *p1);
/*0056b3b0*/ extern void FUN_0056b3b0(void *p0, void *p1);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*006fab78*/ extern undefined4 g_rw_6fab78;

#define RWP_OBJECT_ID_COUNT (200)
#define OBJECT_WORLD_OFFSET (0x24)
#define OBJECT_INDEX_OFFSET (0x20)
#define WORLD_BIT_ARRAY_OFFSET (0x60)

/* rwp_object_set_enabled */

/* "enables/disables the physics body of an object by id" */

/*
 * Enables the physics body of the object in slot object_id of g_rw_6fab78 when enable != 0,
 * disables it when enable == 0. Does nothing if object_id is out of range, the slot is empty,
 * or the owner object has no world (inferred). The world keeps one bit per object at
 * WORLD_BIT_ARRAY_OFFSET (inferred: set = body registered in the world). Returns nothing.
 */
void rwp_object_set_enabled(int object_id,int enable)

{
  char *world;
  char *object;

  if ((((-1 < object_id) && (object_id < RWP_OBJECT_ID_COUNT)) && ((&g_rw_6fab78)[object_id] != 0)) &&
     (object = rwp_owner_object_get((&g_rw_6fab78)[object_id]), object != 0)) {
    world = *(char **)(object + OBJECT_WORLD_OFFSET);
    if (world != 0) {
      /* body already registered and disable requested: unregister it and stop */
      if (((*(uint *)(*(char **)(world + WORLD_BIT_ARRAY_OFFSET) + (uint)(*(ushort *)(object + OBJECT_INDEX_OFFSET) >> 5) * 4) &
           1 << ((byte)*(ushort *)(object + OBJECT_INDEX_OFFSET) & 0x1f)) != 0) && (enable == 0)) {
        FUN_0056b3b0(world,object);
        return;
      }
      /* body already registered and enable requested: nothing to do */
      if ((world != 0) &&
         ((*(uint *)(*(char **)(world + WORLD_BIT_ARRAY_OFFSET) + (uint)(*(ushort *)(object + OBJECT_INDEX_OFFSET) >> 5) * 4) &
          1 << ((byte)*(ushort *)(object + OBJECT_INDEX_OFFSET) & 0x1f)) != 0)) {
        return;
      }
    }
    /* not registered and enable requested: set the world bit, then register the body */
    if (enable != 0) {
      rwp_world_set_bit_60_64(world,object,1);
      FUN_0056b300(*(undefined4 *)(object + OBJECT_WORLD_OFFSET),object);
    }
  }
  return;
}
