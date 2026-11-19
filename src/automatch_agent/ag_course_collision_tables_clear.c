#include "mfl_tier2.h"
/*006f2d88*/ extern undefined4 g_course_collision_6f2d88;
/*006f3588*/ extern undefined4 g_course_collision_6f3588;
/*006f35c8*/ extern undefined4 g_course_6f35c8;
/*006f4b48*/ extern undefined4 g_course_6f4b48;
/*006f690c*/ extern undefined4 g_course_6f690c;


/* course_collision_tables_clear */

/* "clears 0x560 dwords at 0x6F35C8" */

void course_collision_tables_clear(void)

{
  int dword_count;
  undefined4 *cursor;
  
  g_course_6f690c = 0;
  cursor = &g_course_6f35c8;
  for (dword_count = 0x560; dword_count != 0; dword_count = dword_count + -1) {
    *cursor = 0;
    cursor = cursor + 1;
  }
  cursor = &g_course_collision_6f2d88;
  for (dword_count = 0x200; dword_count != 0; dword_count = dword_count + -1) {
    *cursor = 0;
    cursor = cursor + 1;
  }
  cursor = &g_course_collision_6f3588;
  for (dword_count = 0x10; dword_count != 0; dword_count = dword_count + -1) {
    *cursor = 0;
    cursor = cursor + 1;
  }
  cursor = &g_course_6f4b48;
  for (dword_count = 0x771; dword_count != 0; dword_count = dword_count + -1) {
    *cursor = 0;
    cursor = cursor + 1;
  }
  return;
}
