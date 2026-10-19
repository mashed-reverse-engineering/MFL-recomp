#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*004146f0*/ extern int race_event_has_panel();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0064d880*/ extern RpClump **g_cup_64d880;      /* pointer to the scene's clump table (the original loads it from the variable) */
/*008d8b30*/ extern undefined4 player_count;
/* cup_scene_render_players */

/* "sets cull mode and z state, renders the clump and one clump per player" */

void cup_scene_render_players(void)

{
  int i;
  int n;

  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  if (g_cup_64d880[0] != (RpClump *)0x0) {
    RpClumpRender(g_cup_64d880[0]);
  }
  n = get_player_count();
  if (0 < n) {
    i = 0;
    do {
      RpClumpRender(g_cup_64d880[0x10 + i]);
      i = i + 1;
    } while (i < get_player_count());
  }
  n = race_event_has_panel();
  if (0 < n) {
    i = 0;
    do {
      RpClumpRender(g_cup_64d880[0x14 + i]);
      i = i + 1;
    } while (i < race_event_has_panel());
  }
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x2);
  return;
}
