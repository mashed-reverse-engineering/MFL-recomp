#include "mfl_tier2.h"
/*0040ea10*/ extern undefined __cdecl atomic_instance_filter(struct RpAtomic *p0);
/*00422380*/ extern int __cdecl vehicle_get_frame_16c(undefined4 p0);
/*0042a3f0*/ extern void * course_get_rpworld();
/*00475730*/ extern int get_viewport_camera();
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
  RwCamera *viewport_camera;
  RpWorld *world;
  undefined4 camera;
  int update_started;
  RpClump *clump;
  code *callback;
  undefined4 *filter_data;
  undefined4 filter_state;

  filter_state = 0;
  viewport_camera = (RwCamera *)get_viewport_camera(0);
  world = (RpWorld *)course_get_rpworld();
  RpWorldAddCamera(world,viewport_camera);
  camera = get_viewport_camera(0);
  update_started = RwCameraBeginUpdate(camera);
  filter_data = &filter_state;
  /* (inferred) run the atomic filter over the clump only if the update began */
  if (update_started != 0) {
    callback = atomic_instance_filter;
    clump = (RpClump *)vehicle_get_frame_16c(0);
    RpClumpForAllAtomics(clump,callback,filter_data);
    camera = get_viewport_camera(0);
    RwCameraEndUpdate(camera);
  }
  /* detach the camera from the world again */
  viewport_camera = (RwCamera *)get_viewport_camera(0);
  world = (RpWorld *)course_get_rpworld();
  RpWorldRemoveCamera(world,viewport_camera);
  return;
}
