/* compile: /G5 */
#include "mfl_tier2.h"
/*00947ae8*/ extern int g_pvscalculatesectorbounds_947ae8;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sector plugin constructor for PVS data (id -1, 3 strides, rest cleared); returns the object. */
void *PVSSectorConstructor(void *o)
{
  char *e = (char *)o + g_pvscalculatesectorbounds_947ae8;
  FLD(e, int, 0) = -1;
  FLD(e, int, 0x10) = 0;
  FLD(e, int, 0x14) = 3;
  FLD(e, int, 8) = 0;
  FLD(e, int, 0xc) = 0;
  FLD(e, int, 4) = 0;
  FLD(e, int, 0x18) = 0;
  return o;
}
