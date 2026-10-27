#include "mfl_tier2.h"
/*0069cf9c*/ extern undefined4 g_menu_race_length_choice;


/* race_length_setting */

/* 0..2 -> time limit 900000 / 1800000 / 3600000 game ticks (3000 per second = 5 / 10 / 20 minutes)
    */

undefined4 race_length_setting(void)
{
  return g_menu_race_length_choice;
}
