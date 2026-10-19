#include "mfl_tier2.h"
extern char UNK_005df51c[];
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00449700*/ extern undefined __cdecl course_sky_atomic_create(struct RwTexture *p0, int p1, undefined4 p2);
/*008c5720*/ extern undefined4 g_course_nt1_8c5720;
#define FLOAT_BITS_ONE (0x3f800000)
#define FLOAT_BITS_NEG_TWO (0xc0000000)
/*
 * course_nt1_s_cb0 (inferred): course setup callback for NT1 that creates the smoke sky atomic.
 * course_state: course/game state pointer (inferred), passed to func_0x00455770.
 * Reads the u32 at course_state+0xc9d0 (inferred: course data) and stores func_0x004849e0's
 * result in g_course_nt1_8c5720. Returns nothing.
 */

void course_nt1_s_cb0(char *course_state)

{
  RwTexture *smoke_texture;
  int sky_scale;
  undefined4 sky_offset;

  func_0x00455770(course_state);
  g_course_nt1_8c5720 = func_0x004849e0(&UNK_005df51c,*(undefined4 *)(course_state + 0xc9d0));
  /* Sky atomic: "smoke" texture, scale 1.0f and offset -2.0f as float bit patterns (inferred) */
  sky_offset = FLOAT_BITS_NEG_TWO;
  sky_scale = FLOAT_BITS_ONE;
  smoke_texture = (RwTexture *)find_texture_dict_b("smoke");
  course_sky_atomic_create(smoke_texture,sky_scale,sky_offset);
  return;
}


