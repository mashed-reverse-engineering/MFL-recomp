#include "mfl_tier2.h"
/*004a4260*/ extern int timer_init_performance_counter();
/*004a4700*/ extern int dinput_init();
/*004a51f0*/ extern int joypads_init();
/*004a5500*/ extern int dinput_keyboard_init();
/*004a8180*/ extern int __cdecl controller_dialog_run(int p0, int p1);
/*004a9380*/ extern int app_get_app_7e7704();
/*0062ad38*/ extern undefined4 g_app_62ad38;
/*0076e870*/ extern undefined4 g_app_76e870;
/*0076e874*/ extern undefined4 g_language_offset;
/* platform_init */

/* "timer, dinput, joypads search" */

bool platform_init(void)

{
  undefined4 app_window;
  int bitmap_set;
  
  timer_init_performance_counter();
  dinput_init();
  app_window = app_get_app_7e7704();
  joypads_init(app_window);
  app_window = app_get_app_7e7704();
  dinput_keyboard_init(app_window);
  switch(g_language_offset) {
  case 0:
    bitmap_set = 0;
    break;
  default:
    bitmap_set = 0;
    break;
  case 1:
    bitmap_set = 2;
    break;
  case 2:
    bitmap_set = 3;
    break;
  case 3:
    bitmap_set = 4;
    break;
  case 4:
    bitmap_set = 5;
    break;
  case 5:
    bitmap_set = 1;
  }
  controller_dialog_run(g_app_62ad38,bitmap_set);
  return g_app_76e870 == 0;
}


