#include "mfl_tier2.h"
extern char LAB_00456f00[];
/*00496850*/ extern int trigger_sphere_register();

/* float 0.85 as raw bits (inferred from the comment below) */
#define TRIGGER_SPHERE_RADIUS_F32 (0x3f59999a)

/* spline_object_register_trigger_sphere */

/* "registers a trigger sphere at the object position (radius 0.85, callback 0x456f00)" */

/*
 * Registers a trigger sphere (radius 0.85) at the position of a spline object.
 *   spline_obj: the object the trigger belongs to; its position comes from the
 *               block reached through spline_obj->[4]->[4] (inferred)
 * Builds the trigger record on the stack and hands it to trigger_sphere_register.
 * Returns nothing.
 */
void __cdecl spline_object_register_trigger_sphere(char *spline_obj)

{
  char *pos_block;
  /* layout matches struct Trigger in ghidra_types.h (inferred) */
  struct {
    undefined4 pos_x;
    undefined4 pos_y;
    undefined4 pos_z;
    undefined4 radius;
    undefined4 type;
    char *owner;
    undefined4 flags;
    undefined1 *callback;
  } trigger;

  /* copy the object position and the trigger type (inferred from +0x38) */
  pos_block = *(char **)(*(char **)(spline_obj + 4) + 4);
  trigger.pos_x = *(undefined4 *)(pos_block + 0x40);
  trigger.pos_y = *(undefined4 *)(pos_block + 0x44);
  trigger.pos_z = *(undefined4 *)(pos_block + 0x48);
  trigger.type = *(undefined4 *)(spline_obj + 0x38);
  trigger.radius = TRIGGER_SPHERE_RADIUS_F32;
  trigger.owner = spline_obj;
  trigger.flags = 0;
  trigger.callback = &LAB_00456f00;
  trigger_sphere_register(&trigger.pos_x);
  return;
}
