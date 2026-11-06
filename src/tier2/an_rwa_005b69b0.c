/* rwa_virtual_voice_destroy_005b69b0 (0x005b69b0): virtual voice object destroy: stop the voice, unlink it */
#include "mfl_tier2.h"

/*005b7eb0*/ extern void rwa_voicemanager_005b7eb0(void *voice, int op);      /* voice control: 1 stop */

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_virtual_voice_destroy_005b69b0(char *obj)
{
  RwaLink *v = (RwaLink *)(obj + 0x28);

  rwa_voicemanager_005b7eb0(v, 1);
  if (v->next) {
    v->prev->next = v->next;
    v->next->prev = v->prev;
  }
}
