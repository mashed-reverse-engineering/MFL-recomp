#include "mfl_tier2.h"
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*005dda24*/ extern float k_f_0_75;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];


#define CAMERA_OFFSET_ASPECT     0x24
#define CAMERA_OFFSET_FOV        0x58
#define CAMERA_OFFSET_UNKNOWN_6C 0x6c
#define CAMERA_OFFSET_UNKNOWN_70 0x70
#define CAMERA_OFFSET_RW_CAMERA  0x84

#define PLAYER_SLOT_AI_EDITOR    3


/* camera_update_view_window */

/* Updates the RenderWare camera view window from the camera's FOV and aspect ratio. */
void camera_update_view_window(char *camera)
{
  float unscaled_view_window_y;
  RwV2d view_window;
  
  /* Calculate view window dimensions from FOV, aspect ratio, and 0.75 factor */
  view_window.x = ((*(float *)(camera + CAMERA_OFFSET_UNKNOWN_6C) * *(float *)(camera + CAMERA_OFFSET_FOV)) /
              *(float *)(camera + CAMERA_OFFSET_ASPECT)) * k_f_0_75;
  unscaled_view_window_y = *(float *)(camera + CAMERA_OFFSET_UNKNOWN_70) * *(float *)(camera + CAMERA_OFFSET_FOV);
  view_window.y = k_f_0_75 * unscaled_view_window_y;
  /* Use unscaled view window height when the AI editor slot is active */
  if (g_player_slots[PLAYER_SLOT_AI_EDITOR].extra != 0) {
    view_window.y = unscaled_view_window_y;
  }
  RwCameraSetViewWindow(*(RwCamera **)(camera + CAMERA_OFFSET_RW_CAMERA),&view_window);
  return;
}
