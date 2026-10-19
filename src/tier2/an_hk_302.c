#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];

#define RWP_SHAPE_BOX (1)
/* lua_RWP_Object_Box */

/* Lua function RWP_Object_Box (registered by the script engine) */

/*
 * Lua: RWP_Object_Box(index, width, depth, height) (argument meanings inferred)
 * Sets g_rwp_objects[index] to a box shape with the three box_dimensions.
 * lua_state: the Lua state the script engine passes in.
 * Returns 0 (no Lua results).
 */
undefined4 lua_RWP_Object_Box(char *lua_state)

{
  struct RwpObjectDesc *desc;
  int arg_count;
  int object_index;

  arg_count = lua_gettop(lua_state);
  object_index = (int)lua_tonumber(lua_state,arg_count + -3);
  desc = &g_rwp_objects[object_index];
  desc->box_dimensions[0] = (float)lua_tonumber(lua_state,arg_count + -2);
  desc->box_dimensions[1] = (float)lua_tonumber(lua_state,arg_count + -1);
  desc->box_dimensions[2] = (float)lua_tonumber(lua_state,arg_count);
  desc->shape = RWP_SHAPE_BOX;
  return 0;
}


