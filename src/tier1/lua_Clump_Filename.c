#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
#define CLUMP_FILE_SLOT_SIZE (0x40)
/* lua_Clump_Filename */
/* Lua function Clump_Filename (registered by the script engine).
 * Copies a filename into g_course_files->clump_files, which holds 128 slots of
 * CLUMP_FILE_SLOT_SIZE bytes each, and increments g_course_counts->clump_count.
 * Lua args: [index,] filename. With only the filename, the slot is clump_count
 * (inferred: the filename is then stack index 0). With an index, the slot is
 * that index (inferred). Always returns 0 (no Lua results).
 */
undefined4 lua_Clump_Filename(char *lua_state)
{
  char ch;
  int arg_count;
  char *src;
  int clump_index;
  char *dst;

  arg_count = lua_gettop(lua_state);
  if (arg_count == 0) {
    /* no index argument: append to the next free clump slot */
    src = (char *)lua_tostring(lua_state,0);
    dst = g_course_files->clump_files + g_course_counts->clump_count * CLUMP_FILE_SLOT_SIZE;
    g_course_counts->clump_count = g_course_counts->clump_count + 1;
    /* copy the filename, including its terminating NUL */
    do {
      ch = *src;
      src = src + 1;
      *dst = ch;
      dst = dst + 1;
    } while (ch != '\0');
    return 0;
  }
  /* index argument given: write the filename into that slot */
  clump_index = (int)lua_tonumber(lua_state,arg_count + -1);
  src = (char *)lua_tostring(lua_state,arg_count);
  dst = g_course_files->clump_files + clump_index * CLUMP_FILE_SLOT_SIZE;
  /* copy the filename, including its terminating NUL */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  g_course_counts->clump_count = g_course_counts->clump_count + 1;
  return 0;
}
