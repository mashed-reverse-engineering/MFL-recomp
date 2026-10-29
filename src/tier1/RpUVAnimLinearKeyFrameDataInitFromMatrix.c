/* compile: /G5 */
#include "mfl_tier2.h"
/* Copies the 2D transform entries (0, 1, 4, 5, 12, 13) of a matrix into linear UV key frame data; returns the key frame. */
float *RpUVAnimLinearKeyFrameDataInitFromMatrix(float *d, float *m)
{
  d[0] = m[0];
  d[1] = m[1];
  d[2] = m[4];
  d[3] = m[5];
  d[4] = m[12];
  d[5] = m[13];
  return d;
}
