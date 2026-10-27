#include "mfl_tier2.h"
/*0042c110*/ extern int text_draw_box();
#define TRAINING_STRING_22F (0x22f)
#define TRAINING_STRING_230 (0x230)
/* training_text_box_b */
/* "two text_draw_box layers of strings 0x22F / 0x230 (tutorial subtitles)" */
/* Draws the two tutorial subtitle boxes of the training screen (TRAINING_STRING_22F, TRAINING_STRING_230).
   Each box is drawn twice: a black copy offset by (+3, +3) that reads as a drop shadow (inferred),
   then the fill copy at the base position. The fill colour differs from training_text_box_a (inferred: highlight colour).
   Takes no parameters, returns nothing. */
void training_text_box_b(void)
{
  RwRGBA alt;
  RwRGBA shadow;

  alt.red = 0xd8;
  alt.green = 0xd8;
  alt.blue = 0;
  alt.alpha = 0xc8;
  shadow.red = 0;
  shadow.green = 0;
  shadow.blue = 0;
  shadow.alpha = 0xff;
  text_draw_box(TRAINING_STRING_22F,0x42f60000,0x42a60000,0x43c80000,0x42a00000,*(int *)&shadow,0x3f4ccccd);
  text_draw_box(TRAINING_STRING_22F,0x42f00000,0x42a00000,0x43c80000,0x42a00000,*(int *)&alt,0x3f4ccccd);
  text_draw_box(TRAINING_STRING_230,0x42f60000,0x430f0000,0x43c80000,0x42a00000,*(int *)&shadow,0x3f4ccccd);
  text_draw_box(TRAINING_STRING_230,0x42f00000,0x430c0000,0x43c80000,0x42a00000,*(int *)&alt,0x3f4ccccd);
  return;
}
