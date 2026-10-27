#include "mfl_tier2.h"
/*00475730*/ extern char *get_viewport_camera();
/*004a1880*/ extern char *game_get_62ad2c();
/*00668c18*/ extern struct CourseWorld g_course_world;
/*008c8840*/ extern float g_course_8c8840;
/*008c8844*/ extern undefined4 g_course_8c8844;
/*008c8848*/ extern undefined4 g_course_8c8848;
/*008c884c*/ extern undefined4 g_course_fog_8c884c;
/*008c8850*/ extern undefined4 g_course_8c8850;
/*008c8854*/ extern undefined4 g_course_8c8854;
/*008c8858*/ extern undefined4 g_course_8c8858;
/* course_fog_apply */

/* "copies the course fog settings and the viewport colour into the render state at 0x8C8840" */
/*
 * Copies the course fog values from g_course_world (fog_a[1], fog_a[2]) and a value from the
 * viewport camera (camera + 0x80, inferred: fog-related) into the fog globals at 0x8C8840..0x8C884C,
 * then copies the three colour bytes returned by game_get_62ad2c() into g_course_8c8850/54/58.
 * Takes no parameters and returns nothing.
 */

void course_fog_apply(void)

{
  char *camera;
  byte *viewport_colour;
  
  g_course_8c8844 = *(undefined4 *)&g_course_world.fog_a[2];
  camera = get_viewport_camera(0);
  *(undefined4 *)&g_course_8c8840 = *(undefined4 *)(camera + 0x80);
  g_course_8c8848 = *(undefined4 *)&g_course_world.fog_a[1];
  g_course_fog_8c884c = g_course_8c8844;
  /* viewport colour: bytes 0, 1, 2 of the object from game_get_62ad2c() (channel order inferred) */
  viewport_colour = (byte *)game_get_62ad2c();
  g_course_8c8850 = (uint)*viewport_colour;
  camera = game_get_62ad2c();
  g_course_8c8854 = (uint)*(byte *)(camera + 1);
  camera = game_get_62ad2c();
  g_course_8c8858 = (uint)*(byte *)(camera + 2);
  return;
}


