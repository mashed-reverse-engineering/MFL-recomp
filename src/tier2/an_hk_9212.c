#include "mfl_tier2.h"
typedef int CourseId;
typedef int (__cdecl *rwStrcpyFn)(char *, const char *);
/*00406240*/ extern undefined __cdecl scene_object_select_set(undefined4 p0, undefined4 p1);
/*00406970*/ extern undefined __cdecl course_materials_fixup(struct RpWorld *p0, void *p1);
/*0040ac10*/ extern int line_exceptions_clear();
/*0040aca0*/ extern int line_exceptions_load();
/*004145f0*/ extern int trail_clumps_create();
/*00421960*/ extern int course_table_select(uint param_1);
/*004219a0*/ extern undefined __cdecl course_call_cb0(undefined4 p0);
/*00421a60*/ extern undefined __cdecl course_call_world_setup(undefined4 p0);
/*00421a70*/ extern char * course_table_find_entry();
/*00421ac0*/ extern int course_has_cb0();
/*00421b80*/ extern int course_has_world_setup();
/*00427120*/ extern int ai_load();
/*004271a0*/ extern int ai_clear();
/*00429b00*/ extern int vehicle_slots_clear();
/*00429cc0*/ extern int game_open_perm_piz();
/*0042a290*/ extern int course_fog_apply();
/*0042a470*/ extern undefined4 *course_list_scan(int param_1, char *param_2, char *param_3, char *param_4);
/*0042a680*/ extern int course_atomics_set_flag_0x40(struct CourseWorld *world, struct CourseCounts *counts);
/*0042a7b0*/ extern int copters_load();
/*0042b1e0*/ extern int split_sectors_reset();
/*0042f6e0*/ extern int game_content_find(char *buf, char *name);
/*0042fa80*/ extern char * course_content_path_build();
/*00434a20*/ extern int get_second_camera();
/*0044a040*/ extern int gsd_load();
/*0046f130*/ extern undefined __cdecl sound_load_course_audio(undefined4 p0);
/*004741f0*/ extern int course_object_lists_reset();
/*00474300*/ extern undefined __cdecl course_object_pairs_build_clumps(undefined4 p0);
/*00475450*/ extern int course_ai_polygons_build_grid(uint p0);
/*00475730*/ extern int get_viewport_camera();
/*004802a0*/ extern int rwp_trigger_groups_resolve_indices();
/*00488710*/ extern undefined __cdecl ai_polygons_log_count(struct CourseLap *p0, int p1);
/*004893e0*/ extern undefined __cdecl course_create_from_description(struct CourseWorld *p0, struct CourseFiles *p1, struct CourseCounts *p2);
/*0048b4e0*/ extern int course_script_run(char *name, struct CourseFiles *files, struct CourseCounts *counts);
/*0048b5b0*/ extern int lapdata_script_run(char *name, struct CourseLap *lap);
/*0048d680*/ extern int course_collision_tables_clear();
/*0048d6c0*/ extern int __cdecl course_collision_objects_build(undefined4 p0);
/*00493040*/ extern int rwp_world_initialize();
/*004a0aa0*/ extern int rain_buffers_alloc_once();
/*004a1860*/ extern int game_set_option_flags(int r, int g, int b);
/*004a43d0*/ extern undefined __cdecl game_open_piz(char *p0);
/*004a4440*/ extern int game_close_piz();
/*004a8420*/ extern int __cdecl debug_printf(const char *fmt, ...);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*005dd5a2*/ extern undefined g_game_5dd5a2;
/*00664418*/ extern struct CourseCounts g_course_counts_data;
/*00665138*/ extern struct CourseFiles g_course_files_data;
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00675668*/ extern struct CourseLap g_course_lap_data;
/*007d2128*/ extern char *RwEngineInstance;
/*008c87c0*/ extern char g_course_piz_path[];
/* course_open_and_select */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "'Opening piz file [%s]': split sectors reset, table select, world setup" */

void course_open_and_select(CourseId course_id)

{
  char *entry;
  char *path;
  char *scan;
  char *dst;
  char c;
  int iVar5;
  rwStrcpyFn *pfnStrcpy;
  RwCamera *camera;
  char buf[256] = "";

  entry = course_table_find_entry(course_id);
  rwstrcpy(buf,"toastart/tracks/");
  rwstrcat(buf,entry);
  rwstrcat(buf,".piz");
  pfnStrcpy = (rwStrcpyFn *)&rwstrcpy;
  path = course_content_path_build(buf);
  (*pfnStrcpy)(g_course_piz_path,path);
  g_course_counts_data.course_id = course_id;
  split_sectors_reset();
  course_table_select(course_id);
  if (course_has_world_setup() != 0) {
    course_call_world_setup((undefined4)&g_course_world);
  }
  course_object_lists_reset();
  debug_printf("Opening piz file [%s]\n",g_course_piz_path);
  game_open_piz(g_course_piz_path);
  scan = (char *)course_list_scan(0,entry,"COURSE.LUA",&g_game_5dd5a2);
  dst = g_course_piz_path;
  do {
    c = *scan;
    scan = scan + 1;
    *dst = c;
    dst = dst + 1;
  } while (c != '\0');
  game_content_find(buf,"COURSE.LUA");
  course_script_run("COURSE.LUA",&g_course_files_data,&g_course_counts_data);
  g_course_counts_data.course_id = course_id;
  course_create_from_description(&g_course_world,&g_course_files_data,&g_course_counts_data);
  game_content_find(buf,"LAPDATA.LUA");
  lapdata_script_run("LAPDATA.LUA",&g_course_lap_data);
  g_course_counts_data.course_id = course_id;
  ai_polygons_log_count(&g_course_lap_data,g_course_world.ai_worlds[0]);
  rwp_world_initialize();
  rwp_trigger_groups_resolve_indices();
  trail_clumps_create();
  course_ai_polygons_build_grid(g_course_world.collision_bsps[0]);
  course_object_pairs_build_clumps((undefined4)&g_course_world);
  game_close_piz();
  line_exceptions_clear();
  iVar5 = line_exceptions_load();
  if (iVar5 != 0) {
    debug_printf("Loaded existing line exceptions\n");
  }
  else {
    debug_printf("clearing all line exceptions\n");
    line_exceptions_clear();
  }
  ai_clear();
  iVar5 = ai_load();
  if (iVar5 != 0) {
    debug_printf("Loaded AI data\n");
  }
  else {
    debug_printf("No AI Data loaded\n");
  }
  game_open_perm_piz();
  vehicle_slots_clear();
  copters_load();
  gsd_load();
  game_close_piz();
  sound_load_course_audio(course_id);
  course_collision_tables_clear();
  if (g_course_world.occluder_clump != 0) {
    course_collision_objects_build(g_course_world.occluder_clump);
  }
  if (g_course_world.fog_a[1] != g_course_world.fog_a[2]) {
    RwCameraSetFarClipPlane((RwCamera *)get_viewport_camera(0),g_course_world.fog_a[2]);
    RwCamera_fogPlane(get_viewport_camera(0)) = g_course_world.fog_a[1];
    get_viewport_camera(0);
    RwCameraSetFarClipPlane((RwCamera *)get_second_camera(),g_course_world.fog_a[2]);
    RwCamera_fogPlane(get_second_camera()) = g_course_world.fog_a[1];
    get_second_camera();
    game_set_option_flags(*(byte *)&g_course_world.fog_a[0],*((byte *)&g_course_world.fog_a[0] + 1),
                          *((byte *)&g_course_world.fog_a[0] + 2));
  }
  course_materials_fixup((RpWorld *)g_course_world.world,(void *)g_course_world.texture_dictionary);
  if (course_has_cb0() != 0) {
    course_call_cb0((undefined4)&g_course_world);
  }
  course_atomics_set_flag_0x40(&g_course_world,&g_course_counts_data);
  rain_buffers_alloc_once();
  g_course_lap_data.split_sectors[8] = 1;
  scene_object_select_set((undefined4)g_course_world.camera_anim,get_viewport_camera(0));
  g_course_counts_data.course_id = course_id;
  course_fog_apply();
  return;
}
