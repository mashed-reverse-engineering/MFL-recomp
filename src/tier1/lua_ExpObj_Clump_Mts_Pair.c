#include "mfl_tier2.h"
/*00474340*/ extern undefined __cdecl course_object_pair_add(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
#define ARG_COUNT_PAIR_ONLY (2)
/* lua_ExpObj_Clump_Mts_Pair */
/* Lua function ExpObj_Clump_Mts_Pair (registered by the script engine).
   Reads the numeric Lua arguments and passes them to course_object_pair_add()
   as (second_arg, first_arg, third_arg). Always returns 0 (no Lua results).
   lua_state: the Lua state the script engine passes in.
   Arguments: with exactly 2 args, the pair is (arg 1, arg 2) and the third
   argument is 0. Otherwise the last three args are read (count-2, count-1, count)
   (inferred: the third is an optional extra value). */
undefined4 lua_ExpObj_Clump_Mts_Pair(char *lua_state)
{
  int arg_count;
  undefined4 first_arg;
  undefined4 second_arg;
  undefined4 third_arg;

  arg_count = lua_gettop(lua_state);
  if (arg_count == ARG_COUNT_PAIR_ONLY) {
    first_arg = (int)lua_tonumber(lua_state,1);
    second_arg = (int)lua_tonumber(lua_state,arg_count);
    third_arg = 0;
  }
  else {
    first_arg = (int)lua_tonumber(lua_state,arg_count + -2);
    second_arg = (int)lua_tonumber(lua_state,arg_count + -1);
    third_arg = (int)lua_tonumber(lua_state,arg_count);
  }
  course_object_pair_add(second_arg,first_arg,third_arg);
  return 0;
}
