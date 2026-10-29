#include "mfl_tier2.h"
typedef struct {
  char _pad[0xc];
  float expiry;
} SceneSelectable;
/*00651240*/ extern SceneSelectable *g_scene_selected_object;
/*00651244*/ extern float g_scene_select_timer;
/*00651248*/ extern int g_scene_select_param;

/* a selectable scene object: only the expiry time at +0xC is used here */

/* scene_object_select_is_active: a scene object is selected and the selection timer has not passed the object's expiry time. */
int scene_object_select_is_active(void)
{
  if (g_scene_select_param == 0) {
    return 0;
  }
  if (g_scene_select_timer > g_scene_selected_object->expiry) {
    return 0;
  }
  return 1;
}
