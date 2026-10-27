#include "mfl_tier2.h"
/*00475730*/ extern char *get_viewport_camera();
/*004c54d0*/ extern void FUN_004c54d0(void *p0, undefined4 p1, void *p2);
/*005de838*/ extern float k_f_640;
/*005de904*/ extern float k_f_480;
/* world_to_screen_640x480 */
/* "(world pos, out xy): projects a point through the viewport camera into 640x480 coordinates" */
/*
 * Projects a world-space point through the viewport camera into 640x480
 * screen coordinates.
 *   world_pos: world-space point to project.
 *   screen_xy: out, float[2]; receives x and y in 640x480 space.
 *              (inferred: FUN_004c54d0 writes the projected point here)
 */
void world_to_screen_640x480(undefined4 world_pos,float *screen_xy)
{
  int viewport_w;
  int viewport_h;
  char *camera;
  float fW;
  float fH;

  camera = get_viewport_camera(0);
  /* Viewport size from the camera's render target (inferred: +0xc width, +0x10 height). */
  viewport_w = *(int *)(*(char **)(camera + 0x60) + 0xc);
  viewport_h = *(int *)(*(char **)(camera + 0x60) + 0x10);
  fW = (float)viewport_w;
  fH = (float)viewport_h;
  FUN_004c54d0(camera,world_pos,screen_xy);
  /* Normalise by the viewport size, then scale to 640x480 (k_f_640 / k_f_480, inferred). */
  *screen_xy = (*screen_xy / fW) * k_f_640;
  screen_xy[1] = (screen_xy[1] / fH) * k_f_480;
  return;
}
