/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* End-of-file query: virtual call +0x44 of the stream's file system. Returns the query's result; 0x4dbebd tests it.
   VOID_w16: cl 13 schedules the tail call differently when the result is returned (vtable load before the argument store,
   tried 8 forms), so the byte-matching cl 13 build keeps the void form, which leaves the callee's EAX in place exactly
   like the original; every other compiler gets the real return. */
#if defined(_MSC_VER) && _MSC_VER < 1310
typedef void (__cdecl *V1)(void *);
void RwFeof(void *stream)
{
  (*(V1 *)(FLD(stream, char *, 0x38) + 0x44))(stream);
}
#else
typedef int (__cdecl *V1)(void *);
int RwFeof(void *stream)
{
  return (*(V1 *)(FLD(stream, char *, 0x38) + 0x44))(stream);
}
#endif
