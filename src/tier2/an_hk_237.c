#include "mfl_tier2.h"
/*00475730*/ extern int get_viewport_camera();

#define CAMERA_POS_X_OFFSET (0x40)
#define CAMERA_POS_Y_OFFSET (0x44)
#define CAMERA_POS_Z_OFFSET (0x48)

/* camera_get_position */

/* "(out xyz): position of the viewport camera 0" */

/*
 * Writes the position of viewport camera 0 into position[0..2] (x, y, z).
 * position: out, must point to at least 3 words.
 * The camera state is reached through get_viewport_camera(0) and the pointer at offset 4 (inferred).
 */
void camera_get_position(undefined4 *position)

{
  undefined4 pos_y;
  undefined4 pos_z;
  char *camera;

  camera = get_viewport_camera(0);
  camera = *(char **)(camera + 4);
  pos_y = *(undefined4 *)(camera + CAMERA_POS_Y_OFFSET);
  pos_z = *(undefined4 *)(camera + CAMERA_POS_Z_OFFSET);
  *position = *(undefined4 *)(camera + CAMERA_POS_X_OFFSET);
  position[1] = pos_y;
  position[2] = pos_z;
  return;
}


