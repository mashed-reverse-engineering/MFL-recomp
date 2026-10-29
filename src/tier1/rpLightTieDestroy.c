/* compile: /G5 */
#include "mfl_tier2.h"
/*007d2128*/ extern char *RwEngineInstance;
typedef struct LLLink { struct LLLink *next, *prev; } LLLink;
/*007d5294*/ extern int g_rp_7d5294;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
#define ENG(off, T) (*(T *)(RwEngineInstance + (off)))
/* Unlinks a light tie from its sector and light lists and returns it to the engine free list; returns TRUE. */
int rpLightTieDestroy(void *tie)
{
  LLLink *a = (LLLink *)((char *)tie + 0xc);
  LLLink *b = (LLLink *)tie;
  a->prev->next = a->next;
  a->next->prev = a->prev;
  b->prev->next = b->next;
  b->next->prev = b->prev;
  ((void (__cdecl *)(void *, void *))ENG(0x11c, void *))(FLD(RwEngineInstance + g_rp_7d5294, void *, 4), tie);
  return 1;
}
