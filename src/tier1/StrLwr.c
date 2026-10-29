/* compile: /G5 */
#include "mfl_tier2.h"
/* Lower-cases an ASCII string in place; returns it. */
char *StrLwr(char *s)
{
  char *p;
  if (s) for (p = s; *p; p++) if (*p >= 'A' && *p <= 'Z') *p += 0x20;
  return s;
}
