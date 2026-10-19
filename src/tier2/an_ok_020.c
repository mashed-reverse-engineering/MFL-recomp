#include "mfl_tier2.h"
/*0040f700*/ extern double intro_banner_get_x();
/*00413ad0*/ extern int replay_best_get_split_time();
/*004219c0*/ extern undefined __cdecl course_call_cb2(undefined4 p0);
/*004219d0*/ extern undefined __cdecl course_call_cb3(undefined4 p0);
/*00421aa0*/ extern int course_select();
/*00421ae0*/ extern int course_has_cb2();
/*00421af0*/ extern int course_has_cb3();
/*0042ab20*/ extern undefined __cdecl course_camera_add_to_collision_world(struct RwCamera *p0);
/*0042ab60*/ extern undefined __cdecl course_camera_remove_from_collision_world(struct RwCamera *p0);
/*0042c440*/ extern int ui_rt2d_text_draw(int id, float x, float y, unsigned int colour, float scale, int flags);      /* 6 stack args (id,x,y,rgba,scale,flags); x/y/scale are real floats */
/*0042c8e0*/ extern double text_width();
/*0042cdd0*/ extern int ui_draw_texture_640_default();
/*0042d920*/ extern int build_info_display();
/*0042f620*/ extern int game_content_exists_prefixed();
/*00435f10*/ extern int get_career_row();
/*00435f20*/ extern int ui_get_menu_69d690();
/*00435fb0*/ extern int ui_get_menu_69d2d0();
/*004364c0*/ extern int ui_set_triple();
/*004693c0*/ extern int sound_streams_pause();
/*00469400*/ extern int sound_streams_resume();
/*00473550*/ extern undefined __cdecl sound_frame_begin(undefined4 p0, undefined4 p1);
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*004817c0*/ extern undefined __cdecl atomic_plugin_get_flags(undefined4 p0);
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*004882d0*/ extern undefined __cdecl course_ribbons_render(undefined4 p0);
/*00488d80*/ extern undefined __cdecl particle_system_set_current_forward(undefined4 p0, undefined4 p1, undefined4 p2);
/*004891b0*/ extern undefined __cdecl course_world_ray_hit_light_sample(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004a07d0*/ extern int rain_render();
/*004a1880*/ extern int game_get_62ad2c();
/*004a5550*/ extern int log_printf();
/*004a5930*/ extern int joypads_poll_frame();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004b4ba7*/ extern int entry();
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d5bf0*/ extern RwBool RwTextureDestroy(struct RwTexture *p0);
/*004d5e40*/ extern struct RwTexture * RwTextureRead(RwChar *p0, RwChar *p1);
/*004ddb80*/ extern RwChar * RwImageSetPath(RwChar *p0);
/*004f5780*/ extern struct RpWorld * RpWorldRender(struct RpWorld *p0);
/*004f5d10*/ extern RwBool RpWorldInstance(struct RpWorld *p0);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*004f7f90*/ extern RwBool __cdecl RpAtomicInstance(struct RpAtomic *p0);
/*00562d20*/ extern int Rt2dCTMSetIdentity();
/*00562ef0*/ extern int Rt2dDeviceGetClippath();
/*00563030*/ extern int Rt2dDeviceSetCamera();
/*00563160*/ extern int Rt2dClose();
/*005632e0*/ extern int Rt2dCTMPush();
/*005642c0*/ extern int Rt2dPathGetBBox();
/*00564450*/ extern int Rt2dPathDestroy();
/*005644c0*/ extern int Rt2dPathCreate();
/*00564ac0*/ extern int Rt2dBrushDestroy();
/*005662e0*/ extern int Rt2dFontDestroy();
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd334*/ extern float k_f_0_033333335;
/*005dd340*/ extern float k_f_0_5;
/*005dd374*/ extern float k_f_0_2;
/*005ddea8*/ extern float k_f_320;
/*005de244*/ extern float k_f_500;
/*005de808*/ extern float k_f_0_7375;
/*005de80c*/ extern float k_f_0_78125;
/*005de810*/ extern float k_f_0_65416664;
/*005de814*/ extern float k_f_0_34583333;
/*005de818*/ extern float k_f_0_5625;
/*005de890*/ extern float k_f_28;
/*005de8f4*/ extern float k_f_470;
/*0062ac98*/ extern undefined g_ui_62ac98;
/*00664418*/ extern struct CourseCounts g_course_counts_data;
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00675668*/ extern struct CourseLap g_course_lap_data;
/*0068b9f8*/ extern undefined g_dat_68b9f8;
/*0069b9f8*/ extern undefined4 g_course_69b9f8;
/*0069ba00*/ extern float g_ui_layout_scale_x;
/*0069ba04*/ extern float g_ui_layout_scale_y;
/*0069ba08*/ extern undefined4 g_ui_font_texdict;
/*0069ba0c*/ extern undefined4 g_ui_font;
/*0069ba10*/ extern undefined4 g_ui_text_brush;
/*0069ba14*/ extern undefined4 g_ui_brush_14;
/*0069ba18*/ extern undefined4 g_ui_brush_transparent_black;
/*0069ba1c*/ extern undefined4 g_ui_brush_white;
/*0069ba20*/ extern undefined4 g_dat_69ba20;
/*0069bb30*/ extern undefined4 g_build_info_69bb30;
/*0069bb34*/ extern undefined4 g_speaker_69bb34;
/*0069bb38*/ extern undefined4 g_speaker_69bb38;
/*0069bb3c*/ extern undefined4 g_speaker_69bb3c;
/*0069bb40*/ extern undefined4 g_hud_69bb40;
/*0069bb5c*/ extern undefined4 g_course_69bb5c;
/*0069bb60*/ extern undefined4 g_best_69bb60;
/*0069bb64*/ extern undefined4 g_course_69bb64;
/*0069bb68*/ extern undefined4 g_best_69bb68;
/*0069bb6c*/ extern float g_course_69bb6c;
/*0069bb70*/ extern float g_best_69bb70;
/*0076e7f0*/ extern undefined4 g_hud_76e7f0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007eebc0*/ extern struct GameState g_game_state;
/*008c8798*/ extern undefined4 g_intro_banner_offset;
/*008c879c*/ extern undefined4 g_course_8c879c;
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

extern int course_get_record_point(char *param_1,char *param_2);


/* course_sky_clumps_update */

/* "(camera matrix): positions and rotates the sky clumps around the camera" */

extern void course_sky_clumps_update(char *param_1);


/* course_sky_clumps_render */

/* "renders the sky clumps with src / dest blend 5 / 6" */

extern void course_sky_clumps_render(void);


/* course_world_ray_hit_light_forward */

/* "forwards to course_world_ray_hit_light_sample on g_course_world" */

extern void __cdecl course_world_ray_hit_light_forward(char *param_1,char *param_2,char *param_3);


/* course_get_shadow_scale */

/* "g_course_counts_data.shadow_scale" */

extern float10 course_get_shadow_scale(void);


/* atomic_log_not_instancing */

/* "debug print 'Not instancing atomic %d' when the atomic flags have bit 4" */

extern RpAtomic * atomic_log_not_instancing(RpAtomic *atomic,int *param_2);


/* instancing_clump_log */

/*  */

extern void instancing_clump_log(void);


/* course_world_instance_for_viewport */

/* "adds the camera to the collision world, instances the world and removes the camera" */

extern void course_world_instance_for_viewport(void);


/* course_render_cb3_dispatch */

/* "course_call_cb3 + sky + render" */

extern void course_render_cb3_dispatch(char *param_1);


/* ui_2d_layout_rects_init */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "sets the two text rectangles of the 2D UI from the screen size" */

/* TIER2: validated 2026-10-18 authority=diffemu */
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


/* ui_2d_begin */

/* "Rt2dDeviceSetCamera + CTM push / identity" */

extern void ui_2d_begin(char *param_1);


/* ui_2d_destroy */

/* "destroys the Rt2d font and brushes" */

extern void ui_2d_destroy(void);


/* dat_get_string */

/* "(index): entry of the DAT string table at 0x68B9F8 (offset table followed by the strings)" */

extern undefined * dat_get_string(char *param_1);


/* course_get_dat_69ba20 */

/* "return g_dat_69ba20;" */

extern undefined4 course_get_dat_69ba20(void);


/* build_info_set_page */

/* "g_build_info page index (0 / 1 ...)" */

extern void build_info_set_page(char *param_1);


/* speaker_mode_names */

/* "DTSScreen MashedFullyLoaded proLogicII DolbyDig" */

extern void speaker_mode_names(void);


/* build_info_textures_destroy */

/* "destroys the speaker / hud textures of the build info screens" */

extern void build_info_textures_destroy(void);


/* build_info_draw_page_0 */

/* "draws the text lines 0x1ea..0x1f0 and two icons of the first info page" */

extern void build_info_draw_page_0(void);


/* build_info_frame */

/* "one frame of the build info screen: sound update, input poll, display" */

extern void build_info_frame(void);


/* store_u32 */

/* "*dst = value" */

extern void store_u32(undefined4 *param_1,char *param_2);


/* hud_get_8c8798 */

/* "float g_hud_8c8798" */

extern float10 hud_get_8c8798(void);


/* time_trial_default_record_a */

/* "(track, out fraction, out seconds, out minutes): default record time of a track (the first
   table, a different value when g_game_state.mirror_mode is set)" */

extern void time_trial_default_record_a (char *param_1,undefined4 *param_2,undefined4 *param_3,undefined4 *param_4);


/* time_trial_default_record_b */

/* "(track, out fraction, out seconds, out minutes): second default time table, same output layout,
   starts from 9:59.99" */

extern void time_trial_default_record_b (char *param_1,undefined4 *param_2,undefined4 *param_3,undefined4 *param_4);


/* race_lose_banner_draw */

/* "'You Lose!' banner (text 258)" */
/* draws the 'You Lose!' banner: a shadow pass offset by k_f_3, then the main pass at the base x (inferred) */

#define TEXT_YOU_LOSE (0x102)

/* TIER2: unvalidated 2026-10-16 authority=diffemu */
void race_lose_banner_draw(void)
{
  float10 banner_x;
  
  /* shadow pass: black, x = banner_x + k_f_3, y = 63.0 */
  banner_x = (float10)intro_banner_get_x();
  ui_rt2d_text_draw(TEXT_YOU_LOSE,(float)(banner_x + (float10)k_f_3),63.0f,0xff000000,1.0f,2);
  /* main pass: colour 0xff0000ff at the base x, y = 60.0 */
  banner_x = (float10)intro_banner_get_x();
  ui_rt2d_text_draw(TEXT_YOU_LOSE,(float)banner_x,60.0f,0xff0000ff,1.0f,2);
  return;
}


/* intro_banner_slide_y */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(t): 470 - (1 - min(t,30)/30) * 500, vertical slide of the intro banner" */

extern float10 __fastcall intro_banner_slide_y(char *param_1);


/* course_get_course_8c879c */

/* "return g_course_8c879c;" */

extern undefined4 course_get_course_8c879c(void);


/* time_is_better_than_best */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "compares the current time (min * 60 + sec + fraction) with the best time" */

extern undefined4 time_is_better_than_best(void);


/* time_set_current_max */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "sets the current time to 9:59.99 (0x3F7D70A4 = 0.99 fraction)" */

extern void time_set_current_max(void);


/* best_time_store */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* stores the current race time in the best time tables */

extern void __cdecl best_time_store(float fraction,uint seconds,uint minutes,char *param_4,char *param_5, char *param_6,char *param_7,char *param_8);


/* best_time_store_previous */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void best_time_store_previous(void);


/* time_get_fraction */

/* "(index): fraction of the time slot" */

extern float10 time_get_fraction(char *param_1);


/* time_get_minutes */

/* "(index): minutes of the time slot" */

extern undefined4 time_get_minutes(char *param_1);


/* time_get_seconds */

/* "(index): seconds of the time slot" */

extern undefined4 time_get_seconds(char *param_1);


/* best_time_load_current */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void best_time_load_current(void);


/* game_content_find */

/* "Searching piz for %s": pc/ then raw then xbox/ ps2/ gamecube/ */

extern undefined4 game_content_find(char *param_1,char *param_2);



