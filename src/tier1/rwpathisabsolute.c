/* compile: /G5 */
#include "mfl_tier2.h"
/* TRUE for a path starting with a backslash or a drive letter and colon. */
int rwpathisabsolute(char *p)
{
  char c = p[0];
  if (c != '\\' && !(((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) && p[1] == ':')) return 0;
  return 1;
}
