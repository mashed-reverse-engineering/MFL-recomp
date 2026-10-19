#include "mfl_tier2.h"
extern int __cdecl lua_gettop(void *L);
extern double __cdecl lua_tonumber(void *L, int index);
extern char * lua_tostring(void * p0, int p1);
extern struct CourseFiles *g_course_files;
extern struct CourseCounts *g_course_counts;
#define COPTER_FILE_STRIDE (0x40)
/* lua_SetCopter */

/* Lua function SetCopter (registered by the script engine).
 * SetCopter(filename, id): appends a copter file name to the course's copter_files
 * slot at copter_count, stores id in copter_ids for that slot, and bumps copter_count.
 * L is the Lua state; the function returns 0 (no Lua results). Assumes the
 * caller passed at least two arguments (inferred, no check is made). */

undefined4 lua_SetCopter(char *L)

{
  char ch;
  CourseCounts *counts;
  int stack_top;
  char *src;
  char *dst;
  int *copter_count_ptr;

  /* stack_top is the argument count; the file name is the argument below the top */
  stack_top = lua_gettop(L);
  src = (char *)lua_tostring(L,stack_top + -1);
  counts = g_course_counts;
  dst = g_course_files->copter_files + g_course_counts->copter_count * COPTER_FILE_STRIDE;
  /* copy the NUL-terminated file name into the slot, including the terminator */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  /* the id is the top argument; stack_top is reused to hold it */
  copter_count_ptr = &counts->copter_count;
  stack_top = (int)lua_tonumber(L,stack_top);
  g_course_files->copter_ids[*copter_count_ptr] = stack_top;
  *copter_count_ptr = *copter_count_ptr + 1;
  return 0;
}


