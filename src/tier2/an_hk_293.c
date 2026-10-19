#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/* lua_Collision_Bsp_Filename */

/* Lua function Collision_Bsp_Filename (registered by the script engine)
 * Copies the collision BSP filename argument into g_course_files->collision_bsp
 * (NUL-terminated, no length check; the field is 64 bytes) and sets
 * g_course_counts->collision_bsp_count to 1. Returns 0 results.
 * lua_state: Lua state passed by the script engine.
 * arg_count: stack top index, used as the index of the filename argument (inferred).
 */

undefined4 lua_Collision_Bsp_Filename(char *lua_state)

{
  char ch;
  undefined4 arg_count;
  char *src;
  char *dst;

  arg_count = lua_gettop(lua_state);
  src = (char *)lua_tostring(lua_state,arg_count);
  dst = g_course_files->collision_bsp;
  /* copy the string byte by byte, including the terminating NUL */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  g_course_counts->collision_bsp_count = 1;
  return 0;
}


