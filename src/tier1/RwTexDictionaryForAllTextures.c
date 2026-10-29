/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct LLLink { struct LLLink *next, *prev; } LLLink;
typedef int (__cdecl *ForAllFn)(void *, void *);
extern void (__cdecl *g_rx_7d53c4)(void);
/* Calls a callback for every texture of a dictionary until it returns NULL; returns the dictionary. */
void *RwTexDictionaryForAllTextures(void *d, ForAllFn cb, void *data)
{
  LLLink *head = (LLLink *)((char *)d + 8);
  LLLink *l = head->next;
  while (l != head) { LLLink *next = l->next; if (!cb((char *)l - 8, data)) break; l = next; }
  return d;
}
