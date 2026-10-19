#include "mfl_tier2.h"
/*0045d620*/ extern int course_get_matrix_pos_6a291c();
/*0045e0d0*/ extern int __cdecl course_get_8bf050(undefined4 p0);
/*0045e0e0*/ extern double course_get_6a2950();
/*0047a830*/ extern int car_is_active();
/*0047b520*/ extern int car_current_matrix();
/*0047b5f0*/ extern int car_get_drift_angle(float *param_2, uint param_3);
/*0047b750*/ extern int car_get_speed();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd340*/ extern float k_f_0_5;
/*005dd40c*/ extern float k_f_1_5;
/*005dd628*/ extern float k_f_0_25;
/*005dd7f0*/ extern float k_f_30;
/* ai_hazard_target_for_car
 * Picks a ground target (x, z) for the hazard-aiming AI of car `car_index`.
 * unused_1 / unused_2 are never read here.
 * target_xz: out, [0] = x, [1] = z.
 * Returns 1 and writes target_xz when a target is found, 0 otherwise.
 * Target choice, by the distance from the car to the hazard (hazard_pos):
 *   distance > 1.5 (k_f_1_5)                  -> the hazard position
 *   distance > 0.25 (k_f_0_25), car 0 active  -> car 0's position
 *   otherwise                                 -> no target
 * Nothing happens unless course_value is in [0.5, 30] (k_f_0_5, k_f_30).
 * (was: "course 0x21 hazard: target = hazard position when closer than 1.5 else the player 0 position";
 *  the body does not check course 0x21, and the 1.5 test selects the hazard when FAR, not near)
 */

undefined4 __cdecl ai_hazard_target_for_car(char *unused_1,char *unused_2,float *target_xz,uint car_index)

{
  int car_active;
  float *hazard_pos;
  undefined4 car_speed;
  float10 course_value;
  RwReal distance_to_hazard;
  struct {
    int car_matrix;
    undefined1 speed_scratch [4];
    float drift_angle;
    RwV3d car_to_hazard;
  } scratch;

  /* course_value window gate: 0.5 <= value <= 30 (the callee is read twice, as in the original) */
  course_value = (float10)course_get_6a2950();
  if ((float10)k_f_0_5 <= course_value) {
    course_value = (float10)course_get_6a2950();
    if (course_value <= (float10)k_f_30) {
      /* car_index must be valid (inferred from the callee name) */
      car_active = course_get_8bf050(car_index);
      if (car_active != 0) {
        hazard_pos = (float *)course_get_matrix_pos_6a291c();
        /* car_speed and drift_angle are not used below; the calls are kept as they are */
        car_speed = car_get_speed(scratch.speed_scratch,car_index);
        car_get_drift_angle(&scratch.drift_angle,car_index);
        car_current_matrix(&scratch.car_matrix,car_index);
        /* car -> hazard vector (x, y, z); distance_to_hazard is its length */
        scratch.car_to_hazard.x = *hazard_pos - ((RwMatrix *)scratch.car_matrix)->pos.x;
        scratch.car_to_hazard.y = hazard_pos[1] - ((RwMatrix *)scratch.car_matrix)->pos.y;
        scratch.car_to_hazard.z = hazard_pos[2] - ((RwMatrix *)scratch.car_matrix)->pos.z;
        distance_to_hazard = RwV3dLength(&scratch.car_to_hazard);
        /* far hazard (> 1.5): aim at the hazard position */
        if (k_f_1_5 < distance_to_hazard) {
          *target_xz = *hazard_pos;
          target_xz[1] = hazard_pos[2];
          return 1;
        }
        /* mid range (> 0.25): aim at player 0's position, if car 0 is active */
        if (k_f_0_25 < distance_to_hazard) {
          car_active = car_is_active(0);
          if (car_active != 0) {
            car_current_matrix(&scratch.car_matrix,0);
            *target_xz = ((RwMatrix *)scratch.car_matrix)->pos.x;
            target_xz[1] = ((RwMatrix *)scratch.car_matrix)->pos.z;
            return 1;
          }
        }
      }
      return 0;
    }
  }
  return 0;
}


