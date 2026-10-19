#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
#define RWP_SHAPE_CYLINDER (2)
/* lua_RWP_Object_Cylinder */

/* Lua function RWP_Object_Cylinder (registered by the script engine).
 * Sets g_rwp_objects[object_index] to a cylinder: cylinder[0] and cylinder[1]
 * (inferred: radius and height) are taken from the Lua call, shape is set to
 * RWP_SHAPE_CYLINDER. Lua args (inferred): 1 = object index, 2 = cylinder[0], 3 = cylinder[1].
 * lua_state is the Lua state pointer. Returns 0 (no Lua results). */

undefined4 lua_RWP_Object_Cylinder(char *lua_state)

{
  struct RwpObjectDesc *obj;
  int arg_count;

  arg_count = lua_gettop(lua_state);
  obj = &g_rwp_objects[(int)lua_tonumber(lua_state,arg_count + -2)];
  obj->cylinder[0] = (float)lua_tonumber(lua_state,arg_count + -1);
  obj->cylinder[1] = (float)lua_tonumber(lua_state,arg_count);
  obj->shape = RWP_SHAPE_CYLINDER;
  return 0;
}


