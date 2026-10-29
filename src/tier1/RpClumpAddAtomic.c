/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct LLLink { struct LLLink *next, *prev; } LLLink;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Links an atomic into the clump atomic list (at the head) and sets its clump; returns the clump. */
void *RpClumpAddAtomic(void *clump, void *atomic)
{
  LLLink *head = (LLLink *)((char *)clump + 8);
  LLLink *link = (LLLink *)((char *)atomic + 0x40);
  LLLink *first = head->next;
  FLD(atomic, LLLink *, 0x44) = head;
  link->next = first;
  head->next->prev = link;
  head->next = link;
  FLD(atomic, void *, 0x3c) = clump;
  return clump;
}
