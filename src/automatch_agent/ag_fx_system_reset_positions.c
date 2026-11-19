#include "mfl_tier2.h"


/* fx_system_reset_positions */

/* "sets every vertex position to the given point and clears the offsets" */

void fx_system_reset_positions(int *system,struct RwV3d *point)
{
  struct RwV3d zero;
  int n;
  int off;
  zero.x = 0.0f; zero.y = 0.0f; zero.z = 0.0f;
  n = system[3];
  if (n != 0) {
    off = n * 0xc;
    do {
      off -= 0xc;
      *(struct RwV3d *)(*system + off) = *point;
      *(struct RwV3d *)(system[1] + off) = zero;
    } while (--n != 0);
  }
}
