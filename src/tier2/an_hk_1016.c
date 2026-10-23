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
/* ai_debug_draw_lines */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "debug draw of 'race lines', 'inside lines' and 'slow lines' of the AI grid" */

void ai_debug_draw_lines(void)

{
  float fbits1;
  float fbits2;
  float fbits3;
  float fbits4;
  float fbits5;
  float fVar1;
  float fVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  int grid_origin_x;
  double fVar6;
  char local_110[12];
  char aRStack_104[256];
  
  iVar5 = g_ai_editor_menu_cursor;
  switch(g_ai_editor_route_set) {
  default:
    iVar4 = g_ai_editor_route_index * 0x204 + ((int)&g_ai_data_routes_0_);
    break;
  case 1:
    iVar4 = g_ai_editor_route_index * 0x204 + ((int)&g_ai_data_routes_1548_);
    break;
  case 2:
    iVar4 = g_ai_editor_route_index * 0x204 + ((int)&g_ai_data_routes_3096_);
    break;
  case 3:
    iVar4 = g_ai_editor_route_index * 0x204 + ((int)&g_ai_data_routes_4644_);
  }
  ai_route_debug_draw(iVar4);
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
  fVar6 = (double)((float)iVar5 * k_f_20) + (double)k_f_28;      /* x87: the second row edge is computed from the unrounded sum */
  fVar1 = (float)fVar6;
  fVar2 = (float)(fVar6 + (double)k_f_18);
  collision_debug_draw(0x43c40000,*(int *)&fVar1,0x43fc0000,*(int *)&fVar1,0x43c40000,*(int *)&fVar2,0xff404040);
  collision_debug_draw(0x43c40000,*(int *)&fVar2,0x43fc0000,*(int *)&fVar1,0x43fc0000,*(int *)&fVar2,0xff404040);
  if (g_ai_editor_mode == 0) {
    rwsprintf(aRStack_104,"race lines");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x1e,0);
    rwsprintf(aRStack_104,"inside lines");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x32,0);
    rwsprintf(aRStack_104,"slow lines");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x46,0);
    rwsprintf(aRStack_104,"cheat lines");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x5a,0);
    rwsprintf(aRStack_104,"danger data");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x6e,0);
    rwsprintf(aRStack_104,"save data");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x82,0);
    rwsprintf(aRStack_104,"?????");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x96,0);
    rwsprintf(aRStack_104,"clear data");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0xaa,0);
    rwsprintf(aRStack_104,"race");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0xbe,0);
  }
  if (g_ai_editor_mode == 1) {
    rwsprintf(aRStack_104,"clear data?");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x1e,0);
    rwsprintf(aRStack_104,"no");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x46,0);
    rwsprintf(aRStack_104,(RwChar *)&PTR_g_data_736579_005df3cc)
    ;
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x5a,0);
  }
  if (g_ai_editor_mode == 3) {
    rwsprintf(aRStack_104,"save data?");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x1e,0);
    rwsprintf(aRStack_104,"no");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x46,0);
    rwsprintf(aRStack_104,(RwChar *)&PTR_g_data_736579_005df3cc)
    ;
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x5a,0);
  }
  if (g_ai_editor_mode == 4) {
    rwsprintf(aRStack_104,"race options");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x1e,0);
    rwsprintf(aRStack_104,"back");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x46,0);
    rwsprintf(aRStack_104,"reset car");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x5a,0);
    if (g_ai_editor_step_flag_b == 0) {
      rwsprintf(aRStack_104,"start race");
    }
    else {
      rwsprintf(aRStack_104,"stop race");
    }
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x6e,0);
    rwsprintf(aRStack_104,"single step");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x82,0);
    rwsprintf(aRStack_104,"Lap frames");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0xbe,0);
    rwsprintf(aRStack_104,"%d",g_ai_debug_7f0264);
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0xd2,0);
    rwsprintf(aRStack_104,"%d",g_ai_debug_7f0268);
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0xe6,0);
    rwsprintf(aRStack_104,"%d",g_ai_debug_7f026c);
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0xfa,0);
    fVar6 = (double)player_rank_key(0);
    if (((double)k_f_0_5 < fVar6) && (fVar6 < (double)*(float *)&s_333_Lap_frames_005df368)) {
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
              (aRStack_104,"Edit Spline %d",g_ai_editor_route_index);
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x1e,0);
    rwsprintf(aRStack_104,"Edit Point %d",g_ai_7f0280);
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x46,0);
    rwsprintf(aRStack_104,"Insert Point");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x5a,0);
    rwsprintf(aRStack_104,"Delete Point");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x6e,0);
    rwsprintf(aRStack_104,"Back");
    RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x82,0);
  }
  rwsprintf(aRStack_104,"memory=%d",g_ai_data.tiles_used);
  RtCharsetPrintBuffered(g_ai_7eeb64,aRStack_104,400,0x1c2,0);
  if ((g_ai_editor_mode == 0) && (g_ai_editor_menu_cursor == 4)) {
    fVar1 = (float)(g_ai_editor_brush & 3) * k_f_20;
    fVar2 = (float)(((int)(g_ai_editor_brush + ((int)g_ai_editor_brush >> 0x1f & 3U)) >> 2) -
                   g_ai_debug_7f027c);
    if (fVar2 <= float_7_0) {
      if (fVar2 < k_f_0) {
        g_ai_debug_7f027c = g_ai_debug_7f027c + -1;
      }
    }
    else {
      g_ai_debug_7f027c = g_ai_debug_7f027c + 1;
    }
    fVar2 = k_f_400 + fVar1;
    fVar1 = fVar1 + k_f_420;
    collision_debug_draw(*(int *)&fVar2,0x437a0000,*(int *)&fVar1,0x437a0000,*(int *)&fVar2,0x43870000,0xff404040);
    collision_debug_draw(*(int *)&fVar1,0x437a0000,*(int *)&fVar1,0x43870000,*(int *)&fVar2,0x43870000,0xff404040);
    (*(int *)local_110) = 0;
    do {
      ai_debug_draw_marker_box
                ((fbits1 = (float)(*(int *)local_110) * k_f_20 + k_f_410, *(int *)&fbits1),0x43820000,
                 (fbits2 = (float)((*(int *)local_110) + g_ai_debug_7f027c * 4), *(int *)&fbits2));
      (*(int *)local_110) = (*(int *)local_110) + 1;
    } while ((*(int *)local_110) < 4);
  }
  iVar5 = ((int)(g_ai_editor_view_origin_z * (-4.0f)));
  grid_origin_x = (int)(g_ai_editor_view_origin_x * (-4.0f));
  (*(int *)(local_110 + 4)) = 0;
  do {
    (*(int *)local_110) = 0;
    fVar1 = (float)(*(int *)(local_110 + 4)) * k_f_16;
    do {
      cVar3 = ai_grid_cell_lookup((*(int *)local_110 - iVar5) + -0x20,-grid_origin_x - (*(int *)(local_110 + 4)));
      if (cVar3 != -1) {
        ai_debug_draw_marker_box((fbits3 = (float)(*(int *)local_110) * k_f_16 + k_f_8, *(int *)&fbits3),(fbits4 = fVar1 + k_f_8, *(int *)&fbits4),(fbits5 = (float)(int)cVar3, *(int *)&fbits5));
      }
      (*(int *)local_110) = (*(int *)local_110) + 1;
    } while ((*(int *)local_110) < 0x18);
    (*(int *)(local_110 + 4)) = (*(int *)(local_110 + 4)) + 1;
  } while ((*(int *)(local_110 + 4)) < 0x20);
  return;
}


