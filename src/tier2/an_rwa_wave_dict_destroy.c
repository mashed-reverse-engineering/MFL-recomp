/* RenderWare Audio (Criterion Software) wave dictionary destroy, MFL.exe 0x005b81c0 (RwaWaveDictDestroy).  Written from the
 * MFL.exe disassembly (objdump).  Wave dictionary layout used here: +0xc wave list (head of data nodes), +0x1b flag byte
 * (1 = from the dictionary free list, 4 = caller-owned memory), +0x1c link in the list of all dictionaries. */
#include "mfl_tier2.h"

typedef struct RwaLink {
  struct RwaLink *next;
  struct RwaLink *prev;
} RwaLink;

typedef struct RwaDataNode {      /* node of a data list; the list head is the first two words */
  struct RwaDataNode *next, *prev;
  void *data;
} RwaDataNode;

/*005bc0a0*/ extern void RwaWaveDestroy(void *wave);
/*005be530*/ extern RwInt32 RwaLListFindDataIndex(void *list, void *data);      /* -1: not in the list */
/*005be560*/ extern void RwaLListEmpty(void *list);
/*005be850*/ extern void _rwaUniqueIDFreeData(void *object);
/*005bed80*/ extern void RwaFreeListFree(void *freeList, void *object);
/*005bf120*/ extern void FUN_005bf120(void *mem);      /* RWA free */

/*007dab98*/ extern undefined g_data_7dab98;      /* free list of wave dictionaries */
/*007dabbc*/ extern RwaLink g_data_7dabbc;        /* list of all wave dictionaries */
/*007dabf8*/ extern char *g_data_7dabf8;          /* default dictionary */
/*007dabfc*/ extern char *g_data_7dabfc;          /* current dictionary */

/* Takes the dictionary out of the list of dictionaries and destroys every wave that no other dictionary holds (walking the
   wave list backwards); a wave is destroyed twice when no other dictionary exists at all, as the original does.  Then empties
   the wave list, moves the current dictionary back to the default one when it was this one, frees the unique id data and
   releases the memory according to the flag byte. */
/* TIER2: unvalidated 2026-11-12 authority=diffemu */
void RwaWaveDictDestroy(char *dict)
{
  RwaLink *link = (RwaLink *)(dict + 0x1c), *other;
  RwaDataNode *waves = (RwaDataNode *)(dict + 0xc), *node;
  RwUInt8 flags;

  link->prev->next = link->next;
  link->next->prev = link->prev;
  for (node = waves->prev; node != waves; node = node->prev) {
    void *wave = node->data;

    if (g_data_7dabbc.next == &g_data_7dabbc) {
      RwaWaveDestroy(wave);
      RwaWaveDestroy(wave);
      continue;
    }
    for (other = g_data_7dabbc.next;;) {
      char *holder = (char *)other - 0x1c;

      if (RwaLListFindDataIndex(holder + 0xc, wave) != -1 && holder) break;
      other = other->next;
      if (other == &g_data_7dabbc) {
        RwaWaveDestroy(wave);
        break;
      }
    }
  }
  RwaLListEmpty(waves);
  if (dict == g_data_7dabfc) g_data_7dabfc = g_data_7dabf8;
  _rwaUniqueIDFreeData(dict);
  flags = *(RwUInt8 *)(dict + 0x1b);
  if (flags & 1) RwaFreeListFree(&g_data_7dab98, dict);
  else if (!(flags & 4)) FUN_005bf120(dict);
}
