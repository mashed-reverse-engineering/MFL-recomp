#include "mfl_tier2.h"
extern char LAB_004565b0[];
/*00496850*/ extern int trigger_sphere_register();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
#define TRIGGER_SPHERE_RADIUS_BITS (0x3f59999a) /* float 0.85f bit pattern (inferred) */
#define TRIGGER_TYPE_OBJECT (8) /* type 8 trigger sphere (from the description) */
/* trigger_register_object_type8 */

/* "(object): registers a type 8 trigger sphere at the object's frame position"
 * object: pointer to the object (passed as int); its frame's LTM gives the sphere centre.
 * Returns nothing; the sphere is handed to trigger_sphere_register. */

void trigger_register_object_type8(int object)

{
  RwMatrix *frame_ltm;
  /* trigger sphere record: centre x/y/z, radius (float bits), type, owner object, reserved, extra data */
  struct {
    RwReal pos_x;
    RwReal pos_y;
    RwReal pos_z;
    undefined4 radius;
    undefined4 trigger_type;
    int object;
    undefined4 reserved;
    undefined1 *extra_data;
  } frame;

  /* object->(+8 pointer)->(+4 field) is the object's frame; take its LTM (the world transform) */
  frame_ltm = RwFrameGetLTM(*(RwFrame **)(*(char **)(object + 8) + 4));
  /* sphere centre = translation part of the frame's LTM */
  frame.pos_x = (frame_ltm->pos).x;
  frame.pos_y = (frame_ltm->pos).y;
  frame.pos_z = (frame_ltm->pos).z;
  /* fixed radius and type 8 for this trigger kind */
  frame.radius = TRIGGER_SPHERE_RADIUS_BITS;
  frame.trigger_type = TRIGGER_TYPE_OBJECT;
  frame.object = object;
  frame.reserved = 0;
  /* static data block of the trigger (global LAB_004565b0, contents unknown) */
  frame.extra_data = &LAB_004565b0;
  /* hand the sphere record to the trigger system */
  trigger_sphere_register(&frame.pos_x);
  return;
}


