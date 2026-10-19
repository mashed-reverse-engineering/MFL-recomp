#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/* lua_AI_Bsp_Filename */

/* Lua function AI_Bsp_Filename (registered by the script engine).
 * L is the Lua state. Copies the last argument (the AI BSP file name) into
 * g_course_files->ai_bsp, a 64 byte field with no length check, then sets
 * g_course_counts->ai_bsp_count to 1. Returns 0 results. (inferred from the dossier strings) */

undefined4 lua_AI_Bsp_Filename(char *L)

{
  char ch;
  undefined4 arg_count;
  char *src;
  char *dst;

  /* stack top = number of arguments, so arg_count is the index of the last one */
  arg_count = lua_gettop(L);
  src = (char *)lua_tostring(L,arg_count);
  dst = g_course_files->ai_bsp;
  /* copy byte by byte, including the terminating NUL */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  g_course_counts->ai_bsp_count = 1;
  return 0;
}


