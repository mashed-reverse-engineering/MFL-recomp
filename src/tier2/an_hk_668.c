#include "mfl_tier2.h"
typedef double unkbyte10;
/*005deb2c*/ extern float k_f_1_57;
/*007eebc0*/ extern struct GameState g_game_state;
extern double sin(double);

/* Option slider index holding the volume setting (inferred from the function name) */
#define OPTION_SLIDER_VOLUME (2)

/* option_get_volume_2 */

/* "sin(slider[2] * pi/2)" */

/* Returns the volume gain for the volume option slider: sin(slider * pi/2).
   The slider value is a float stored in the uint option_sliders array; it is
   scaled by pi/2 (k_f_1_57) and passed through sin(). Takes no parameters. */
unkbyte10 option_get_volume_2(void)

{
  unkbyte10 volume;

  volume = sin((float10)*(float *)&g_game_state.option_sliders[OPTION_SLIDER_VOLUME] * (float10)k_f_1_57);
  return volume;
}


