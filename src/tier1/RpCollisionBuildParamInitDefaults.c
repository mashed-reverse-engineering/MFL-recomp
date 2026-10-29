/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Zeroes the three collision build parameters. */
void RpCollisionBuildParamInitDefaults(void *o)
{
  FLD(o, int, 0) = 0;
  FLD(o, int, 4) = 0;
  FLD(o, int, 8) = 0;
}
