#include "mfl_tier2.h"
/*00434bb0*/ extern int get_event_type();
/*004c4050*/ extern int FUN_004c4050(undefined4 p0);
/*005dd338*/ extern float k_f_0_01;
/*0064cbc0*/ extern undefined4 g_cup_64cbc0;
/*0064cbd0*/ extern float g_cup_64cbd0;
/*007ef7d8*/ extern undefined4 g_camera_7ef7d8;

/* colours written to the light block (inferred ARGB, alpha 0xff) */
#define CUP_LIGHT_COLOUR_EVENT_3 (0xff0a66d8)
#define CUP_LIGHT_COLOUR_EVENT_4 (0xfff5f0f0)
#define CUP_LIGHT_COLOUR_DEFAULT (0xff14c5ff)
/* offset of the colour word inside the light block */
#define LIGHT_BLOCK_COLOUR_OFFSET (4)

/*
 * Per-frame update of the cup scene light colour (inferred).
 * Stores a float in g_cup_64cbd0 derived from the camera value (negated, scaled by k_f_0_01).
 * Looks up the light block through FUN_004c4050(g_cup_64cbc0) and writes one ARGB colour
 * into it, chosen by the current event type: 3 -> CUP_LIGHT_COLOUR_EVENT_3,
 * 4 -> CUP_LIGHT_COLOUR_EVENT_4, anything else -> CUP_LIGHT_COLOUR_DEFAULT.
 * Takes no parameters and returns nothing.
 * (was: "inferred: colour depends on the event type (3 -> 0xFF0A66D8) and the race clock";
 * the body reads no race clock, it reads the camera value)
 */
void cup_scene_update_light_colour(void)

{
  int light_block;
  int event_type;
  RwRGBA colour_event_3;
  RwRGBA colour_event_4;
  RwRGBA colour_default;

  /* camera value negated and scaled into g_cup_64cbd0 (meaning unknown) */
  g_cup_64cbd0 = (float)(-(int)g_camera_7ef7d8) * k_f_0_01;
  light_block = FUN_004c4050(g_cup_64cbc0);
  colour_event_3.red = 0xd8;
  colour_event_3.green = 0x66;
  colour_event_3.blue = 0x0a;
  colour_event_3.alpha = 0xff;
  colour_event_4.red = 0xf0;
  colour_event_4.green = 0xf0;
  colour_event_4.blue = 0xf5;
  colour_event_4.alpha = 0xff;
  colour_default.red = 0xff;
  colour_default.green = 0xc5;
  colour_default.blue = 0x14;
  colour_default.alpha = 0xff;
  /* event type 3: blue colour */
  event_type = get_event_type();
  if (event_type == 3) {
    *(int *)(light_block + LIGHT_BLOCK_COLOUR_OFFSET) = *(int *)&colour_event_3;
    return;
  }
  event_type = get_event_type();
  if (event_type == 4) {
    *(int *)(light_block + LIGHT_BLOCK_COLOUR_OFFSET) = *(int *)&colour_event_4;
    return;
  }
  *(int *)(light_block + LIGHT_BLOCK_COLOUR_OFFSET) = *(int *)&colour_default;
  return;
}


