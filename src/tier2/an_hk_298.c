#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;

#define FILENAME_SLOT_SIZE (0x40) /* bytes per filename in the course's powerup_files array (8 slots) */

/* lua_Powerup_Filename */

/* Lua function Powerup_Filename (registered by the script engine)
 *
 * Copies the filename given as the last argument into the next free slot of
 * g_course_files->powerup_files (slot = powerup_file_count) and increments
 * powerup_file_count. lua_state is the Lua state the script engine passes in.
 * Returns 0 (no Lua return values). (inferred: called once per Powerup_Filename line of COURSE.LUA)
 * No bounds check: more than 8 names overflow powerup_files, and a name of 64 or more
 * bytes (including the NUL) overruns its slot. */

undefined4 lua_Powerup_Filename(char *lua_state)

{
  char ch;
  int powerup_index;
  undefined4 last_arg_index;
  char *name;
  char *dest;

  last_arg_index = lua_gettop(lua_state);
  powerup_index = g_course_counts->powerup_file_count;
  name = (char *)lua_tostring(lua_state,last_arg_index);
  dest = g_course_files->powerup_files + powerup_index * FILENAME_SLOT_SIZE;
  /* copy the string byte by byte, including its terminating NUL */
  do {
    ch = *name;
    name = name + 1;
    *dest = ch;
    dest = dest + 1;
  } while (ch != '\0');
  g_course_counts->powerup_file_count = g_course_counts->powerup_file_count + 1;
  return 0;
}


