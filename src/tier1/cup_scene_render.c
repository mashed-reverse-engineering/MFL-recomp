#include "mfl_tier2.h"
/*00421ca0*/ extern int vehicle_particles_render();
/*00421cf0*/ extern int game_set_particles_60761c();
/*00421d00*/ extern int game_set_particles_607620();
/*00421d10*/ extern int game_set_65a9b8_607624();
/*00421d30*/ extern int game_set_65a9c0_65a9c4();
/*00421fe0*/ extern int vehicle_set_field_198();
/*004221c0*/ extern float vehicle_get_float_184();
/*00423c90*/ extern undefined __cdecl vehicle_render(int p0, struct RwCamera *p1);
/*0042a440*/ extern void * course_get_directional_light();
/*00475730*/ extern char *get_viewport_camera();
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*0050b5e0*/ extern int nop_stub();
/*0064cbd4*/ extern undefined4 g_cup_64cbd4;
/*0064cbd8*/ extern undefined4 g_cup_64cbd8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* car_index value meaning "no car": the cup scene is not drawn */
#define NO_CAR_INDEX (-1)
/* RwCameraClear clear flag: clear the Z buffer (inferred) */
#define RW_CAMERA_CLEAR_Z (2)
/* bit pattern of 1.0f, sent as the float value of render state 0x65a9b8 (inferred) */
/* cup_scene_render */
/* "clears the camera and renders the cup scene clump(s)"
 * Draws the cup scene for one car. Clears the viewport camera, points the
 * directional light's frame at the camera, renders the car with particles
 * off, then restores the light's original frame.
 *   threshold: float compared with the car's value; its bits are passed to
 *              game_set_65a9b8_607624 when the value is at or below it (inferred)
 *   car_index: car to draw, NO_CAR_INDEX draws nothing
 * Returns nothing. */
void cup_scene_render(float threshold,int car_index)
{
  RwFrame *light_frame;
  void *light;
  RwRGBA black;
  void *cam;

  light = (void *)course_get_directional_light();
  light_frame = *(RwFrame **)((char *)light + 4);
  black.red = '\0';
  black.green = '\0';
  black.blue = '\0';
  black.alpha = '\0';
  cam = RwCameraGetCurrentCamera();
  if (car_index != NO_CAR_INDEX) {
    if (cam != (void *)0x0) {
      RwCameraEndUpdate(cam);
    }
    RwCameraClear((RwCamera *)get_viewport_camera(0),&black,RW_CAMERA_CLEAR_Z);
    _rwObjectHasFrameSetFrame(light,g_cup_64cbd8);
    RwCameraBeginUpdate(get_viewport_camera(0));
    if (vehicle_get_float_184(car_index) <= threshold) {
      game_set_65a9b8_607624(1,*(int *)&threshold);
    }
    game_set_particles_60761c(0);
    game_set_particles_607620(0);
    game_set_65a9c0_65a9c4(1,g_cup_64cbd4);
    vehicle_set_field_198(car_index,0);
    vehicle_render(car_index,RwCameraGetCurrentCamera());
    nop_stub(car_index,RwCameraGetCurrentCamera());
    vehicle_particles_render(RwCameraGetCurrentCamera());
    game_set_65a9c0_65a9c4(0,0);
    game_set_particles_60761c(1);
    game_set_particles_607620(1);
    game_set_65a9b8_607624(0,FLOAT_ONE_BITS);
    _rwObjectHasFrameSetFrame(light,light_frame);
  }
  return;
}
