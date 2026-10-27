// cluster-entries: menu_markers_enabled_for_event
/* Installed cluster menu_markers_enabled_for_event (tools/cluster_install.py from output/cluster_pilot_kd/menu_markers_enabled_for_event.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00434bb0*/ extern int get_event_type();
/*0069cf94*/ extern undefined4 g_menu_markers_enabled;
/* menu_markers_enabled_for_event */

/* "0 for event type 0x12 else g_menu_markers_enabled" */

uint menu_markers_enabled_for_event(void)

{
  int event_type;
  
  event_type = get_event_type();
  return -(uint)(event_type != EVENT_QUICK_RACE) & g_menu_markers_enabled;
}

