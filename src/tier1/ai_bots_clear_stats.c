#include "mfl_tier2.h"
/*004b4ba7*/ extern int entry();
/*006089b0*/ extern undefined4 g_ai_bots_6089b0;
/*008c9998*/ extern undefined4 g_ai_counter_9998;
/*008c999c*/ extern float g_ai_skill_bias;
/*008c99a0*/ extern float g_ai_skill_score;
/*008c99a4*/ extern undefined4 g_ai_career_skill_table;
/*008c9a40*/ extern undefined4 g_ai_career_counter_table;

/* byte size of the career counter / skill tables walked by ai_bots_clear_stats (inferred: 45 dwords) */
#define AI_CAREER_TABLE_BYTES (0xb4)


/* ai_bots_clear_stats */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "clears counters 0x8C9998 / 0x8C999C / 0x8C99A0 and the per bot tables"
   Clears g_ai_counter_9998, g_ai_skill_bias and g_ai_skill_score, then for each
   4 byte entry zeroes g_ai_career_counter_table and copies the value from
   g_ai_bots_6089b0 into g_ai_career_skill_table. Takes no parameters, returns nothing. */

/* TIER2: validated 2026-10-18 authority=diffemu */
void ai_bots_clear_stats(void)
{
  struct { int entries_left; int table_offset; } loop;

  g_ai_skill_bias = 0;
  g_ai_counter_9998 = 0;
  g_ai_skill_score = 0;
  loop.table_offset = 0;
  /* outer loop: one pass per 3 entries (inner loop), until loop.table_offset reaches AI_CAREER_TABLE_BYTES */
  do {
    loop.entries_left = 3;
    do {
      *(undefined4 *)((char *)&g_ai_career_counter_table + loop.table_offset) = 0;
      *(undefined4 *)((char *)&g_ai_career_skill_table + loop.table_offset) =
           *(undefined4 *)((char *)&g_ai_bots_6089b0 + loop.table_offset);
      loop.table_offset = loop.table_offset + 4;
      loop.entries_left = loop.entries_left + -1;
    } while (loop.entries_left != 0);
  } while (loop.table_offset < AI_CAREER_TABLE_BYTES);
  return;
}
