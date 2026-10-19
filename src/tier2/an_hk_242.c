#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00486a60*/ extern int ptank_set_colour();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*006a1818*/ extern undefined4 g_course_6a1818;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*
 * Loads the "Headlight" texture from the texture dictionary, allocates the
 * headlight particle system (0x40 bytes, via the engine's rwmalloc) into
 * g_course_6a1818, sets it up with the texture and sets its colour.
 * Takes no parameters, returns nothing. The 0x807 / 0xa0 arguments to
 * particle_system_setup_ptank are not known (inferred: flags and a size).
 */

/* Headlight */

void load_headlight(void)

{
  RwTexture *headlight_texture;

  headlight_texture = (RwTexture *)find_texture_dict_b("Headlight");
  g_course_6a1818 = RwMalloc(0x40,0);
  particle_system_setup_ptank(g_course_6a1818,0x807,0xa0,headlight_texture);
  ptank_set_colour(g_course_6a1818,0);
  return;
}


