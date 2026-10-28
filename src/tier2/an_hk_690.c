#include "mfl_tier2.h"
/*0042a440*/ extern char *course_get_directional_light();
/*00456740*/ extern int trigger_register_object_type8();
/*004567b0*/ extern int course_rotating_object_integrate();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005dd40c*/ extern float k_f_1_5;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005de490*/ extern float k_f_0_02;
/*005df4d0*/ extern float k_f_3_5;
/*005df568*/ extern float k_f_0_0185;
extern double cos(double);
extern double sin(double);

/* obj[] slots (int indices into the object; float slots are read through (float *)&obj[N]) */
#define PARENT_SLOT (2)
#define YAW_SLOT (8)
#define DRIFT_X_SLOT (9)
#define DRIFT_Z_SLOT (10)
#define SPIN_PHASE_SLOT (0xb)
#define DRIFT_PHASE_SLOT (0xc)
#define ATTACH_A_SLOT (0xd)
#define ATTACH_B_SLOT (0x2a)
#define SHADE_A_SLOT (0x42)
#define SHADE_B_SLOT (0x45)

#define DRIFT_AMPLITUDE (0.3499999940395355)
#define YAW_AMPLITUDE (0.15000000596046448)
#define ATTACH_A_HEIGHT (3.52)
#define ATTACH_B_HEIGHT (3.51)

/* course_rotating_object_update */

/* "(object, ticks): animates a rotating object (sin / cos offsets, frame rotate) and re-registers
   its trigger" */

/* Per-tick update of a rotating course object. obj is the object as an int slot array (see the
   defines above). ticks is the frame counter; negative values are read as unsigned (+2^32).
   Effects: the yaw and drift slots wobble, the spin phase advances, the object's frame is rotated
   about its own position, the trigger is re-registered and integrated, two attach matrices are
   rebuilt, and a light-facing shade value is stored. Returns nothing.
   (inferred: ticks are 50 Hz, so time_s = ticks / 50 is seconds; the slot meanings are from usage) */

void course_rotating_object_update(int *obj,int ticks)

{
  /* time_s: ticks as seconds; reused below for the shade value */
  float time_s;
  /* ltm: scratch pointer to a frame's LTM, reused for the parent and own frames */
  RwMatrix *ltm;
  /* frame_ptr: the object's RwFrame, then the directional light's RwFrame (inferred) */
  char *frame_ptr;
  /* sin_phase / cos_phase: trig terms used for the wobble */
  double sin_phase;
  double cos_phase;
  /* scratch: temporary vectors. up_x/y/z hold the up vector (0, 1.0f, 0) as raw int bits */
  struct {
    RwV3d pivot_shift;
    RwV3d at_dir;
    RwV3d right_dir;
    int up_x;
    int up_y;
    int up_z;
    RwV3d ltm_pos;
    RwV3d up_axis;
  } scratch;

  /* Convert ticks to seconds (time_s = ticks / 50, see the header note) */
  time_s = (float)ticks;
  if (ticks < 0) {
    time_s = time_s + k_f_4_2949673e9;
  }
  time_s = time_s * k_f_0_02;
  /* Rotation axis for the yaw below: straight up */
  scratch.up_axis.x = 0.0;
  scratch.up_axis.y = 1.0;
  scratch.up_axis.z = 0.0;
  /* Wobble: drift_x and drift_z accumulate small sin/cos steps, yaw is a sin term, spin phase advances with time */
  sin_phase = (double)sin((double)*(float *)&obj[SPIN_PHASE_SLOT]);
  *(float *)&obj[DRIFT_X_SLOT] = (float)((double)(DRIFT_AMPLITUDE) * sin_phase + (double)*(float *)&obj[DRIFT_X_SLOT]);
  cos_phase = (double)cos((double)*(float *)&obj[DRIFT_PHASE_SLOT]);
  *(float *)&obj[DRIFT_Z_SLOT] = (float)(cos_phase * (double)(DRIFT_AMPLITUDE) + (double)*(float *)&obj[DRIFT_Z_SLOT]);
  *(float *)&obj[YAW_SLOT] = (float)(sin_phase * (double)(YAW_AMPLITUDE));
  *(float *)&obj[SPIN_PHASE_SLOT] = (time_s * k_f_0_0185 + *(float *)&obj[SPIN_PHASE_SLOT]);
  /* Rotate the object's frame about its own position: shift by -pos (+0x40..0x48 is
     frame->modelling.pos), rotate about up_axis by the yaw, then shift back by +pos */
  frame_ptr = *(char **)(*obj + 4);
  scratch.pivot_shift.x = *(float *)(frame_ptr + 0x40) * k_f_n1;
  scratch.pivot_shift.y = *(float *)(frame_ptr + 0x44) * k_f_n1;
  scratch.pivot_shift.z = *(float *)(frame_ptr + 0x48) * k_f_n1;
  RwFrameTranslate(*(RwFrame **)(*obj + 4),&scratch.pivot_shift,rwCOMBINEPOSTCONCAT);
  RwFrameRotate(*(RwFrame **)(*obj + 4),&scratch.up_axis,*(float *)&obj[YAW_SLOT],rwCOMBINEPOSTCONCAT);
  scratch.pivot_shift.x = scratch.pivot_shift.x * k_f_n1;
  scratch.pivot_shift.y = scratch.pivot_shift.y * k_f_n1;
  scratch.pivot_shift.z = scratch.pivot_shift.z * k_f_n1;
  RwFrameTranslate(*(RwFrame **)(*obj + 4),&scratch.pivot_shift,rwCOMBINEPOSTCONCAT);
  /* Re-register the trigger; integrate gets the float bits of time_s */
  trigger_register_object_type8(obj);
  course_rotating_object_integrate(obj,*(int *)&time_s);
  /* Attach matrix A: at height ATTACH_A_HEIGHT, using the PARENT_SLOT frame's position and
     at/right, with at and right flattened to y = 0 and normalized (inferred) */
  scratch.up_x = 0;
  scratch.up_y = FLOAT_ONE_BITS;
  scratch.up_z = 0;
  ltm = RwFrameGetLTM(*(RwFrame **)(obj[PARENT_SLOT] + 4));
  scratch.ltm_pos.x = (ltm->pos).x;
  scratch.ltm_pos.y = (ltm->pos).y;
  scratch.ltm_pos.z = (ltm->pos).z;
  ltm = RwFrameGetLTM(*(RwFrame **)(obj[PARENT_SLOT] + 4));
  scratch.at_dir.x = (ltm->at).x;
  scratch.at_dir.y = (ltm->at).y;
  scratch.at_dir.z = (ltm->at).z;
  ltm = RwFrameGetLTM(*(RwFrame **)(obj[PARENT_SLOT] + 4));
  scratch.right_dir.x = (ltm->right).x;
  scratch.right_dir.z = (ltm->right).z;
  scratch.at_dir.y = 0.0;
  scratch.right_dir.y = 0.0;
  RwV3dNormalize(&scratch.at_dir,&scratch.at_dir);
  RwV3dNormalize(&scratch.right_dir,&scratch.right_dir);
  scratch.ltm_pos.y = ATTACH_A_HEIGHT;
  RwMatrixTranslate((RwMatrix *)(obj + ATTACH_A_SLOT),&scratch.ltm_pos,rwCOMBINEREPLACE);
  *(float *)&obj[0x15] = scratch.at_dir.x;
  *(float *)&obj[0x16] = scratch.at_dir.y;
  *(float *)&obj[0x17] = scratch.at_dir.z;
  obj[0x11] = scratch.up_x;
  obj[0x12] = scratch.up_y;
  obj[0x13] = scratch.up_z;
  (((RwMatrix *)(obj + ATTACH_A_SLOT))->right).x = scratch.right_dir.x;
  *(float *)&obj[0xe] = scratch.right_dir.y;
  *(float *)&obj[0xf] = scratch.right_dir.z;
  /* Attach matrix B: same as A, but from the object's own frame at height ATTACH_B_HEIGHT */
  scratch.up_x = 0;
  scratch.up_y = FLOAT_ONE_BITS;
  scratch.up_z = 0;
  ltm = RwFrameGetLTM(*(RwFrame **)(*obj + 4));
  scratch.ltm_pos.x = (ltm->pos).x;
  scratch.ltm_pos.y = (ltm->pos).y;
  scratch.ltm_pos.z = (ltm->pos).z;
  ltm = RwFrameGetLTM(*(RwFrame **)(*obj + 4));
  scratch.at_dir.x = (ltm->at).x;
  scratch.at_dir.y = (ltm->at).y;
  scratch.at_dir.z = (ltm->at).z;
  ltm = RwFrameGetLTM(*(RwFrame **)(*obj + 4));
  scratch.right_dir.x = (ltm->right).x;
  scratch.right_dir.z = (ltm->right).z;
  scratch.at_dir.y = 0.0;
  scratch.right_dir.y = 0.0;
  RwV3dNormalize(&scratch.at_dir,&scratch.at_dir);
  RwV3dNormalize(&scratch.right_dir,&scratch.right_dir);
  scratch.ltm_pos.y = ATTACH_B_HEIGHT;
  RwMatrixTranslate((RwMatrix *)(obj + ATTACH_B_SLOT),&scratch.ltm_pos,rwCOMBINEREPLACE);
  *(float *)&obj[0x32] = scratch.at_dir.x;
  *(float *)&obj[0x33] = scratch.at_dir.y;
  *(float *)&obj[0x34] = scratch.at_dir.z;
  obj[0x2e] = scratch.up_x;
  obj[0x2f] = scratch.up_y;
  obj[0x30] = scratch.up_z;
  (((RwMatrix *)(obj + ATTACH_B_SLOT))->right).x = scratch.right_dir.x;
  *(float *)&obj[0x2b] = scratch.right_dir.y;
  *(float *)&obj[0x2c] = scratch.right_dir.z;
  /* Shade: dot(light at, at_dir) mapped from [-1, 1] to [3.5, 8.0] (inferred), stored in both shade slots */
  frame_ptr = course_get_directional_light();
  frame_ptr = *(char **)(frame_ptr + 4);
  time_s = (*(float *)(frame_ptr + 0x38) * scratch.at_dir.z +
           scratch.at_dir.x * *(float *)(frame_ptr + 0x30) +
           *(float *)(frame_ptr + 0x34) * scratch.at_dir.y + float_1_0) * k_f_1_5 * k_f_1_5 +
          k_f_3_5;
  *(float *)&obj[SHADE_A_SLOT] = time_s;
  *(float *)&obj[SHADE_B_SLOT] = time_s;
  return;
}


