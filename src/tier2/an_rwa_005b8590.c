/* rwa_wave_dict_close_module_005b8590 (0x005b8590): wave dictionaries (_rwaWaveDictCloseModule): destroy every dictionary, the free list, reset the module state */
#include "mfl_tier2.h"

/*005b81c0*/ extern void RwaWaveDictDestroy(char *dict);
/*005beec0*/ extern void RwaFreeListDestroy(void *list);

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;
/*007dabbc*/ extern RwaLink g_data_7dabbc;      /* the dictionary list (dictionary link at +0x1c) */
/*007dab98*/ extern undefined g_data_7dab98;    /* dictionary free list */
/*007dabf0*/ extern RwUInt32 g_data_7dabf0;
/*007dabf4*/ extern RwUInt32 g_data_7dabf4;     /* closing */
/*007dabf8*/ extern RwUInt32 g_data_7dabf8;
/*007dabfc*/ extern RwUInt32 g_data_7dabfc;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_wave_dict_close_module_005b8590(void)
{
  g_data_7dabf4 = 1;
  while (g_data_7dabbc.next != &g_data_7dabbc)
    RwaWaveDictDestroy((char *)g_data_7dabbc.next - 0x1c);
  RwaFreeListDestroy(&g_data_7dab98);
  g_data_7dabbc.next = &g_data_7dabbc;
  g_data_7dabbc.prev = &g_data_7dabbc;
  g_data_7dabf8 = 0;
  g_data_7dabfc = 0;
  g_data_7dabf0 = 0;
  g_data_7dabf4 = 0;
}
