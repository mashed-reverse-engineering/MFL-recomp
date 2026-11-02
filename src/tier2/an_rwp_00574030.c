/* rwp_trilist_vslot_clear_typedata (0x00574030): triangle list shape virtual method: clear type data, return 1 */
#include "mfl_tier2.h"

typedef struct MflTrilistShape {
  unsigned char pad00[0x40];
  void *data;   /* 0x40 */
} MflTrilistShape;

/* Virtual method of the triangle list shape: clears the type data pointer, returns 1. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_trilist_vslot_clear_typedata(MflTrilistShape *s)
{
  s->data = 0;
  return 1;
}
