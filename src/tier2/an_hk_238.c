#include "mfl_tier2.h"
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd41c*/ extern float float_5_0;
/*005ddc28*/ extern float k_f_0;

#define PATH_POINT_STRIDE (8)
#define PATH_POINT_Y_OFFSET (4)
#define PATH_POINT_COUNT_OFFSET (0x200)

/* camera_path_sample */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(path, index, t, out): interpolates between the ring buffer points of a camera path"
 *
 * Evaluates a cubic (Catmull-Rom style, inferred) segment between points index-1 .. index+2 of the
 * path. path: ring buffer of 2-float points (8 bytes each), point count at PATH_POINT_COUNT_OFFSET.
 * index: segment start point (inferred; the function first steps back one point to index-1).
 * t: position along the segment, clamped to [k_f_0, float_1_0].
 * out: receives the interpolated (x, y) pair.
 */

void camera_path_sample(char *path,int index,float t,float *out)

{
  float *prev_pt;
  float *cur_pt;
  float *next_pt;
  float *after_pt_y;
  int point_count;
  float t_clamped;
  float t_sq;

  /* step back one point and wrap a negative index to the end of the ring (inferred) */
  index = index + -1;
  if (index < 0) {
    index = index + *(int *)(path + PATH_POINT_COUNT_OFFSET);
  }
  point_count = *(int *)(path + PATH_POINT_COUNT_OFFSET);
  prev_pt = (float *)(path + index * PATH_POINT_STRIDE);
  /* gather the next points, wrapping each index past the end of the ring */
  index = index + 1;
  if (point_count <= index) {
    index = index - point_count;
  }
  cur_pt =(float *)(path + index * PATH_POINT_STRIDE);
  index = index + 1;
  if (point_count <= index) {
    index = index - point_count;
  }
  next_pt = (float *)(path + index * PATH_POINT_STRIDE);
  index = index + 1;
  if (point_count <= index) {
    index = index - point_count;
  }
  /* clamp t to [k_f_0, float_1_0]; values above 1.0 become 1.0 (inferred) */
  t_clamped = t;      /* NaN passes unchanged (original: fcomp / test ah,0x41 / jne, fcomp / test ah,5 / jp) */
  if (t > float_1_0) {
    t_clamped = float_1_0;
  }
  else if (t < k_f_0) {
    t_clamped = k_f_0;
  }
  t_sq = t_clamped * t_clamped;
  after_pt_y = (float *)(path + PATH_POINT_Y_OFFSET + index * PATH_POINT_STRIDE);
  /* cubic blend of the four points, scaled by k_f_0_5 (0.5 inferred): x component (out[0]) */
  *out = (*cur_pt + *cur_pt +
             (*next_pt - *prev_pt) * t_clamped +
             (((*cur_pt * k_f_3 - *prev_pt) - *next_pt * k_f_3) + *(float *)(path + index * PATH_POINT_STRIDE)) *
             t_sq * t_clamped +
             ((*next_pt * k_f_4 + ((*prev_pt + *prev_pt) - *cur_pt * float_5_0)) -
             *(float *)(path + index * PATH_POINT_STRIDE)) * t_sq) * k_f_0_5;
  /* same blend for the y component (out[1]) */
  out[1] = (cur_pt[1] + cur_pt[1] +
               (next_pt[1] - prev_pt[1]) * t_clamped +
               (((cur_pt[1] * k_f_3 - prev_pt[1]) - next_pt[1] * k_f_3) + *after_pt_y) * t_sq * t_clamped +
               ((next_pt[1] * k_f_4 + ((prev_pt[1] + prev_pt[1]) - cur_pt[1] * float_5_0)) - *after_pt_y)
               * t_sq) * k_f_0_5;
  return;
}


