#include "mfl_tier2.h"
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, void *p1, int p2);
/*0056b300*/ extern void FUN_0056b300(void *p0, void *p1);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/* debris_apply_velocity_a */
/* "(body, vec3 or zero): applies an impulse to the physics body"
 *
 * body_handle: handle of the owning object, resolved with rwp_owner_object_get.
 * velocity: pointer to a 3-float vector to copy into the body's velocity
 *   slot, or NULL to use a zero vector.
 *
 * Returns nothing. Writes the velocity into the physics record at
 * desc[1] (inferred: a 32-byte per-body record in a table owned by the world)
 * and sets the body's bit in the world (rwp_world_set_bit_5c).
 */
void debris_apply_velocity_a(undefined4 body_handle,undefined4 *velocity)
{
  ushort bit_index;
  undefined4 *dst;
  char *world;
  int *desc;
  char *obj;
  struct {
    float local_c;
    float local_8;
    float local_4;
  } frame;

  frame.local_c = 0.0f;
  frame.local_8 = 0.0f;
  frame.local_4 = 0.0f;
  if (velocity == (undefined4 *)0x0) {
    velocity = (undefined4 *)&frame.local_c;
  }
  obj = rwp_owner_object_get(body_handle);
  world = *(char **)(obj + 0x24);
  /* If the world exists and the object's bit is already set in the world's
     flag table (inferred), skip the FUN_0056b300 call. */
  if (world != 0) {
    if ((*(uint *)(*(char **)(world + 0x60) + (uint)((bit_index = *(ushort *)(obj + 0x20)) >> 5) * 4) & 1 << (bit_index & 0x1f)) != 0) goto LAB_004248e8;
  }
  FUN_0056b300(world,obj);
LAB_004248e8:
  /* Find the destination: desc is obj->+0x10->+4; dst is the record at
     desc[1] * 0x20 bytes past a base pointer read through desc[0] (inferred). */
  desc = *(int **)(*(char **)(obj + 0x10) + 4);
  dst = (undefined4 *)(*(char **)(*(char **)(*desc + 0x10) + 8) + 0x10 + desc[1] * 0x20);
  /* Copy the three velocity components into the record. */
  *(RwV3d *)dst = *(RwV3d *)velocity;
  /* Set bit 1 for this object in the world (meaning of the bit: inferred). */
  rwp_world_set_bit_5c(*(undefined4 *)(obj + 0x24),obj,1);
  return;
}
