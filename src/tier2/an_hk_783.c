#include "mfl_tier2.h"
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056a210*/ extern void FUN_0056a210(undefined4 p0, undefined4 st_arg2, undefined4 st_arg3, undefined4 st_arg4);
/*0056a4b0*/ extern void FUN_0056a4b0(undefined4 p0, undefined4 p1);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*006fab78*/ extern undefined4 g_rw_6fab78;
/*00701c9c*/ extern undefined4 g_rw_701c9c;
#define WORLD_OBJECT_SLOTS (200)
#define PHYSICS_OBJECT_WORLD_LINK_OFFSET (0x24)
#define PHYSICS_OBJECT_FLAGS_OFFSET (0x8)
/* rwp_object_set_in_world */

/* "(index, flag): adds / removes the physics object from the physics world" */

/* Adds or removes the physics object in slot slot_index of g_rw_6fab78 from the physics world.
 * slot_index: 0..WORLD_OBJECT_SLOTS-1, must have a non-zero handle in g_rw_6fab78 (otherwise nothing happens).
 * add_to_world: non-zero = add, zero = remove.
 * Removing is only done when the object is currently in the world (link at +0x24 non-zero);
 * adding is only done when it is not (link zero), and then clears flag bits 0x0c at +0x8 (inferred) before the add call.
 * Returns nothing. */
void rwp_object_set_in_world(int slot_index,int add_to_world)

{
  int physics_object;
  undefined4 physics_world;

  /* valid slot with a handle */
  if (((-1 < slot_index) && (slot_index < WORLD_OBJECT_SLOTS)) && ((&g_rw_6fab78)[slot_index] != 0)) {
    physics_object = rwp_owner_object_get((&g_rw_6fab78)[slot_index]);
    physics_world = rw_first_field((int *)g_rw_701c9c);
    if (add_to_world == 0) {
      /* remove: only if currently in the world */
      if (*(int *)(physics_object + PHYSICS_OBJECT_WORLD_LINK_OFFSET) != 0) {
        FUN_0056a4b0(physics_world,physics_object);
      }
    }
    /* add: only if not already in the world */
    else if (*(int *)(physics_object + PHYSICS_OBJECT_WORLD_LINK_OFFSET) == 0) {
      *(uint *)(physics_object + PHYSICS_OBJECT_FLAGS_OFFSET) = *(uint *)(physics_object + PHYSICS_OBJECT_FLAGS_OFFSET) & 0xfffffff3;
      FUN_0056a210(physics_world,physics_object,0xffffffff,7);
      return;
    }
  }
  return;
}


