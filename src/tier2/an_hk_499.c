#include "mfl_tier2.h"
/*00405ab0*/ extern int vec3_multiply_components();
/* aabb_max_axis_distance_to_point */

/* "(box min/max, point): per axis distance to the nearer side, vec3_multiply_components squares,
   returns the max" */

/* (inferred) box = {min xyz, max xyz}; min_d* / max_d* are the per axis (squared) distances to the min / max side */
double __cdecl
aabb_max_axis_distance_to_point
          (float *box,float *point,char *unused_3,char *unused_4,char *unused_5,
          char *unused_6,char *unused_7,char *unused_8,char *unused_9,
          char *unused_10,char *unused_11,char *unused_12)

{
  struct {
    float min_dx;
    float min_dy;
    float min_dz;
    float max_dx;
    float max_dy;
    float max_dz;
  } frame;
  
  frame.min_dx = *point - *box;
  frame.min_dy = point[1] - box[1];
  frame.min_dz = point[2] - box[2];
  frame.max_dx = *point - box[3];
  frame.max_dy = point[1] - box[4];
  frame.max_dz = point[2] - box[5];
  vec3_multiply_components(&frame.min_dx,&frame.min_dx,&frame.min_dx);
  vec3_multiply_components(&frame.max_dx,&frame.max_dx,&frame.max_dx);
  if (frame.min_dx < frame.max_dx) {
    frame.min_dx = frame.max_dx;
  }
  if (frame.min_dy < frame.max_dy) {
    frame.min_dy = frame.max_dy;
  }
  if (frame.min_dz < frame.max_dz) {
    frame.min_dz = frame.max_dz;
  }
  return (double)frame.min_dz + (double)frame.min_dy + (double)frame.min_dx;
}


