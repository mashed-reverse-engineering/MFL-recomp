#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/* general_anim_files is an array of fixed-size name slots, this many bytes each (8 slots) */
#define GENERAL_ANIM_SLOT_SIZE (0x40)
/* lua_General_Anim_Filename */
/* Lua function General_Anim_Filename (registered by the script engine).
 * Stores a general animation file name from COURSE.LUA into g_course_files->general_anim_files
 * and increments g_course_counts->general_anim_count.
 * lua_state: the Lua state the script engine passes in.
 * Two call forms (from the code, inferred):
 *   General_Anim_Filename(name)         - no index: the name goes into the next free slot
 *                                         (general_anim_count), name read from index 0.
 *   General_Anim_Filename(index, name)  - the number argument picks the slot, the string is the last argument.
 * The count is only incremented, never set, so out-of-order indices leave it out of step with the slots.
 * Returns 0 (no Lua results).
 */
undefined4 lua_General_Anim_Filename(char *lua_state)
{
  char ch;
  int arg_count;
  char *src;
  int anim_index;
  char *dst;

  arg_count = lua_gettop(lua_state);
  /* No-argument form (gettop == 0): append the name at the next free slot. */
  if (arg_count == 0) {
    src = (char *)lua_tostring(lua_state,0);
    dst = g_course_files->general_anim_files + g_course_counts->general_anim_count * GENERAL_ANIM_SLOT_SIZE;
    g_course_counts->general_anim_count = g_course_counts->general_anim_count + 1;
    /* Copy the name byte by byte, including the terminating NUL. */
    do {
      ch = *src;
      src = src + 1;
      *dst = ch;
      dst = dst + 1;
    } while (ch != '\0');
    return 0;
  }
  /* Index form: second-to-last argument is the slot number, last argument is the name. */
  anim_index = (int)lua_tonumber(lua_state,arg_count + -1);
  src = (char *)lua_tostring(lua_state,arg_count);
  dst = g_course_files->general_anim_files + anim_index * GENERAL_ANIM_SLOT_SIZE;
  /* Copy the name byte by byte, including the terminating NUL. */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  g_course_counts->general_anim_count = g_course_counts->general_anim_count + 1;
  return 0;
}
