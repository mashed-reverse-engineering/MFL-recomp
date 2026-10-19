#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/* lua_Occluder_Filename */

/* Lua function Occluder_Filename (registered by the script engine).
 * Takes the string at the top of the Lua stack (the last argument) and copies it,
 * including the terminating NUL, into g_course_files->occluder (the occluder BSP filename).
 * Parameter: lua_state, the Lua state (lua_State *) the script engine passes in.
 * Returns: 0 (no Lua return values). */

undefined4 lua_Occluder_Filename(char *lua_state)

{
  char ch;
  undefined4 arg_count;
  char *src;
  char *dst;

  arg_count = lua_gettop(lua_state);
  src = (char *)lua_tostring(lua_state,arg_count);
  dst = g_course_files->occluder;
  /* Copy the string byte by byte until the NUL terminator has been copied.
   * No bounds check: the destination is a fixed 64-byte buffer (inferred). */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  return 0;
}


