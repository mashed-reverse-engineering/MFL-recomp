#include "mfl_tier2.h"
/*008c8860*/ extern undefined4 g_course_8c8860;
/*008c88a4*/ extern undefined4 g_game_8c88a4;
/*008c88b0*/ extern undefined4 g_course_8c88b0;
/*008c88f4*/ extern undefined4 g_game_8c88f4;
/*008c8900*/ extern undefined4 g_course_active_8c8900;
/*008c8944*/ extern undefined4 g_game_8c8944;
/*008c8950*/ extern undefined4 g_course_active_8c8950;
/*008c8994*/ extern undefined4 g_game_8c8994;
/*008c89a0*/ extern undefined4 g_course_active_8c89a0;
/*008c89e4*/ extern undefined4 g_game_8c89e4;
/*008c89f0*/ extern undefined4 g_course_active_8c89f0;
/*008c8a34*/ extern undefined4 g_game_8c8a34;
/*008c8a40*/ extern undefined4 g_course_active_8c8a40;
/*008c8a84*/ extern undefined4 g_game_8c8a84;
/*008c8a90*/ extern undefined4 g_course_active_8c8a90;
/*008c8ad4*/ extern undefined4 g_game_8c8ad4;


/* course_active_objects_count */

/* "counts the course object slots (0x14 dwords each) that have a clump and an atomic" */

/* Counts the course object slots that have both a game pointer (g_game_*) and a course
   pointer (g_course_* / g_course_active_*); the 8 slot pairs are checked one by one. */
int course_active_objects_count(void)
{
  int count;                           /* number of slots that are in use */

  count = 0;
  if ((g_game_8c88a4 != 0) && (g_course_8c8860 != 0)) {
    count = 1;
  }
  if ((g_game_8c88f4 != 0) && (g_course_8c88b0 != 0)) {
    count = count + 1;
  }
  if ((g_game_8c8944 != 0) && (g_course_active_8c8900 != 0)) {
    count = count + 1;
  }
  if ((g_game_8c8994 != 0) && (g_course_active_8c8950 != 0)) {
    count = count + 1;
  }
  if ((g_game_8c89e4 != 0) && (g_course_active_8c89a0 != 0)) {
    count = count + 1;
  }
  if ((g_game_8c8a34 != 0) && (g_course_active_8c89f0 != 0)) {
    count = count + 1;
  }
  if ((g_game_8c8a84 != 0) && (g_course_active_8c8a40 != 0)) {
    count = count + 1;
  }
  if ((g_game_8c8ad4 != 0) && (g_course_active_8c8a90 != 0)) {
    count = count + 1;
  }
  return count;
}
