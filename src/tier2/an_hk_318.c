#include "mfl_tier2.h"
/*0058c8a0*/ extern char *rwp_owner_object_get(int p0);
/*005de204*/ extern float k_f_0_0027777778;
/*006fd498*/ extern undefined4 g_rw_6fd498;
/* car_add_velocity_impl */

/* "adds velocity*(1/360) to the body linear velocity" */

/* Adds velocity_delta * (1/360) to the linear velocity of the car's physics body.
 * velocity_delta: float[3] (x, y, z) velocity increment, in the same units as the body velocity.
 * player_index: index into g_rw_6fd498; selects the owner object (car) whose body is updated.
 * Returns nothing. */
void __cdecl car_add_velocity_impl(float *velocity_delta,int player_index)

{
  int *body_info;
  float scaled_y;
  float scaled_z;
  char *owner_object;
  float *body_velocity;

  owner_object = rwp_owner_object_get((&g_rw_6fd498)[player_index]);
  body_info = *(int **)(*(char **)(owner_object + 0x10) + 4);
  body_velocity = (float *)(body_info[1] * 0x20 + *(char **)(*(char **)(*body_info + 0x10) + 8)); /* (inferred) body velocity vector inside the body's array */
  /* scale the y and z components by 1/360 before the adds below */
  scaled_y = velocity_delta[1] * k_f_0_0027777778;
  scaled_z = velocity_delta[2] * k_f_0_0027777778;
  /* x, y, z: body velocity += velocity_delta * (1/360) */
  *body_velocity = *velocity_delta * k_f_0_0027777778 + *body_velocity;
  body_velocity[1] = scaled_y + body_velocity[1];
  body_velocity[2] = scaled_z + body_velocity[2];
  return;
}


