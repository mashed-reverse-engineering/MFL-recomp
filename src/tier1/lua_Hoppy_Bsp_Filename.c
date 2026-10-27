#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/* lua_Hoppy_Bsp_Filename */
/* Lua function Hoppy_Bsp_Filename (registered by the script engine).
 *
 * Copies the string passed as the last argument into g_course_files->hoppy_bsp,
 * the course's Hoppy BSP filename (a 64-byte buffer).
 * lua_state: the lua_State * the script engine passes in.
 * Returns 0 (no Lua results).
 * No length check on the copy and no NULL check on lua_tostring (inferred from the code).
 */
undefined4 lua_Hoppy_Bsp_Filename(char *lua_state)
{
  char ch;
  undefined4 filename_index;
  char *src;
  char *dst;

  filename_index = lua_gettop(lua_state);
  src = (char *)lua_tostring(lua_state,filename_index);
  dst = g_course_files->hoppy_bsp;
  /* Copy the string byte by byte, including the terminating NUL. */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  return 0;
}
