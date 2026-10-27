#include "mfl_tier2.h"
/*0042c110*/ extern int text_draw_box();
#define TRAINING_STRING_F2 (0xf2)
#define TRAINING_STRING_232 (0x232)
#define TRAINING_STRING_233 (0x233)
/* training_text_box_a */
/* "strings 0xF2 / 0x232 / 0x233" */
/* Draws the three tutorial text boxes of the training screen (TRAINING_STRING_F2, _232, _233).
   Each box is drawn twice: a black copy offset by (+3, +3) that reads as a drop shadow (inferred),
   then the white copy at the base position. The 7th argument (0.8f) is passed unchanged (inferred: alpha / scale).
   Takes no parameters, returns nothing. */
void training_text_box_a(void)
{
  RwRGBA shadow;
  RwRGBA fill;

  fill.red = 0xff;
  fill.green = 0xff;
  fill.blue = 0xff;
  fill.alpha = 0xc8;
  shadow.red = 0;
  shadow.green = 0;
  shadow.blue = 0;
  shadow.alpha = 0xff;
  text_draw_box(TRAINING_STRING_F2,0x42f60000,0x42a60000,0x43c80000,0x42a00000,*(int *)&shadow,0x3f4ccccd);
  text_draw_box(TRAINING_STRING_F2,0x42f00000,0x42a00000,0x43c80000,0x42a00000,*(int *)&fill,0x3f4ccccd);
  text_draw_box(TRAINING_STRING_232,0x42f60000,0x42e20000,0x43c80000,0x42a00000,*(int *)&shadow,0x3f4ccccd);
  text_draw_box(TRAINING_STRING_232,0x42f00000,0x42dc0000,0x43c80000,0x42a00000,*(int *)&fill,0x3f4ccccd);
  text_draw_box(TRAINING_STRING_233,0x42f60000,0x430f0000,0x43c80000,0x42a00000,*(int *)&shadow,0x3f4ccccd);
  text_draw_box(TRAINING_STRING_233,0x42f00000,0x430c0000,0x43c80000,0x42a00000,*(int *)&fill,0x3f4ccccd);
  return;
}
