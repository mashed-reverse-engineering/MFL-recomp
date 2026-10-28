#include "mfl_tier2.h"
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, void *p1, int p2);
/*0056b300*/ extern void FUN_0056b300(void *p0, void *p1);
/*0056bc20*/ extern void rwp_body_add_force_at_point(void *p0, void *p1, undefined4 p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/* debris_apply_velocity_b */

/* "(body, vec3 or zero, flag): same with extra param" */

/* (inferred names) body = physics body handle, velocity = vec3 or NULL for zero, matrix = frame matrix or 0 for the owner's default; unused_N are the trailing
   stack parameters the original declares but never reads. */
void __cdecl
debris_apply_velocity_b
          (undefined4 body,undefined4 *velocity,int matrix,char *unused_4,char *unused_5,
          char *unused_6,char *unused_7,char *unused_8,char *unused_9,
          char *unused_10,char *unused_11,char *unused_12,char *unused_13,
          char *unused_14,char *unused_15,char *unused_16,char *unused_17)

{
  char *world;
  int *link;
  char *owner;
  struct {
    undefined4 zero_x;
    undefined4 zero_y;
    undefined4 zero_z;
  } frame;
  
  frame.zero_x = 0;
  frame.zero_y = 0;
  frame.zero_z = 0;
  if (velocity == (undefined4 *)0x0) {
    velocity = &frame.zero_x;
  }
  owner = rwp_owner_object_get(body);
  world = *(char **)(owner + 0x24);
  if (world != 0) {
    if ((*(uint *)(*(char **)(world + 0x60) + (uint)(*(ushort *)(owner + 0x20) >> 5) * 4) &
        1 << ((byte)*(ushort *)(owner + 0x20) & 0x1f)) != 0) goto LAB_00424998;
  }
  FUN_0056b300(world,owner);
LAB_00424998:
  link = *(int **)(*(char **)(owner + 0x10) + 4);
  if (matrix == 0) {
    matrix = **(int **)(*link + 0x10) + 0x30 + link[1] * 0x40;
  }
  rwp_body_add_force_at_point(link,velocity,matrix);
  rwp_world_set_bit_5c(*(undefined4 *)(owner + 0x24),owner,1);
  return;
}


