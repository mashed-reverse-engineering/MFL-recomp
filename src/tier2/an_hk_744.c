#include "mfl_tier2.h"
/*0041d580*/ extern undefined __cdecl btb_panel_slot_set_position(undefined4 p0, void *p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005ddc28*/ extern float k_f_0;
/*005de2f8*/ extern float k_f_n90;
/*00658d00*/ extern undefined4 g_time_trial_658d00[0x1000];
/*00658d4c*/ extern undefined4 g_hudpanel_658d4c;
/*00658dcc*/ extern char g_time_trial_658dcc[];

/* floats per panel point record in g_hudpanel_658d4c (inferred from the pointer step) */
#define PANEL_POINT_STRIDE (0x10)
/* words written per slot in the output record array (x, y, value) */
#define OUT_SLOT_WORDS (3)
/* float indices of the position row in a car matrix (RwMatrix pos, inferred) */
#define RWMATRIX_POS_X (0xc)
#define RWMATRIX_POS_Y (0xd)
#define RWMATRIX_POS_Z (0xe)
/* time_trial_panel_step_part_a
 * Per-slot step for the time trial panel. For slot i it takes car_matrix_array[i] (a float *,
 * RwMatrix-like: [0..2] right axis (inferred), [12..14] position) and the panel point at
 * g_hudpanel_658d4c + i * PANEL_POINT_STRIDE (x at [-1], y at [0], z at [+1]).
 * It computes the panel->car vector, normalises it, takes its component along the car's
 * right axis (side_dot), scales that by the distance and k_f_n90 (-90, inferred from the
 * name), stores {0, 0, value} in g_time_trial_658d00 at slot i, then calls
 * btb_panel_slot_set_position(i, record). Returns nothing. unused_arg is never read.
 * (was: "per slot direction vector between car and panel point, normalised length")
 */

void __cdecl time_trial_panel_step_part_a(char *car_matrix_array,char *unused_arg)

{
  float *car_matrix;
  int slot_index;
  undefined4 *slot_out;
  float *panel_pos;
  float dist_to_car;
  struct {
    float side_dot;
    RwV3d panel_to_car;
  } frame;

  slot_index = 0;
  slot_out = g_time_trial_658d00;
  panel_pos = (float *)&g_hudpanel_658d4c;
  /* One pass per panel point, until panel_pos reaches the end of the table at g_time_trial_658dcc */
  do {
    car_matrix = *(float **)(car_matrix_array + slot_index * 4);
    frame.panel_to_car.x = car_matrix[RWMATRIX_POS_X] - panel_pos[-1];
    frame.panel_to_car.y = car_matrix[RWMATRIX_POS_Y] - *panel_pos;
    frame.panel_to_car.z = car_matrix[RWMATRIX_POS_Z] - panel_pos[1];
    /* Car exactly on the panel point: no direction, so the value is 0 */
    if (((frame.panel_to_car.x == k_f_0) && (frame.panel_to_car.y == k_f_0)) && (frame.panel_to_car.z == k_f_0)) {
      frame.side_dot = 0.0;
      dist_to_car = k_f_0;
    }
    /* Normalise in place (RwV3dNormalize returns the distance), then project on the car's right axis */
    else {
      dist_to_car = RwV3dNormalize(&frame.panel_to_car,&frame.panel_to_car);
      frame.side_dot = car_matrix[2] * frame.panel_to_car.z + frame.panel_to_car.x * *car_matrix + car_matrix[1] * frame.panel_to_car.y;
    }
    *slot_out = 0;
    slot_out[1] = 0;
    *(float *)&slot_out[2] = frame.side_dot * dist_to_car * k_f_n90;
    btb_panel_slot_set_position(slot_index,slot_out);
    panel_pos = panel_pos + PANEL_POINT_STRIDE;
    slot_index = slot_index + 1;
    slot_out = slot_out + OUT_SLOT_WORDS;
  } while ((char *)panel_pos < ((char *)&g_time_trial_658dcc));
  return;
}


