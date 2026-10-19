#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;

/* one entry of g_course_files->uva_files: a 64-byte, NUL-terminated file name */
#define UVA_FILE_SLOT_SIZE (0x40)

/* lua_UVA_Filename */

/* Lua function UVA_Filename(slot, name) (registered by the script engine).
   Copies the file name `name` into slot `slot` of g_course_files->uva_files
   (0-based slot number, inferred) and increments g_course_counts->uva_count.
   L is the lua_State; the two arguments are the last two values on its stack.
   Returns 0 (no Lua results). */

undefined4 lua_UVA_Filename(char *L)

{
  char ch;
  int arg_count;
  int uva_index;
  char *src;
  char *dst;

  arg_count = lua_gettop(L);
  uva_index = (int)lua_tonumber(L,arg_count + -1);
  src = (char *)lua_tostring(L,arg_count);
  dst = g_course_files->uva_files + uva_index * UVA_FILE_SLOT_SIZE;
  /* copy the name byte by byte, including its terminating NUL */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  g_course_counts->uva_count = g_course_counts->uva_count + 1;
  return 0;
}


