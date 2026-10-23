#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*00415370*/ extern undefined __cdecl hud_player_marker_position_calc_with_facing(void *p0, void *p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, void *p6, void *p7, undefined4 p8, undefined4 p9, void *p10);
/*004154b0*/ extern undefined __cdecl hud_player_marker_position_calc(void *p0, void *p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, void *p6, void *p7, undefined4 p8, undefined4 p9, void *p10);
/*004162f0*/ extern int trail_set_param();
/*00422150*/ extern int vehicle_get_flags_10();
/*00422190*/ extern int vehicle_get_flag_400();
/*00422280*/ extern int vehicle_part_frame();
/*004222a0*/ extern int vehicle_part_matrix();
/*0044c140*/ extern int __cdecl player_distance_warning(undefined4 p0);
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0047b480*/ extern double car_probe_ground_distance();
/*0047b520*/ extern int car_current_matrix();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd370*/ extern float k_f_0_3;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb48*/ extern float k_f_16;
/*005de1ac*/ extern float k_f_0_125;
/*005de1b0*/ extern float k_f_n0_05;
/*005de1b4*/ extern float k_f_0_0625;
/*005de1b8*/ extern float k_f_31;
/*0060392c*/ extern float g_hud_player_60392c;
/*0065314c*/ extern undefined4 g_car_65314c;
/*00653160*/ extern undefined4 g_hud_653160;
/*00653164*/ extern undefined4 g_hud_player_653164;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7c0*/ extern undefined4 g_cars_7ef7c0;
/*007ef804*/ extern undefined4 g_tick_number;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*00801b0c*/ extern undefined4 g_car_801b0c;
/*008c9f00*/ extern undefined g_hud_player_8c9f00;
/*008cb500*/ extern undefined g_hud_8cb500;
/*008d8b00*/ extern undefined4 g_trail_8d8b00;
#define FLASH_PHASE_MASK (0x1f)
#define HUD_MARKER_STRIDE (0x2c)
#define CONTROLLER_NONE (-1)
/* hud_player_markers_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per player distance to the camera and flashing (tick & 0x1F) used for the markers above the cars
   (inferred)" */
/* Per frame update of the player markers drawn over the cars (inferred from the callees).
   For each player slot it measures the camera-to-car distance, then places that car's
   marker parts (vehicle part ids 0x19-0x1c, 0x37/0x38, 0x39/0x3a, 0x2a) through
   hud_player_marker_position_calc(_with_facing). marker_context is passed through to those
   calls unchanged (inferred). Returns nothing. */

void hud_player_markers_update(undefined4 marker_context)

{
  float ground_distance;
  PlayerSlot *slot;
  int result;
  int loop_result;
  int player_index;
  double ground_probe;
  RwReal camera_distance;
  undefined4 part_id;
  undefined1 *part_matrix_ptr;
  struct {
    float loop_offset;
    float marker_fade;
    undefined1 local_78;
    undefined1 local_77;
    undefined1 local_76;
    undefined1 local_75;
    undefined1 local_74;
    undefined1 local_73;
    undefined1 local_72;
    undefined1 local_71;
    undefined1 part_matrix [32];
    float local_50;
    float local_4c;
    float local_48;
    undefined1 pad_44[8];
    float marker_height;
    undefined1 pad_38[8];
    undefined1 local_30;
    undefined1 local_2f;
    undefined1 local_2e;
    undefined1 local_2d;
    undefined1 local_2c;
    undefined1 local_2b;
    undefined1 local_2a;
    undefined1 local_29;
    PlayerSlot *slot_cursor;
    char *car_matrix;
    int local_20;
    float flash_phase;
    int player_count;
    int vehicle_flag_400;
    int local_10;
    RwV3d camera_to_car;
  } frame;
  
  frame.local_78 = 0xff;
  frame.local_77 = 0xff;
  frame.local_76 = 0xff;
  frame.local_75 = 0xff;
  frame.local_30 = 0xff;
  frame.local_2f = 0;
  frame.local_2e = 0;
  frame.local_2d = 0xff;
  frame.local_74 = 0xff;
  frame.local_73 = 0;
  frame.local_72 = 0;
  frame.local_71 = 0xff;
  frame.local_2c = 0;
  frame.local_2b = 0;
  frame.local_2a = 0xff;
  frame.local_29 = 0xff;
  frame.player_count = get_player_count();
  frame.flash_phase = (float)(g_tick_number & FLASH_PHASE_MASK);
  player_index = 0;
  g_trail_8d8b00 = 0;
  g_hud_653160 = 0;
  g_hud_player_653164 = 0;
  /* flash_phase: tick & 0x1f folded into a triangle wave and scaled to 0..1 (inferred) */
  if (k_f_16 < frame.flash_phase) {
    frame.flash_phase = k_f_31 - frame.flash_phase;
  }
  frame.flash_phase = frame.flash_phase * k_f_0_0625;
  /* One pass per player slot */
  if (0 < frame.player_count) {
    frame.slot_cursor = g_player_slots;
    do {
      slot = frame.slot_cursor;
      /* Offset from the viewport camera to this car, and its length (camera_distance) */
      car_current_matrix(&frame.car_matrix,player_index);
      ground_distance = *(float *)(frame.car_matrix + 0x30);
      result = get_viewport_camera_matrix(0);
      frame.camera_to_car.x = ground_distance - *(float *)(result + 0x30);
      ground_distance = *(float *)(frame.car_matrix + 0x34);
      result = get_viewport_camera_matrix(0);
      frame.camera_to_car.y = ground_distance - *(float *)(result + 0x34);
      ground_distance = *(float *)(frame.car_matrix + 0x38);
      result = get_viewport_camera_matrix(0);
      frame.camera_to_car.z = ground_distance - *(float *)(result + 0x38);
      camera_distance = RwV3dLength(&frame.camera_to_car);
      g_hud_player_60392c = camera_distance * k_f_0_5 - float_1_0;
      if (g_hud_player_60392c <= k_f_n0_05) {
        if (g_hud_player_60392c < k_f_n1) {
          g_hud_player_60392c = -1.0;
        }
      }
      else {
        g_hud_player_60392c = -0.05;
      }
      /* Ground distance under the car (ground_distance; the same float is a scratch value above) */
      ground_probe = (double)car_probe_ground_distance
                                 (*(undefined4 *)(frame.car_matrix + 0x30),*(undefined4 *)(frame.car_matrix + 0x34),
                                  *(undefined4 *)(frame.car_matrix + 0x38),&g_car_801b0c);
      ground_distance = (float)ground_probe;
      /* Slot has a controller. Braking car with no flags set: brake markers (parts 0x39/0x3a), then
         a fading copy loop of 4 passes (inferred) */
      if (CONTROLLER_NONE < slot->controller) {
        vehicle_get_flags_10(player_index,0,&frame.local_20);
        vehicle_get_flag_400(player_index,&frame.vehicle_flag_400);
        if (((g_player_inputs[slot->controller].brake != 0) && (frame.local_20 == 0)) && (frame.vehicle_flag_400 == 0)
           ) {
          result = vehicle_part_frame(player_index,0x39);
          if (result != 0) {
            part_matrix_ptr = frame.part_matrix;
            part_id = 0x39;
            result = player_index;
            vehicle_part_matrix(player_index, 0x39, frame.part_matrix);
            frame.local_50 = frame.local_50 * k_f_n1;
            frame.local_4c = frame.local_4c * k_f_n1;
            frame.local_48 = frame.local_48 * k_f_n1;
            hud_player_marker_position_calc_with_facing
                      (((char *)&g_hud_8cb500 + g_hud_653160 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,0x3f800000,
                       *(int *)&g_hud_player_60392c,1,&frame.local_30,&g_hud_653160,result,part_id,part_matrix_ptr);
          }
          result = vehicle_part_frame(player_index,0x3a);
          if (result != 0) {
            part_matrix_ptr = frame.part_matrix;
            part_id = 0x3a;
            result = player_index;
            vehicle_part_matrix(player_index, 0x3a, frame.part_matrix);
            frame.local_50 = frame.local_50 * k_f_n1;
            frame.local_4c = frame.local_4c * k_f_n1;
            frame.local_48 = frame.local_48 * k_f_n1;
            hud_player_marker_position_calc_with_facing
                      (((char *)&g_hud_8cb500 + g_hud_653160 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,0x3f800000,
                       *(int *)&g_hud_player_60392c,1,&frame.local_30,&g_hud_653160,result,part_id,part_matrix_ptr);
          }
          frame.loop_offset = 0.0;
          frame.marker_fade = 1.0;
          result = 4;
          do {
            loop_result = vehicle_part_frame(player_index,0x39);
            if (loop_result != 0) {
              part_matrix_ptr = frame.part_matrix;
              part_id = 0x39;
              loop_result = player_index;
              vehicle_part_matrix(player_index, 0x39, frame.part_matrix);
              frame.marker_height = (ground_distance - (frame.marker_height - ground_distance)) + frame.loop_offset;
              frame.local_50 = frame.local_50 * k_f_n1;
              frame.local_4c = frame.local_4c * k_f_n1;
              frame.local_48 = frame.local_48 * k_f_n1;
              hud_player_marker_position_calc_with_facing
                        (((char *)&g_hud_player_8c9f00 + g_hud_player_653164 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,*(int *)&frame.marker_fade
                         ,*(int *)&g_hud_player_60392c,0,&frame.local_74,&g_hud_player_653164,loop_result,part_id,part_matrix_ptr);
            }
            loop_result = vehicle_part_frame(player_index,0x3a);
            if (loop_result != 0) {
              part_matrix_ptr = frame.part_matrix;
              part_id = 0x3a;
              loop_result = player_index;
              vehicle_part_matrix(player_index, 0x3a, frame.part_matrix);
              frame.marker_height = (ground_distance - (frame.marker_height - ground_distance)) + frame.loop_offset;
              frame.local_50 = frame.local_50 * k_f_n1;
              frame.local_4c = frame.local_4c * k_f_n1;
              frame.local_48 = frame.local_48 * k_f_n1;
              hud_player_marker_position_calc_with_facing
                        (((char *)&g_hud_player_8c9f00 + g_hud_player_653164 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,*(int *)&frame.marker_fade
                         ,*(int *)&g_hud_player_60392c,0,&frame.local_74,&g_hud_player_653164,loop_result,part_id,part_matrix_ptr);
            }
            result = result + -1;
            frame.loop_offset = frame.loop_offset - k_f_0_05;
            frame.marker_fade = frame.marker_fade - k_f_0_125;
          } while (result != 0);
        }
      }
      /* Race mode: part 0x2a marker for each used slot, except car g_cars_7ef7c0 (inferred).
         Other modes: trail parameter from player_distance_warning (trail on when it is set and game_mode != GAME_MODE_AIRSTRIKE) */
      if (game_mode == GAME_MODE_HARE_AND_HOUNDS) {
        if (player_index != g_cars_7ef7c0) {
          result = race_setup_slot_used(player_index);
          if (result != 0) {
            result = vehicle_part_frame(player_index,0x2a);
            if (result != 0) {
              part_matrix_ptr = frame.part_matrix;
              part_id = 0x2a;
              result = player_index;
              vehicle_part_matrix(player_index, 0x2a, frame.part_matrix);
              frame.marker_height = frame.marker_height + k_f_0_3;
              hud_player_marker_position_calc
                        (((char *)&g_hud_8cb500 + g_hud_653160 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,*(int *)&frame.flash_phase,0xbe4ccccd,1,                          &frame.local_2c,&g_hud_653160,result,part_id,part_matrix_ptr);
            }
          }
        }
      }
      else {
        result = player_distance_warning(player_index);
        if ((result == 0) || (game_mode == GAME_MODE_AIRSTRIKE)) {
          part_id = 0;
        }
        else {
          part_id = 1;
        }
        trail_set_param(player_index,0,part_id);
      }
      /* Car flagged in g_car_65314c: parts 0x19-0x1c (4 fading copies) when flags_10 is 0,
         then parts 0x37/0x38 (8 fading copies) when the second flags_10 is 0 (inferred) */
      if ((&g_car_65314c)[player_index] != 0) {
        vehicle_get_flags_10(player_index,&frame.local_10,0);
        if (frame.local_10 == 0) {
          result = vehicle_part_frame(player_index,0x19);
          if (result != 0) {
            part_matrix_ptr = frame.part_matrix;
            part_id = 0x19;
            result = player_index;
            vehicle_part_matrix(player_index, 0x19, frame.part_matrix);
            hud_player_marker_position_calc_with_facing
                      (((char *)&g_hud_8cb500 + g_hud_653160 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,0x3f800000,
                       *(int *)&g_hud_player_60392c,1,&frame.local_78,&g_hud_653160,result,part_id,part_matrix_ptr);
          }
          result = vehicle_part_frame(player_index,0x1a);
          if (result != 0) {
            part_matrix_ptr = frame.part_matrix;
            part_id = 0x1a;
            result = player_index;
            vehicle_part_matrix(player_index, 0x1a, frame.part_matrix);
            hud_player_marker_position_calc_with_facing
                      (((char *)&g_hud_8cb500 + g_hud_653160 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,0x3f800000,
                       *(int *)&g_hud_player_60392c,1,&frame.local_78,&g_hud_653160,result,part_id,part_matrix_ptr);
          }
          result = vehicle_part_frame(player_index,0x1c);
          if (result != 0) {
            part_matrix_ptr = frame.part_matrix;
            part_id = 0x1c;
            result = player_index;
            vehicle_part_matrix(player_index, 0x1c, frame.part_matrix);
            hud_player_marker_position_calc_with_facing
                      (((char *)&g_hud_8cb500 + g_hud_653160 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,0x3f800000,
                       *(int *)&g_hud_player_60392c,1,&frame.local_78,&g_hud_653160,result,part_id,part_matrix_ptr);
          }
          result = vehicle_part_frame(player_index,0x1b);
          if (result != 0) {
            part_matrix_ptr = frame.part_matrix;
            part_id = 0x1b;
            result = player_index;
            vehicle_part_matrix(player_index, 0x1b, frame.part_matrix);
            hud_player_marker_position_calc_with_facing
                      (((char *)&g_hud_8cb500 + g_hud_653160 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,0x3f800000,
                       *(int *)&g_hud_player_60392c,1,&frame.local_78,&g_hud_653160,result,part_id,part_matrix_ptr);
          }
          frame.loop_offset = 0.0;
          frame.marker_fade = 1.0;
          result = 4;
          do {
            loop_result = vehicle_part_frame(player_index,0x19);
            if (loop_result != 0) {
              part_matrix_ptr = frame.part_matrix;
              part_id = 0x19;
              loop_result = player_index;
              vehicle_part_matrix(player_index, 0x19, frame.part_matrix);
              frame.marker_height = (ground_distance - (frame.marker_height - ground_distance)) + frame.loop_offset;
              hud_player_marker_position_calc_with_facing
                        (((char *)&g_hud_player_8c9f00 + g_hud_player_653164 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,*(int *)&frame.marker_fade
                         ,*(int *)&g_hud_player_60392c,0,&frame.local_78,&g_hud_player_653164,loop_result,part_id,part_matrix_ptr);
            }
            loop_result = vehicle_part_frame(player_index,0x1a);
            if (loop_result != 0) {
              part_matrix_ptr = frame.part_matrix;
              part_id = 0x1a;
              loop_result = player_index;
              vehicle_part_matrix(player_index, 0x1a, frame.part_matrix);
              frame.marker_height = (ground_distance - (frame.marker_height - ground_distance)) + frame.loop_offset;
              hud_player_marker_position_calc_with_facing
                        (((char *)&g_hud_player_8c9f00 + g_hud_player_653164 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,*(int *)&frame.marker_fade
                         ,*(int *)&g_hud_player_60392c,0,&frame.local_78,&g_hud_player_653164,loop_result,part_id,part_matrix_ptr);
            }
            loop_result = vehicle_part_frame(player_index,0x1c);
            if (loop_result != 0) {
              part_matrix_ptr = frame.part_matrix;
              part_id = 0x1c;
              loop_result = player_index;
              vehicle_part_matrix(player_index, 0x1c, frame.part_matrix);
              frame.marker_height = (ground_distance - (frame.marker_height - ground_distance)) + frame.loop_offset;
              hud_player_marker_position_calc_with_facing
                        (((char *)&g_hud_player_8c9f00 + g_hud_player_653164 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,*(int *)&frame.marker_fade
                         ,*(int *)&g_hud_player_60392c,0,&frame.local_78,&g_hud_player_653164,loop_result,part_id,part_matrix_ptr);
            }
            loop_result = vehicle_part_frame(player_index,0x1b);
            if (loop_result != 0) {
              part_matrix_ptr = frame.part_matrix;
              part_id = 0x1b;
              loop_result = player_index;
              vehicle_part_matrix(player_index, 0x1b, frame.part_matrix);
              frame.marker_height = (ground_distance - (frame.marker_height - ground_distance)) + frame.loop_offset;
              hud_player_marker_position_calc_with_facing
                        (((char *)&g_hud_player_8c9f00 + g_hud_player_653164 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,*(int *)&frame.marker_fade
                         ,*(int *)&g_hud_player_60392c,0,&frame.local_78,&g_hud_player_653164,loop_result,part_id,part_matrix_ptr);
            }
            result = result + -1;
            frame.loop_offset = frame.loop_offset - k_f_0_05;
            frame.marker_fade = frame.marker_fade - k_f_0_125;
          } while (result != 0);
        }
        vehicle_get_flags_10(player_index,&frame.local_20,0);
        if (frame.local_20 == 0) {
          result = vehicle_part_frame(player_index,0x37);
          if (result != 0) {
            part_matrix_ptr = frame.part_matrix;
            part_id = 0x37;
            result = player_index;
            vehicle_part_matrix(player_index, 0x37, frame.part_matrix);
            frame.local_50 = frame.local_50 * k_f_n1;
            frame.local_4c = frame.local_4c * k_f_n1;
            frame.local_48 = frame.local_48 * k_f_n1;
            hud_player_marker_position_calc_with_facing
                      (((char *)&g_hud_8cb500 + g_hud_653160 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,0x3e800000,
                       *(int *)&g_hud_player_60392c,1,&frame.local_74,&g_hud_653160,result,part_id,part_matrix_ptr);
          }
          result = vehicle_part_frame(player_index,0x38);
          if (result != 0) {
            part_matrix_ptr = frame.part_matrix;
            part_id = 0x38;
            result = player_index;
            vehicle_part_matrix(player_index, 0x38, frame.part_matrix);
            frame.local_50 = frame.local_50 * k_f_n1;
            frame.local_4c = frame.local_4c * k_f_n1;
            frame.local_48 = frame.local_48 * k_f_n1;
            hud_player_marker_position_calc_with_facing
                      (((char *)&g_hud_8cb500 + g_hud_653160 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,0x3e800000,
                       *(int *)&g_hud_player_60392c,1,&frame.local_74,&g_hud_653160,result,part_id,part_matrix_ptr);
          }
          frame.loop_offset = 0.0;
          frame.marker_fade = 1.0;
          result = 8;
          do {
            loop_result = vehicle_part_frame(player_index,0x37);
            if (loop_result != 0) {
              part_matrix_ptr = frame.part_matrix;
              part_id = 0x37;
              loop_result = player_index;
              vehicle_part_matrix(player_index, 0x37, frame.part_matrix);
              frame.marker_height = (ground_distance - (frame.marker_height - ground_distance)) + frame.loop_offset;
              frame.local_50 = frame.local_50 * k_f_n1;
              frame.local_4c = frame.local_4c * k_f_n1;
              frame.local_48 = frame.local_48 * k_f_n1;
              hud_player_marker_position_calc_with_facing
                        (((char *)&g_hud_player_8c9f00 + g_hud_player_653164 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,*(int *)&frame.marker_fade
                         ,*(int *)&g_hud_player_60392c,0,&frame.local_74,&g_hud_player_653164,loop_result,part_id,part_matrix_ptr);
            }
            loop_result = vehicle_part_frame(player_index,0x38);
            if (loop_result != 0) {
              part_matrix_ptr = frame.part_matrix;
              part_id = 0x38;
              loop_result = player_index;
              vehicle_part_matrix(player_index, 0x38, frame.part_matrix);
              frame.marker_height = (ground_distance - (frame.marker_height - ground_distance)) + frame.loop_offset;
              frame.local_50 = frame.local_50 * k_f_n1;
              frame.local_4c = frame.local_4c * k_f_n1;
              frame.local_48 = frame.local_48 * k_f_n1;
              hud_player_marker_position_calc_with_facing
                        (((char *)&g_hud_player_8c9f00 + g_hud_player_653164 * HUD_MARKER_STRIDE),frame.part_matrix,marker_context,*(int *)&frame.marker_fade
                         ,*(int *)&g_hud_player_60392c,0,&frame.local_74,&g_hud_player_653164,loop_result,part_id,part_matrix_ptr);
            }
            result = result + -1;
            frame.loop_offset = frame.loop_offset - k_f_0_05;
            frame.marker_fade = frame.marker_fade - k_f_0_125;
          } while (result != 0);
        }
      }
      player_index = player_index + 1;
      frame.slot_cursor = frame.slot_cursor + 1;
    } while (player_index < frame.player_count);
  }
  return;
}


