#include "mfl_tier2.h"
/*0042ab20*/ extern undefined __cdecl course_camera_add_to_collision_world(struct RwCamera *p0);
/*0042ab60*/ extern undefined __cdecl course_camera_remove_from_collision_world(struct RwCamera *p0);
/*00475730*/ extern int get_viewport_camera();
/*004f5d10*/ extern RwBool RpWorldInstance(struct RpWorld *p0);
/*00668c18*/ extern struct CourseWorld g_course_world;


/* course_world_instance_for_viewport */

/* "adds the camera to the collision world, instances the world and removes the camera" */

void course_world_instance_for_viewport(void)

{
  RwCamera *camera;
  undefined4 viewport_camera;
  int update_ok;
  
  camera = (RwCamera *)get_viewport_camera(0);
  course_camera_add_to_collision_world(camera);
  viewport_camera = get_viewport_camera(0);
  update_ok = RwCameraBeginUpdate(viewport_camera);
  if (update_ok != 0) {
    instancing_clump_log();
    RpWorldInstance((RpWorld *)g_course_world.world);
    viewport_camera = get_viewport_camera(0);
    RwCameraEndUpdate(viewport_camera);
  }
  camera = (RwCamera *)get_viewport_camera(0);
  course_camera_remove_from_collision_world(camera);
  return;
}
