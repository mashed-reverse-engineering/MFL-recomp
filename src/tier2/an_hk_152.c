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
  char *cursor;

  ch = *path;
  cursor = path;
  /* Walk forward to the terminating NUL; cursor ends on it. */
  while (ch != '\0') {
    next = cursor + 1;
    cursor = cursor + 1;
    ch = *next;
  }
  ch = *cursor;
  /* Walk backward from the NUL looking for the last '.'; stop when cursor reaches path. */
  while( true ) {
    if (ch == '.') {
      return cursor + 1;
    }
    cursor = cursor + -1;
    if (cursor == path) break;
    ch = *cursor;
  }
  return path;
}


