#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*005e64a4*/ extern float k_f_0;

/* Per-channel blend of two RGBA byte colours: out = a + (b - a) * t with t clamped to 0..1 (the name is historic). */
void
colour_floats_to_bytes(unsigned char *out, unsigned char *a, unsigned char *b, float t)
{
  if (t < k_f_0) {
    t = k_f_0;
  } else if (t > float_1_0) {
    t = float_1_0;
  }
  out[0] = (unsigned char)(int)((float)(b[0] - a[0]) * t + (float)a[0]);
  out[1] = (unsigned char)(int)((float)(b[1] - a[1]) * t + (float)a[1]);
  out[2] = (unsigned char)(int)((float)(b[2] - a[2]) * t + (float)a[2]);
  out[3] = (unsigned char)(int)((float)(b[3] - a[3]) * t + (float)a[3]);
}
