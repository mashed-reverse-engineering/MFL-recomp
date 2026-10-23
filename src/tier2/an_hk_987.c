#include "mfl_tier2.h"
/*0042c440*/ extern int ui_rt2d_text_draw(int text_id, float x, float y, uint colour, float scale, int flags);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd61c*/ extern float k_f_0_001;
/*005dd634*/ extern float k_f_2;
/*005dd7e8*/ extern float k_f_60;
/*005dd7fc*/ extern float k_f_0_625;
/*005dd800*/ extern float k_f_120;
/*005ddc28*/ extern float k_f_0;
/*0064d8d0*/ extern float g_hud_bomb_64d8d0;
/*007ef7f4*/ extern float g_tick_seconds;
/*0041a370*/ extern float ai_get_counter();
/* hud_bomb_timer_text */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "inferred: 60.0 second text handling driven by the float at FUN_0041a370(0)" */

void hud_bomb_timer_text(void)

{
  float fVar1;
  double fVar2;
  float local_4;
  
  local_4 = 60.0;
  fVar2 = (double)ai_get_counter(0);
  if (fVar2 <= (double)k_f_2) {
    g_hud_bomb_64d8d0 = 0.0;
  }
  else {
    if (g_hud_bomb_64d8d0 == k_f_0) {
      g_hud_bomb_64d8d0 = 3.0;
    }
    g_hud_bomb_64d8d0 = g_hud_bomb_64d8d0 - g_tick_seconds;
    if (g_hud_bomb_64d8d0 < k_f_0) {
      g_hud_bomb_64d8d0 = 0.001;
      return;
    }
    if (k_f_0_001 < g_hud_bomb_64d8d0) {
      if (g_hud_bomb_64d8d0 * k_f_0_625 < k_f_0_5) {
        fVar1 = float_1_0 - g_hud_bomb_64d8d0 * k_f_0_625;
        local_4 = (float_1_0 - (fVar1 + fVar1)) * k_f_120 + k_f_60;
      }
      ui_rt2d_text_draw(0x22d,323.0f,local_4 + k_f_3 + k_f_60,0xff000000,0.9f,2);      /* shadow */
      ui_rt2d_text_draw(0x22d,320.0f,local_4 + k_f_60,0xc800d805,0.9f,2);
      return;
    }
  }
  return;
}


