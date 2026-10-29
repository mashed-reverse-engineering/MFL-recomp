#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*00456b20*/ extern int course_rotating_object_update();
/*004574a0*/ extern int course_spline_objects_update();
/*00475730*/ extern char *get_viewport_camera();
/*0047a470*/ extern int car_set_wheel_contact();
/*0047b360*/ extern int car_wheel_get_field_88_a();
/*0048e470*/ extern int rwp_object_sync_and_reset();
/*0049c5f0*/ extern undefined __cdecl fx_snow_ptank_update(int p0, struct RwCamera *p1, undefined1 *p2, float p3, float p4);
/*005e64a4*/ extern float k_f_0;
/*006a2164*/ extern undefined4 g_course_rouabout_6a2164;
/*006a2168*/ extern undefined4 g_course_6a2168;
/*006a2188*/ extern undefined4 g_course_6a2188[0x1000];
/*006a218c*/ extern undefined4 g_course_rouabout_6a218c;
/*006a2190*/ extern undefined4 g_course_rouabout_6a2190;
/*006a2194*/ extern undefined4 g_course_rouabout_6a2194;
/*006a21a8*/ extern undefined4 g_course_rouabout_6a21a8;
/*006a23c8*/ extern undefined4 g_course_rouabout_6a23c8;
/*006a23cc*/ extern float g_course_rouabout_6a23cc;
/*006a2634*/ extern undefined4 g_course_6a2634;
/*007ef7f4*/ extern float g_tick_seconds;
/*008c53e0*/ extern undefined4 g_course_8c53e0;
/*008d8b30*/ extern undefined4 player_count;

#define WHEELS_PER_CAR (4) /* inferred: wheel index passed to car_wheel_get_field_88_a */
#define GATE_COUNT (8) /* inferred: flags at g_course_6a2168 end where g_course_6a2188 starts */
#define SNOW_FRAME_SENTINEL (-0x939394) /* inferred: tag written before fx_snow_ptank_update */
#define RESET_VEC_Y (0xbf800000) /* float bit pattern of -1.0f */

/* course_rouabout_cb1 */


/*
 * Per-frame update for the Rouabout course (inferred: course callback).
 * Updates the snow effect and the course's rotating and spline objects, then drives the wheel contact
 * table and the gate sequence. Gate state is kept in g_course_rouabout_6a2164 (triggered) and
 * g_course_rouabout_6a23c8 (next gate index); g_course_rouabout_6a23cc is the timer between gates.
 * unused_param_1 is never read. update_arg is passed through to the spline and rotating object updates
 * (meaning not known, inferred frame/time context). Returns nothing.
 */
void course_rouabout_cb1(char *unused_param_1,undefined4 update_arg)

{
  RwCamera *camera;
  int player_index;
  int count_or_wheel_index;
  undefined4 *contact_ptr;
  int wheel_index;
  int *snow_frame_ptr;
  float snow_param_3;
  float snow_param_4;
  struct {
    int wheel_field_88;
    int player_total;
    undefined4 reset_x;
    undefined4 reset_y;
    undefined4 reset_z;
  } frame;

  /* Snow effect and course objects update every frame, before the wheel logic. */
  course_spline_objects_update(update_arg);
  snow_param_4 = 0.05;
  snow_param_3 = 0.2;
  snow_frame_ptr = &frame.wheel_field_88;
  frame.wheel_field_88 = SNOW_FRAME_SENTINEL;
  camera = (RwCamera *)get_viewport_camera(0);
  fx_snow_ptank_update(0,camera,(undefined1 *)snow_frame_ptr,snow_param_3,snow_param_4);
  course_rotating_object_update(&g_course_8c53e0,update_arg);
  /* Mode A (g_course_6a2634 == 0, inferred meaning): every wheel of every player gets contact 1. */
  if (g_course_6a2634 == 0) {
    player_index = 0;
    count_or_wheel_index = get_player_count();
    if (0 < count_or_wheel_index) {
      do {
        wheel_index = 0;
        contact_ptr = g_course_6a2188;
        do {
          car_set_wheel_contact(contact_ptr,player_index,wheel_index,1);
          contact_ptr = contact_ptr + 1;
          wheel_index = wheel_index + 1;
        } while ((char *)contact_ptr < ((char *)&g_course_rouabout_6a21a8));
        player_index = player_index + 1;
      } while (player_index < count_or_wheel_index);
    }
  }
  /*
   * Mode B: count_or_wheel_index holds the player count here, then the wheel index during the scan.
   * Contacts come from the stored table (contact_ptr[-8]).
   */
  else {
    count_or_wheel_index = 0;
    frame.player_total = get_player_count();
    player_index = 0;
    if (0 < frame.player_total) {
      do {
        wheel_index = 0;
        /*
         * Trigger scan (only while not yet triggered): read each wheel's field 0x88 and, on a match with a
         * table value, start the gate sequence. The counter is not reset per player, so as written only the
         * first player is scanned (inferred from source, check disassembly).
         */
        if (g_course_rouabout_6a2164 == 0) {
          for (; count_or_wheel_index < WHEELS_PER_CAR; count_or_wheel_index = count_or_wheel_index + 1) {
            frame.wheel_field_88 = 0;
            car_wheel_get_field_88_a(player_index,count_or_wheel_index,&frame.wheel_field_88);
            if ((((frame.wheel_field_88 == g_course_6a2188[0]) || (frame.wheel_field_88 == g_course_rouabout_6a218c)) ||
                (frame.wheel_field_88 == g_course_rouabout_6a2190)) || (frame.wheel_field_88 == g_course_rouabout_6a2194))
            {
              g_course_rouabout_6a2164 = 1;
              g_course_rouabout_6a23c8 = 0;
              g_course_rouabout_6a23cc = 0.1;
            }
          }
        }
        contact_ptr = g_course_6a2188;
        do {
          car_set_wheel_contact(contact_ptr,player_index,wheel_index,contact_ptr[-8]);
          contact_ptr = contact_ptr + 1;
          wheel_index = wheel_index + 1;
        } while ((char *)contact_ptr < ((char *)&g_course_rouabout_6a21a8));
        player_index = player_index + 1;
      } while (player_index < frame.player_total);
    }
    /*
     * Gate sequence: once triggered, count g_course_rouabout_6a23cc down by g_tick_seconds. When it drops
     * below k_f_0, open the next gate (flag in g_course_6a2168, reset via rwp_object_sync_and_reset with the
     * vector (0, -1, 0), inferred) and return. One gate per call.
     */
    if (((g_course_rouabout_6a2164 != 0) &&
        (g_course_rouabout_6a23cc = g_course_rouabout_6a23cc - g_tick_seconds,
        g_course_rouabout_6a23cc < k_f_0)) && ((int)g_course_rouabout_6a23c8 < GATE_COUNT)) {
      g_course_rouabout_6a23cc = 0.1;
      (&g_course_6a2168)[g_course_rouabout_6a23c8] = 1;
      frame.reset_x = 0;
      frame.reset_y = RESET_VEC_Y;
      frame.reset_z = 0;
      rwp_object_sync_and_reset((&g_course_rouabout_6a21a8)[g_course_rouabout_6a23c8],&frame.reset_x);
      g_course_rouabout_6a23c8 = g_course_rouabout_6a23c8 + 1;
      return;
    }
  }
  return;
}
