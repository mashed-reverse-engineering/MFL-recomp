#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00449700*/ extern undefined __cdecl course_sky_atomic_create(struct RwTexture *p0, int p1, undefined4 p2);
/* course_sandsnow_cb0
 * Sets up the sky for the SANDSNOW course (inferred from name): looks up the "smoke"
 * texture dictionary entry and creates the sky atomic with scale 1.0f and offset -2.0f
 * (inferred: 0x3f800000 and 0xc0000000 are the IEEE bit patterns of 1.0f and -2.0f).
 * course_ctx: course/world context passed to the two setup calls (inferred).
 * Returns nothing.
 */

void course_sandsnow_cb0(undefined4 course_ctx)

{
  RwTexture *texture_dict;
  int sky_scale;
  undefined4 sky_offset;

  /* course setup calls; (5, 0) args meaning unknown */
  func_0x00458450(course_ctx,5,0);
  func_0x004584b0(course_ctx);
  /* sky params as float bits: -2.0f offset, 1.0f scale (inferred) */
  sky_offset = 0xc0000000;
  sky_scale = 0x3f800000;
  course_sky_atomic_create((RwTexture *)find_texture_dict_b("smoke"),0x3f800000,0xc0000000);
  return;
}


