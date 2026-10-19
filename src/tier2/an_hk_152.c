#include "mfl_tier2.h"
/* FUN_005bf940 */

/* Returns a pointer to the file extension in `path`: the part after its last '.' (inferred).
   If no '.' is found, returns `path` itself. Note: path[0] is never examined, so a name
   whose only '.' is the first character also returns `path`. Empty input is not handled;
   the backward scan runs past the start of the buffer. */
char * FUN_005bf940(char *path)
{
  char *next;
  char ch;
  char *start;

  start = path;
  ch = *path;
  while (ch != '\0') {
    next = path + 1;
    path = path + 1;
    ch = *next;
  }
  ch = *path;
  while( true ) {
    if (ch == '.') {
      return path + 1;
    }
    path = path + -1;
    if (path == start) break;
    ch = *path;
  }
  return start;
}


