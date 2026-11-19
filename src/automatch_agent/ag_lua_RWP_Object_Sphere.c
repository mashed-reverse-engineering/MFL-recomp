#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];


/* lua_RWP_Object_Sphere */

/* Lua function RWP_Object_Sphere (registered by the script engine) */

undefined4 lua_RWP_Object_Sphere(undefined4 lua_state)

{
  struct RwpObjectDesc *p_iVar1;
  int arg_count;
  int object_index;
  double fVar2;
  
  arg_count = lua_gettop(lua_state);
  object_index = (int)lua_tonumber(lua_state,arg_count + -1);
  p_iVar1 = &g_rwp_objects[object_index];
  p_iVar1->sphere_radius = (float)lua_tonumber(lua_state,arg_count);
  p_iVar1->shape = 3;
  return 0;
}
