#include "mfl_tier2.h"
/*00544e80*/ extern RwUInt32 __cdecl RpRandom(void);
/*005de498*/ extern float k_f_4_656613en10;
float random_range(float lo, float hi)
{
  unsigned int r = RpRandom() & 0x7fffffff;
  return (hi - lo) * ((float)r * k_f_4_656613en10) + lo;
}
