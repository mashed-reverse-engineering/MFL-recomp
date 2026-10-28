#include "mfl_tier2.h"
typedef int (*RwSprintfFunc)(RwChar *, RwChar *, ...);
#define rwsprintf RW_GLOBAL(0xc4, RwSprintfFunc)      /* stringFuncs.vecSprintf */
extern char s_333_Lap_frames_005df368[];
extern char PTR_g_data_736579_005df3cc[];
/*00409eb0*/ extern double player_rank_key();
/*0041a020*/ extern char ai_grid_cell_lookup(int column, int row);
/*0044c5c0*/ extern int collision_debug_draw();
/*0044c6f0*/ extern undefined __cdecl ai_debug_draw_marker_box(undefined4 p0, undefined4 p1, undefined4 p2);
/*0044e040*/ extern int ai_route_debug_draw();
/*004b2b48*/ extern int _ftol();
/*004b2ccd*/ extern undefined __cdecl start(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*00568cc0*/ extern int RtCharsetPrintBuffered();
/*005dd340*/ extern float k_f_0_5;
/*005ddb14*/ extern float float_7_0;
/*005ddb48*/ extern float k_f_16;
/*005ddb50*/ extern float k_f_8;
/*005ddc28*/ extern float k_f_0;
/*005ddea0*/ extern float k_f_20;
/*005de890*/ extern float k_f_28;
/*005df308*/ extern float k_f_410;
/*005df30c*/ extern float k_f_420;
/*005df310*/ extern float k_f_400;
/*005df448*/ extern float k_f_18;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007eeb64*/ extern undefined4 g_ai_7eeb64;
/*007f023c*/ extern undefined4 g_ai_editor_mode;
/*007f024c*/ extern undefined4 g_ai_editor_menu_cursor;
/*007f0250*/ extern undefined4 g_ai_editor_route_set;
/*007f0254*/ extern undefined4 g_ai_editor_route_index;
/*007f025c*/ extern undefined4 g_ai_editor_step_flag_b;
/*007f0260*/ extern int g_ai_editor_step_counter;
/*007f0264*/ extern int g_ai_debug_7f0264;
/*007f0268*/ extern int g_ai_debug_7f0268;
/*007f026c*/ extern int g_ai_debug_7f026c;
/*007f0270*/ extern int g_ai_debug_7f0270;
/*007f0274*/ extern int g_ai_debug_7f0274;
/*007f0278*/ extern undefined4 g_ai_editor_brush;
/*007f027c*/ extern int g_ai_debug_7f027c;
/*007f0280*/ extern undefined4 g_ai_7f0280;
/*007f0284*/ extern struct AiData g_ai_data;
/*007f0244*/ extern float g_ai_editor_view_origin_z;
/*007f0240*/ extern float g_ai_editor_view_origin_x;
/*008014ac*/ extern char g_ai_data_routes_4644_[];
/*00800ea0*/ extern char g_ai_data_routes_3096_[];
/*00800894*/ extern char g_ai_data_routes_1548_[];
/*00800288*/ extern char g_ai_data_routes_0_[];
/* Size in bytes of one AI route record (see an_hk_1011.c ai_editor). */
#define ROUTE_RECORD_SIZE (0x204)
/* ai_debug_draw_lines */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "debug draw of 'race lines', 'inside lines' and 'slow lines' of the AI grid" */

void ai_debug_draw_lines(void)

{
  float marker_x_bits;
  float marker_z_bits;
  float cell_x_bits;
  float cell_y_bits;
  float cell_value_bits;
  float coord_a;
  float coord_b;
  char cell_value;
  int route;
  int scratch_int;
  int grid_origin_x;
  double coord_wide;
  char loop_ij[12];
  char text_buffer[256];
  
  scratch_int = g_ai_editor_menu_cursor;
  switch(g_ai_editor_route_set) {
  default:
    route = g_ai_editor_route_index * ROUTE_RECORD_SIZE + ((int)&g_ai_data_routes_0_);
    break;
  case 1:
    route = g_ai_editor_route_index * ROUTE_RECORD_SIZE + ((int)&g_ai_data_routes_1548_);
    break;
  case 2:
    route = g_ai_editor_route_index * ROUTE_RECORD_SIZE + ((int)&g_ai_data_routes_3096_);
    break;
  case 3:
    route = g_ai_editor_route_index * ROUTE_RECORD_SIZE + ((int)&g_ai_data_routes_4644_);
  }
  /* Debug overlay of the built-in AI editor (inferred): draws the selected route, grid boxes, the editor menu text (right column at x=400, rows 20 px apart) for the current g_ai_editor_mode, then the AI grid cells around the view origin (24 x 32 cells). */
  ai_route_debug_draw(route);
  collision_debug_draw(0x43c00000,0,0x44000000,0,0x43c00000,0x44000000,0xff202020);
  collision_debug_draw(0x44000000,0,0x44000000,0x44000000,0x43c00000,0x44000000,0xff202020);
  collision_debug_draw(0x43400000,0x43400000,0x43500000,0x43400000,0x43400000,0x43410000,0xffffffff)
  ;
  collision_debug_draw(0x43400000,0x43400000,0x43400000,0x43500000,0x43410000,0x43400000,0xffffffff)
  ;
  collision_debug_draw(0x43400000,0x43500000,0x43500000,0x43500000,0x43400000,0x43510000,0xffffffff)
  ;
  collision_debug_draw(0x43500000,0x43400000,0x43500000,0x43500000,0x43510000,0x43400000,0xffffffff)
  ;
  coord_wide = (double)((float)scratch_int * k_f_20) + (double)k_f_28;      /* x87: the second row edge is computed from the unrounded sum */
  coord_a = (float)coord_wide;
  coord_b = (float)(coord_wide + (double)k_f_18);
  collision_debug_draw(0x43c40000,*(int *)&coord_a,0x43fc0000,*(int *)&coord_a,0x43c40000,*(int *)&coord_b,0xff404040);
  collision_debug_draw(0x43c40000,*(int *)&coord_b,0x43fc0000,*(int *)&coord_a,0x43fc0000,*(int *)&coord_b,0xff404040);
  if (g_ai_editor_mode == 0) {
    rwsprintf(text_buffer,"race lines");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x1e,0);
    rwsprintf(text_buffer,"inside lines");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x32,0);
    rwsprintf(text_buffer,"slow lines");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x46,0);
    rwsprintf(text_buffer,"cheat lines");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x5a,0);
    rwsprintf(text_buffer,"danger data");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x6e,0);
    rwsprintf(text_buffer,"save data");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x82,0);
    rwsprintf(text_buffer,"?????");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x96,0);
    rwsprintf(text_buffer,"clear data");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0xaa,0);
    rwsprintf(text_buffer,"race");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0xbe,0);
  }
  if (g_ai_editor_mode == 1) {
    rwsprintf(text_buffer,"clear data?");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x1e,0);
    rwsprintf(text_buffer,"no");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x46,0);
    rwsprintf(text_buffer,(RwChar *)&PTR_g_data_736579_005df3cc)
    ;
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x5a,0);
  }
  if (g_ai_editor_mode == 3) {
    rwsprintf(text_buffer,"save data?");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x1e,0);
    rwsprintf(text_buffer,"no");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x46,0);
    rwsprintf(text_buffer,(RwChar *)&PTR_g_data_736579_005df3cc)
    ;
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x5a,0);
  }
  /* Race options: also tracks the last three lap times (frames) in g_ai_debug_7f0264/68/6c while the rank key of player 0 is between 0.5 and the "Lap frames" threshold (inferred). */
  if (g_ai_editor_mode == 4) {
    rwsprintf(text_buffer,"race options");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x1e,0);
    rwsprintf(text_buffer,"back");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x46,0);
    rwsprintf(text_buffer,"reset car");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x5a,0);
    if (g_ai_editor_step_flag_b == 0) {
      rwsprintf(text_buffer,"start race");
    }
    else {
      rwsprintf(text_buffer,"stop race");
    }
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x6e,0);
    rwsprintf(text_buffer,"single step");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x82,0);
    rwsprintf(text_buffer,"Lap frames");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0xbe,0);
    rwsprintf(text_buffer,"%d",g_ai_debug_7f0264);
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0xd2,0);
    rwsprintf(text_buffer,"%d",g_ai_debug_7f0268);
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0xe6,0);
    rwsprintf(text_buffer,"%d",g_ai_debug_7f026c);
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0xfa,0);
    coord_wide = (double)player_rank_key(0);
    if (((double)k_f_0_5 < coord_wide) && (coord_wide < (double)*(float *)&s_333_Lap_frames_005df368)) {
      if (g_ai_debug_7f0270 == 0) {
        g_ai_debug_7f0274 = g_ai_editor_step_counter;
      }
      g_ai_debug_7f0270 = 1;
      if (0x3c < g_ai_editor_step_counter - g_ai_debug_7f0274) {
        g_ai_debug_7f026c = g_ai_debug_7f0268;
        g_ai_debug_7f0268 = g_ai_debug_7f0264;
        g_ai_debug_7f0270 = 0;
        g_ai_debug_7f0264 = g_ai_editor_step_counter - g_ai_debug_7f0274;
      }
    }
  }
  if (g_ai_editor_mode == 6) {
    rwsprintf
              (text_buffer,"Edit Spline %d",g_ai_editor_route_index);
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x1e,0);
    rwsprintf(text_buffer,"Edit Point %d",g_ai_7f0280);
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x46,0);
    rwsprintf(text_buffer,"Insert Point");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x5a,0);
    rwsprintf(text_buffer,"Delete Point");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x6e,0);
    rwsprintf(text_buffer,"Back");
    RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x82,0);
  }
  rwsprintf(text_buffer,"memory=%d",g_ai_data.tiles_used);
  RtCharsetPrintBuffered(g_ai_7eeb64,text_buffer,400,0x1c2,0);
  if ((g_ai_editor_mode == 0) && (g_ai_editor_menu_cursor == 4)) {
    coord_a = (float)(g_ai_editor_brush & 3) * k_f_20;
    coord_b = (float)(((int)(g_ai_editor_brush + ((int)g_ai_editor_brush >> 0x1f & 3U)) >> 2) -
                   g_ai_debug_7f027c);
    if (!(coord_b > float_7_0)) {
      if (coord_b < k_f_0) {
        g_ai_debug_7f027c = g_ai_debug_7f027c + -1;
      }
    }
    else {
      g_ai_debug_7f027c = g_ai_debug_7f027c + 1;
    }
    coord_b = k_f_400 + coord_a;
    coord_a = coord_a + k_f_420;
    collision_debug_draw(*(int *)&coord_b,0x437a0000,*(int *)&coord_a,0x437a0000,*(int *)&coord_b,0x43870000,0xff404040);
    collision_debug_draw(*(int *)&coord_a,0x437a0000,*(int *)&coord_a,0x43870000,*(int *)&coord_b,0x43870000,0xff404040);
    (*(int *)loop_ij) = 0;
    do {
      ai_debug_draw_marker_box
                ((marker_x_bits = (float)(*(int *)loop_ij) * k_f_20 + k_f_410, *(int *)&marker_x_bits),0x43820000,
                 (marker_z_bits = (float)((*(int *)loop_ij) + g_ai_debug_7f027c * 4), *(int *)&marker_z_bits));
      (*(int *)loop_ij) = (*(int *)loop_ij) + 1;
    } while ((*(int *)loop_ij) < 4);
  }
  scratch_int = ((int)(g_ai_editor_view_origin_z * (-4.0f)));
  grid_origin_x = (int)(g_ai_editor_view_origin_x * (-4.0f));
  (*(int *)(loop_ij + 4)) = 0;
  do {
    (*(int *)loop_ij) = 0;
    coord_a = (float)(*(int *)(loop_ij + 4)) * k_f_16;
    do {
      cell_value = ai_grid_cell_lookup((*(int *)loop_ij - scratch_int) + -0x20,-grid_origin_x - (*(int *)(loop_ij + 4)));
      if (cell_value != -1) {
        ai_debug_draw_marker_box((cell_x_bits = (float)(*(int *)loop_ij) * k_f_16 + k_f_8, *(int *)&cell_x_bits),(cell_y_bits = coord_a + k_f_8, *(int *)&cell_y_bits),(cell_value_bits = (float)(int)cell_value, *(int *)&cell_value_bits));
      }
      (*(int *)loop_ij) = (*(int *)loop_ij) + 1;
    } while ((*(int *)loop_ij) < 0x18);
    (*(int *)(loop_ij + 4)) = (*(int *)(loop_ij + 4)) + 1;
  } while ((*(int *)(loop_ij + 4)) < 0x20);
  return;
}


