/* compile: /G5 */
#include "mfl_tier2.h"
typedef void (__cdecl *V1)(void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* End-of-file query: virtual call +0x44 of the stream's file system. */
void RwFeof(void *stream)
{
  (*(V1 *)(FLD(stream, char *, 0x38) + 0x44))(stream);
}
