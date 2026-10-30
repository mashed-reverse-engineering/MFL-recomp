// cluster-entries: car_init_dynamics
/* Installed cluster car_init_dynamics (tools/cluster_install.py from output/cluster_pilot_j1/car_init_dynamics.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
/*00435f10*/ extern int get_career_row(void);
/*00434bb0*/ extern int get_event_type(void);
/*004379a0*/ extern int menu_event_course_index_get(void);
/*0040e680*/ extern int race_setup_get_course(int row);
/*004d5410*/ extern RwReal _rwSqrt(RwReal p0);
/*006292b8*/ extern float g_car_handling_param_b8;
/*006292c0*/ extern float g_car_init_6292c0;
/*006292c4*/ extern float g_car_handling_param_c4;
/*006292c8*/ extern float g_car_init_6292c8;
/*006292cc*/ extern float g_car_init_6292cc;
/*006292d0*/ extern float g_car_init_6292d0;
/*006292d4*/ extern float g_car_init_6292d4;
/*006292d8*/ extern float g_car_init_6292d8;
/*006292dc*/ extern float g_car_init_6292dc;
/*006292e0*/ extern float g_car_handling_scale;
/*006292ec*/ extern float g_car_default_speed_scale;
/*006292f0*/ extern int car_handling_table;
/*006292f8*/ extern int g_car_6292f8;
/*007ef760*/ extern float g_car_init_7ef760;
/*007ef764*/ extern float g_car_init_7ef764;
/*007ef768*/ extern float g_car_init_7ef768;
/*007ef76c*/ extern float g_car_init_7ef76c;
/*008b06e0*/ extern struct Car player_car[16];
/*005dd62c*/ extern float k_f_100;
/*005dd61c*/ extern float k_f_0_001;
/*005dfd64*/ extern float k_f_0_2777778;
/*005dfd68*/ extern float k_f_3_6;
/*005dfd78*/ extern float k_f_1_05;
/*005e64a4*/ extern float k_f_0;

/* rounding point: the original stores the value to a stack float and reloads it */
static float float_barrier(float v) { return v; }

/* Sets the default dynamics of player car `index` (< 16): handling parameters from the table row of the course / event, wheel
   and collision point positions, lever arms of the contact points, inertia.  Returns 1, or 0 for an index out of range. */
int car_init_dynamics(unsigned int index)

{
  float dz;
  float dy;
  Car *car;
  unsigned int slot = index;
  int row;
  int event;
  int *table_entry;
  float *point;
  float *wheel_pos;
  float *wheel_it;
  float dx;
  RwReal dist;
  float max_radius;
  float sum_len, r0, r1, r2, r3, share;      /* unrounded x87 values in the original */
  float inertia;
  int count;
  
  car = &player_car[index];
  row = get_career_row();
  event = get_event_type();
  if (((event == EVENT_QUICK_RACE_11) || (event = get_event_type(), event == EVENT_QUICK_RACE_12)) ||
     (event = get_event_type(), event == EVENT_QUICK_RACE_13)) {
    row = menu_event_course_index_get();
  }
  event = get_event_type();
  if (event == EVENT_CUP_4) {
    row = row + 0xf;
  }
  event = get_event_type();
  if (event == EVENT_CUP_5) {
    row = row + 0x1e;
  }
  row = race_setup_get_course(row);
  event = get_event_type();
  if (event == EVENT_TURBO_TOURNAMENT) {
    row = 0x48;
  }
  g_car_handling_param_b8 = k_f_100;
  g_car_handling_param_c4 = 40000.0;
  g_car_handling_scale = 1.0;
  g_car_default_speed_scale = 1.5;
  if (car_handling_table != -1) {
    table_entry = &g_car_6292f8;
    event = car_handling_table;
    do {
      if (event == row) {
        g_car_handling_param_b8 = (float)table_entry[-1];
        g_car_handling_param_c4 = (float)*table_entry;
        g_car_handling_scale = (float)table_entry[1] * k_f_0_001;
        g_car_default_speed_scale = (float)table_entry[2] * k_f_0_001;
      }
      event = table_entry[3];
      table_entry = table_entry + 5;
    } while (event != -1);
  }
  g_car_init_7ef760 = g_car_handling_param_b8;
  g_car_init_7ef764 = g_car_handling_param_c4;
  g_car_init_7ef768 = g_car_handling_scale;
  g_car_init_7ef76c = g_car_default_speed_scale;
  if (index < MAX_CARS) {
    car->speed_scale = g_car_default_speed_scale;
    car->wheels[0].pos_z = 1.08288;
    car->wheels[1].pos_z = 1.08288;
    car->wheels[2].pos_z = -1.14624;
    car->wheels[3].pos_z = -1.14624;
    car->wheels[0].state = 2;
    car->wheels[1].state = 2;
    dx = g_car_handling_scale;
    car->wheels[0].handling_scale = g_car_handling_scale;
    car->wheels[1].handling_scale = dx;
    car->wheels[2].handling_scale = dx;
    car->wheels[3].handling_scale = dx;
    car->wheels[0].const_34 = 34.0;
    car->wheels[1].const_34 = 34.0;
    car->wheels[2].const_34 = 34.0;
    car->wheels[3].const_34 = 34.0;
    car->handling_c0 = g_car_init_6292c0;
    car->handling_c4 = g_car_handling_param_c4;
    car->handling_params[0] = g_car_init_6292c8;
    car->handling_params[1] = g_car_init_6292cc;
    car->handling_params[2] = g_car_init_6292d0;
    car->handling_params[3] = g_car_init_6292d4;
    car->handling_params[4] = g_car_init_6292d8;
    car->handling_params[5] = g_car_init_6292dc;
    car->wheel_points[2] = 0.3008;
    car->wheel_points[5] = 0.3008;
    wheel_pos = &car->wheels[0].pos_y;
    point = car->wheel_points + 1;
    car->active = 1;
    car->controls_disabled = 0;
    *(undefined4 *)&car->field_0x150 = 0x3e19999a;
    *(undefined4 *)&car->field_0x158 = 0x3fa51eb8;
    car->mass = 1000.0;
    car->com_offset[0] = 0.0;
    car->com_offset[1] = 0.0;
    car->com_offset[2] = 0.0;
    car->wheels[0].pos_x = 0.6012;
    *wheel_pos = 0.397094;
    car->wheels[1].pos_x = -0.6012;
    car->wheels[1].pos_y = 0.397094;
    car->wheels[2].pos_x = 0.91224;
    car->wheels[2].pos_y = 0.529524;
    car->wheels[3].pos_x = -0.91224;
    car->wheels[3].pos_y = 0.529524;
    car->wheels[2].state = 1;
    car->wheels[3].state = 1;
    car->wheel_points[0] = 0.167;
    *point = 0.0;
    car->wheel_points[3] = -0.167;
    car->wheel_points[4] = 0.0;
    car->wheel_points[6] = 0.2534;
    car->wheel_points[7] = 0.0;
    car->wheel_points[8] = -0.3184;
    car->wheel_points[9] = -0.2534;
    car->wheel_points[10] = 0.0;
    car->wheel_points[0xb] = -0.3184;
    car->matrix_current = 0;
    car->matrix_previous = 1;
    car->velocity_history_index = 0;
    car->velocity_input[2] = 0.0;
    car->velocity_input[1] = 0.0;
    car->velocity_input[0] = 0.0;
    car->time_step_const = 0.001;
    max_radius = 0.0;
    count = 4;
    wheel_it = wheel_pos;
    do {
      dx = wheel_it[1] * k_f_0_2777778;
      wheel_it[3] = *wheel_it;
      dy = *wheel_it * k_f_0_2777778;      /* scaled components squared, summed in the original order [-1], [1], [0] */
      dz = wheel_it[-1] * k_f_0_2777778;
      dist = dz * dz;
      dist = dist + dx * dx;
      dist = dist + dy * dy;
      dx = dist;
      if (dx != k_f_0) {
        dx = _rwSqrt(dx);
      }
      if (max_radius < dx) {
        max_radius = dx;
      }
      wheel_it = wheel_it + 0x31;
      count = count + -1;
    } while (count != 0);
    car->wheel_extent = max_radius * k_f_1_05;
    row = 4;
    do {
      dist = wheel_pos[1] * wheel_pos[1];
      dist = dist + *wheel_pos * *wheel_pos;
      dist = dist + wheel_pos[-1] * wheel_pos[-1];
      dist = _rwSqrt(dist);
      wheel_pos[2] = dist;
      wheel_pos = wheel_pos + 0x31;
      row = row + -1;
    } while (row != 0);
    row = 4;
    wheel_pos = &car->contacts[0].lever_arm;
    do {
      dx = point[-1] - car->com_offset[0];
      dy = *point - car->com_offset[1];
      dz = point[1] - car->com_offset[2];
      dist = dz * dz;      /* z, y, x order of the original */
      dist = dist + dy * dy;
      dist = dist + dx * dx;
      dist = _rwSqrt(dist);
      point = point + 3;
      row = row + -1;
      *wheel_pos = dist * k_f_3_6;
      wheel_pos = wheel_pos + 0x10;
    } while (row != 0);
    point = &car->contacts[4].lever_arm;
    wheel_pos = car->bounds_points + 1;
    row = 8;
    do {
      dx = wheel_pos[-1] - car->com_offset[0];
      dy = *wheel_pos - car->com_offset[1];
      dz = wheel_pos[1] - car->com_offset[2];
      dist = dz * dz;      /* z, y, x order of the original */
      dist = dist + dy * dy;
      dist = dist + dx * dx;
      dist = _rwSqrt(dist);
      *point = dist * k_f_3_6;
      if (max_radius < dist) {
        max_radius = dist;
      }
      wheel_pos = wheel_pos + 3;
      point = point + 0x10;
      row = row + -1;
    } while (row != 0);
    point = car->body_points + 1;
    row = 6;
    wheel_pos = &car->contacts[0xc].lever_arm;
    do {
      dx = point[-1] - car->com_offset[0];
      dy = *point - car->com_offset[1];
      dz = point[1] - car->com_offset[2];
      dist = dz * dz;      /* z, y, x order of the original */
      dist = dist + dy * dy;
      dist = dist + dx * dx;
      dist = _rwSqrt(dist);
      point = point + 3;
      row = row + -1;
      *wheel_pos = dist * k_f_3_6;
      wheel_pos = wheel_pos + 0x10;
    } while (row != 0);
    car->bounds_radius = max_radius * k_f_1_05;
    sum_len = car->wheels[1].pos_length + car->wheels[0].pos_length;
    sum_len = sum_len + car->wheels[2].pos_length;
    sum_len = sum_len + car->wheels[3].pos_length;
    r0 = sum_len / car->wheels[0].pos_length;
    r1 = sum_len / car->wheels[1].pos_length;
    r2 = sum_len / car->wheels[2].pos_length;
    r3 = sum_len / car->wheels[3].pos_length;
    {
      float rs = r2 + r3;      /* sums and products as separate statements, original order */
      rs = rs + r1;
      rs = rs + r0;
      share = car->mass / rs;
    }
    {
      float t2 = r2 * share, t1 = r1 * share, t0 = r0 * share, t3 = r3 * share;      /* one product per statement, source order */
      t2 = t2 * car->wheels[2].pos_length;
      t1 = t1 * car->wheels[1].pos_length;
      t0 = t0 * car->wheels[0].pos_length;
      t3 = t3 * car->wheels[3].pos_length;
      inertia = float_barrier(t2 + t1 + t0 + t3);
    }
    car->inertia_sum = inertia;
    car->inertia_inverse = 1.0f / inertia;
    return 1;
  }
  return 0;
}

