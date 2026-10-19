#include "mfl_tier2.h"
/*00457010*/ extern undefined __cdecl spline_object_register_trigger_sphere(undefined4 p0);
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*00556360*/ extern RwReal RpSplineFindMatrix(struct RpSpline *p0, RwInt32 p1, RwReal p2, struct RwV3d *p3, RwMatrix *p4);
/*005dd330*/ extern float float_1_0;
/*005dd61c*/ extern float k_f_0_001;
/*005ddb10*/ extern float k_f_0_99;
/*005ddc28*/ extern float k_f_0;
/*005de6a8*/ extern float k_f_n0_5;

/* word indices into the object block (obj[i]) */
#define OBJ_MATRIX_TARGET (0xf)
#define OBJ_TRANSLATION (0x13)
#define OBJ_T (2)
#define OBJ_SPEED (3)
#define OBJ_END_MARGIN (4)
#define OBJ_START_T (5)
#define SPLINE_FIND_MATRIX_ARG (10)
/* spline_object_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "RpSplineFindMatrix of the spline object and sets its frame transform" */

/* Moves a spline-driven object along its spline and places its render object there.
 * obj: block of 4-byte words (layout inferred): obj[0] spline, obj[1] holds a pointer whose +4 is the RwFrame,
 *   obj[OBJ_T] spline parameter t (0..1), obj[OBJ_SPEED] speed per unit dt, obj[OBJ_END_MARGIN] (inferred) end margin
 *   (upper limit is 1 - margin), obj[OBJ_START_T] lower limit of t, obj[OBJ_MATRIX_TARGET] render object,
 *   obj[OBJ_TRANSLATION] RwV3d offset applied after the spline matrix.
 * dt: time step. Returns nothing; updates obj[OBJ_T] and obj[OBJ_SPEED]. */

void spline_object_update(undefined4 *obj,float dt)

{
  float new_t;
  float t_max;
  struct {
    RwV3d up_vector;
    RwMatrix matrix;
  } frame;

  /* up_vector = (0,1,0) (inferred: reference up direction for the spline matrix) */
  frame.up_vector.x = 0.0;
  frame.up_vector.y = 1.0;
  frame.up_vector.z = 0.0;
  RpSplineFindMatrix((RpSpline *)*obj,SPLINE_FIND_MATRIX_ARG,*(float *)&obj[OBJ_T],&frame.up_vector,&frame.matrix);
  RwFrameTransform(*(RwFrame **)(obj[1] + 4),&frame.matrix,rwCOMBINEREPLACE);
  RwMatrixTranslate(&frame.matrix,(RwV3d *)(obj + OBJ_TRANSLATION),rwCOMBINEPOSTCONCAT);
  rwp_object_set_matrix(obj[OBJ_MATRIX_TARGET],&frame.matrix);
  new_t = dt * *(float *)&obj[OBJ_SPEED] + *(float *)&obj[OBJ_T];
  *(float *)&obj[OBJ_T] = new_t;
  t_max = float_1_0 - *(float *)&obj[OBJ_END_MARGIN];
  /* t passed an end of the spline: step back, reverse and halve speed, clamp t just inside the end (ping-pong) */
  if ((new_t >= t_max) || (new_t <= *(float *)&obj[OBJ_START_T])) {
    *(float *)&obj[OBJ_T] = new_t - (*(float *)&obj[OBJ_SPEED] + *(float *)&obj[OBJ_SPEED]);
    new_t = *(float *)&obj[OBJ_SPEED] * (-0.5f);
    *(float *)&obj[OBJ_SPEED] = new_t;
    if (*(float *)&obj[OBJ_T] >= t_max) {
      if (new_t > 0.0f) {
        *(float *)&obj[OBJ_SPEED] = -new_t;
      }
      *(float *)&obj[OBJ_T] = t_max - 0.001f;
    }
    if (*(float *)&obj[OBJ_T] <= *(float *)&obj[OBJ_START_T]) {
      if (*(float *)&obj[OBJ_SPEED] < 0.0f) {
        *(float *)&obj[OBJ_SPEED] = -*(float *)&obj[OBJ_SPEED];
      }
      *(float *)&obj[OBJ_T] = *(float *)&obj[OBJ_START_T] + 0.001f;
    }
  }
  /* damp speed each frame */
  *(float *)&obj[OBJ_SPEED] = *(float *)&obj[OBJ_SPEED] * 0.99f;
  spline_object_register_trigger_sphere((undefined4)obj);
  return;
}


