#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00449700*/ extern undefined __cdecl course_sky_atomic_create(struct RwTexture *p0, int p1, undefined4 p2);

/* IEEE-754 single-precision bit patterns, passed as integers */
#define FLOAT_ONE_BITS (0x3f800000)
#define FLOAT_NEG_TWO_BITS (0xc0000000)

/* course_angels_cb0
 * Course setup for Angels: looks up the "smoke" texture in the texture dictionary
 * and creates the course sky atomic from it (inferred: the two float arguments are
 * a scale of 1.0 and a bias of -2.0, from the bit patterns). Takes no parameters, returns nothing.
 */

void course_angels_cb0(void)

{
  RwTexture *smoke_texture;
  int sky_scale_bits;
  undefined4 sky_bias_bits;

  sky_bias_bits = FLOAT_NEG_TWO_BITS;
  sky_scale_bits = FLOAT_ONE_BITS;
  course_sky_atomic_create((RwTexture *)find_texture_dict_b("smoke"),FLOAT_ONE_BITS,FLOAT_NEG_TWO_BITS);
  return;
}


