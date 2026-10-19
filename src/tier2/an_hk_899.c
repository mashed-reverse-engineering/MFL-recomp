#include "mfl_tier2.h"
/*0040a8d0*/ extern int course_token_list_skip_to_entry();
/*0042b1d0*/ extern int course_get_record_base();
/*004b3284*/ extern double FUN_004b3284(double p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005ddacc*/ extern float k_f_360;
/*005ddad4*/ extern float k_f_90;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005ddc20*/ extern float k_f_45;
/*005e64a4*/ extern float k_f_0;
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac6c*/ extern float g_fx_62ac6c;
/*0062ac70*/ extern float g_fx_62ac70;
#define RAD_TO_DEG (57.295799255371094) /* 180/pi as stored by the game (float rounding) */
#define WANDER_SAMPLE_COUNT (3) /* track entries sampled per target */
/* fx_wander_target */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/*
 * Heading statistics over WANDER_SAMPLE_COUNT consecutive track entries, starting at
 * token_cursor (a local copy: the caller's cursor is not changed).
 *
 * For each entry the horizontal direction (x, z) is taken from the record and its angle to
 * the reference vector (g_car_62ac68, g_fx_62ac6c, g_fx_62ac70) is computed (inferred: acos
 * of the dot product, in degrees, then 180 - angle). The angle is signed by the cross
 * product and folded into one 90 degree period (inferred: roughly -45..45).
 *
 * out_mean_angle:   average folded heading over the samples.
 * out_total_change: sum of |heading change| between consecutive samples.
 *
 * Returns 1. If a sampled direction has zero horizontal length it returns 1 early and writes
 * neither output.
 *
 * (was: "inferred: random target points from FUN_0042b1d0 normalised"; nothing here is random)
 */

undefined4 fx_wander_target(undefined4 token_cursor,float *out_mean_angle,float *out_total_change)

{
  RwV3d *rec;
  int entry_index;
  float10 angle;
  float10 dot; /* also reused as the heading change below */
  float10 cross_y;
  struct {
    float angle_sum;
    float angle_change;
    float prev_angle;
    RwV3d vec;
  } acc;

  acc.angle_change = 0.0;
  acc.angle_sum = 0.0;
  acc.prev_angle = 0.0;
  entry_index = 0;
  /* one pass per sampled track entry */
  do {
    rec = (RwV3d *)course_get_record_base(token_cursor);
    acc.vec.x = rec->x;
    acc.vec.y = 0.0;
    acc.vec.z = rec->z;
    /* no horizontal direction: no heading, leave the outputs alone */
    if ((acc.vec.x == k_f_0) && (acc.vec.z == k_f_0)) {
      return 1;
    }
    RwV3dNormalize(&acc.vec,&acc.vec);
    dot = (float10)acc.vec.y * g_fx_62ac6c + acc.vec.x * g_car_62ac68 + acc.vec.z * g_fx_62ac70;
    /* clamp to [-1, 1] before acos */
    if (dot < k_f_n1) {
      dot = k_f_n1;
    }
    else if (float_1_0 < dot) {
      dot = float_1_0;
    }
    /* FUN_004b3284: acos (inferred); result in radians, converted to degrees */
    angle = (float10)FUN_004b3284((double)dot);
    angle = -(angle * (float10)(RAD_TO_DEG) - (float10)(90.0)) + (float10)k_f_90;
    /* cross product of the record's full vector with the reference; only its y part is kept */
    acc.vec.x = g_fx_62ac70 * rec->y - g_fx_62ac6c * rec->z;
    cross_y = (float10)g_car_62ac68 * rec->z - g_fx_62ac70 * rec->x;
    acc.vec.y = (float)cross_y;
    acc.vec.z = g_fx_62ac6c * rec->x - g_car_62ac68 * rec->y;
    /* negative cross_y: the heading is on the other side, go round the full circle */
    if (cross_y < k_f_0) {
      angle = (float10)k_f_360 - angle;
    }
    /* fold the heading into one 90 degree period (inferred: roughly -45..45) */
    if ((float10)k_f_90 < angle) {
      do {
        angle = angle - (float10)k_f_90;
      } while ((float10)k_f_90 < angle);
    }
    if ((float10)k_f_45 < angle) {
      angle = angle - (float10)k_f_90;
    }
    acc.angle_sum = (float)((float10)acc.angle_sum + angle);
    /* change from the previous sample; the first sample has no previous one */
    if (entry_index != 0) {
      dot = (float10)acc.prev_angle - angle;
      if (dot < k_f_0) {
        dot = -dot;
      }
      acc.angle_change = (float)(dot + (float10)acc.angle_change);
    }
    acc.prev_angle = (float)angle;
    token_cursor = course_token_list_skip_to_entry(token_cursor, 0x0);
    entry_index = entry_index + 1;
  } while (entry_index < WANDER_SAMPLE_COUNT);
  /* outputs: mean heading (sum / 3) and the total change between samples */
  *out_mean_angle = acc.angle_sum * k_f_0_33333334;
  *out_total_change = acc.angle_change;
  return 1;
}


