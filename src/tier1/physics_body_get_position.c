#include "mfl_tier2.h"
/*0058c8a0*/ extern char *rwp_owner_object_get(int p0);
/*006fd498*/ extern undefined4 g_rw_6fd498;
#define MAX_CARS (16)
#define BODY_ROW_SIZE (0x20)
/* physics_body_get_position */
/* "(out, car): vec3 from the physics body of g_rw_6fd498[car], rows of 0x20 bytes" */
/*
 * Copies the position of car `car_index` into out_pos[0..2] (x, y, z).
 * out_pos: caller-owned array of 3 undefined4 words.
 * car_index: index into g_rw_6fd498; valid values are 0..MAX_CARS-1.
 * Returns 1 if the position was written, 0 if the index is out of range or
 * the car has no physics body.
 * The position is the first 3 words of a 0x20-byte row in the body's table
 * (inferred: body_ref[1] is the row index, body_ref[0] points at the table owner).
 */
undefined4 __cdecl physics_body_get_position(undefined4 *out_pos,uint car_index)
{
  int *body_ref;
  char *owner_object;
  undefined4 *body_pos;

  /* reject out-of-range car indexes */
  if (car_index < MAX_CARS) {
    /* owner object of this car's slot (inferred) */
    owner_object = rwp_owner_object_get((&g_rw_6fd498)[car_index]);
    body_ref = *(int **)(*(char **)(owner_object + 0x10) + 4);
    /* no physics body attached: report failure */
    if (body_ref != (int *)0x0) {
      /* row address = table base + row index * row size (inferred) */
      body_pos = (undefined4 *)(body_ref[1] * BODY_ROW_SIZE + *(char **)(*(char **)(*body_ref + 0x10) + 8));
      *out_pos = *body_pos;
      out_pos[1] = body_pos[1];
      out_pos[2] = body_pos[2];
      return 1;
    }
  }
  return 0;
}
