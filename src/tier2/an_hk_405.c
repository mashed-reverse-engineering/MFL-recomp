#include "mfl_tier2.h"
/*0040c710*/ extern int round_setup_limits();
/*0040cc60*/ extern int player_get_points();
/*0040ff70*/ extern int race_get_last_active_player();
/*00431730*/ extern int menu_event_0x11_reset_page();
/*008c8078*/ extern undefined4 g_training_lesson;
/*008c807c*/ extern undefined4 g_car_8c807c;
/*008c86c4*/ extern undefined4 g_car_8c86c4;
/*008c86c8*/ extern undefined4 g_car_8c86c8;
/*008c8720*/ extern undefined4 g_hit_shake;
/* race_end_check_points */

/* "when the race has ended with no points, shows the result (ui_fn_431730(2))" */

/*
 * Shows the end-of-race menu page. Called once the race has ended.
 * Chooses the page from the training lesson (g_training_lesson), the car state
 * (g_car_8c86c8, inferred) and the result of race_get_last_active_player()
 * (inferred), then calls menu_event_0x11_reset_page() with the page number.
 * Takes no parameters and returns nothing.
 */
void race_end_check_points(void)

{
  int points;

  /* lesson 0: if g_car_8c86c8 == 1, page 2 when the result is 0, otherwise page 3 */
  if (g_training_lesson == 0) {
    round_setup_limits();
    if (g_car_8c86c8 == 1) {
      points = race_get_last_active_player();
      if (points == 0) {
        menu_event_0x11_reset_page(2);
        return;
      }
      goto LAB_0043a1f5;
    }
  }
  /* lesson 1: if g_car_8c86c8 is 1 or 4, page 2 when the result is 0, otherwise page 4 */
  else if (g_training_lesson == 1) {
    round_setup_limits();
    if ((g_car_8c86c8 == 1) || (g_car_8c86c8 == 4)) {
      points = race_get_last_active_player();
      if (points == 0) {
LAB_0043a1e5:
        menu_event_0x11_reset_page(2);
        return;
      }
LAB_0043a1b5:
      menu_event_0x11_reset_page(4);
      return;
    }
  }
  else {
    /* lesson 2: page 3 when the result is not 0; otherwise page 2 if player_get_points(0) is 6, else page 1 */
    if (g_training_lesson == 2) {
      points = race_get_last_active_player();
      if (points != 0) {
        round_setup_limits();
        goto LAB_0043a1f5;
      }
      points = player_get_points(0);
      if (points == 6) {
        round_setup_limits();
        goto LAB_0043a1e5;
      }
      goto LAB_0043a27a;
    }
    /* lesson 3: when the result is 0 and g_hit_shake > 9, show page g_car_8c807c * 2 + 2
       and advance g_car_8c807c; otherwise show page 100 */
    if (g_training_lesson == 3) {
      round_setup_limits();
      points = race_get_last_active_player();
      if ((points == 0) && (9 < (int)g_hit_shake)) {
        menu_event_0x11_reset_page(g_car_8c807c * 2 + 2);
        g_car_8c807c = g_car_8c807c + 1;
        g_car_8c86c4 = 0;
      }
      else {
        menu_event_0x11_reset_page(100);
      }
    }
    if (g_training_lesson != 4) {
      /* lesson 5: page 1 when g_hit_shake > 0x13, otherwise page 3 */
      if (g_training_lesson == 5) {
        round_setup_limits();
        if (0x13 < (int)g_hit_shake) goto LAB_0043a27a;
        menu_event_0x11_reset_page(3);
      }
      return;
    }
    /* lesson 4: if g_car_8c86c8 is 1 or 4, page 2 when the result is 0, otherwise page 4 */
    round_setup_limits();
    if ((g_car_8c86c8 == 1) || (g_car_8c86c8 == 4)) {
      points = race_get_last_active_player();
      if (points == 0) {
        menu_event_0x11_reset_page(2);
        return;
      }
      goto LAB_0043a1b5;
    }
  }
  /* fallback: page 1 when the result is 0, otherwise page 3 */
  points = race_get_last_active_player();
  if (points == 0) {
LAB_0043a27a:
    menu_event_0x11_reset_page(1);
    return;
  }
LAB_0043a1f5:
  menu_event_0x11_reset_page(3);
  return;
}


