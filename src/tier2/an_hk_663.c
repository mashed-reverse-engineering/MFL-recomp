#include "mfl_tier2.h"
/*0042c440*/ extern int ui_rt2d_text_draw();
/*005dd378*/ extern float k_f_0_1;
/*0064d8bc*/ extern float g_hud_flash_64d8bc;
extern float k_f_n127;
extern double sin(double);
#define HUD_FLASH_TEXT_ID (0x2af)
#define HUD_FLASH_SHADOW_X (0x43a00000) /* 320.0f */
#define HUD_FLASH_SHADOW_Y (0x43be0000) /* 380.0f */
#define HUD_FLASH_WHITE_X (0x439e0000) /* 316.0f */
#define HUD_FLASH_WHITE_Y (0x43bc0000) /* 376.0f */
#define HUD_FLASH_SCALE (0x3f99999a) /* 1.2f, 5th draw arg (inferred: scale) */
#define HUD_FLASH_ALPHA_BASE (0x80U) /* alpha = 128 - pulse */
#define HUD_FLASH_WHITE_RGB (0xffffff)
/* hud_flash_text */

/* "text 0x2AF pulsing (sine alpha) at two offsets, shadow + white" */

/*
 * Draws the HUD_FLASH_TEXT_ID message twice per call: a black shadow at
 * (SHADOW_X, SHADOW_Y) and a white copy at (WHITE_X, WHITE_Y), both with
 * alpha pulsing on a sine. Each call advances g_hud_flash_64d8bc by k_f_0_1
 * (inferred from name: 0.1) as the sine phase. No parameters, no return value.
 */
void hud_flash_text(void)

{
  char pulse;
  struct {
    undefined4 shadow_rgba;
    undefined4 white_rgba;
  } colours;

  pulse = (int)((double)sin((double)g_hud_flash_64d8bc) * (double)k_f_n127);
  g_hud_flash_64d8bc = g_hud_flash_64d8bc + k_f_0_1;
  colours.shadow_rgba = (uint)(byte)(HUD_FLASH_ALPHA_BASE - pulse) << 0x18;
  colours.white_rgba = CONCAT13(HUD_FLASH_ALPHA_BASE - pulse,HUD_FLASH_WHITE_RGB);
  ui_rt2d_text_draw(HUD_FLASH_TEXT_ID,HUD_FLASH_SHADOW_X,HUD_FLASH_SHADOW_Y,colours.shadow_rgba,HUD_FLASH_SCALE,2);
  ui_rt2d_text_draw(HUD_FLASH_TEXT_ID,HUD_FLASH_WHITE_X,HUD_FLASH_WHITE_Y,colours.white_rgba,HUD_FLASH_SCALE,2);
  return;
}


