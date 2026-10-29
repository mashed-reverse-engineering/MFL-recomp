/* compile: /G5 */
#include "mfl_tier2.h"
/* strchr: first occurrence of c (also the terminator) or NULL. */
char *StrChr(char *s, char c)
{
  char *r = 0;
  char ch;
  do { ch = *s; if (ch == c) { r = s; break; } s++; } while (ch);
  return r;
}
