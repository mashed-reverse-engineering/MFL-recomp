/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef int (__cdecl *Fn3)(void *, void *, void *);
/*007d2158*/ extern int g_rw_7d2158;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Transforms a direction vector by a matrix through the engine standard-function table; returns the output. */
void *RwV3dTransformVector(void *out, void *in, void *m)
{
  ((Fn3)FLD(RwEngineInstance + g_rw_7d2158, void *, 0x10))(out, in, m);
  return out;
}
