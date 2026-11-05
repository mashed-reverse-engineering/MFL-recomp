/* rwa_string_compare_005be630 (0x005be630): string compare that treats the same pointer as equal without reading it */
#include "mfl_tier2.h"

/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_string_compare_005be630(RwChar *a, RwChar *b)
{
  if (a == b)
    return 0;
  return rwstrcmp(a, b);
}
