#include "mfl_tier2.h"
/*00401210*/ extern int cup_scene_setup_frames();
/*0042a3f0*/ extern void * course_get_rpworld();
/*004f4440*/ extern struct RpWorld * RpWorldAddCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f4470*/ extern struct RpWorld * RpWorldRemoveCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*0064cbbc*/ extern undefined4 g_cup_64cbbc;
/*0064cbc0*/ extern undefined4 g_cup_64cbc0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* cup_scene_camera_attach */
/* "moves the current camera into the cup scene world (RpWorldRemoveCamera / AddCamera)"
 *
 * Takes the current camera (RwCameraGetCurrentCamera()) out of the course world and puts it
 * into the cup scene world g_cup_64cbbc (inferred), renders the cup clump g_cup_64cbc0
 * (inferred) with depth test and depth write on, then moves the camera back into the course
 * world. Takes no parameters and returns nothing. The begin/end update calls on the camera
 * are balanced: the update the game has open is closed first and reopened at the end.
 */
void cup_scene_camera_attach(void)
{
  RwCamera *camera;
  RpWorld *course_world;
  int began_update;
  RwCamera *camera_copy;

  /* end the update the game has open on the current camera, if any */
  camera = RwCameraGetCurrentCamera();
  if (camera != (RwCamera *)0x0) {
    RwCameraEndUpdate(camera);
  }
  /* move the camera out of the course world and into the cup scene world */
  RpWorldRemoveCamera((RpWorld *)course_get_rpworld(),camera);
  RpWorldAddCamera(g_cup_64cbbc,camera);
  cup_scene_setup_frames();
  /* draw the cup clump with depth test and depth write on, only if the update opened */
  began_update = RwCameraBeginUpdate(camera);
  if (began_update != 0) {
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    RpClumpRender(g_cup_64cbc0);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    RwCameraEndUpdate(camera);
  }
  /* move the camera back from the cup scene world into the course world */
  RpWorldRemoveCamera(g_cup_64cbbc,camera);
  RpWorldAddCamera((RpWorld *)course_get_rpworld(),camera);
  /* reopen the update on the current camera for the rest of the frame */
  if (camera != (RwCamera *)0x0) {
    RwCameraBeginUpdate(camera);
  }
  return;
}
