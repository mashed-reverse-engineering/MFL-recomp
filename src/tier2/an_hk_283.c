#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;

/* Each entry of CourseFiles.mts_files is a fixed 64-byte, NUL-terminated name */
#define MTS_FILENAME_SIZE (0x40)

/* lua_Mts_Filename */

/*
 * Lua function Mts_Filename (registered by the script engine).
 * Stores a course MTS filename in g_course_files->mts_files and bumps g_course_counts->mts_count.
 *   Mts_Filename()            - name is stack index 0, goes to slot mts_count (inferred)
 *   Mts_Filename(index, name) - slot is the number before the last argument, name is the last argument
 * The name is copied including its NUL terminator. Returns 0 results.
 * No bounds check on the slot. A single-argument call reads the slot from stack index 0 (inferred: unused path).
 */

undefined4 lua_Mts_Filename(char *lua_state)

{
  char ch;
  int arg_count;
  char *src;
  int slot;
  char *dst;

  arg_count = lua_gettop(lua_state);
  if (arg_count == 0) {
    src = (char *)lua_tostring(lua_state,0);
    dst = g_course_files->mts_files + g_course_counts->mts_count * MTS_FILENAME_SIZE;
    g_course_counts->mts_count = g_course_counts->mts_count + 1;
    /* copy the name byte by byte, up to and including its NUL */
    do {
      ch = *src;
      src = src + 1;
      *dst = ch;
      dst = dst + 1;
    } while (ch != '\0');
    return 0;
  }
  slot = (int)lua_tonumber(lua_state,arg_count + -1);
  src = (char *)lua_tostring(lua_state,arg_count);
  dst = g_course_files->mts_files + slot * MTS_FILENAME_SIZE;
  /* copy the name byte by byte, up to and including its NUL */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  g_course_counts->mts_count = g_course_counts->mts_count + 1;
  return 0;
}


