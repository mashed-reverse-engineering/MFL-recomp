#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
#define ROT_ANGLE_Z_OFS (0x24)
#define ROT_ANGLE_X_OFS (0x28)
#define ROT_VEL_Z_OFS (0x14)
#define ROT_VEL_X_OFS (0x1c)
/* course_rotating_object_integrate */
/* "rotates the frame about x/z by the stored angles and integrates damped angular velocities" */
/* Per-frame update of a rotating course object (inferred from the body).
 *   rot_obj: object whose +4 pointer leads to the RwFrame that is rotated.
 *            Offsets used: +0x14 angular velocity z (inferred), +0x1c angular
 *            velocity x (inferred), +0x24 angle z (inferred), +0x28 angle x
 *            (inferred), +0x18 a value that only decays here (not used by this function).
 *   dt:      time step.
 * Returns nothing; the frame is re-posed and the angles/velocities are updated in place.
 */
void course_rotating_object_integrate(char *rot_obj,float dt)
{
  RwFrame *frame;
  float new_angle_z;
  float new_angle_x;
  RwV3d origin;
  RwV3d x_axis;
  RwV3d z_axis;
  RwV3d saved_pos;

  x_axis.x = 1.0;
  x_axis.y = 0.0;
  x_axis.z = 0.0;
  z_axis.x = 0.0;
  z_axis.y = 0.0;
  z_axis.z = 1.0;
  origin.x = 0.0;
  origin.y = 0.0;
  origin.z = 0.0;
  frame = *(RwFrame **)(*(char **)(rot_obj + 4) + 4);
  /* remember the frame position, rotate about the origin, then move it back */
  saved_pos = (RwFrame_modelling(frame)).pos;
  RwFrameTranslate(frame,&origin,rwCOMBINEREPLACE);
  RwFrameRotate(frame,&x_axis,*(RwReal *)(rot_obj + ROT_ANGLE_X_OFS),rwCOMBINEPOSTCONCAT);
  RwFrameRotate(frame,&z_axis,-*(float *)(rot_obj + ROT_ANGLE_Z_OFS),rwCOMBINEPOSTCONCAT);
  RwFrameTranslate(frame,&saved_pos,rwCOMBINEPOSTCONCAT);
  /* integrate the angles (explicit Euler), then damp the velocities (inferred) */
  new_angle_z = dt * *(float *)(rot_obj + ROT_VEL_Z_OFS) + *(float *)(rot_obj + ROT_ANGLE_Z_OFS);
  *(float *)(rot_obj + ROT_ANGLE_Z_OFS) = new_angle_z;
  new_angle_x = dt * *(float *)(rot_obj + ROT_VEL_X_OFS) + *(float *)(rot_obj + ROT_ANGLE_X_OFS);
  *(float *)(rot_obj + ROT_ANGLE_X_OFS) = new_angle_x;
  *(float *)(rot_obj + 0x18) = *(float *)(rot_obj + 0x18) * 0.99f;
  *(float *)(rot_obj + ROT_VEL_Z_OFS) = *(float *)(rot_obj + ROT_VEL_Z_OFS) * 0.99f - new_angle_z * dt * 0.001f;
  *(float *)(rot_obj + ROT_VEL_X_OFS) = *(float *)(rot_obj + ROT_VEL_X_OFS) * 0.99f - new_angle_x * dt * 0.001f;
  return;
}
