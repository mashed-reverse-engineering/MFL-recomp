// cluster-entries: intro_banner_start_cup
/* Installed cluster intro_banner_start_cup (tools/cluster_install.py from output/cluster_pilot_k/intro_banner_start_cup.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00430ac0*/ extern int screen_height();
/*00434bb0*/ extern int get_event_type();
/*005de8f8*/ extern float k_f_48000;
/*008c8790*/ extern undefined4 g_intro_banner_text_id;
/*008c8794*/ extern float g_intro_banner_slide_step;
/*008c8798*/ extern undefined4 g_intro_banner_offset;
/* intro_banner_start_cup */

/* "for cup events 3..5 starts the banner id 0xEE" */

void intro_banner_start_cup(void)

{
  short screen_h;
  int event_type;
  
  event_type = get_event_type();
  if (((event_type != EVENT_CUP_3) && (event_type = get_event_type(), event_type != EVENT_CUP_4)) &&
     (event_type = get_event_type(), event_type != EVENT_CUP_5)) { /* not one of the cup events EVENT_CUP_3..5 */
    return;
  }
  if (g_intro_banner_text_id == 0) {
    screen_h = screen_height();
    g_intro_banner_text_id = 0xee;
    g_intro_banner_offset = 0;
    g_intro_banner_slide_step = -(k_f_48000 / (float)(int)screen_h);
  }
  return;
}

