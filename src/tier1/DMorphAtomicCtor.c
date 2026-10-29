/* compile: /G5 */
#include "mfl_tier2.h"
/*007da7ac*/ extern int g_rp_7da7ac;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Atomic plugin constructor: clears three DMorph fields; returns the object. */
void *DMorphAtomicCtor(void *o)
{
  char *e = (char *)o + g_rp_7da7ac;
  FLD(e, int, 0) = 0;
  FLD(e, int, 4) = 0;
  FLD(e, int, 8) = 0;
  return o;
}
