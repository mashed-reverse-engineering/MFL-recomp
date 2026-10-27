#include "mfl_tier2.h"
/*00481fd0*/ extern float random_range(float lo, float hi);
/*005dd370*/ extern float k_f_0_3;
/* Maps an ARGB colour constant to a palette index (white picks index 15 or 16 at random). */
unsigned int color_code_to_index(unsigned int c)
{
  switch (c) {
  case 0xff1a3380: return 12;
  case 0xff0080c8: case 0xff005ab4: case 0xff0064b4: case 0xff14508c: case 0xff80c800: return 14;
  case 0xff646464: return 1;
  case 0xff968080: return 6;
  case 0xffa08080: return 7;
  case 0xffb48080: return 9;
  case 0xffc88080: case 0xff141414: return 10;
  case 0xffdc8080: return 8;
  case 0xffff8040: return 13;
  case 0xffffffff: if (random_range(0.0f, 1.0f) > k_f_0_3) return 16; return 15;
  case 0xff961e5a: case 0xffc8c8c8: case 0xffc81e5a: case 0xffb4b4b4: return 15;
  case 0xffe00000: return 0;
  }
  return 0;
}
