#include "mfl_tier2.h"
/*005dd374*/ extern float k_f_0_2;
/*005de808*/ extern float k_f_0_7375;
/*005de80c*/ extern float k_f_0_78125;
/*005de810*/ extern float k_f_0_65416664;
/*005de814*/ extern float k_f_0_34583333;
/*005de818*/ extern float k_f_0_5625;
/*0069ba00*/ extern float g_ui_layout_scale_x;
/*0069ba04*/ extern float g_ui_layout_scale_y;
/*008c87a0*/ extern undefined4 g_course_8c87a0;
/*008c87a4*/ extern float g_ui_2d_8c87a4;
/*008c87a8*/ extern float g_ui_2d_8c87a8;
/*008c87ac*/ extern float g_ui_2d_8c87ac;
/*008c87b0*/ extern undefined4 g_course_8c87b0;
/*008c87b4*/ extern float g_ui_2d_8c87b4;
/*008c87b8*/ extern float g_ui_2d_8c87b8;
/*008c87bc*/ extern float g_ui_2d_8c87bc;
/* course_get_record_point */
/* "(i, j): 0x681840 + i * 0x4C + j * 0xC" */
/* course_sky_clumps_update */
/* "(camera matrix): positions and rotates the sky clumps around the camera" */
/* course_sky_clumps_render */
/* "renders the sky clumps with src / dest blend 5 / 6" */
/* course_world_ray_hit_light_forward */
/* "forwards to course_world_ray_hit_light_sample on g_course_world" */
/* course_get_shadow_scale */
/* "g_course_counts_data.shadow_scale" */
/* atomic_log_not_instancing */
/* "debug print 'Not instancing atomic %d' when the atomic flags have bit 4" */
/* instancing_clump_log */
/*  */
/* course_world_instance_for_viewport */
/* "adds the camera to the collision world, instances the world and removes the camera" */
/* course_render_cb3_dispatch */
/* "course_call_cb3 + sky + render" */
/* ui_2d_layout_rects_init */
/* "sets the two text rectangles of the 2D UI from the screen size" */
void ui_2d_layout_rects_init(void)
{
  /* the two undefined4 globals hold float bit patterns (value inferred) */
  g_course_8c87a0 = 0x3e660000;
  g_course_8c87b0 = 0x3dec0000;
  /* text rect 1 (inferred): 8c87a8 = x from scale_x, 8c87ac = height from scale_y, 8c87a4 = top = 0.654 - height */
  g_ui_2d_8c87a8 = k_f_0_5625 * g_ui_layout_scale_x;
  g_ui_2d_8c87ac = k_f_0_34583333 * g_ui_layout_scale_y;
  g_ui_2d_8c87a4 = k_f_0_65416664 - g_ui_2d_8c87ac;      /* diffemu: the original subtracts the stored (rounded) float, not the x87 product */
  /* text rect 2 (inferred): 8c87b8 = x from scale_x, 8c87bc = height from scale_y, 8c87b4 = top = 0.7375 - height */
  g_ui_2d_8c87b8 = k_f_0_78125 * g_ui_layout_scale_x;
  g_ui_2d_8c87bc = k_f_0_2 * g_ui_layout_scale_y;
  g_ui_2d_8c87b4 = k_f_0_7375 - g_ui_2d_8c87bc;
  return;
}
