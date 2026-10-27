#include "mfl_tier2.h"
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*00555ff0*/ extern struct RwV3d * RpSplineFindPosition(struct RpSpline *p0, RwInt32 p1, RwReal p2, struct RwV3d *p3, struct RwV3d *p4);
/*005dd330*/ extern float float_1_0;
/*005dd61c*/ extern float k_f_0_001;
/* spline_get_length */
/* "samples the RpSpline at 0.001 steps and sums the segment length" */
/* Returns the approximate arc length of `spline` (inferred): the sum of straight-line distances between consecutive samples along the curve.
   spline - curve to measure. Returns the total length as double. */
double spline_get_length(RpSpline *spline)
{
  RwReal segment_length;
  float sample_t;
  float total_length;
  RwV3d pos_cur;
  RwV3d pos_prev;
  RwV3d delta;
  RwV3d scratch_vec; /* second output of RpSplineFindPosition, never read (inferred) */

  total_length = 0.0;
  sample_t = 0.001;
  /* One step per iteration: distance between the points at sample_t - 0.001 and sample_t.
     Loop runs while sample_t <= 1.0 (written as two float compares; same meaning). */
  do {
    RpSplineFindPosition(spline,10,sample_t - k_f_0_001,&pos_prev,&scratch_vec);
    RpSplineFindPosition(spline,10,sample_t,&pos_cur,&scratch_vec);
    delta.x = pos_cur.x - pos_prev.x;
    delta.y = pos_cur.y - pos_prev.y;
    delta.z = pos_cur.z - pos_prev.z;
    segment_length = RwV3dLength(&delta);
    total_length = segment_length + total_length;
    sample_t = sample_t + 0.001f;
  } while (sample_t <= float_1_0);
  return (double)total_length;
}
