#include "mfl_tier2.h"
/*004069f0*/ extern int skydome_follow_camera();
/*0040c620*/ extern int course_editor_read_vec_chunk();
/*004219b0*/ extern int course_call_cb1();
/*00421aa0*/ extern int course_select();
/*00421ad0*/ extern int course_has_cb1();
/*0042a300*/ extern int course_apply_clip_planes();
/*0042a6d0*/ extern int course_world_uva_dicts_set_ranks();
/*0042abb0*/ extern int course_uv_anim_update();
/*0042ac30*/ extern int course_uv_anims_update();
/*0042ace0*/ extern int course_camera_roll_update();
/*00475730*/ extern char *get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*00480bc0*/ extern int rwp_trigger_groups_update();
/*0048d730*/ extern int course_collision_objects_cull(struct RwCamera *p0);
/*004a07c0*/ extern int rain_drops_update_and_respawn();
/*00664418*/ extern struct CourseCounts g_course_counts_data;
/*00668c18*/ extern struct CourseWorld g_course_world;
/* course_update_frame */
/* "per frame: sky follows the camera, course_call_cb1, car updates" */
void course_update_frame(undefined4 param_1,undefined4 param_2)
{
  course_collision_objects_cull((RwCamera *)get_viewport_camera(0));
  course_editor_read_vec_chunk();
  skydome_follow_camera(&g_course_world,param_2);
  course_select(param_1);
  if (course_has_cb1() != 0) {
    course_call_cb1(&g_course_world,param_2);
  }
  course_world_uva_dicts_set_ranks(&g_course_world,&g_course_counts_data);
  course_uv_anims_update(&g_course_world,param_2);
  course_uv_anim_update(&g_course_world,&g_course_counts_data,param_2);
  rain_drops_update_and_respawn(get_viewport_camera_matrix(0));
  course_camera_roll_update(&g_course_world,get_viewport_camera(0));
  course_apply_clip_planes();
  rwp_trigger_groups_update(&g_course_world);
  return;
}
