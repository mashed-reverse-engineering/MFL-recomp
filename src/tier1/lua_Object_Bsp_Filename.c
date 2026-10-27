#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*006f2ae8*/ extern struct CourseFiles *g_course_files;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
#define OBJECT_BSP_SLOT_SIZE (0x40) /* bytes per name in CourseFiles.object_bsp_files (16 slots) */
/* lua_Object_Bsp_Filename */
/* Lua function Object_Bsp_Filename (registered by the script engine).
 * Copies a BSP file name into the course's object_bsp_files list and bumps object_bsp_count.
 * With arguments, the name is the last argument and the slot number is the argument before it
 * (inferred: Object_Bsp_Filename(slot, name)).
 * With no arguments, the name is read from stack index 0 and appended at object_bsp_count.
 * Returns 0 (no Lua results). */
undefined4 lua_Object_Bsp_Filename(char *L)
{
  char ch;
  int arg_count;
  char *src;
  int slot;
  char *dst;

  arg_count = lua_gettop(L);
  /* No arguments: append the name at the current end of the list. */
  if (arg_count == 0) {
    src = (char *)lua_tostring(L,0);
    dst = g_course_files->object_bsp_files + g_course_counts->object_bsp_count * OBJECT_BSP_SLOT_SIZE;
    g_course_counts->object_bsp_count = g_course_counts->object_bsp_count + 1;
    /* Copy the NUL-terminated name, including the terminator. */
    do {
      ch = *src;
      src = src + 1;
      *dst = ch;
      dst = dst + 1;
    } while (ch != '\0');
    return 0;
  }
  /* Slot number from the argument before the name. */
  slot = (int)lua_tonumber(L,arg_count + -1);
  src = (char *)lua_tostring(L,arg_count);
  dst = g_course_files->object_bsp_files + slot * OBJECT_BSP_SLOT_SIZE;
  /* Copy the NUL-terminated name into the slot, including the terminator. */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  g_course_counts->object_bsp_count = g_course_counts->object_bsp_count + 1;
  return 0;
}
