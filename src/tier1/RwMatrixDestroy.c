/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef int (__cdecl *Fn2)(void *, void *);
/*007d2150*/ extern int g_rw_7d2150;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
#define ENG(off, T) (*(T *)(RwEngineInstance + (off)))
/* Returns a matrix to the matrix free list (engine free-list callback); returns TRUE. */
int RwMatrixDestroy(void *m)
{
  ((Fn2)ENG(0x11c, void *))(FLD(RwEngineInstance, void *, g_rw_7d2150), m);
  return 1;
}
