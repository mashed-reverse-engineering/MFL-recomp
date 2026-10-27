#include "mfl_tier2.h"
/*00464aa0*/ extern int powerups_clear();
/*00464c30*/ extern int powerup_place(float *position, undefined4 param_2);
/*00464dd0*/ extern int powerup_set_respawn_time(int index, float respawn);
/* training_powerups_setup */
/* "powerups_clear and places 20 powerups with a 10 second respawn (training layout)" */
void __cdecl training_powerups_setup(undefined4 param_1)
{
  int powerup_index;
  float *position;
  int count;
  /* x, y, z of the 20 training powerups (a local table in the original, built on the stack) */
  float positions[60] = {
    3.85099196f, 0.25f, 15.1630678f,
    4.28567123f, 0.25f, 40.8499451f,
    3.45450711f, 0.25f, 66.1299286f,
    4.45368195f, 0.25f, 92.8549042f,
    3.89412808f, 0.25f, 127.736923f,
    4.72413015f, 0.25f, 149.723816f,
    3.89502811f, 0.25f, 171.904541f,
    5.37399006f, 0.25f, 194.550262f,
    4.0399332f, 0.25f, 223.496613f,
    4.40978622f, 0.224671006f, 249.608978f,
    1.42782795f, 0.224700004f, 254.549286f,
    -5.137918f, 0.224675f, 249.800842f,
    -5.04138994f, 0.25f, 223.149597f,
    -4.28898096f, 0.25f, 202.516632f,
    -5.30803299f, 0.25f, 174.625824f,
    -4.9020319f, 0.25f, 143.545685f,
    -4.60728121f, 0.25f, 113.664558f,
    -6.00728989f, 0.25f, 71.8187637f,
    -4.89531708f, 0.25f, 28.2484474f,
    -3.45005608f, 0.25f, -3.21297503f,
  };
  
  powerups_clear();
  position = positions;
  count = 0x14;
  do {
    powerup_index = powerup_place(position,param_1);
    powerup_set_respawn_time(powerup_index,10.0f);
    position = position + 3;
    count = count + -1;
  } while (count != 0);
  return;
}
