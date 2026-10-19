#include "mfl_tier2.h"
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*0045d620*/ extern int course_get_matrix_pos_6a291c();
/*0045e0d0*/ extern int __cdecl course_get_8bf050(undefined4 p0);
/*0045e0e0*/ extern double course_get_6a2950();
/*0047b520*/ extern int car_current_matrix();
/*0047b5f0*/ extern int car_get_drift_angle(float *param_2, uint param_3);
/*0047b750*/ extern int car_get_speed();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd62c*/ extern float k_f_100;
/*005dd7f0*/ extern float k_f_30;
/*005ddad4*/ extern float k_f_90;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/* ai_bot_course_0x21_target_check */
#define COURSE_WARZONE (33)
/* RwMatrix.pos x/y/z byte offsets inside the car matrix (ghidra_types.h struct RwMatrixTag) */
#define MATRIX_POS_X_OFFSET (0x30)
#define MATRIX_POS_Y_OFFSET (0x34)
#define MATRIX_POS_Z_OFFSET (0x38)

/* Course-specific AI drive check for bot `bot_index`, course COURSE_WARZONE (0x21) only.
 * Writes two flag bytes into ai_out: ai_out[4] is always 0; ai_out[5] is 0xff when the
 * car's speed is above k_f_100 and its drift angle is below k_f_90 (both thresholds inferred
 * from the names, values not checked), and 0 otherwise. Writes nothing when the course is not
 * COURSE_WARZONE, course_get_6a2950() is above k_f_30 (inferred), state_70 is 10, or
 * course_get_8bf050(bot_index) returns 0.
 * The distance from the car to the hazard position is computed (RwV3dLength) but its result
 * is not used. (was: "distance to the hazard matrix position, speed >100 and drift <90") */

void ai_bot_course_0x21_target_check(uint bot_index,char *ai_out)

{
  CourseId course_id;
  int bot_ok;
  float *course_pos;
  undefined4 speed_result;
  float10 course_metric;
  struct {
    int car_matrix_ptr;
    float speed;
    float drift_angle;
    RwV3d offset;
  } probe;

  /* Gate: right course, course metric small enough, bot not in state 10, bot_ok set */
  course_id = current_course_id();
  if ((((course_id == COURSE_WARZONE) &&
       (course_metric = (float10)course_get_6a2950(), course_metric <= (float10)k_f_30)) &&
      (g_ai_bots[bot_index].state_70 != 10)) && (bot_ok = course_get_8bf050(bot_index), bot_ok != 0)) {
    course_pos = (float *)course_get_matrix_pos_6a291c();
    speed_result = car_get_speed(&probe.speed,bot_index);
    car_get_drift_angle(&probe.drift_angle,bot_index);
    car_current_matrix(&probe.car_matrix_ptr,bot_index);
    probe.offset.x = *course_pos - *(float *)(probe.car_matrix_ptr + MATRIX_POS_X_OFFSET);
    probe.offset.y = course_pos[1] - *(float *)(probe.car_matrix_ptr + MATRIX_POS_Y_OFFSET);
    probe.offset.z = course_pos[2] - *(float *)(probe.car_matrix_ptr + MATRIX_POS_Z_OFFSET);
    RwV3dLength(&probe.offset);
    /* Fast and not drifting hard: set flag byte 5 (ai_out[5]) to 0xff */
    if ((k_f_100 < probe.speed) && (probe.drift_angle < k_f_90)) {
      *(undefined1 *)(ai_out + 4) = 0;
      *(undefined1 *)(ai_out + 5) = 0xff;
      return;
    }
    *(undefined1 *)(ai_out + 4) = 0;
    *(undefined1 *)(ai_out + 5) = 0;
  }
  return;
}


