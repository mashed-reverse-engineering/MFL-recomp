/* rwa_wave_def_destroy_005bcdd0 (0x005bcdd0): wave definitions (RwaWaveDefDestroy): refused while a dictionary uses it; destroy callback, free list, owned data, unlink, free */
#include "mfl_tier2.h"

/*005b7f70*/ extern int RwaWaveDictUsingAllWaveDef(char *def, int a2);
/*005beec0*/ extern void RwaFreeListDestroy(void *list);
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */
/*005be850*/ extern void _rwaUniqueIDFreeData(void *id);
/*005bed80*/ extern void RwaFreeListFree(void *list, void *item);
/*007db7a0*/ extern undefined g_data_7db7a0;      /* definition free list */

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_wave_def_destroy_005bcdd0(char *def)
{
  RwaLink *l = (RwaLink *)(def + 0x40);

  if (RwaWaveDictUsingAllWaveDef(def, 1))
    return 0;
  if (*(void (**)(char *))(def + 0x20))
    (*(void (**)(char *))(def + 0x20))(def);
  if (*(void **)(def + 0x3c))
    RwaFreeListDestroy(*(void **)(def + 0x3c));
  if (*(unsigned char *)(def + 0x48) & 1)
    FUN_005bf120(*(void **)(def + 0x28));
  l->prev->next = l->next;
  l->next->prev = l->prev;
  _rwaUniqueIDFreeData(def);
  if (*(unsigned char *)(def + 0x48) & 2)
    RwaFreeListFree(&g_data_7db7a0, def);
  return 1;
}
