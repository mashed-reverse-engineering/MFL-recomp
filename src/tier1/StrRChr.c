/* compile: /G5 */
#include "mfl_tier2.h"
/* Last occurrence of a character in a string, or NULL. */
char *StrRChr(char *s, char c)
{
  char *r = 0;
  char ch;
  do { ch = *s; if (ch == c) r = s; s++; } while (ch);
  return r;
}
