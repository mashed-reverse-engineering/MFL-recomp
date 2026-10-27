#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2aec*/ extern struct CourseCounts *g_course_counts;


/* lua_Physics_Object */

/* Lua function Physics_Object (registered by the script engine) */
/* Appends one entry to g_course_counts->clump_mts_pairs (4 slots: byte, byte, word) from the three Lua args. Field meaning: unknown. */

undefined4 lua_Physics_Object(undefined4 lua_state)
{
  int pair_index;
  CourseCounts *counts;
  byte byte_value;
  undefined2 word_value;
  int arg_count;

  arg_count = lua_gettop(lua_state);
  pair_index = g_course_counts->clump_mts_pair_count;
  /* first Lua arg (arg_count - 2) stored as byte slot 0 of the entry */
  byte_value = (int)lua_tonumber(lua_state,arg_count + -2);
  g_course_counts->clump_mts_pairs[pair_index * 4] = byte_value;
  /* second Lua arg (arg_count - 1) stored as byte slot 1 */
  byte_value = (int)lua_tonumber(lua_state,arg_count + -1);
  g_course_counts->clump_mts_pairs[pair_index * 4 + 1] = byte_value;
  /* last Lua arg stored as 16-bit word in slots 2-3 */
  word_value = (int)lua_tonumber(lua_state,arg_count);
  counts = g_course_counts;
  *(undefined2 *)(g_course_counts->clump_mts_pairs + pair_index * 4 + 2) = word_value;
  counts->clump_mts_pair_count = counts->clump_mts_pair_count + 1;
  return 0;
}
