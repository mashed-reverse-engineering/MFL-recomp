#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/* lua_Camera_Anim_Filename */
/* Lua function Camera_Anim_Filename (registered by the script engine).
 * Takes the file name from the last argument on the Lua stack and copies it
 * into g_course_files->camera_anim. Returns 0 (no Lua results).
 * lua_state: the Lua state the script engine passes in.
 * No length check: the copy stops at the NUL terminator, so a name of 64
 * bytes or more overruns camera_anim (inferred: scripts must keep it short). */
undefined4 lua_Camera_Anim_Filename(char *lua_state)
{
  char ch;
  undefined4 top_index;
  char *src;
  char *dst;

  top_index = lua_gettop(lua_state);
  src = (char *)lua_tostring(lua_state,top_index);
  dst = g_course_files->camera_anim;
  /* copy the string byte by byte, including its NUL terminator */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  return 0;
}
