#include "mfl_tier2.h"
/*0062ac2c*/ extern float g_app_62ac2c;
/*0062ac30*/ extern float g_app_62ac30;
/*0062ac34*/ extern undefined4 g_app_62ac34;
/*0062ac38*/ extern undefined4 g_app_62ac38;
/*0062ac3c*/ extern undefined4 g_app_62ac3c;
/*0062ac40*/ extern undefined4 g_app_62ac40;
/*0062ac44*/ extern undefined4 g_app_62ac44;
/*0062ac48*/ extern undefined4 g_app_62ac48;
/*0062ac4c*/ extern undefined4 g_app_62ac4c;
/*0062ac50*/ extern undefined4 g_app_62ac50;
/*0062ac54*/ extern undefined4 g_app_62ac54;
/*0062ac58*/ extern undefined4 g_app_62ac58;
/*0062ac5c*/ extern undefined4 g_app_62ac5c;
/*0062ac60*/ extern undefined4 g_app_62ac60;
/*0062c5d0*/ extern undefined1 g_app_62c5d0;
/*0062c5d1*/ extern undefined1 g_app_62c5d1;
/*0062c5d2*/ extern undefined1 g_app_62c5d2;
/*0062c5d3*/ extern undefined1 g_app_62c5d3;
/*0062c5d4*/ extern undefined g_ui_colour_62c5d4;
/*0062c5d5*/ extern undefined g_ui_colour_62c5d5;
/*0062c5d6*/ extern undefined g_ui_colour_62c5d6;
/*0062c5d7*/ extern undefined g_ui_colour_62c5d7;
/*0076e3c4*/ extern undefined4 g_app_76e3c4;
/*0076e3c8*/ extern undefined4 g_app_76e3c8;

/* Default colour channels written by ui_colour_a_default (inferred: R, G, B, A from the g_ui_colour_* names) */
#define UI_COLOUR_DEFAULT_RED (0x78)
#define UI_COLOUR_DEFAULT_GREEN (0x80)
#define UI_COLOUR_DEFAULT_BLUE (0x88)
#define UI_COLOUR_DEFAULT_ALPHA (0x40)

/* ui_colour_a_default */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "g_app_62c5d0..d3 = (0x78, 0x80, 0x88, 0x40)" */

/*
 * Resets the UI colour and tuning globals to their default values. Takes no
 * parameters and returns nothing. Writes two colour records (g_app_62c5d0..d3
 * and g_ui_colour_62c5d4..d7; the second has alpha 0) and a set of float
 * tuning values (g_app_62ac2c..62ac60, g_app_76e3c4/c8) (inferred roles).
 */
void ui_colour_a_default(void)

{
  /* colour record 1: RGBA, alpha 0x40 */
  g_app_62c5d0 = UI_COLOUR_DEFAULT_RED;
  g_app_62c5d1 = UI_COLOUR_DEFAULT_GREEN;
  g_app_62c5d2 = UI_COLOUR_DEFAULT_BLUE;
  g_app_62c5d3 = UI_COLOUR_DEFAULT_ALPHA;
  /* colour record 2: same RGB, alpha 0 */
  g_ui_colour_62c5d4 = UI_COLOUR_DEFAULT_RED;
  g_ui_colour_62c5d5 = UI_COLOUR_DEFAULT_GREEN;
  g_ui_colour_62c5d6 = UI_COLOUR_DEFAULT_BLUE;
  g_ui_colour_62c5d7 = 0;
  /* float globals (values decoded from the bit patterns): +/-4.0f pairs, then 0.5f, 1.1f, 0.7f, 1.0f, 0.0f, 0.1f */
  g_app_62ac3c = 0xc0800000;
  g_app_62ac40 = 0x40800000;
  g_app_76e3c4 = 0;
  g_app_76e3c8 = 0;
  g_app_62ac44 = 0xc0800000;
  g_app_62ac48 = 0x40800000;
  g_app_62ac4c = 0x3f000000;
  g_app_62ac50 = 0x3f8ccccd;
  g_app_62ac54 = 0x3f333333;
  g_app_62ac58 = 0x3f800000;
  g_app_62ac5c = 0;
  g_app_62ac60 = 0x3dcccccd;
  /* 0.25f pair, then 3.0f and 6.0f bit patterns */
  g_app_62ac30 = 0.25f;
  g_app_62ac2c = 0.25f;
  g_app_62ac34 = 0x40400000;
  g_app_62ac38 = 0x40c00000;
  return;
}


