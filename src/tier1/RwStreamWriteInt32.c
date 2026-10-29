/* compile: /G5 */
#include "mfl_tier2.h"
/*004dbfc0*/ extern void *__cdecl RwStreamWrite(void *s, void *p, int n);
/* Writes n bytes to a stream (RwStreamWrite); returns the stream. */
void *RwStreamWriteInt32(void *s, void *p, int n)
{
  RwStreamWrite(s, p, n);
  return s;
}
