#include "mfl_tier2.h"
/*004693c0*/ extern int sound_streams_pause();
/*00469400*/ extern int sound_streams_resume();
/*00473550*/ extern undefined __cdecl sound_frame_begin(undefined4 p0);
/*004a41f0*/ extern int timer_ticks();
/*004a5b50*/ extern int __cdecl joypad_get_field_770fe8(undefined4 p0);
/*0062ad30*/ extern undefined4 g_debug_keys_62ad30;
/*0076e804*/ extern undefined4 g_debug_keys_76e804;
/*0076e808*/ extern int g_debug_keys_76e808;
/*0076e820*/ extern int g_debug_keys_76e820;

#define PROFILE_FRAMES (60)
#define JOYPAD_FIELD_ARM_KEY (3) /* inferred: pressing it arms the debug flag */
#define JOYPAD_FIELD_CANCEL_KEY (4) /* inferred: pressing it disarms the flag */

/* debug_keys_and_profile */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* reads two debug keys and averages the frame time over PROFILE_FRAMES frames.
 * Called once per frame; takes no parameters and returns nothing.
 * JOYPAD_FIELD_ARM_KEY sets g_debug_keys_62ad30. While that flag is set and
 * JOYPAD_FIELD_CANCEL_KEY is not held, the sound streams are paused, one sound
 * frame is begun, and the streams are resumed (inferred: single-step audio debugging).
 * Holding JOYPAD_FIELD_CANCEL_KEY clears the flag.
 * Every frame, the timer delta since entry is added to g_debug_keys_76e820. After
 * PROFILE_FRAMES frames the average goes to g_debug_keys_76e808 and the accumulators reset. */

void debug_keys_and_profile(void)

{
  int frame_start_ticks;
  int result;

  /* sample the timer, then read the arm key */
  frame_start_ticks = timer_ticks();
  result = joypad_get_field_770fe8(JOYPAD_FIELD_ARM_KEY);
  /* arm key pressed: set the debug flag */
  if (result != 0) {
    g_debug_keys_62ad30 = 1;
  }
  /* cancel key not held: step the sound if armed */
  result = joypad_get_field_770fe8(JOYPAD_FIELD_CANCEL_KEY);
  if (result != 0) {
    g_debug_keys_62ad30 = 0;
  }
  else if (g_debug_keys_62ad30 != 0) {
    sound_streams_pause(0x0);
    sound_frame_begin(0);
    sound_streams_resume();
  }
  result = timer_ticks();
  g_debug_keys_76e820 = g_debug_keys_76e820 + (result - frame_start_ticks);
  g_debug_keys_76e804 = g_debug_keys_76e804 + 1;
  if (g_debug_keys_76e804 == PROFILE_FRAMES) {
    g_debug_keys_76e808 = g_debug_keys_76e820 / PROFILE_FRAMES;
    g_debug_keys_76e820 = 0;
    g_debug_keys_76e804 = 0;
  }
  return;
}


