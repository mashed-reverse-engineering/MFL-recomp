/* rwp_world_sizes_add_body (0x0058cb40): count one more body in the world size record */
#include "mfl_tier2.h"

/*0063a5dc*/ extern int g_data_63a5dc;   /* the last body's second count */

typedef struct MflWorldSizes {
  int pad0;
  int bodies;   /* 0x4 */
  int rows;   /* 0x8 */
  int pad0c;
  int count_b;   /* 0x10 */
  int count_c;   /* 0x14 */
  int pad18;
  int words_b;   /* 0x1c: 5 words per count_b entry */
  int words_c;   /* 0x20: 5 words per count_b entry */
} MflWorldSizes;

/* Adds a body with n rows and m entries of the second kind (two tables of m entries, two of 5 m words) and remembers m.  Returns s. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
MflWorldSizes *rwp_world_sizes_add_body(MflWorldSizes *s, int n, int m)
{
  s->bodies++;
  s->rows += n;
  s->count_b += m;
  s->count_c += m;
  s->words_b += m * 5;
  s->words_c += m * 5;
  g_data_63a5dc = m;
  return s;
}
