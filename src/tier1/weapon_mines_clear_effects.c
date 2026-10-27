#include "mfl_tier2.h"
/*00486ef0*/ extern int particles_render();
/*006a8ab8*/ extern undefined g_weapon_6a8ab8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;


/* weapon_mines_clear_effects */

/* Renders the mine effect particles with z-write disabled. */
void weapon_mines_clear_effects(void)
{
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  particles_render(&g_weapon_6a8ab8);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  return;
}
