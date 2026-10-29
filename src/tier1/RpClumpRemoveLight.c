/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct LLLink { struct LLLink *next, *prev; } LLLink;
/*007d5360*/ extern int g_rp_7d5360;
typedef struct Lnk3 { void *w; LLLink l; } Lnk3;
/* Unlinks a light from the clump light list and clears its link; returns the clump. */
void *RpClumpRemoveLight(void *clump, void *light)
{
  Lnk3 *b = (Lnk3 *)((char *)light + g_rp_7d5360);
  b->l.prev->next = b->l.next;
  b->l.next->prev = b->l.prev;
  b->l.prev = 0;
  b->l.next = 0;
  b->w = 0;
  return clump;
}
