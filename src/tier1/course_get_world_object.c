#include "mfl_tier2.h"
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00408900*/ extern int fx_objects_destroy();
/*00413880*/ extern int current_course_id_with_mirror();
/*004219c0*/ extern undefined __cdecl course_call_cb2(undefined4 p0);
/*004219d0*/ extern undefined __cdecl course_call_cb3(undefined4 p0);
/*004219e0*/ extern undefined __cdecl course_call_cb4(undefined4 p0);
/*004219f0*/ extern undefined __cdecl course_call_cb5(undefined4 p0);
/*00421a00*/ extern int course_call_cb6();
/*00421a10*/ extern undefined __cdecl course_call_cb7(undefined4 p0);
/*00421a20*/ extern int course_call_cb8();
/*00421a30*/ extern int course_call_cb9();
/*00421a40*/ extern int course_call_cb10();
/*00421a50*/ extern int course_call_cb11();
/*00421aa0*/ extern int course_select();
/*00421ae0*/ extern int course_has_cb2();
/*00421af0*/ extern int course_has_cb3();
/*00421b00*/ extern int course_has_cb4();
/*00421b10*/ extern int course_has_cb5();
/*00421b20*/ extern int course_has_cb6();
/*00421b30*/ extern int course_has_cb7();
/*00421b40*/ extern int course_has_cb8();
/*00421b50*/ extern int course_has_cb9();
/*00421b60*/ extern int course_has_cb10();
/*00421b70*/ extern int course_has_cb11();
/*00429c20*/ extern int vehicle_slot_set();
/*00429cd0*/ extern undefined __stdcall load_copter_model(char *p0);
/*0042a110*/ extern int course_destroy_extra_clumps();
/*0046a010*/ extern int sound_stop_all_voices_and_streams();
/*00473a50*/ extern int course_unload_atomics_6ae24c();
/*00474880*/ extern int course_objects_destroy_atomics();
/*00475510*/ extern int course_grid_buffers_free();
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*004817c0*/ extern undefined __cdecl atomic_plugin_get_flags(undefined4 p0);
/*00485030*/ extern int frame_follow_path_step();
/*004882d0*/ extern undefined __cdecl course_ribbons_render(undefined4 p0);
/*00488d80*/ extern undefined __cdecl particle_system_set_current_forward(undefined4 p0, undefined4 p1, undefined4 p2);
/*00488da0*/ extern undefined __cdecl course_destroy_world(struct CourseWorld *p0, struct CourseCounts *p1);
/*004891b0*/ extern undefined __cdecl course_world_ray_hit_light_sample(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*00490ef0*/ extern int rwp_world_flush();
/*00490f30*/ extern int rwp_world_destroy();
/*00492e00*/ extern int rwp_objects_reset_all();
/*004a07d0*/ extern int rain_render();
/*004a0850*/ extern int fx_buffer_free();
/*004a1860*/ extern int game_set_option_flags();
/*004a1880*/ extern int game_get_62ad2c();
/*004b2b48*/ extern int _ftol();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d56a0*/ extern struct RwV3d * RwV3dTransformVector(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*004f5780*/ extern struct RpWorld * RpWorldRender(struct RpWorld *p0);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*004f7f90*/ extern RwBool __cdecl RpAtomicInstance(struct RpAtomic *p0);
/*005dd330*/ extern float float_1_0;
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005de6a8*/ extern float k_f_n0_5;
/*006095bc*/ extern undefined4 g_course_race_6095bc;
/*006095c0*/ extern undefined4 g_course_update_6095c0;
/*006095c4*/ extern undefined1 g_course_update_6095c4;
/*00664418*/ extern struct CourseCounts g_course_counts_data;
/*00665138*/ extern struct CourseFiles g_course_files_data;
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00675668*/ extern struct CourseLap g_course_lap_data;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* course_get_world_object */

/* "returns &g_course_world" */

CourseWorld * course_get_world_object(void)
{
  return &g_course_world;
}
