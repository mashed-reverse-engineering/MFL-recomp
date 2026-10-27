#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
/* lua_RWP_Object_CentreOfMassOffset */
/* Lua function RWP_Object_CentreOfMassOffset (registered by the script engine)
 * Sets the centre of mass offset (x, y, z) of one RWP object.
 * Arguments, counted from the top of the Lua stack: object index, x, y, z.
 * lua_state: the Lua state the script engine passes in.
 * Returns 0 (no Lua return values). */
undefined4 lua_RWP_Object_CentreOfMassOffset(char *lua_state)
{
  struct RwpObjectDesc *object;
  int arg_count;

  arg_count = lua_gettop(lua_state);
  object = &g_rwp_objects[(int)lua_tonumber(lua_state,arg_count + -3)];
  object->centre_of_mass_offset[0] = (float)lua_tonumber(lua_state,arg_count + -2);
  object->centre_of_mass_offset[1] = (float)lua_tonumber(lua_state,arg_count + -1);
  object->centre_of_mass_offset[2] = (float)lua_tonumber(lua_state,arg_count);
  return 0;
}
