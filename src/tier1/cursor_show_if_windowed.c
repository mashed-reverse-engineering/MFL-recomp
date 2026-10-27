#include "mfl_tier2.h"
/*004a4640*/ extern int cursor_show(void);
/* Tail-calls cursor_show (shows the mouse cursor when running windowed). */
int cursor_show_if_windowed(void)
{
  return cursor_show();
}
