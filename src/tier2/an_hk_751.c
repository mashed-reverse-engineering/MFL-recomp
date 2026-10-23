#include "mfl_tier2.h"
/*004219c0*/ extern undefined __cdecl course_call_cb2(void *p0);
/*004219d0*/ extern undefined __cdecl course_call_cb3(void *p0);
/*00421aa0*/ extern int course_select();
/*00421ae0*/ extern int course_has_cb2();
/*00421af0*/ extern int course_has_cb3();
/*0042b230*/ extern int course_sky_clumps_update();
/*0042b2a0*/ extern int course_sky_clumps_render();
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*004882d0*/ extern undefined __cdecl course_ribbons_render(void *p0);
/*00488d80*/ extern undefined __cdecl particle_system_set_current_forward(void *p0, undefined4 p1, undefined4 p2);
/*004a07d0*/ extern int rain_render();
/*004a1880*/ extern int game_get_62ad2c();
/*004f5780*/ extern struct RpWorld * RpWorldRender(struct RpWorld *p0);
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00675668*/ extern struct CourseLap g_course_lap_data;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* Red byte of the fog colour is ORed with this bit (inferred); it lands at bit 30 of the packed colour */
#define FOG_COLOR_RED_FLAG (0x4000)

/* course_render_cb3_dispatch */

/* "course_call_cb3 + sky + render" */

/*
 * Course render pass for the cb3 layer: draws the sky (or cb3 callback) and the
 * world with fog, then rain. Does nothing for the course layer when
 * split_sectors[8] is 0; rain_render still runs.
 * select_arg: passed to course_select() before each callback/render step (inferred).
 * Returns nothing.
 */
void course_render_cb3_dispatch(undefined4 select_arg)

{
  byte fog_green;
  byte fog_red;
  RwDevice *open_device;
  int scratch;
  byte *fog_color_ptr;
  undefined4 viewport_camera;
  undefined4 particle_forward_arg;

  if (g_course_lap_data.split_sectors[8] != 0) {
    /* turn off fog and depth test/write while drawing the sky or the cb3 callback */
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    course_select(select_arg);
    /* no cb3 callback: draw the sky clumps from the viewport camera matrix; otherwise run the callback */
    scratch = course_has_cb3();
    if (scratch == 0) {
      viewport_camera = get_viewport_camera_matrix(0);
      course_sky_clumps_update(viewport_camera);
      course_sky_clumps_render();
    }
    else {
      course_call_cb3(&g_course_world);
    }
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x1);
    /* fog colour: bytes 0, 1, 2 of game_get_62ad2c() are red, green, blue (inferred); packed as 0x40RRGGBB */
    open_device = &RwGlobalsRelease_dOpenDevice(RwEngineInstance);
    fog_color_ptr = (byte *)game_get_62ad2c();
    scratch = game_get_62ad2c();
    fog_green = *(byte *)(scratch + 1);
    fog_red = *fog_color_ptr;
    scratch = game_get_62ad2c();
    (*RwDevice_fpRenderStateSet(open_device))
              (rwRENDERSTATEFOGCOLOR,
               (void *)(((uint)fog_green | (fog_red | FOG_COLOR_RED_FLAG) << 8) << 8 | (uint)*(byte *)(scratch + 2)));
    RwRenderStateSet(rwRENDERSTATEFOGTYPE,(void *)0x1);
    /* split_sectors[7] == 0: set particle forward from the viewport camera; otherwise render collision_bsps[0] world */
    if (g_course_lap_data.split_sectors[7] == 0) {
      particle_forward_arg = 0;
      viewport_camera = get_viewport_camera(0x0);
      particle_system_set_current_forward(&g_course_world,viewport_camera,particle_forward_arg);
    }
    else {
      RpWorldRender((RpWorld *)g_course_world.collision_bsps[0]);
    }
    /* split_sectors[6] != 0: draw ribbons */
    if (g_course_lap_data.split_sectors[6] != 0) {
      course_ribbons_render(&g_course_lap_data);
    }
    /* run the cb2 callback if the course has one */
    course_select(select_arg);
    scratch = course_has_cb2();
    if (scratch != 0) {
      course_call_cb2(&g_course_world);
    }
  }
  rain_render();
  RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x0);
  return;
}


