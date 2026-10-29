// cluster-entries: course_warzone_cb1_sub_45d1b0
/* Installed cluster course_warzone_cb1_sub_45d1b0 (tools/cluster_install.py from output/cluster_pilot_k/course_warzone_cb1_sub_45d1b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0045cb40*/ extern int course_zone_players_transform_update();
/*0045ce00*/ extern int course_zone_cars_check_and_push();
/*0045cff0*/ extern int anim_stop();
/*0045d010*/ extern int course_zone_corners_to_world();
/*0045d110*/ extern int course_zone_read_frame_state();
/*00475730*/ extern int get_viewport_camera();
/*004c4890*/ extern int __cdecl FUN_004c4890(int a1, int a2);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*0053e4d0*/ extern RwBool RtAnimInterpolatorSubAnimTime(struct RtAnimInterpolator *p0, RwReal p1);
/*0053e820*/ extern RwBool RtAnimInterpolatorSetCurrentTime(struct RtAnimInterpolator *p0, RwReal p1);
/*00544450*/ extern RwBool RpHAnimHierarchyUpdateMatrices(struct RpHAnimHierarchy *p0);
/*005dd634*/ extern float k_f_2;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005e64a4*/ extern float k_f_0;
/*005df668*/ extern float k_f_6_66en4;
/*005df66c*/ extern float k_f_3_33en4;
/*007ef798*/ extern float g_fx_ptank_7ef798;
/*008bf050*/ extern undefined4 g_car_8bf050;
/*008bf054*/ extern undefined4 g_data_8bf054;
/*008bf058*/ extern undefined4 g_data_8bf058;
/*008bf05c*/ extern undefined4 g_data_8bf05c;
/* course_warzone_cb1_sub_45d1b0 */

/* single caller course_warzone_cb1 */

/* rounding point: the original spills this product to a float before the add */
static float float_barrier(float value)
{
  return value;
}

/* warzone animated zone record (offsets from the pointer the callers pass; fields as used by this function) */
typedef struct WarzoneZone {
    char unknown_00[8];
    RpAtomic *atomic;               /* 0x08 */
    char unknown_0c[0x2c];
    float center_x, center_y, center_z; /* 0x38: bounding sphere centre for the cull test */
    float time_step;                /* 0x44 */
    undefined4 state;               /* 0x48 */
    int enabled;                    /* 0x4c */
    RpHAnimHierarchy *hierarchy;    /* 0x50 */
    undefined4 matrix_copy[16];     /* 0x54: copy of *matrix_src */
    undefined4 *matrix_src;         /* 0x94 */
    char unknown_98[0xc];
    float fade;                     /* 0xa4 */
    char unknown_a8[0x20];
    float anim_time;                /* 0xc8 -> g_fx_ptank_7ef798 */
} WarzoneZone;

/* zone fields used: +0x48 state (0 reset, 1 fade out, 2 play forward, 3 play backward, 4 hold, 5 cull test), +0x4c enabled, +0x50 HAnim hierarchy, +0x54 matrix copy of *(+0x94), +0xa4 fade, +0x44 time step, +0xc8 animation time (-> g_fx_ptank_7ef798) */
void course_warzone_cb1_sub_45d1b0(WarzoneZone *zone, int ticks)

{
  float step;
  RwCamera *camera;
  RwFrustumTestResult frustum_result;
  int i;
  int i_or_hier;
  undefined4 *src_words;
  undefined4 *dst_words;
  RwSphere *sphere;
  RwSphere zone_sphere;
  
  if (zone->enabled != 0) {
    switch(zone->state) {
    case 0:
      RtAnimInterpolatorSetCurrentTime
                (*(RtAnimInterpolator **)((int)zone->hierarchy + 0x20),0.0);
      *(undefined4 *)&zone->anim_time = 0;
      RpHAnimHierarchyUpdateMatrices(zone->hierarchy);
      course_zone_read_frame_state(zone);
      src_words = zone->matrix_src;
      dst_words = zone->matrix_copy;
      for (i_or_hier = 0x10; i_or_hier != 0; i_or_hier = i_or_hier + -1) {
        *dst_words = *src_words;
        src_words = src_words + 1;
        dst_words = dst_words + 1;
      }
      course_zone_players_transform_update(zone,ticks);
      *(undefined4 *)&zone->fade = 0x3f800000;
      course_zone_cars_check_and_push(zone,ticks);
      g_fx_ptank_7ef798 = zone->anim_time;
      return;
    case 1:
      step = (float)ticks;
      if (ticks < 0) {
        step = step + k_f_4_2949673e9;
      }
      step = zone->fade - step * k_f_3_33en4;
      zone->fade = step;
      if (step < k_f_0) {
        zone->state = 2;
      }
      i_or_hier = (int)zone->hierarchy;
      src_words = zone->matrix_src;
      dst_words = zone->matrix_copy;
      for (i = 0x10; i != 0; i = i + -1) {
        *dst_words = *src_words;
        src_words = src_words + 1;
        dst_words = dst_words + 1;
      }
      *(undefined4 *)&zone->time_step = 0;
      RtAnimInterpolatorSetCurrentTime(*(RtAnimInterpolator **)(i_or_hier + 0x20),0.0);
      *(undefined4 *)&zone->anim_time = 0;
      RpHAnimHierarchyUpdateMatrices(zone->hierarchy);
      course_zone_read_frame_state(zone);
      course_zone_cars_check_and_push(zone,ticks);
      g_fx_ptank_7ef798 = zone->anim_time;
      return;
    case 2:
      step = (float)ticks;
      src_words = zone->matrix_src;
      dst_words = zone->matrix_copy;
      for (i_or_hier = 0x10; i_or_hier != 0; i_or_hier = i_or_hier + -1) {
        *dst_words = *src_words;
        src_words = src_words + 1;
        dst_words = dst_words + 1;
      }
      if (ticks < 0) {
        step = step + k_f_4_2949673e9;
      }
      step = float_barrier(step * k_f_6_66en4);
      zone->anim_time = step + zone->anim_time;
      zone->time_step = step;
      FUN_004c4890((int)zone->hierarchy,*(int *)&step);
      course_zone_read_frame_state(zone);
      course_zone_cars_check_and_push(zone,ticks);
      if ((((g_car_8bf050 == 0) && (g_data_8bf054 == 0)) && (g_data_8bf058 == 0)) &&
         ((g_data_8bf05c == 0 && (k_f_2 < zone->anim_time)))) {
        zone->state = 3;
      }
      course_zone_corners_to_world(zone);
      g_fx_ptank_7ef798 = zone->anim_time;
      return;
    case 3:
      step = (float)ticks;
      src_words = zone->matrix_src;
      dst_words = zone->matrix_copy;
      for (i_or_hier = 0x10; i_or_hier != 0; i_or_hier = i_or_hier + -1) {
        *dst_words = *src_words;
        src_words = src_words + 1;
        dst_words = dst_words + 1;
      }
      if (ticks < 0) {
        step = step + k_f_4_2949673e9;
      }
      step = step * k_f_6_66en4;
      zone->time_step = step;
      zone->anim_time = zone->anim_time - step;
      RtAnimInterpolatorSubAnimTime(*(RtAnimInterpolator **)((int)zone->hierarchy + 0x20),step)
      ;
      if (zone->anim_time <= k_f_0) {
        anim_stop(zone);
        *(undefined4 *)&zone->anim_time = 0;
        RtAnimInterpolatorSetCurrentTime
                  (*(RtAnimInterpolator **)((int)zone->hierarchy + 0x20),0.0);
      }
      RpHAnimHierarchyUpdateMatrices(zone->hierarchy);
      course_zone_read_frame_state(zone);
      course_zone_cars_check_and_push(zone,ticks);
      g_fx_ptank_7ef798 = zone->anim_time;
      return;
    case 4:
      step = (float)ticks;
      src_words = zone->matrix_src;
      dst_words = zone->matrix_copy;
      for (i_or_hier = 0x10; i_or_hier != 0; i_or_hier = i_or_hier + -1) {
        *dst_words = *src_words;
        src_words = src_words + 1;
        dst_words = dst_words + 1;
      }
      if (ticks < 0) {
        step = step + k_f_4_2949673e9;
      }
      step = step * k_f_0;
      zone->time_step = step;
      FUN_004c4890((int)zone->hierarchy,*(int *)&step);
      course_zone_read_frame_state(zone);
      course_zone_cars_check_and_push(zone,ticks);
      g_fx_ptank_7ef798 = zone->anim_time;
      return;
    case 5:
      if ((RpAtomic_interpolator_flags(zone->atomic) & 2) != 0) {
        _rpAtomicResyncInterpolatedSphere(zone->atomic);
      }
      zone_sphere.radius = *(RwReal *)((int)zone->atomic + 0x28);
      zone_sphere.center.x = zone->center_x;
      zone_sphere.center.z = zone->center_z;
      src_words = zone->matrix_src;
      dst_words = zone->matrix_copy;
      for (i_or_hier = 0x10; i_or_hier != 0; i_or_hier = i_or_hier + -1) {
        *dst_words = *src_words;
        src_words = src_words + 1;
        dst_words = dst_words + 1;
      }
      zone_sphere.center.y = zone->center_y;
      sphere = &zone_sphere;
      camera = (RwCamera *)get_viewport_camera(0);
      frustum_result = RwCameraFrustumTestSphere(camera,sphere);
      if (frustum_result == rwSPHEREOUTSIDE) {
        anim_stop(zone);
        *(undefined4 *)&zone->anim_time = 0;
      }
      course_zone_cars_check_and_push(zone,ticks);
    }
  }
  g_fx_ptank_7ef798 = zone->anim_time;
  return;
}

