#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008886c4*/ extern char g_rwp_objects_0__mts_0_[];
/*00888684*/ extern char g_rwp_objects_0__dff_0_[];
/*00888640*/ extern char g_rwp_objects[];

#define RWP_OBJECT_STRIDE (0x110) /* bytes per entry in g_rwp_objects (inferred) */

/* lua_RWP_Object */

/* Lua function RWP_Object (registered by the script engine)
 *
 * Stores three strings in one entry of g_rwp_objects (inferred: a table of
 * RWP_OBJECT_STRIDE-byte records). Lua stack, relative to the top arg_count:
 *   [-3] object index (number), [-2] name, [-1] dff path, [0] mts path.
 * The index selects the record; each string is copied with the engine's strcpy.
 * lua_state: the Lua state. Returns 0 (no Lua results).
 */

undefined4 lua_RWP_Object(char *lua_state)

{
  RwStringFunctions *string_funcs;
  int arg_count;
  int object_offset;
  RwChar *src;

  /* name -> record +0x4 (inferred) */
  arg_count = lua_gettop(lua_state);
  object_offset = (int)lua_tonumber(lua_state,arg_count + -3);
  object_offset = object_offset * RWP_OBJECT_STRIDE;
  string_funcs = &RwGlobalsRelease_stringFuncs(RwEngineInstance);
  src = (RwChar *)lua_tostring(lua_state,arg_count + -2);
  (*RwStringFunctions_vecStrcpy(string_funcs))((RwChar *)(object_offset + ((LONG_PTR)&g_rwp_objects + 0x4)),src);
  /* dff path (inferred) */
  string_funcs = &RwGlobalsRelease_stringFuncs(RwEngineInstance);
  src = (RwChar *)lua_tostring(lua_state,arg_count + -1);
  (*RwStringFunctions_vecStrcpy(string_funcs))((RwChar *)(object_offset + ((LONG_PTR)&g_rwp_objects_0__dff_0_)),src);
  /* mts path (inferred) */
  string_funcs = &RwGlobalsRelease_stringFuncs(RwEngineInstance);
  src = (RwChar *)lua_tostring(lua_state,arg_count);
  (*RwStringFunctions_vecStrcpy(string_funcs))((RwChar *)(object_offset + ((LONG_PTR)&g_rwp_objects_0__mts_0_)),src);
  return 0;
}


