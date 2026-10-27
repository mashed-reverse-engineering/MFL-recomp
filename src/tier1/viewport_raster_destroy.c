#include "mfl_tier2.h"
/*00495af0*/ extern undefined __cdecl rwp_camera_destroy_frame(void *p0, struct RwTexture *p1);
/*00702244*/ extern undefined g_physics_702244;
/*00702254*/ extern undefined g_physics_702254;
/* physics_buffer_rewind */
/* "+0x1C = +0x18" */
/* replay_record_save_frame */
/* "copies a 0x67 dword record and its array into the record list" */
/* viewport_raster_create */
/* "(index 0..3, w, h, flags): creates the raster pair of a viewport" */
#define VIEWPORT_LAST_INDEX (3)
/* viewport_raster_destroy */
/* "(index 0..3): destroys the raster pair" */
/* Destroys the raster pair of one viewport (viewport_index: 0..VIEWPORT_LAST_INDEX).
   The frame lives in the g_physics_702244 slot and the texture in the g_physics_702254 slot,
   both 4-byte entries per viewport. Returns 1 if the pair was destroyed, 0 otherwise. */
undefined4 viewport_raster_destroy(uint viewport_index)
{
  RwTexture *texture;

  if (viewport_index >= VIEWPORT_LAST_INDEX + 1) {
    return 0;
  }
  if (*(void **)((char *)&g_physics_702244 + viewport_index * 4) == (void *)0x0) {
    return 0;
  }
  texture = *(RwTexture **)((char *)&g_physics_702254 + viewport_index * 4);
  if (texture == (RwTexture *)0x0) {
    return 0;
  }
  rwp_camera_destroy_frame(*(void **)((char *)&g_physics_702244 + viewport_index * 4),texture);
  ((undefined4 *)&g_physics_702244)[viewport_index] = 0;
  ((undefined4 *)&g_physics_702254)[viewport_index] = 0;
  return 1;
}
