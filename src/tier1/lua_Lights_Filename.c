#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/* lua_Lights_Filename */
/* Lua function Lights_Filename (registered by the script engine).
 * Sets the course lights filename: copies the string that is the last Lua argument,
 * including its terminating NUL, into g_course_files->lights (64 bytes, no bounds check).
 * lua_state: the Lua state passed in by the script engine.
 * Returns 0 (no Lua results). */
undefined4 lua_Lights_Filename(char *lua_state)
{
  char ch;
  undefined4 arg_count;
  char *src;
  char *dst;

  arg_count = lua_gettop(lua_state);
  src = (char *)lua_tostring(lua_state,arg_count);
  dst = g_course_files->lights;
  /* Copy byte by byte, stopping after the NUL is copied. */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  return 0;
}
