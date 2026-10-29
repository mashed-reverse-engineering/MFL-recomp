/* compile: /G5 */
#include "mfl_tier2.h"
/*007da72c*/ extern int g_rp_7da72c;
/*004dc8b0*/ extern void *__cdecl RwStreamWriteInt32(void *s, void *p, int n);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Stream write of the MatFX flag word of an object (4 bytes). */
void *lib_550010(void *s, void *p, void *o)
{
  int v = FLD(o, int, g_rp_7da72c);
  return RwStreamWriteInt32(s, &v, 4);
}
