#include "mfl_tier2.h"
extern struct TrailSlot g_trails[6];
/*004146f0*/ extern int race_event_has_panel();
/*004223e0*/ extern int vehicle_get_field_18c();
/*00465250*/ extern int material_uv_scroll_update();
/*00475730*/ extern int get_viewport_camera();
/*00484770*/ extern double distance_between_points();
/*0048e3d0*/ extern int rwp_object_get_user_data();
/*004c42f0*/ extern void FUN_004c42f0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd41c*/ extern float float_5_0;
/*005dd61c*/ extern float k_f_0_001;
/*005dd62c*/ extern float k_f_100;
/*005ddafc*/ extern float k_f_0_05;
/*005de198*/ extern float k_f_n100;
/*005de19c*/ extern float k_f_1_0en4;
/*00603920*/ extern undefined4 g_trail_603920;
/*00603924*/ extern undefined4 g_trail_render_603924;
/*00603928*/ extern undefined4 g_trail_render_603928;
/*007ef804*/ extern undefined4 g_tick_number;
extern double cos(double);
extern double sin(double);
/* trail_render */

/* "renders the trail records (inferred)" */

void trail_render(void)

{
  RwFrame *frame;
  float delta_y;
  TrailSlot *trail_cursor;
  int panel_count;
  uint user_data;
  int camera;
  float *end_pos;
  TrailSlot *trail;
  float *start_pos;
  double view_dist;
  struct {
    int trail_i;
    undefined1 pad_40[4];
    RwV3d scale;
    float rot_cos;
    float rot_sin;
    undefined4 rot_zero;
    undefined1 pad_24[8];
    float delta_z;
    float ext_a_x;
    float ext_a_y;
    float ext_a_z;
    float ext_b_x;
    float ext_b_y;
    float ext_b_z;
  } frame_obj;
  
  frame_obj.trail_i = 0;
  vehicle_get_field_18c(0);
  panel_count = race_event_has_panel();
  if (0 < panel_count) {
    trail = g_trails;
    end_pos = &g_trails[0].w14;
    trail_cursor = g_trails;
    do {
      start_pos = &trail_cursor->w08;
      frame_obj.scale.x = g_trail_603920;
      frame_obj.rot_cos = 0.0;
      frame_obj.rot_sin = 1.0;
      frame_obj.rot_zero = 0;
      frame_obj.scale.y = g_trail_render_603924;
      frame_obj.scale.z = g_trail_render_603928;
      frame = *(RwFrame **)(trail->w00 + 4);
      if ((trail->active != 0) &&
         (!(*(float *)&trail->w20 <= k_f_1_0en4))) {
        user_data = rwp_object_get_user_data(trail->object_a);
        if (((user_data & 8) == 0) &&
           (((user_data = rwp_object_get_user_data(trail->object_b), (user_data & 8) == 0 &&
             (user_data = rwp_object_get_user_data(trail->object_a), (user_data & 2) == 0)) &&
            (user_data = rwp_object_get_user_data(trail->object_b), (user_data & 2) == 0)))) {
          delta_y = trail->w0c - trail->w18;
          frame_obj.delta_z = trail->w10 - trail->w1c;
          frame_obj.ext_a_x = (*start_pos - *end_pos) * k_f_100 + *start_pos;
          frame_obj.ext_a_y = k_f_100 * delta_y + start_pos[1];
          frame_obj.scale.x = 4.0;
          frame_obj.scale.y = 4.0;
          frame_obj.ext_a_z = frame_obj.delta_z * k_f_100 + start_pos[2];
          frame_obj.ext_b_x = (*start_pos - *end_pos) * k_f_n100 + *end_pos;
          frame_obj.ext_b_y = delta_y * k_f_n100 + end_pos[1];
          frame_obj.ext_b_z = frame_obj.delta_z * k_f_n100 + end_pos[2];
          frame_obj.scale.z = *(float *)&trail->w20 * k_f_4;
          camera = get_viewport_camera(0);
          view_dist = (double)distance_between_points(&frame_obj.ext_a_x,&frame_obj.ext_b_x,*(int *)(camera + 4) + 0x40);
          if (!((double)k_f_0_001 > view_dist)) {
            if ((double)float_5_0 < view_dist) {
              view_dist = (double)float_5_0;
            }
          }
          else {
            view_dist = (double)k_f_0_001;
          }
          frame_obj.scale.x = (RwReal)((double)frame_obj.scale.x * view_dist * (double)k_f_0_5);
          frame_obj.scale.y = (RwReal)(view_dist * (double)k_f_0_5 * (double)frame_obj.scale.y);
          material_uv_scroll_update(trail->w00,0);
          view_dist = (double)cos((double)(int)g_tick_number * (double)k_f_0_05);
          frame_obj.rot_cos = (float)view_dist;
          view_dist = (double)sin((double)(int)g_tick_number * (double)k_f_0_05);
          frame_obj.rot_sin = (float)view_dist;
          RwFrameTranslate(frame,(RwV3d *)start_pos,rwCOMBINEREPLACE);
          FUN_004c42f0((undefined4)frame,(undefined4)end_pos,(undefined4)&frame_obj.rot_cos);
          RwFrameScale(frame,&frame_obj.scale,rwCOMBINEPRECONCAT);
        }
        else {
          trail->active = 0;
        }
      }
      frame_obj.trail_i = frame_obj.trail_i + 1;
      trail = trail + 1;
      trail_cursor = (TrailSlot *)(start_pos + 9);
      end_pos = end_pos + 0xb;
    } while (frame_obj.trail_i < panel_count);
  }
  return;
}


