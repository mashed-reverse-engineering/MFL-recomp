#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/* lua_World_Bsp_Filename */
/* Lua function World_Bsp_Filename (registered by the script engine).
 * Copies the string given as the last argument into g_course_files->world_bsp
 * (the course's world BSP filename, 64 bytes). The copy is NUL-terminated and
 * has no bounds check, so a longer name overruns the field (inferred).
 * Returns 0 results to Lua. */
undefined4 lua_World_Bsp_Filename(char *L)
{
  char ch;
  undefined4 arg_count;
  char *src;
  char *dst;

  arg_count = lua_gettop(L);
  src = (char *)lua_tostring(L,arg_count);
  dst = g_course_files->world_bsp;
  /* copy loop: byte by byte, including the terminating '\0' */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  return 0;
}
