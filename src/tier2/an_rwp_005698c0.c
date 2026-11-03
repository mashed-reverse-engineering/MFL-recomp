#include "mfl_tier2.h"

/* RwCameraCalcPixelRay: the world-space ray (line = start xyz, end xyz) through pixel `pix` of the camera's raster.  u = x / width, v = y / height
   interpolate bilinearly between the four corners of the near frustum face (camera + 0x124, 12 bytes each) for the start and of the far face
   (camera + 0x154) for the end.  The volatile floats are the frame spills of the original (every spilled partial result is rounded to float).
   Returns the camera (this build leaves cam in eax; the only caller, hudpanel_marker_calc_pixel_ray, passes it on and its caller ignores it). */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
char *RwCameraCalcPixelRay(char *cam, float *line, float *pix)
{
  int *raster = *(int **)(cam + 0x60);
  float u = pix[0] / (float)raster[3];
  float v = pix[1] / (float)raster[4];
  float *c;
  volatile float m0, m1, s0, s1, s2, d2, e2, p0, p1, p2;
  float m2;

  c = (float *)(cam + 0x124);
  e2 = c[0x20 / 4] - c[0x2c / 4];
  m0 = c[0x18 / 4] - c[0x24 / 4];
  m0 = m0 * u;
  m0 = c[0x24 / 4] + m0;
  m1 = c[0x28 / 4] + (c[0x1c / 4] - c[0x28 / 4]) * u;
  d2 = c[0x14 / 4] - c[2];
  s0 = c[0xc / 4] - c[0];
  s0 = s0 * u;
  s0 = s0 + c[0];
  s1 = c[1] + (c[0x10 / 4] - c[1]) * u;
  m2 = c[0x2c / 4] + e2 * u;
  s2 = c[2] + d2 * u;
  line[0] = (m0 - s0) * v + s0;
  p1 = (m1 - s1) * v;
  line[1] = p1 + s1;
  p2 = (m2 - s2) * v;
  line[2] = p2 + s2;

  c = (float *)(cam + 0x154);
  e2 = c[0x20 / 4] - c[0x2c / 4];
  m0 = c[0x18 / 4] - c[0x24 / 4];
  m0 = m0 * u;
  m0 = c[0x24 / 4] + m0;
  m1 = c[0x28 / 4] + (c[0x1c / 4] - c[0x28 / 4]) * u;
  d2 = c[0x14 / 4] - c[2];
  s0 = c[0xc / 4] - c[0];
  s0 = s0 * u;
  s0 = c[0] + s0;
  s1 = c[1] + (c[0x10 / 4] - c[1]) * u;
  s2 = c[2] + d2 * u;
  p0 = (m0 - s0) * v;
  line[3] = p0 + s0;
  p1 = (m1 - s1) * v;
  line[4] = p1 + s1;
  line[5] = (c[0x2c / 4] + e2 * u - s2) * v + s2;
  return cam;
}
