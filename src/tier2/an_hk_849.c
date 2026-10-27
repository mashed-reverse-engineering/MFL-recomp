#include "mfl_tier2.h"
/*0041a380*/ extern int ai_ftol_noop_wrapper();
/*0044a410*/ extern int camera_update_view_window();
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode();
/*005dd7f4*/ extern float k_f_80;
/*0060d698*/ extern undefined g_camera_record_60d698;
/*006a06ec*/ extern undefined4 g_camera_record_6a06ec;
/*006a06f0*/ extern undefined4 g_camera_record_6a06f0;
/*006a06f8*/ extern undefined4 g_camera_record_6a06f8;
/*007ef7b8*/ extern undefined4 game_mode;
/*008c5928*/ extern undefined4 g_car_8c5928;
/*008c592c*/ extern undefined4 g_car_8c592c;
/*008c5930*/ extern undefined4 g_camera_8c5930;
/*008c5934*/ extern undefined4 g_car_8c5934;
/*008c598c*/ extern undefined4 g_car_8c598c;
/*008c5a00*/ extern float g_car_8c5a00;
/*008c5a08*/ extern undefined g_camera_record_8c5a08;
/*008c5a0c*/ extern undefined4 g_camera_record_8c5a0c;
/*008c5ae0*/ extern undefined g_camera_record_8c5ae0;
/*008c7440*/ extern undefined4 g_car_8c7440;
/*008c7e1c*/ extern undefined4 g_camera_8c7e1c;
/*008c7e20*/ extern undefined4 g_camera_8c7e20;

/* Ring of 4096 camera records (counter g_car_8c7440). */
#define CAMERA_RECORD_RING_MASK (0xfff)
/* Bit 8 of the counter (every 256 records) picks between two alternate cars (inferred). */
#define RING_BUCKET_BIT (0x100)
/* One car record: 0xd8 bytes = 0x36 words (inferred: per-car camera record). */
#define CAR_RECORD_STRIDE (0xd8)
#define CAR_RECORD_STRIDE_WORDS (0x36)
/* Offset in the camera object of the car id passed to car_is_active (inferred). */
#define CAMERA_CAR_OFFSET (0x20)
/* Switching the viewed car needs this many pending calls, and this many calls in total (inferred: frames). */
#define CAMERA_SWITCH_PENDING_FRAMES (7)
#define CAMERA_SWITCH_TOTAL_FRAMES (0x1e)

/* camera_record_add */

/* "adds one camera record to the ring at 0x8C7440 (mask 0xFFF)" */

/*
 * Advances the camera record ring by one and picks which car's record the camera should show.
 * camera: the camera object (car id at +0x20, record data written to +0x58, clump frame at +0x84).
 * Result: g_car_8c5928 and camera+0x58 are set from the chosen car, then camera_update_view_window(camera) runs.
 * Note: `camera` is overwritten by car_get_mode's out-pointer in the game_mode == GAME_MODE_BEAT_THE_BOMB branch, so
 * camera_addr keeps the original pointer. selected_car is scratch until the final pick.
 */
void __cdecl camera_record_add(int camera)

{
  float min_score;
  int camera_addr;
  uint ring_slot;
  float *record_ptr;
  int selected_car;
  int car_index;
  undefined4 *puVar6; /* unused in this body */
  int group_last;
  undefined4 *puVar8; /* unused in this body */
  undefined1 car_mode_out [4]; /* out-value of car_get_mode */

  camera_addr = camera;
  g_car_8c7440 = g_car_8c7440 + 1;
  ring_slot = g_car_8c7440 & CAMERA_RECORD_RING_MASK;
  /* No camera in game modes 5, or 10 (no car mode / car inactive), or 8 (ai_ftol_noop_wrapper returns -1). */
  if (game_mode == GAME_MODE_KILL_COPTER) {
    selected_car = 0;
  }
  else if ((game_mode == GAME_MODE_BEAT_THE_BOMB) &&
          ((car_get_mode(0,&camera,car_mode_out), camera != 0 ||
           (selected_car = car_is_active(0), selected_car == 0)))) {
    selected_car = 0;
  }
  else if ((game_mode == GAME_MODE_SURVIVAL) && (selected_car = ai_ftol_noop_wrapper(0), selected_car == -1)) {
    selected_car = 0;
  }
  else {
    selected_car = car_is_active(*(undefined4 *)(camera_addr + CAMERA_CAR_OFFSET));
    if (selected_car == 0) {
      selected_car = 0;
    }
    else {
      /* Per-bucket mode (16 buckets of 256 ring slots): 0 = none, 2 = alternate pair (RING_BUCKET_BIT),
         3 = fixed car g_camera_8c7e1c when its score is above k_f_80, otherwise scan; other values scan (inferred). */
      selected_car = *(int *)((char *)&g_camera_record_60d698 + ((int)ring_slot >> 8) * 4);
      if (selected_car == 0) {
        selected_car = 0;
      }
      else if (selected_car == 2) {
        selected_car = g_camera_8c7e20;
        if ((g_car_8c7440 & RING_BUCKET_BIT) != 0) {
          selected_car = g_car_8c5934;
        }
      }
      else if ((selected_car != 3) ||
              (selected_car = g_camera_8c7e1c,
              !(*(float *)((char *)&g_camera_record_8c5a08 + g_camera_8c7e1c * CAR_RECORD_STRIDE) > k_f_80))) {      /* NaN takes the branch (original: test ah,0x41; je) */
        min_score = *(float *)((char *)&g_camera_record_8c5a08 + g_camera_8c5930 * CAR_RECORD_STRIDE);
        car_index = g_camera_8c5930;
        selected_car = g_camera_8c5930;
        /* Scan the records from g_camera_8c5930 to g_car_8c592c in groups of 4 (unrolled),
           keeping the smallest float at record offset 0 as min_score (inferred). group_last is the 4th index of the group. */
        if (3 < ((int)g_car_8c592c - (int)g_camera_8c5930) + 1) {
          group_last = g_camera_8c5930 + 3;
          record_ptr = (float *)((char *)&g_camera_record_8c5ae0 + g_camera_8c5930 * CAR_RECORD_STRIDE);
          do {
            if (record_ptr[-CAR_RECORD_STRIDE_WORDS] < min_score) {
              min_score = record_ptr[-CAR_RECORD_STRIDE_WORDS];
              selected_car = car_index;
            }
            if (*record_ptr < min_score) {
              selected_car = group_last + -2;
              min_score = *record_ptr;
            }
            if (record_ptr[CAR_RECORD_STRIDE_WORDS] < min_score) {
              selected_car = group_last + -1;
              min_score = record_ptr[CAR_RECORD_STRIDE_WORDS];
            }
            if (record_ptr[0x6c] < min_score) {
              min_score = record_ptr[0x6c];
              selected_car = group_last;
            }
            group_last = group_last + 4;
            car_index = car_index + 4;
            record_ptr = record_ptr + CAR_RECORD_STRIDE;
          } while (group_last < (int)g_car_8c592c);
        }
        /* Tail: the remaining records one at a time. */
        if (car_index <= (int)g_car_8c592c) {
          record_ptr = (float *)((char *)&g_camera_record_8c5a08 + car_index * CAR_RECORD_STRIDE);
          do {
            if (*record_ptr < min_score) {
              min_score = *record_ptr;
              selected_car = car_index;
            }
            car_index = car_index + 1;
            record_ptr = record_ptr + CAR_RECORD_STRIDE_WORDS;
          } while (car_index <= (int)g_car_8c592c);
        }
      }
    }
  }
  /* Hysteresis: the viewed car (g_camera_record_6a06ec) changes only after the new pick has been
     pending for more than CAMERA_SWITCH_PENDING_FRAMES calls and the total counter exceeds CAMERA_SWITCH_TOTAL_FRAMES.
     The pending counter (6a06f0) and the total counter (6a06f8) reset on commit. */
  g_camera_record_6a06f8 = g_camera_record_6a06f8 + 1;
  car_index = selected_car;
  if (((selected_car != g_camera_record_6a06ec) &&
      (g_camera_record_6a06f0 = g_camera_record_6a06f0 + 1, car_index = g_camera_record_6a06ec,
      CAMERA_SWITCH_PENDING_FRAMES < (int)g_camera_record_6a06f0)) &&
     (CAMERA_SWITCH_TOTAL_FRAMES < (int)g_camera_record_6a06f8)) {
    g_camera_record_6a06f0 = 0;
    g_camera_record_6a06f8 = 0;
    car_index = selected_car;
    g_camera_record_6a06ec = selected_car;
  }
  /* Copy the chosen car's record into the globals and the camera, then refresh the view. */
  g_car_8c5928 = (&g_camera_record_8c5a0c)[car_index * CAR_RECORD_STRIDE_WORDS];
  *(undefined4 *)(camera_addr + 0x58) = ((undefined4 *)&g_car_8c5a00)[car_index * CAR_RECORD_STRIDE_WORDS];
  camera_update_view_window(camera_addr);
  *(RwMatrix *)(*(char **)(*(char **)(camera_addr + 0x84) + 4) + 0x10) = *(RwMatrix *)(&g_car_8c598c + car_index * CAR_RECORD_STRIDE_WORDS);
  return;
}


