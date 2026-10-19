#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0040fdb0*/ extern int get_player_count();
/*004222a0*/ extern int vehicle_part_matrix();
/*0042a3f0*/ extern int course_get_rpworld();
/*00452b90*/ extern undefined __cdecl viewport_camera_sync_to_main(undefined4 p0, undefined4 p1, undefined4 p2);
/*0045b730*/ extern int course_clumps_destroy();
/*0045c7c0*/ extern int course_atomics_flags_clear();
/*0045c7f0*/ extern int course_atomics_render_3();
/*0045d560*/ extern int course_clump_render_with_physics();
/*0045e440*/ extern int weapon_pickups_clear();
/*0045f330*/ extern undefined __cdecl explosion_trigger_callback(struct RwV3d *p0, float *p1);
/*00465f60*/ extern int weapon_effect_get_slot_record();
/*00467a60*/ extern int weapons_update_frame();
/*00467e90*/ extern int course_get_rpworld_00467e90();
/*00472340*/ extern int sfx_play_player_set_at();
/*004726a0*/ extern int sfx_play_at();
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0047a470*/ extern int car_set_wheel_contact();
/*0047a5b0*/ extern int car_add_velocity();
/*0047b520*/ extern int car_current_matrix();
/*00481ea0*/ extern int player_marker_set_clump();
/*00481ee0*/ extern int player_marker_set_enabled();
/*00481fd0*/ extern double random_range();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0);
/*00484ad0*/ extern int pool_list_destroy();
/*00491ab0*/ extern int car_enter_rwp_mode();
/*00492b20*/ extern int car_apply_rwp_impulse();
/*00492d10*/ extern int rwp_object_set_active();
/*00495bc0*/ extern int viewport_raster_destroy();
/*00495c30*/ extern int viewport_get_camera();
/*00496850*/ extern int trigger_sphere_register();
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();
/*00499b40*/ extern int explosion_decal();
/*0049bd90*/ extern int fx_system_destroy();
/*0049bdd0*/ extern int fx_system_render();
/*0049c1a0*/ extern int particles_slot_set_vectors();
/*0049c280*/ extern int fx_snow_ptank_create();
/*0049c5f0*/ extern undefined __cdecl fx_snow_ptank_update(int p0, struct RwCamera *p1, undefined1 *p2, float p3, float p4);
/*0049c8f0*/ extern int particle_slot_call_render_method();
/*0049f400*/ extern int fx_ptank_atomic_destroy();
/*004a5d70*/ extern int object_render();
/*004a5da0*/ extern int object_destroy_frames();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004b322b*/ extern int exit();
/*004b4ba7*/ extern int entry();
/*004b5a47*/ extern int positive();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1160*/ extern struct RwFrame * RwFrameAddChild(struct RwFrame *p0, struct RwFrame *p1);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004f4440*/ extern struct RpWorld * RpWorldAddCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f4470*/ extern struct RpWorld * RpWorldRemoveCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f44a0*/ extern struct RpWorld * RpWorldAddAtomic(struct RpWorld *p0, struct RpAtomic *p1);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00545d00*/ extern RwBool RpAtomicDestroy_00545d00(struct RpAtomic *p0);
/*005dd340*/ extern float k_f_0_5;
/*005dd370*/ extern float k_f_0_3;
/*005dd378*/ extern float k_f_0_1;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005ddb20*/ extern float k_f_0_9;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005ddc28*/ extern float k_f_0;
/*005de194*/ extern float k_f_0_13;
/*0060d9c4*/ extern undefined g_car_60d9c4;
/*0060d9c8*/ extern undefined4 g_course_60d9c8;
/*0060d9cc*/ extern undefined4 g_course_60d9cc;
/*0060d9d8*/ extern undefined4 g_course_player_60d9d8;
/*0064d894*/ extern undefined4 g_course_64d894;
/*006a06e0*/ extern undefined4 g_sky_atomic;
/*006a26c4*/ extern float g_course_6a26c4;
/*006a26cc*/ extern undefined4 g_course_clumps_6a26cc;
/*006a2724*/ extern float g_course_6a2724;
/*006a272c*/ extern undefined4 g_course_clumps_6a272c;
/*006a2780*/ extern undefined4 g_course_sands_6a2780;
/*006a2794*/ extern float g_course_superg_6a2794;
/*006a2888*/ extern undefined g_course_6a2888;
/*006a291c*/ extern undefined4 g_course_get_6a291c;
/*006a2950*/ extern float g_course_get_6a2950;
/*006a29a4*/ extern undefined4 g_course_warzone_6a29a4;
/*006a29bc*/ extern undefined4 g_weapon_mortar_6a29bc;
/*006a29c0*/ extern undefined4 g_weapon_6a29c0;
/*006a29c4*/ extern undefined g_weapon_mortar_6a29c4;
/*006a29c8*/ extern undefined4 g_weapon_6a29c8;
/*006a29cc*/ extern undefined4 g_weapon_6a29cc;
/*006a29d0*/ extern undefined4 g_weapon_6a29d0;
/*006a2a28*/ extern undefined g_weapon_6a2a28;
/*006a2a70*/ extern undefined4 g_weapon_6a2a70;
/*006a2a80*/ extern undefined4 g_weapon_6a2a80;
/*006a4c28*/ extern struct ExplosionEntry g_explosion_queue[100];
/*006a5a38*/ extern undefined4 g_explosion_queue_6a5a38;
/*006a5a6c*/ extern undefined4 g_weapon_drum_6a5a6c;
/*006a5a78*/ extern undefined4 g_weapon_drum_6a5a78;
/*006a5c60*/ extern undefined g_weapon_6a5c60;
/*006a5c6c*/ extern undefined4 g_weapon_drum_6a5c6c;
/*006a5c70*/ extern undefined4 g_weapon_drum_6a5c70;
/*006a5c74*/ extern undefined g_weapon_drum_6a5c74;
/*006a5c80*/ extern undefined4 g_weapon_drum_6a5c80;
/*006a5d10*/ extern undefined4 g_weapon_6a5d10;
/*006a5d14*/ extern undefined4 g_weapon_drum_6a5d14;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008bf050*/ extern undefined4 g_car_8bf050;
/*008bf060*/ extern undefined4 g_course_warzone_8bf060;
/*008bf080*/ extern undefined4 g_course_8bf080;
/*008bf084*/ extern undefined4 g_course_training_8bf084;
/*008bf088*/ extern undefined4 g_course_sands_8bf088;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*008d8b30*/ extern undefined4 player_count;
#define QUAD_VERTEX_COUNT (4)
#define IM2D_VERTEX_FLOATS (7) /* sizeof(RwIm2DVertex) / sizeof(float) */
/* draw_textured_quad_uv */

/* "(tex, x, y, w, h, colour, uv[4]): fills g_im2d_vertices with the uv rectangle and draws the
   quad"
   Draws one screen-space quad (triangle strip of QUAD_VERTEX_COUNT Im2D vertices) at (x, y) of
   size width x height, textured with texture_raster (NULL = untextured).
   colour: packed colour word from the caller; its bytes are rearranged into the vertex colour (inferred).
   uv: {u0, v0, u1, v1} texture rectangle, one value per corner (inferred from the assignments below).
   No return value. */

/* TIER2: validated 2026-10-18 authority=diffemu */
void draw_textured_quad_uv(void *texture_raster,float x,float y,float width,float height,uint colour, RwReal *uv)
{
  RwGlobalsRelease *engine;
  RwReal *vertex_z;

  engine = RwEngineInstance;
  g_im2d_vertices[0].u = *uv;
  g_im2d_vertices[1].u = uv[2];
  g_im2d_vertices[0].v = uv[1];
  g_im2d_vertices[2].v = uv[3];
  g_im2d_vertices[0].x = x;
  g_im2d_vertices[0].y = y;
  g_im2d_vertices[1].x = width + x;
  g_im2d_vertices[0].z = RwIm2DGetNearScreenZ();
  g_im2d_vertices[1].y = y;
  g_im2d_vertices[2].x = x;
  g_im2d_vertices[2].y = height + y;
  g_im2d_vertices[0].rhw = 1.0;
  vertex_z = &g_im2d_vertices[0].z;
  g_im2d_vertices[1].v = g_im2d_vertices[0].v;
  g_im2d_vertices[2].u = g_im2d_vertices[0].u;
  g_im2d_vertices[3].x = g_im2d_vertices[1].x;
  g_im2d_vertices[3].y = g_im2d_vertices[2].y;
  g_im2d_vertices[3].u = g_im2d_vertices[1].u;
  g_im2d_vertices[3].v = g_im2d_vertices[2].v;
  /* Loop over the four vertices: set colour, z (zBufferNear) and rhw (1.0) on each one (inferred). */
  do {
    *(RwUInt32 *)(vertex_z + 2) = ((((uint)(((colour >> 16) & 0xffff) >> 8) << 8 | colour & 0xff) << 8 |
                         colour >> 8 & 0xff) << 8 | ((colour >> 16) & 0xffff) & 0xff);
    *vertex_z = RwGlobalsRelease_dOpenDevice_zBufferNear(engine);
    vertex_z[1] = 1.0;
    vertex_z = vertex_z + IM2D_VERTEX_FLOATS;
  } while (vertex_z < &g_im2d_vertices[QUAD_VERTEX_COUNT].z);
  /* Bind the raster and use linear filtering; with no raster, just unbind it (inferred). */
  if (texture_raster == (void *)0x0) {
    (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(engine))(rwRENDERSTATETEXTURERASTER,(void *)0x0);
  }
  else {
    (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(engine))(rwRENDERSTATETEXTURERASTER,texture_raster);
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,(void *)rwFILTERLINEAR);
  }
  /* Overlay state: depth test and write off, vertex alpha on, then draw the strip. */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
  RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP,g_im2d_vertices,QUAD_VERTEX_COUNT);
  /* Restore depth write and test (inferred: the caller expects them on). */
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  return;
}
