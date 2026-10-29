#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/*00406b90*/ extern undefined __cdecl ktc_add_pickup(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12, undefined4 p13, undefined4 p14, undefined4 p15, undefined4 p16);
/*004357e0*/ extern int __cdecl career_record_ptr(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12, undefined4 p13, undefined4 p14, undefined4 p15, undefined4 p16);
/*00474340*/ extern undefined __cdecl course_object_pair_add(undefined4 p0, undefined4 p1, undefined4 p2);
/*00480130*/ extern int text_page_reset_line();
/*00480150*/ extern int text_page_next();
/*00480190*/ extern int text_page_add_string_id();
/*00487ee0*/ extern undefined __cdecl triangle_normal_cross(struct RwV3d *p0, float *p1, float *p2, float *p3);
/*00487f80*/ extern undefined __cdecl vector_normalize_copy(struct RwV3d *p0, float *p1, RwReal *p2);
/*0048ce30*/ extern int lua_init();
/*0048ce50*/ extern int lua_state_close();
/*0048cea0*/ extern int __cdecl lua_run_piz_script(undefined4 p0, undefined4 p1);
/*00490400*/ extern undefined __cdecl rwp_world_step_pipeline(undefined4 p0, undefined4 p1);
/*00498f40*/ extern int fx_ring_add_point();
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*004a5d70*/ extern int object_render();
/*004a5da0*/ extern int object_destroy_frames();
/*004a5ed0*/ extern undefined __cdecl video_raster_create(undefined4 p0);
/*004a61a0*/ extern undefined __cdecl video_present_frame(undefined4 p0, undefined4 p1);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*004c72d0*/ extern int lua_pushnumber();
/*004c73e0*/ extern int lua_setglobal();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*005dd330*/ extern float float_1_0;
/*005e64a4*/ extern float k_f_0;
/*006299dc*/ extern undefined4 g_lua_current_type;
/*006299e0*/ extern float g_lua_current_respawn;
/*006f2adc*/ extern float g_lua_current_pos;
/*006f2ae0*/ extern float g_gfx_6f2ae0;
/*006f2ae4*/ extern float g_gfx_6f2ae4;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/*006f2b00*/ extern undefined4 g_lua_state;
/*006f2d88*/ extern undefined4 g_course_collision_6f2d88;
/*006f3588*/ extern undefined4 g_course_collision_6f3588;
/*006f35c8*/ extern undefined4 g_course_6f35c8;
/*006f4b48*/ extern undefined4 g_course_6f4b48;
/*006f60c8*/ extern undefined g_course_6f60c8;
/*006f68c8*/ extern undefined4 g_course_6f68c8;
/*006f6908*/ extern undefined4 g_course_6f6908;
/*006f690c*/ extern undefined4 g_course_6f690c;
/*006f6910*/ extern undefined4 g_data_6f6910;
/*006f6918*/ extern undefined4 g_data_6f6918;
/*006f6920*/ extern undefined4 g_data_6f6920;
/*006f6928*/ extern undefined4 g_data_6f6928;
/*006f6930*/ extern undefined4 g_rw_6f6930;
/*006f6934*/ extern undefined4 g_data_6f6934;
/*006f693c*/ extern undefined4 g_ptank_atomic;
/*006f6944*/ extern undefined4 g_ptank_lock_6f6944;
/*006fa7d0*/ extern undefined4 g_rw_6fa7d0;
/*006fa7d8*/ extern undefined4 g_rw_6fa7d8;
/*006fcdd8*/ extern undefined4 g_rw_6fcdd8;
/*006fd418*/ extern undefined4 g_physics_6fd418;
/*00701c94*/ extern int g_rw_701c94;
/*00701c98*/ extern int g_physics_701c98;
/*00771038*/ extern undefined4 g_app_771038;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*00888640*/ extern struct RwpObjectDesc g_rwp_objects[32];
/*0088a844*/ extern undefined4 g_rw_88a844;
/*0089de40*/ extern undefined4 g_gfx_89de40;
/*0089de60*/ extern undefined4 g_gfx_89de60;
/*0089df68*/ extern undefined4 g_gfx_89df68;
/*0089df94*/ extern undefined4 g_gfx_89df94;
/*008a0248*/ extern undefined4 g_gfx_8a0248;
/*008a0250*/ extern undefined4 g_gfx_8a0250;
/*008a025c*/ extern undefined g_lua_physics_8a025c;
/*008a032c*/ extern undefined4 g_gfx_8a032c;
/* lua_Shadow_Scale */

/* Sets the course shadow scale factor in course counts from the Lua script argument. */

undefined4 lua_Shadow_Scale(undefined4 lua_state)
{
  int arg_count;
  double shadow_scale;
  
  /* Retrieve the scale argument from the top of the Lua stack */
  arg_count = lua_gettop(lua_state);
  shadow_scale = (double)lua_tonumber(lua_state,arg_count);
  g_course_counts->shadow_scale = (float)shadow_scale;
  return 0;
}
