#include "mfl_tier2.h"
/*0040ea10*/ extern undefined __cdecl atomic_instance_filter(struct RpAtomic *p0);
/*00422380*/ extern char *__cdecl vehicle_get_frame_16c(undefined4 p0);
/*0042a3f0*/ extern void * course_get_rpworld();
/*00475730*/ extern char *get_viewport_camera();
/*004f4440*/ extern struct RpWorld * RpWorldAddCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f4470*/ extern struct RpWorld * RpWorldRemoveCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/* camera_world_attach */
/* "adds the main camera to the current world"
 *
 * Adds the viewport camera to the current course world, then, if the camera
 * update starts, runs atomic_instance_filter over the vehicle clump's atomics
 * (inferred: vehicle 0) between RwCameraBeginUpdate and RwCameraEndUpdate.
 * Finally removes the camera from the world again.
 * Takes no parameters, returns nothing. */
void camera_world_attach(void)
{
  undefined4 filter_state;

  filter_state = 0;
  RpWorldAddCamera((RpWorld *)course_get_rpworld(),(RwCamera *)get_viewport_camera(0));
  /* (inferred) run the atomic filter over the clump only if the update began */
  if (RwCameraBeginUpdate(get_viewport_camera(0)) != 0) {
    RpClumpForAllAtomics((RpClump *)vehicle_get_frame_16c(0),atomic_instance_filter,&filter_state);
    RwCameraEndUpdate(get_viewport_camera(0));
  }
  /* detach the camera from the world again */
  RpWorldRemoveCamera((RpWorld *)course_get_rpworld(),(RwCamera *)get_viewport_camera(0));
  return;
}
