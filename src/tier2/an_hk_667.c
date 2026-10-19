#include "mfl_tier2.h"
typedef double unkbyte10;
/*005deb2c*/ extern float k_f_1_57;
/*007eebc0*/ extern struct GameState g_game_state;
extern double sin(double);
/* option_get_volume_1 */

/* "sin(slider[1] * pi/2)" */

/* Returns the volume for option slider 1: sin(slider * pi/2). The slider is a float
   stored in the uint slot g_game_state.option_sliders[1] (inferred: range 0..1). */
unkbyte10 option_get_volume_1(void)

{
  unkbyte10 volume;

  volume = sin((float10)*(float *)&g_game_state.option_sliders[1] * (float10)k_f_1_57);
  return volume;
}


