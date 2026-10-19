#include "mfl_tier2.h"
/*0042b920*/ extern int ui_draw_line(float param_2, float param_3, float param_4, float param_5, undefined1 *param_6);
/* ui_draw_table_grid */

/* "draws the outer frame and row separators of the stats table with ui_draw_line" */

void ui_draw_table_grid(undefined4 colour)

{
  ui_draw_line(130.0,120.0,510.0,120.0,(undefined1 *)&colour);
  ui_draw_line(130.0,162.0,510.0,162.0,(undefined1 *)&colour);
  ui_draw_line(130.0,328.0,510.0,328.0,(undefined1 *)&colour);
  ui_draw_line(130.0,360.0,510.0,360.0,(undefined1 *)&colour);
  ui_draw_line(130.0,120.0,130.0,360.0,(undefined1 *)&colour);
  ui_draw_line(510.0,120.0,510.0,360.0,(undefined1 *)&colour);
  return;
}
