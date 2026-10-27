#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/*006f2af4*/ extern undefined4 g_sky_count;
#define SKY_FILENAME_SIZE (0x40)
/* lua_Sky_Filename */
/* Lua function Sky_Filename (registered by the script engine)
 *
 * Copies a sky texture filename into g_course_files->sky_files, one slot
 * of SKY_FILENAME_SIZE bytes per sky. Returns 0 (no Lua results).
 *   Sky_Filename(name)        - appends to the next free slot (g_sky_count)
 *   Sky_Filename(index, name) - writes to slot `index` (inferred: the
 *                               number is the second-to-last argument)
 * lua_state: Lua state passed in by the engine.
 * No bounds check on the slot index or the copy length (inferred from the code). */
undefined4 lua_Sky_Filename(char *lua_state)
{
  char ch;
  int arg_count;
  char *src;
  int sky_index;
  char *dst;

  arg_count = lua_gettop(lua_state);
  if (arg_count == 0) {
    src = (char *)lua_tostring(lua_state,0);
    dst = g_course_files->sky_files + g_sky_count * SKY_FILENAME_SIZE;
    g_sky_count = g_sky_count + 1;
    do {
      ch = *src;
      src = src + 1;
      *dst = ch;
      dst = dst + 1;
    } while (ch != '\0');
    return 0;
  }
  /* Two or more arguments: the second-to-last is the slot index, the last is the name. */
  sky_index = (int)lua_tonumber(lua_state,arg_count + -1);
  src = (char *)lua_tostring(lua_state,arg_count);
  dst = g_course_files->sky_files + sky_index * SKY_FILENAME_SIZE;
  /* Copy the string, including its terminating NUL. */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  g_sky_count = g_sky_count + 1;
  return 0;
}
