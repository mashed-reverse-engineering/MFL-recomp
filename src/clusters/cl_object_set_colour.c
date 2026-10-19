// cluster-entries: object_set_colour
/* Installed cluster object_set_colour (tools/cluster_install.py from output/cluster_pilot_zy/object_set_colour.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c5410*/ extern void FUN_004c5410(int handle, int flags, int enable);
/*004c5380*/ extern void FUN_004c5380(int handle, unsigned char *rgba);

/* Sets the colour (r,g,b,a as 0..1 floats, stored as 4 bytes) on the object's material handle at +0x184. `unused` is never read. */
void object_set_colour(int object, int unused, float r, float g, float b, float a)
{
  unsigned char rgba[4];

  rgba[0] = (unsigned char)(int)(r * 255.0f);
  rgba[1] = (unsigned char)(int)(g * 255.0f);
  rgba[2] = (unsigned char)(int)(b * 255.0f);
  rgba[3] = (unsigned char)(int)(a * 255.0f);
  FUN_004c5410(*(int *)(object + 0x184), 0x40, 1);
  FUN_004c5380(*(int *)(object + 0x184), rgba);
}

