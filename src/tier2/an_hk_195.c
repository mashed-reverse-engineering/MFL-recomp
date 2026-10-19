#include "mfl_tier2.h"
/* FUN_004b1140 */

/* Walks a zero-terminated 16-bit (wchar_t-style) string and counts its characters
 * (inferred). wstr: the string. The count is left in len and is not returned:
 * the function returns void. */
void __stdcall FUN_004b1140(char *wstr)

{
  int len;

  /* scan forward until the 0 code unit; len ends as the index of the terminator (inferred) */
  len = -1;
  do {
    len = len + 1;
  } while (*(short *)(wstr + len * 2) != 0);
  return;
}


