#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/* lua_Texture_Dictionary_Filename */

/* Lua function Texture_Dictionary_Filename (registered by the script engine) */

/*
 * Lua binding: Texture_Dictionary_Filename(name). Copies the string argument
 * (last argument on the stack) byte by byte, including the terminating NUL,
 * into g_course_files->texture_dictionary (64 bytes). Returns 0 to Lua.
 * No length check (inferred): a name of 64+ chars overruns into sky_files.
 */
undefined4 lua_Texture_Dictionary_Filename(char *lua_state)

{
  char ch;
  undefined4 last_arg_index;
  char *src;
  CourseFiles *dst;

  last_arg_index = lua_gettop(lua_state);
  src = (char *)lua_tostring(lua_state,last_arg_index);
  dst = g_course_files;
  /* copy loop: one byte per iteration, stops after the NUL is copied */
  do {
    ch = *src;
    src = src + 1;
    dst->texture_dictionary[0] = ch;
    dst = (CourseFiles *)(dst->texture_dictionary + 1);
  } while (ch != '\0');
  return 0;
}


