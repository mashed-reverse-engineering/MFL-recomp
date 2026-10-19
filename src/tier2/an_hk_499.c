#include "mfl_tier2.h"
/*00405ab0*/ extern int vec3_multiply_components();
/* aabb_max_axis_distance_to_point */

/* "(box min/max, point): per axis distance to the nearer side, vec3_multiply_components squares,
   returns the max" */

float10 __cdecl
aabb_max_axis_distance_to_point
          (float *param_1,float *param_2,char *param_3,char *param_4,char *param_5,
          char *param_6,char *param_7,char *param_8,char *param_9,
          char *param_10,char *param_11,char *param_12)

{
  struct {
    float local_18;
    float local_14;
    float local_10;
    float local_c;
    float local_8;
    float local_4;
  } frame;
  
  frame.local_18 = *param_2 - *param_1;
  frame.local_14 = param_2[1] - param_1[1];
  frame.local_10 = param_2[2] - param_1[2];
  frame.local_c = *param_2 - param_1[3];
  frame.local_8 = param_2[1] - param_1[4];
  frame.local_4 = param_2[2] - param_1[5];
  vec3_multiply_components(&frame.local_18,&frame.local_18,&frame.local_18);
  vec3_multiply_components(&frame.local_c,&frame.local_c,&frame.local_c);
  if (frame.local_18 < frame.local_c) {
    frame.local_18 = frame.local_c;
  }
  if (frame.local_14 < frame.local_8) {
    frame.local_14 = frame.local_8;
  }
  if (frame.local_10 < frame.local_4) {
    frame.local_10 = frame.local_4;
  }
  return (float10)frame.local_10 + (float10)frame.local_14 + (float10)frame.local_18;
}


