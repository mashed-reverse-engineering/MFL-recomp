#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];


/* lua_RWP_Object_BaseLabel */

/* Lua function RWP_Object_BaseLabel (registered by the script engine) */

undefined4 lua_RWP_Object_BaseLabel(undefined4 lua_state)

{
  int arg_count;
  int object_index;
  struct RwpObjectDesc *pdesc;
  
  arg_count = lua_gettop(lua_state);
  object_index = (int)lua_tonumber(lua_state,arg_count + -1);
  pdesc = &g_rwp_objects[object_index];
  pdesc->base_label = (int)lua_tonumber(lua_state,arg_count);
  return 0;
}
