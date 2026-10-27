#include "mfl_tier2.h"

/* Entry size in 32-bit words (4 words = 16 bytes per entry). */
#define TABLE_6C3F38_WORDS_PER_ENTRY 4

/*006c3f38*/ extern undefined4 g_gfx_6c3f38;


/* table_get_6c3f38 */

/* Word of table entry `index` at 0x6C3F38 (stride 16 bytes). */
/* "int at 0x6C3F38 + i * 16" */

undefined4 __cdecl table_get_6c3f38(int index)
{
  return (&g_gfx_6c3f38)[index * TABLE_6C3F38_WORDS_PER_ENTRY];
}
