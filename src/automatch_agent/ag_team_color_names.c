#include "mfl_tier2.h"
/*0040d130*/ extern int __cdecl find_texture_a(RwChar *p0);


/* team_color_names */

/* "NFLRed NFLBluejay NFLMelon NFLGold" */

/* Returns the colour texture (callers use eax); an id above 9 returns the id itself, as the original's eax (the switch index) does. */
int team_color_names(undefined4 param_1)

{
  switch(param_1) {
  case 0:
    return find_texture_a("NFLRed");
  case 1:
    return find_texture_a("NFLBluejay");
  case 2:
    return find_texture_a("NFLMelon");
  case 3:
    return find_texture_a("NFLGold");
  case 4:
    return find_texture_a("NFLPink");
  case 5:
    return find_texture_a("NFLShadow");
  case 6:
    return find_texture_a("NFLCopter");
  case 7:
    return find_texture_a("NFLBomb");
  case 8:
    return find_texture_a("NFLFugitive");
  case 9:
    return find_texture_a("NFLSurvival");
  default:
    return param_1;
  }
}
