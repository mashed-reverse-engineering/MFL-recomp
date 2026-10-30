/* compile: /G5 */
/* Grow an axis-aligned bounds (max xyz at [0..2], min xyz at [4..6]) so that it contains the point. */
void rwp_math_0056f900(float *bounds, float *point)
{
  if (bounds[4] > point[0]) {
    bounds[4] = point[0];
  } else {
    if (bounds[0] < point[0]) {
      bounds[0] = point[0];
    }
  }
  if (bounds[5] > point[1]) {
    bounds[5] = point[1];
  } else {
    if (bounds[1] < point[1]) {
      bounds[1] = point[1];
    }
  }
  if (bounds[6] > point[2]) {
    bounds[6] = point[2];
    return;
  }
  if (bounds[2] < point[2]) {
    bounds[2] = point[2];
  }
}
