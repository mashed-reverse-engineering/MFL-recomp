#include "mfl_tier2.h"
/*004b322b*/ extern __declspec(noreturn) void __cdecl exit(int);
#define RGBA_ALPHA_OPAQUE (0xff)
#define EXIT_BAD_TEAM (0xfffffff2)
/* team_color_rgba */
/* Writes the RGBA colour of a team into rgba[0..3] (one byte each).
 * team: 0..5 selects the palette entry; any other value calls exit(-14) and never returns.
 * rgba: 4-byte output buffer.
 * (was: "(team, out): switch giving the RGBA of a team colour") */
void team_color_rgba(undefined4 team,undefined1 *rgba)
{
  /* One case per team palette entry (inferred: 0 red, 1 blue, 2 olive, 3 yellow, 4 pink, 5 black). Alpha is always opaque. */
  switch(team) {
  case 0:
    *rgba = 0x98;
    rgba[1] = 0x3a;
    rgba[2] = 0x3d;
    rgba[3] = RGBA_ALPHA_OPAQUE;
    return;
  case 1:
    *rgba = 0x4e;
    rgba[1] = 0x89;
    rgba[2] = 0xae;
    rgba[3] = RGBA_ALPHA_OPAQUE;
    return;
  case 2:
    *rgba = 0x61;
    rgba[1] = 0x76;
    rgba[2] = 0x56;
    rgba[3] = RGBA_ALPHA_OPAQUE;
    return;
  case 3:
    *rgba = 0xdb;
    rgba[1] = 0xc3;
    rgba[2] = 0x62;
    rgba[3] = RGBA_ALPHA_OPAQUE;
    return;
  case 4:
    *rgba = 0xeb;
    rgba[1] = 0xa7;
    rgba[2] = 0xa7;
    rgba[3] = RGBA_ALPHA_OPAQUE;
    return;
  case 5:
    *rgba = 0;
    rgba[1] = 0;
    rgba[2] = 0;
    rgba[3] = RGBA_ALPHA_OPAQUE;
    return;
  default:
                    /* WARNING: Subroutine does not return */
    exit(EXIT_BAD_TEAM);
  }
}
