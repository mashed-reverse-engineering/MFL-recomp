#include "mfl_tier2.h"
/*0042b1d0*/ extern char *course_get_record_base();
/*005ddc28*/ extern float k_f_0;
/* player_route_progress_update */

/* "dot of the course record base vector with the car vector, stores progress at +0xAC, increments
   the wrong-way counter at +0xA8 when negative" */

void player_route_progress_update(char *player_state,char *car_state)

{
  float sum;
  float progress;
  
  /* the partial sums are float temporaries (spilled to the stack, rounded to 32 bit) */
  sum = *(float *)(course_get_record_base(*(undefined4 *)(player_state + 0x20)) + 4) * *(float *)(car_state + 0x24);
  sum = *(float *)(car_state + 0x20) * *(float *)course_get_record_base(*(undefined4 *)(player_state + 0x20)) + sum;
  progress = *(float *)(course_get_record_base(*(undefined4 *)(player_state + 0x20)) + 8) * *(float *)(car_state + 0x28) + sum;
  *(float *)(player_state + 0xac) = progress;
  if (progress < k_f_0) {
    *(int *)(player_state + 0xa8) = *(int *)(player_state + 0xa8) + 1;
    return;
  }
  *(undefined4 *)(player_state + 0xa8) = 0;
  return;
}
