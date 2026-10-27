#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
#define SHAPE_CAPSULE (4)
/* lua_RWP_Object_Capsule */
/* Lua function RWP_Object_Capsule (registered by the script engine).
 * Reads the last three Lua arguments as (object index, capsule[0], capsule[1]) (inferred),
 * stores the two capsule radii in g_rwp_objects[index] and sets its shape to SHAPE_CAPSULE.
 * lua_state: the Lua state the script engine passes in.
 * Returns 0 (no Lua results). */
undefined4 lua_RWP_Object_Capsule(char *lua_state)
{
  struct RwpObjectDesc *object;
  int arg_count;
  int object_index;

  /* Arguments are counted from the top of the Lua stack: arg_count is the last one. */
  arg_count = lua_gettop(lua_state);
  object_index = (int)lua_tonumber(lua_state,arg_count + -2);
  /* Select the object, then store the two capsule radii and mark it as a capsule. */
  object = &g_rwp_objects[object_index];
  object->capsule[0] = (float)lua_tonumber(lua_state,arg_count + -1);
  object->capsule[1] = (float)lua_tonumber(lua_state,arg_count);
  object->shape = SHAPE_CAPSULE;
  return 0;
}
