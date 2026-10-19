#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*006f2aec*/ extern struct CourseCounts *g_course_counts;

#define CLUMP_MTS_PAIR_SIZE (4) /* bytes per entry: u8 clump, u8 mts, u16 terminator */
#define CLUMP_MTS_PAIR_END (0xff) /* both bytes of the u16 0xFFFF terminator */

/* lua_Clump_Mts_Pair */

/* Lua function Clump_Mts_Pair (registered by the script engine) */

/* Appends one clump/MTS pair to the current course's clump_mts_pairs table (16 entries max, no bounds check).
   Lua args: the last two values on the stack are the clump id (second to last) and the MTS id (last) (inferred from the order of the reads).
   Each entry is 4 bytes: u8 clump, u8 mts, u16 0xFFFF terminator. Returns 0 results. */
undefined4 lua_Clump_Mts_Pair(char *lua_state)

{
  byte *pair_terminator;
  int pair_index;
  CourseCounts *counts;
  CourseCounts *pair_counts;
  byte value;
  int arg_count;

  arg_count = lua_gettop(lua_state);
  pair_index = g_course_counts->clump_mts_pair_count;
  /* write the clump id into the new entry */
  value = (int)lua_tonumber(lua_state,arg_count + -1);
  counts = g_course_counts;
  g_course_counts->clump_mts_pairs[pair_index * CLUMP_MTS_PAIR_SIZE] = value;
  /* write the MTS id into the new entry */
  value = (int)lua_tonumber(lua_state,arg_count);
  counts->clump_mts_pairs[pair_index * CLUMP_MTS_PAIR_SIZE + 1] = value;
  /* mark the entry's trailing u16 as the 0xFFFF terminator */
  pair_counts = g_course_counts;
  pair_terminator = counts->clump_mts_pairs + pair_index * CLUMP_MTS_PAIR_SIZE + 2;
  pair_terminator[0] = CLUMP_MTS_PAIR_END;
  pair_terminator[1] = CLUMP_MTS_PAIR_END;
  /* commit the entry by bumping the pair count */
  pair_counts->clump_mts_pair_count = pair_counts->clump_mts_pair_count + 1;
  return 0;
}


