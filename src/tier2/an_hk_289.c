#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/* lua_AI_Data_Filename */

/* Lua function AI_Data_Filename (registered by the script engine).
 * L is the Lua state. The last argument on the Lua stack is the AI data
 * file name string; it is copied, including its terminating NUL, into
 * g_course_files->ai_data (a 64-byte buffer). There is no length check, so
 * a longer name overruns the buffer (inferred from the copy loop).
 * Returns 0, the number of values pushed back to Lua (inferred).
 */

undefined4 lua_AI_Data_Filename(char *L)

{
  char ch;
  undefined4 top_index;
  char *src;
  char *dst;

  /* Fetch the string at the top of the stack; dst is the destination buffer. */
  top_index = lua_gettop(L);
  src = (char *)lua_tostring(L,top_index);
  dst = g_course_files->ai_data;
  /* Byte-by-byte copy. The NUL is copied too and ends the loop. */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  return 0;
}


