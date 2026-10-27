#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/* byte size of one slot in CourseFiles.spline_files (16 slots) */
#define SPLINE_FILENAME_STRIDE (0x40)
/* lua_Spline_Filename */
/* Lua function Spline_Filename (registered by the script engine).
 * Copies a spline file name into CourseFiles.spline_files and bumps CourseCounts.spline_count.
 * Argument handling (inferred from the code, not from a script):
 *   no args:   name is read at stack index 0 and goes into the next free slot (spline_count)
 *   n >= 1:    slot number is read at index n-1, name at index n (Spline_Filename(slot, name)
 *              when n == 2); spline_count is bumped after the copy
 * Copies the string including its NUL terminator with no bounds check against the slot size.
 * Always returns 0 (no Lua results). */
undefined4 lua_Spline_Filename(char *lua_state)
{
  char ch;
  int arg_count;
  char *src;
  int spline_index;
  char *dst;

  arg_count = lua_gettop(lua_state);
  /* no arguments (inferred): the name is read at index 0 and goes into the next free slot;
   * spline_count is bumped before the copy */
  if (arg_count == 0) {
    src = (char *)lua_tostring(lua_state,0);
    dst = g_course_files->spline_files + g_course_counts->spline_count * SPLINE_FILENAME_STRIDE;
    g_course_counts->spline_count = g_course_counts->spline_count + 1;
    /* copy the name byte by byte, up to and including the NUL */
    do {
      ch = *src;
      src = src + 1;
      *dst = ch;
      dst = dst + 1;
    } while (ch != '\0');
    return 0;
  }
  /* one or more arguments: slot number at index arg_count-1, name at index arg_count;
   * the name is copied into that slot and spline_count is bumped after the copy */
  spline_index = (int)lua_tonumber(lua_state,arg_count + -1);
  src = (char *)lua_tostring(lua_state,arg_count);
  dst = g_course_files->spline_files + spline_index * SPLINE_FILENAME_STRIDE;
  /* copy the name byte by byte, up to and including the NUL */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  g_course_counts->spline_count = g_course_counts->spline_count + 1;
  return 0;
}
