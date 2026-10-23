#include "mfl_tier2.h"
/*0042b1d0*/ extern char *course_get_record_base();
/*005ddc28*/ extern float k_f_0;
/* player_route_progress_update */

/* "dot of the course record base vector with the car vector, stores progress at +0xAC, increments
   the wrong-way counter at +0xA8 when negative" */

void player_route_progress_update(char *param_1,char *param_2)

{
  float sum;
  float progress;
  
  /* the partial sums are float temporaries (spilled to the stack, rounded to 32 bit) */
  sum = *(float *)(course_get_record_base(*(undefined4 *)(param_1 + 0x20)) + 4) * *(float *)(param_2 + 0x24);
  sum = *(float *)(param_2 + 0x20) * *(float *)course_get_record_base(*(undefined4 *)(param_1 + 0x20)) + sum;
  progress = *(float *)(course_get_record_base(*(undefined4 *)(param_1 + 0x20)) + 8) * *(float *)(param_2 + 0x28) + sum;
  *(float *)(param_1 + 0xac) = progress;
  if (progress < k_f_0) {
    *(int *)(param_1 + 0xa8) = *(int *)(param_1 + 0xa8) + 1;
    return;
  }
  *(undefined4 *)(param_1 + 0xa8) = 0;
  return;
}
