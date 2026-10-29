/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct LLLink { struct LLLink *next, *prev; } LLLink;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Unlinks an object from its frame's object list when it has a frame; returns the object. */
void *rwObjectHasFrameReleaseFrame(void *o)
{
  if (FLD(o, int, 4)) { LLLink *l = (LLLink *)((char *)o + 8); l->prev->next = l->next; l->next->prev = l->prev; }
  return o;
}
