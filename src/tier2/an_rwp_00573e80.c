#include "mfl_tier2.h"
/* Constructor of the "visited flags" plugin of a physics (Rwp) object: allocates a bit list large enough for
   (object->bitCount / 8 + 1) 32 bit words, stores it at the plugin offset inside the object and sets every bit. */
struct RwpFlagList {
  unsigned int *words;       /* points just behind this header */
  int byteSize;              /* header + words */
};
#define RWP_MEM_HINT 0x30900
#define RWP_FLAGS_HEADER_SIZE 8
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0063a548*/ extern int g_rwp_trilist_sector_plugin_offset;

/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void *rwp_trilist_sector_flags_ctor_00573e80(char *object)
{
  unsigned int wordCount = (*(unsigned short *)(object + 0x84) >> 3) + 1;
  struct RwpFlagList **slot = (struct RwpFlagList **)(object + g_rwp_trilist_sector_plugin_offset);
  int byteSize = wordCount * sizeof(unsigned int) + RWP_FLAGS_HEADER_SIZE;
  unsigned int i;

  *slot = (struct RwpFlagList *)RwMalloc(byteSize, RWP_MEM_HINT);
  (*slot)->words = (unsigned int *)(*slot + 1);
  (*slot)->byteSize = byteSize;
  for (i = 0; i < wordCount; i++) {
    (*slot)->words[i] = 0xffffffff;
  }
  return object;
}
