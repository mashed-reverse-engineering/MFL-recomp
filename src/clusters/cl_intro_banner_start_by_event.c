// cluster-entries: intro_banner_start_by_event
/* Installed cluster intro_banner_start_by_event (tools/cluster_install.py from output/cluster_pilot_zy/intro_banner_start_by_event.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00434bb0*/ extern int get_event_type(void);
/*00430ac0*/ extern short screen_height(void);
/*008c8790*/ extern int g_intro_banner_text_id;
/*008c8794*/ extern float g_intro_banner_slide_step;
/*005de8f8*/ extern float k_f_48000;

/* Starts the intro banner (text id 0x25 for event types 0..2, else 0x40) unless one is already showing; the slide step scales with the screen height. */
void intro_banner_start_by_event(void)
{
  int text_id = 0x40;

  if (get_event_type() == EVENT_TIME_TRIAL_0 || get_event_type() == EVENT_TIME_TRIAL_1 || get_event_type() == EVENT_TIME_TRIAL_2) {
    text_id = 0x25;
  }
  if (g_intro_banner_text_id == 0) {
    g_intro_banner_slide_step = -(k_f_48000 / (float)(int)screen_height());
    g_intro_banner_text_id = text_id;
  }
}

