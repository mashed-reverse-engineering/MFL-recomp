// cluster-entries: grid_position
/* Installed cluster grid_position (tools/cluster_install.py from output/cluster_pilot_j1/grid_position.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#define GRID_SLOT_HARE (100) /* grid slot of the chased car in Hare and Hounds (GAME_MODE_HARE_AND_HOUNDS) */
/*0042b1c0*/ extern int course_get_record_pos();
/*0042b1d0*/ extern int course_get_record_base();
/*0042b210*/ extern int course_get_record_point();
/*00434bb0*/ extern int get_event_type();
/*00482030*/ extern double dot_product_2d_clamped();
/*004a8420*/ extern int debug_printf();
/*004b3284*/ extern double FUN_004b3284(double p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5540*/ extern RwReal RwV2dNormalize(struct RwV2d *p0, struct RwV2d *p1);
/*005dd328*/ extern float k_f_0_6;
/*005dd36c*/ extern float k_f_0_4;
/*005ddacc*/ extern float k_f_360;
/*005ddaec*/ extern float k_f_57_29578;
/*005ddb18*/ extern float float_0_8;
/*005ddb20*/ extern float k_f_0_9;
/*005ddc10*/ extern float k_f_3_3;
/*005ddc14*/ extern float k_f_2_2;
/*005ddc18*/ extern float k_f_1_1;
/*005ddc28*/ extern float k_f_0;
/*00651ab0*/ extern undefined4 g_fx_651ab0;
/*00651ab4*/ extern undefined4 g_grid_position_651ab4;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7d4*/ extern undefined4 g_race_time_ticks;
/* Start grid position of one car of the race (computes a position and a heading from the route record; "Invalid number of cars in race" / "Invalid position").  Ghidra-style locals kept: the x87 expression order is the verified one. */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* computes a position/heading for one of the cars of the race ("Invalid number of cars in race";
   "Invalid position") */

void grid_position(undefined4 route,int grid_slot,int car_count,float *out_pos,float *out_heading)

{
  float tmp_f;
  float start_x;
  int row_or_event;
  float *rec_ptr;
  float *rec_ptr2;
  int event_type;
  float10 heading_deg;
  struct {
    RwV2d route_dir;
    float vec_x;
    float vec_y;
    float vec_z;
  } frame;
  RwV3d fwd;
  struct {
    float start_x_f;
    float start_y;
    float start_z;
    float fwd04_x;
    float fwd04_y;
    float fwd04_z;
    float side11_x;
    float side11_y;
    float side11_z;
    float side_x;
    float side_y;
    float side_z;
    float side22_x;
    float side22_y;
    float side22_z;
    float side33_x;
    float side33_y;
    float side33_z;
  } frame2;
  
  frame.vec_x = 0.0;
  frame.vec_y = 1.0;
  if ((g_race_time_ticks != g_fx_651ab0) &&
     (g_grid_position_651ab4 = g_grid_position_651ab4 + 1, car_count <= (int)g_grid_position_651ab4)) {
    g_grid_position_651ab4 = 0;
  }
  row_or_event = get_event_type();
  if (row_or_event == EVENT_TRAINING) {
    g_grid_position_651ab4 = 0;
  }
  g_fx_651ab0 = g_race_time_ticks;
  if (((game_mode == GAME_MODE_RACE) || (game_mode == GAME_MODE_CHASE_FUGITIVE)) || (game_mode == GAME_MODE_SURVIVAL)) {
    g_grid_position_651ab4 = 3;
  }
  rec_ptr = (float *)course_get_record_pos(route);
  start_x = *rec_ptr;
  frame2.start_y = rec_ptr[1];
  frame2.start_z = rec_ptr[2];
  frame2.start_x_f = start_x;
  rec_ptr = (float *)course_get_record_base(route);
  frame.route_dir.x = *rec_ptr;
  frame2.side_y = rec_ptr[1];
  frame.route_dir.y = rec_ptr[2];
  frame2.side_x = frame.route_dir.x;
  frame2.side_z = frame.route_dir.y;
  RwV2dNormalize(&frame.route_dir,&frame.route_dir);
  heading_deg = (float10)dot_product_2d_clamped(&frame.vec_x,&frame.route_dir);
  heading_deg = (float10)FUN_004b3284((double)heading_deg);
  heading_deg = heading_deg * (float10)k_f_57_29578;
  if (frame.route_dir.x < k_f_0) {
    heading_deg = (float10)k_f_360 - heading_deg;
  }
  *out_heading = (float)heading_deg;
  rec_ptr = (float *)course_get_record_point(route,0);
  rec_ptr2 = (float *)course_get_record_point(route,3);
  fwd.x = *rec_ptr - *rec_ptr2;
  row_or_event = course_get_record_point(route,0);
  tmp_f = *(float *)(row_or_event + 4);
  row_or_event = course_get_record_point(route,3);
  fwd.y = tmp_f - *(float *)(row_or_event + 4);
  row_or_event = course_get_record_point(route,0);
  tmp_f = *(float *)(row_or_event + 8);
  row_or_event = course_get_record_point(route,3);
  fwd.z = tmp_f - *(float *)(row_or_event + 8);
  RwV3dNormalize(&fwd,&fwd);
  frame2.fwd04_x = fwd.x * k_f_0_4;
  frame2.fwd04_y = fwd.y * k_f_0_4;
  frame2.fwd04_z = fwd.z * k_f_0_4;
  frame2.side11_x = frame2.side_x * k_f_1_1;
  frame2.side11_y = frame2.side_y * k_f_1_1;
  frame2.side11_z = frame2.side_z * k_f_1_1;
  frame2.side22_x = frame2.side_x * k_f_2_2;
  frame2.side22_y = frame2.side_y * k_f_2_2;
  frame2.side22_z = frame2.side_z * k_f_2_2;
  frame2.side33_x = frame2.side_x * k_f_3_3;
  frame2.side33_y = frame2.side_y * k_f_3_3;
  frame2.side33_z = frame2.side_z * k_f_3_3;
  if (grid_slot == GRID_SLOT_HARE) {
    frame.vec_x = frame2.start_x_f + frame2.side22_x;
    frame.vec_y = frame2.start_y + frame2.side22_y;
    frame2.start_z = frame2.start_z + frame2.side22_z;
LAB_0040a700:
    *out_pos = frame.vec_x;
    out_pos[1] = frame.vec_y;
    out_pos[2] = frame2.start_z;
    return;
  }
  if (grid_slot < 4) {
    fwd.x = fwd.x * float_0_8;
    fwd.y = fwd.y * float_0_8;
    start_x = fwd.z * float_0_8;
    frame.vec_x = frame2.fwd04_x * float_0_8;
    frame.vec_y = frame2.fwd04_y * float_0_8;
    fwd.z = frame2.fwd04_z * float_0_8;
    switch(grid_slot) {
    case 0:
      {
        frame.vec_z = frame2.start_z + start_x;
        frame.vec_y = (frame2.start_y + fwd.y) - frame.vec_y;
        frame.vec_x = (frame2.start_x_f + fwd.x) - frame.vec_x;
        fwd.z = frame.vec_z - fwd.z;
      }
      break;
    case 1:
      {
        frame.vec_x = frame2.start_x_f - fwd.x - frame.vec_x;
        frame.vec_z = frame2.start_z - start_x;
        frame.vec_y = ((frame2.start_y - fwd.y) - frame.vec_y) - frame2.side11_y;
        frame.vec_x = frame.vec_x - frame2.side11_x;
        frame2.side11_z = frame.vec_z - fwd.z - frame2.side11_z;
      }
      goto LAB_0040a68e;
    case 2:
      {
        frame.vec_x = frame2.start_x_f + fwd.x + frame.vec_x;
        frame.vec_z = frame2.start_z + start_x;
        frame.vec_y = (frame2.start_y + fwd.y + frame.vec_y) - frame2.side22_y;
        frame.vec_x = frame.vec_x - frame2.side22_x;
        frame2.start_z = frame.vec_z + fwd.z - frame2.side22_z;
      }
      goto LAB_0040a700;
    case 3:
      {
        frame.vec_x = frame2.start_x_f - fwd.x + frame.vec_x;
        frame.vec_z = frame2.start_z - start_x;
        frame.vec_y = ((frame2.start_y - fwd.y) + frame.vec_y) - frame2.side33_y;
        frame.vec_x = frame.vec_x - frame2.side33_x;
        fwd.z = frame.vec_z + fwd.z - frame2.side33_z;
      }
      break;
    default:
      return;
    }
    goto LAB_0040a772;
  }
  for (row_or_event = grid_slot + -4 + g_grid_position_651ab4; car_count <= row_or_event;
      row_or_event = row_or_event - car_count) {
  }
  switch(car_count) {
  case 1:
    if (row_or_event == 0) {
LAB_0040a48b:
      *out_pos = start_x;
      out_pos[1] = frame2.start_y;
      out_pos[2] = frame2.start_z;
      return;
    }
    break;
  case 2:
    fwd.x = fwd.x * k_f_0_6;
    fwd.y = fwd.y * k_f_0_6;
    fwd.z = fwd.z * k_f_0_6;
    if (row_or_event == 0) {
      frame.vec_x = frame2.start_x_f + fwd.x;
      frame.vec_y = frame2.start_y + fwd.y;
      frame2.side11_z = frame2.start_z + fwd.z;
      goto LAB_0040a68e;
    }
    if (row_or_event == 1) {
      frame.vec_x = frame2.start_x_f - fwd.x;
      frame.vec_y = frame2.start_y - fwd.y;
      fwd.z = frame2.start_z - fwd.z;
      goto LAB_0040a772;
    }
    break;
  case 3:
    fwd.x = fwd.x * k_f_0_9;
    fwd.y = fwd.y * k_f_0_9;
    fwd.z = fwd.z * k_f_0_9;
    if (row_or_event == 0) {
      frame.vec_x = frame2.start_x_f + fwd.x;
      frame.vec_y = frame2.start_y + fwd.y;
      fwd.z = frame2.start_z + fwd.z;
      goto LAB_0040a772;
    }
    if (row_or_event == 1) goto LAB_0040a48b;
    if (row_or_event == 2) {
      frame.vec_x = frame2.start_x_f - fwd.x;
      frame.vec_y = frame2.start_y - fwd.y;
      frame2.start_z = frame2.start_z - fwd.z;
      goto LAB_0040a700;
    }
    break;
  case 4:
    fwd.x = fwd.x * float_0_8;
    fwd.y = fwd.y * float_0_8;
    fwd.z = fwd.z * float_0_8;
    frame2.fwd04_x = frame2.fwd04_x * float_0_8;
    frame2.fwd04_y = frame2.fwd04_y * float_0_8;
    frame2.fwd04_z = frame2.fwd04_z * float_0_8;
    event_type = get_event_type();
    if (event_type == EVENT_TRAINING) {
      if (row_or_event != 0) {
        if (row_or_event == 1) {
          {
            frame.vec_z = frame2.start_z - fwd.z;
            frame.vec_y = (frame2.start_y - fwd.y) - frame2.fwd04_y;
            frame.vec_x = (frame2.start_x_f - fwd.x) - frame2.fwd04_x;
            frame2.start_z = frame.vec_z - frame2.fwd04_z;
          }
          goto LAB_0040a700;
        }
        if (row_or_event != 2) {
          return;
        }
        frame.vec_x = ((((frame2.start_x_f - fwd.x) - fwd.x) - frame2.side11_x) - frame2.side11_x) - frame2.side11_x;
        frame.vec_y = ((((frame2.start_y - fwd.y) - fwd.y) - frame2.side11_y) - frame2.side11_y) - frame2.side11_y;
        frame2.side11_z = ((((frame2.start_z - fwd.z) - fwd.z) - frame2.side11_z) - frame2.side11_z) - frame2.side11_z
        ;
        goto LAB_0040a68e;
      }
      {
        frame.vec_z = frame2.start_z + fwd.z;
        frame.vec_y = frame2.start_y + fwd.y + frame2.fwd04_y;
        frame.vec_x = frame2.start_x_f + fwd.x + frame2.fwd04_x;
        fwd.z = frame.vec_z + frame2.fwd04_z;
      }
LAB_0040a772:
      *out_pos = frame.vec_x;
      out_pos[1] = frame.vec_y;
      out_pos[2] = fwd.z;
      return;
    }
    switch(row_or_event) {
    case 0:
      {
        frame.vec_z = frame2.start_z + fwd.z;
        frame.vec_y = (frame2.start_y + fwd.y) - frame2.fwd04_y;
        frame.vec_x = (frame2.start_x_f + fwd.x) - frame2.fwd04_x;
        frame2.side11_z = frame.vec_z - frame2.fwd04_z;
      }
      break;
    case 1:
      {
        frame.vec_x = frame2.start_x_f - fwd.x - frame2.fwd04_x;
        frame.vec_z = frame2.start_z - fwd.z;
        frame.vec_y = ((frame2.start_y - fwd.y) - frame2.fwd04_y) - frame2.side11_y;
        frame.vec_x = frame.vec_x - frame2.side11_x;
        frame2.start_z = frame.vec_z - frame2.fwd04_z - frame2.side11_z;
      }
      goto LAB_0040a700;
    case 2:
      {
        frame.vec_x = frame2.start_x_f + fwd.x + frame2.fwd04_x;
        frame.vec_z = frame2.start_z + fwd.z;
        frame.vec_y = (frame2.start_y + fwd.y + frame2.fwd04_y) - frame2.side22_y;
        frame.vec_x = frame.vec_x - frame2.side22_x;
        fwd.z = frame.vec_z + frame2.fwd04_z - frame2.side22_z;
      }
      goto LAB_0040a772;
    case 3:
      {
        frame.vec_x = frame2.start_x_f - fwd.x + frame2.fwd04_x;
        frame.vec_z = frame2.start_z - fwd.z;
        frame.vec_y = ((frame2.start_y - fwd.y) + frame2.fwd04_y) - frame2.side33_y;
        frame.vec_x = frame.vec_x - frame2.side33_x;
        frame2.side11_z = frame.vec_z + frame2.fwd04_z - frame2.side33_z;
      }
      break;
    default:
      goto switchD_0040a651_default;
    }
LAB_0040a68e:
    *out_pos = frame.vec_x;
    out_pos[1] = frame.vec_y;
    out_pos[2] = frame2.side11_z;
    return;
  default:
    debug_printf("Invalid number of cars in race (%d)\n",car_count);
    return;
  }
switchD_0040a651_default:
  debug_printf("Invalid position (%d)\n",row_or_event);
  return;
}

