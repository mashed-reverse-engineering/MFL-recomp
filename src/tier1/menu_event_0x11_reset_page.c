#include "mfl_tier2.h"


/*0069cbd4*/ extern undefined4 g_event_type;
/*0069d2d8*/ extern undefined4 g_car_69d2d8;
/*0069d2dc*/ extern undefined4 g_ui_69d2dc;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*008c8078*/ extern undefined4 g_training_lesson;
/*008c807c*/ extern undefined4 g_car_8c807c;
/*008c86c8*/ extern undefined4 g_car_8c86c8;


/* menu_event_0x11_reset_page */

/* Resets menu record 0 and sets its page ID to the current training lesson for tutorial events. */
void menu_event_0x11_reset_page(int unknown)
{
  struct MenuRecord *menu_record;
  if (g_event_type == EVENT_TRAINING) {
    /* Reset car state if unknown parameter is 0 */
    if (unknown == 0) {
      g_car_8c807c = 0;
    }
    g_car_8c86c8 = unknown;
    /* Reset primary menu record and configure page for training lesson */
    menu_record = &g_menu_records[0];
    menu_record->page_id = g_training_lesson;
    menu_record->selected = 0;
    g_ui_69d2dc = 0;
    menu_record->item_list = 0;
    g_car_69d2d8 = 1;
    menu_record->entry_enabled[0] = 0;
    menu_record->entry_enabled[1] = 0;
    menu_record->entry_enabled[2] = 0;
  }
  return;
}
