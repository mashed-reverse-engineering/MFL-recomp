#include "mfl_tier2.h"
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
#define CLUMP_FADE_ARG_COUNT (4)
#define CLUMP_FADE_STRIDE (5)
#define CLUMP_FADE_A_FIELD (1)
#define CLUMP_FADE_B_FIELD (2)
#define CLUMP_FADE_FLOAT_FIELD (4)
/* lua_ClumpFade */
/* Lua function ClumpFade (registered by the script engine)
 *
 * ClumpFade(clump_index, a, b, value) (argument names inferred): writes one fade record of
 * CLUMP_FADE_STRIDE dwords into g_course_counts->clump_fade, at the record for clump_index:
 *   [0] = 1 (enable), [1] = a, [2] = b, [3] not written, [4] = value (float, truncated to int first).
 * lua_state: the Lua state the arguments are read from.
 * Returns 0 in both cases. Wrong argument count: prints an error, writes nothing.
 * clump_index is not range checked against the record count (inferred from the code). */
undefined4 lua_ClumpFade(char *lua_state)
{
  int clump_index;
  uint arg_value;
  int arg4_int;

  /* Exactly four arguments are required. clump_index is reused: first the argument count, then the clump index. */
  clump_index = lua_gettop((void *)lua_state);
  if (clump_index != CLUMP_FADE_ARG_COUNT) {
    FID_conflict__wprintf("ClumpFade ERROR, number of argument is not four\n");
    return 0;
  }
  /* Record for this clump: enable flag, then the a and b values from arguments 2 and 3, then the float from argument 4. */
  clump_index = (int)lua_tonumber((void *)lua_state,1);
  g_course_counts->clump_fade[clump_index * CLUMP_FADE_STRIDE] = 1;
  arg_value = (int)lua_tonumber((void *)lua_state,2);
  g_course_counts->clump_fade[clump_index * CLUMP_FADE_STRIDE + CLUMP_FADE_A_FIELD] = arg_value;
  arg_value = (int)lua_tonumber((void *)lua_state,3);
  g_course_counts->clump_fade[clump_index * CLUMP_FADE_STRIDE + CLUMP_FADE_B_FIELD] = arg_value;
  arg4_int = (int)lua_tonumber((void *)lua_state,4);
  *(float *)&g_course_counts->clump_fade[clump_index * CLUMP_FADE_STRIDE + CLUMP_FADE_FLOAT_FIELD] = (float)arg4_int;
  return 0;
}
