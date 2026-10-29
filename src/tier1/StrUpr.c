/* compile: /G5 */
#include "mfl_tier2.h"
/* Upper-cases an ASCII string in place; returns it. */
char *StrUpr(char *s)
{
  char *p;
  if (s) for (p = s; *p; p++) if (*p >= 'a' && *p <= 'z') *p -= 0x20;
  return s;
}
