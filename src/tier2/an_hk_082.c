#include "mfl_tier2.h"
/*00486ef0*/ extern int particles_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*005ddc28*/ extern float k_f_0;
/*005df8c4*/ extern float k_f_800;
/*006a8bc0*/ extern struct Powerup g_powerups[25];
/*006a9390*/ extern undefined g_powerup_glow;
/*006a93d0*/ extern undefined4 g_powerup_count;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* sizeof(struct Powerup) in floats: 0x50 bytes */
#define POWERUP_STRIDE_FLOATS (0x14)

/* powerups_render */

/* renders the available powerups that are in front of the camera and close enough
 *
 * Walks g_powerups from the last slot (g_powerup_count) down to slot 0. Inside the loop,
 * spawn_pos points one slot past the powerup being tested, so the powerup's own fields are
 * reached with negative offsets: state at -0x1a floats, clump at -0x22, spawn position at -0x14.
 * Takes no parameters and returns nothing; leaves z-write and z-test enabled.
 */

void powerups_render(void)

{
  char *camera_frame;
  float delta_x;
  float delta_y;
  float delta_z;
  float *spawn_pos;
  int powerup_index;

  /* camera_frame: the camera's frame (inferred); its matrix is read at +0x30 (forward, inferred) and +0x40 (position, inferred) */
  powerup_index = g_powerup_count;
  camera_frame = *(char **)((char *)RwCameraGetCurrentCamera() + 4);
  /* glow particles: z-write off, z-test on */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  particles_render(&g_powerup_glow);
  /* powerup clumps: z-write on, z-test on */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  /* loop over the powerups from the last slot down to slot 0 */
  if (powerup_index != 0) {
    spawn_pos = g_powerups[powerup_index].spawn_position;
    do {
      /* only powerups in state 1 (available) are drawn */
      if (*(int *)&spawn_pos[-0x1a] == 1) {
        /* vector from the camera to the powerup's spawn position */
        delta_x = spawn_pos[-0x14] - *(float *)(camera_frame + 0x40);
        delta_y = spawn_pos[-0x13] - *(float *)(camera_frame + 0x44);
        delta_z = spawn_pos[-0x12] - *(float *)(camera_frame + 0x48);
        /* in front of the camera (dot product with the forward vector > 0, inferred)
           and squared distance under k_f_800 (inferred max render distance squared) */
        if ((k_f_0 <*(float *)(camera_frame + 0x38) * delta_z +
                     delta_x * *(float *)(camera_frame + 0x30) + *(float *)(camera_frame + 0x34) * delta_y) &&
           (delta_z * delta_z + delta_x * delta_x + delta_y * delta_y < k_f_800)) {
          RpClumpRender(*(RpClump **)&spawn_pos[-0x22]);
        }
      }
      powerup_index = powerup_index + -1;
      spawn_pos = spawn_pos + -POWERUP_STRIDE_FLOATS;
    } while (powerup_index != 0);
  }
  return;
}


