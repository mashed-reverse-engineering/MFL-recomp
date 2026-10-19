#include "mfl_tier2.h"
/*0042cc80*/ extern int ui_draw_texture_640(undefined4 *param_3, float param_4, float param_5, float param_6, float param_7, int param_8, int param_9);
/* ui_draw_texture_640_default */

/* "ui_draw_texture_640 with flag 0" */

/*
 * Draws a texture through ui_draw_texture_640 with its last flag argument set to 0.
 * texture: texture pointer, passed through unchanged.
 * x, y, width, height: screen-space rectangle (inferred from the float arguments).
 * extra_int: the 8th argument, passed through unchanged (inferred meaning).
 * Returns nothing; the callee's int result is discarded.
 */
void ui_draw_texture_640_default(undefined4 *texture,float x,float y,float width,float height,
               int extra_int)
{

  ui_draw_texture_640(texture,x,y,width,height,extra_int,0);
  return;
}


