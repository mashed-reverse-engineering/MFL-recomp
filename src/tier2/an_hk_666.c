#include "mfl_tier2.h"
typedef double unkbyte10;
/*005deb2c*/ extern float k_f_1_57;
/*007eebc0*/ extern struct GameState g_game_state;
extern double sin(double);
/* option_get_volume_0 */

/* "sin(slider[0] * pi/2): music / master volume curve" */

/* Returns the music / master volume multiplier for the options menu, read from
 * the first option slider (option_sliders[0], float bits, range 0..1 (inferred)).
 * Volume = sin(slider * pi/2): 0 at slider 0, 1 at slider 1, steepest near 0.
 * No parameters. Returns a double (float10 in Ghidra). */
unkbyte10 option_get_volume_0(void)

{
  unkbyte10 volume;

  volume = sin((float10)*(float *)&g_game_state.option_sliders[0] * (float10)k_f_1_57);
  return volume;
}


