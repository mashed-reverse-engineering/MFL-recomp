/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef int (__cdecl *Fn4)(void *, void *, void *, int);
/*007d2158*/ extern int g_rw_7d2158;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Transforms an array of vectors through the engine standard-function table; returns the output. */
void *RwV3dTransformVectors(void *out, void *in, int n, void *m)
{
  ((Fn4)FLD(RwEngineInstance + g_rw_7d2158, void *, 0x14))(out, in, n, m);
  return out;
}
