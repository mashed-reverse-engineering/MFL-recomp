#include "mfl_tier2.h"
/*00486ef0*/ extern int particles_render();
/*004c65d0*/ extern void FUN_004c65d0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3, undefined4 p4);
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd9ac*/ extern float k_f_255;
/*006a5d2c*/ extern undefined4 g_weapon_flashbang_6a5d2c;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008bf000*/ extern undefined g_weapon_8bf000;
/*006a5d8c*/ extern char g_weapon_flashbang_6a5d8c[];
#define FLASH_STATE_ACTIVE (2) /* entry state value meaning "flash still fading" (inferred) */
#define FLASH_ENTRY_STRIDE (6) /* entry size in floats (24 bytes) */
/* weapon_flashbang_clear_effects */


/* Draws the flashbang clear-screen effect each frame. Takes no parameters and returns nothing.
   Draws the particles in g_weapon_8bf000 with the depth test off, then, for every flash entry
   in the active state, draws a full-screen white overlay whose alpha falls as the entry's fade
   value rises (inferred). */
void weapon_flashbang_clear_effects(void)

{
  undefined1 alpha;
  float *entry_fade;
  undefined4 overlay_rgba;

  /* Particles go over the scene with the depth test off (inferred). */
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  particles_render(&g_weapon_8bf000);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  /* Walk the flash entry table. The state int sits 8 bytes before the fade float. */
  entry_fade = (float *)&g_weapon_flashbang_6a5d2c;
  do {
    if (*(int *)&entry_fade[-2] == FLASH_STATE_ACTIVE) {
      /* Start from white RGB (0xffffff); the alpha byte is replaced below. */
      overlay_rgba = 0xffffffff;
      /* alpha = 255 * sqrt(1 - 0.5 * fade) (inferred). */
      alpha = (int)(_rwSqrt(float_1_0 - *entry_fade * k_f_0_5) * k_f_255);
      overlay_rgba = CONCAT13(alpha,(undefined3)overlay_rgba);
      FUN_004c65d0(RwCameraGetCurrentCamera(),0,0,&overlay_rgba,0);
    }
    entry_fade = entry_fade + FLASH_ENTRY_STRIDE;
  } while ((LONG_PTR)entry_fade < ((LONG_PTR)&g_weapon_flashbang_6a5d8c));
  return;
}


