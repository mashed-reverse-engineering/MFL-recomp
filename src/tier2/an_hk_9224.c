#include "mfl_tier2.h"
/*00409eb0*/ extern float __cdecl player_rank_key(uint car);
/*0040a950*/ extern int __cdecl fx_wander_target(int key, float *a, float *b);
/*0040fdc0*/ extern int race_get_phase();
/*004b2b48*/ extern int _ftol();
/*004b3284*/ extern double crt_acos_body(double p0);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd364*/ extern float k_f_n1;
/*005dd634*/ extern float k_f_2;
/*005dd7e8*/ extern float k_f_60;
/*005dd7f0*/ extern float k_f_30;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005ddacc*/ extern float k_f_360;
/*005ddad4*/ extern float k_f_90;
/*005ddc20*/ extern float k_f_45;
/*005e64a4*/ extern float k_f_0;
/*005de8c4*/ extern float k_f_0_016666668;
/*005debe4*/ extern float k_f_0_022222223;
/*005dfd7c*/ extern float k_f_1_6666666en4;
/*006292e4*/ extern float g_race_update_6292e4;
/*0062ac68*/ extern float g_car_62ac68;
/*0062ac6c*/ extern float g_fx_62ac6c;
/*0062ac70*/ extern float g_fx_62ac70;
/*0062ac74*/ extern float g_car_62ac74;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01a0*/ extern undefined1 g_cars_7f01a0;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008b06e0*/ extern struct Car player_car[16];
/* car_auto_steer_assist_step */

/* "when the player gives no steering input steers the car matrix toward the route heading
   (fx_wander_target, limit by speed)" */

undefined4 car_auto_steer_assist_step(uint car_index,int step_count)

{
  float *car_forward;
  double step_count_f;
  uint car_index_copy;
  int race_phase;
  undefined4 rank_key;
  PlayerInput *input;
  int car_byte_offset;
  double acos_result;
  double dot;
  struct {
    float target_heading;
    float target_spread;
    RwV3d forward_flat;
    float heading;
  } frame;
  double angle;
  double wrapped;
  double spread;
  float t;
  
  car_index_copy = car_index;
  /* Auto-steer assist (inferred): when the car's input has neither left nor right pressed (and the race phase allows control), rotates the car's current matrix about the g_car_62ac74 axis toward the heading from fx_wander_target; step_count scales the turn, capped by speed. Returns 1 if car_index is valid, else 0. */
  if (car_index < 0x10) {
    car_byte_offset = (int)car_index * 0xd04;
    input = g_player_inputs + g_player_slots[(int)car_index].controller;
    if (player_car[(int)car_index].mode != 0) {
      input = (PlayerInput *)&g_cars_7f01a0;
    }
    if (player_car[(int)car_index].controls_disabled != 0) {
      input = (PlayerInput *)&g_cars_7f01a0;
    }
    race_phase = race_get_phase();
    if (((race_phase != 6) && (race_phase = race_get_phase(), race_phase != 0xb)) &&
       (race_phase = race_get_phase(), race_phase != RACE_PHASE_WINNER_DONE)) {
      input = (PlayerInput *)&g_cars_7f01a0;
    }
    if ((input->left == 0) && (input->right == 0)) {
      car_forward = player_car[(int)car_index].forward;
      frame.forward_flat.x = *car_forward;
      frame.forward_flat.z = player_car[(int)car_index].forward[2];
      if ((frame.forward_flat.x != k_f_0) || (frame.forward_flat.z != k_f_0)) {
        frame.forward_flat.y = 0.0;
        RwV3dNormalize(&frame.forward_flat,&frame.forward_flat);
        dot = g_fx_62ac70 * frame.forward_flat.z + g_car_62ac68 * frame.forward_flat.x + g_fx_62ac6c * frame.forward_flat.y;      /* kept in the x87 register (unrounded) and passed as a double, not stored to a float */
        if (dot < k_f_n1) {
          dot = k_f_n1;
        }
        else if (dot > float_1_0) {
          dot = float_1_0;
        }
        acos_result = (double)crt_acos_body(dot);
        frame.heading = (float)(-(acos_result * (double)(57.295799255371094) - 90.0) + (double)k_f_90);      /* the original subtracts a double 90.0 constant here (k_f_0 at 5ddc28 is that double, read as float it is 0) */      /* a float variable: the original stores it (rounded) before 360 - heading */
        frame.forward_flat.x = g_fx_62ac70 * player_car[(int)car_index].forward[1] -
                    g_fx_62ac6c * player_car[(int)car_index].forward[2];
        frame.forward_flat.y = g_car_62ac68 * player_car[(int)car_index].forward[2] - g_fx_62ac70 * *car_forward;
        frame.forward_flat.z = g_fx_62ac6c * *car_forward - g_car_62ac68 * player_car[(int)car_index].forward[1];
        t = frame.heading;
        if (frame.forward_flat.y < k_f_0) {
          t = k_f_360 - frame.heading;
        }
        rank_key = (int)player_rank_key(car_index);
        fx_wander_target(rank_key,&frame.target_heading,&frame.target_spread);
        wrapped = t - frame.target_heading;      /* double: the original keeps this in st(0) through the wrap loops and the compares (no float store) */
        for (; wrapped < k_f_0; wrapped = wrapped + k_f_90) {
        }
        for (; k_f_90 < wrapped; wrapped = wrapped - k_f_90) {
        }
        if (!(k_f_30 > wrapped)) {
          if (!(k_f_60 > wrapped)) {
            angle = k_f_90 - wrapped;
          }
          else {
            angle = k_f_45 - wrapped;
          }
        }
        else {
          angle = -wrapped;
        }
        t = player_car[car_index_copy].speed * k_f_1_6666666en4;
        if (k_f_2 < t) {
          t = 2.0f;
        }
        if (t < angle) {
          angle = t;
        }
        if (angle < -t) {
          angle = -t;
        }
        step_count_f = (float)step_count;
        if (step_count < 0) {
          step_count_f = step_count_f + k_f_4_2949673e9;
        }
        spread = frame.target_spread * k_f_4;      /* double: the original keeps it in st(0) (45 - spread cancels, a float store changes the angle by 1e-4) */
        if (spread < k_f_0) {
          spread = -spread;
        }
        if (k_f_45 < spread) {
          spread = k_f_45;
        }
        angle = angle * step_count_f;      /* one statement per factor: a single product chain gets reassociated by cl /O2 (the original multiplies in this order) */
        angle = angle * k_f_0_016666668;
        angle = angle * g_race_update_6292e4;
        angle = angle * (k_f_45 - spread);
        angle = angle * k_f_0_022222223;
        if (angle != k_f_0) {
          RwMatrixRotate((RwMatrix *)((char *)player_car + player_car[car_index_copy].matrix_current * 0x40 + 0x928 + car_byte_offset),
                         (RwV3d *)&g_car_62ac74,angle,rwCOMBINEPRECONCAT);
        }
      }
    }
    return 1;
  }
  return 0;
}


