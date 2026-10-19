#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];

/* Lua stack offsets from the top (arg_count) of the call's arguments */
#define ARG_OBJECT_FROM_TOP (-5)
#define ARG_PROPERTY_A_FROM_TOP (-4)
#define ARG_PROPERTY_B_FROM_TOP (-3)
#define ARG_PROPERTY_C_FROM_TOP (-2)
#define ARG_PROPERTY_D_FROM_TOP (-1)
#define DEFAULT_SHATTER_VELOCITY (8.0)

/* lua_RWP_Object_Properties */

/* Lua function RWP_Object_Properties (registered by the script engine).
 * Sets the physical properties of one entry in g_rwp_objects from the Lua call
 * RWP_Object_Properties(object_index, property_a, property_b, property_c, property_d, material)
 * (argument order inferred from the stack offsets and the struct field comments).
 * Parameter L: the Lua state. object_index indexes g_rwp_objects directly (inferred).
 * shatter_velocity is reset to its default on every call (inferred).
 * Returns 0 (no Lua results). */

undefined4 lua_RWP_Object_Properties(void *L)

{
  struct RwpObjectDesc *desc;
  int arg_count;

  arg_count = lua_gettop(L);
  desc = &g_rwp_objects[(int)lua_tonumber(L,arg_count + ARG_OBJECT_FROM_TOP)];
  desc->property_a = (float)lua_tonumber(L,arg_count + ARG_PROPERTY_A_FROM_TOP);
  desc->property_b = (float)lua_tonumber(L,arg_count + ARG_PROPERTY_B_FROM_TOP);
  desc->property_c = (float)lua_tonumber(L,arg_count + ARG_PROPERTY_C_FROM_TOP);
  desc->property_d = (float)lua_tonumber(L,arg_count + ARG_PROPERTY_D_FROM_TOP);
  desc->material = (int)lua_tonumber(L,arg_count);
  desc->shatter_velocity = DEFAULT_SHATTER_VELOCITY;
  return 0;
}
