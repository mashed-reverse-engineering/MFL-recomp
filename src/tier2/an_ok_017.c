#include "mfl_tier2.h"
/*00408900*/ extern int fx_objects_destroy();
/*004219e0*/ extern undefined __cdecl course_call_cb4(undefined4 p0);
/*004219f0*/ extern undefined __cdecl course_call_cb5(undefined4 p0);
/*00421a00*/ extern int course_call_cb6();
/*00421a10*/ extern undefined __cdecl course_call_cb7(undefined4 p0);
/*00421a20*/ extern int course_call_cb8();
/*00421a30*/ extern int course_call_cb9();
/*00421a40*/ extern int course_call_cb10();
/*00421a50*/ extern int course_call_cb11();
/*00421aa0*/ extern int course_select();
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
/*00434a20*/ extern int get_second_camera();
/*0046a010*/ extern int sound_stop_all_voices_and_streams();
/*00473a50*/ extern int course_unload_atomics_6ae24c();
/*00474880*/ extern int course_objects_destroy_atomics();
/*00475510*/ extern int course_grid_buffers_free();
/*00475730*/ extern int get_viewport_camera();
/*00485030*/ extern int frame_follow_path_step();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00488da0*/ extern undefined __cdecl course_destroy_world(struct CourseWorld *p0, struct CourseCounts *p1);
/*00490ef0*/ extern int rwp_world_flush();
/*00490f30*/ extern int rwp_world_destroy();
/*00492e00*/ extern int rwp_objects_reset_all();
/*004a0850*/ extern int fx_buffer_free();
/*004a1860*/ extern int game_set_option_flags();
/*004a1880*/ extern int game_get_62ad2c();
/*004a43d0*/ extern undefined __cdecl game_open_piz(char *p0);
/*004b2b48*/ extern int _ftol();
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d1bd0*/ extern struct RwCamera * RwCameraSetNearClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d56a0*/ extern struct RwV3d * RwV3dTransformVector(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*004f6240*/ extern struct RwSphere * RpAtomicGetWorldBoundingSphere(struct RpAtomic *p0);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005de6a8*/ extern float k_f_n0_5;
/*006095bc*/ extern undefined4 g_course_race_6095bc;
/*006095c0*/ extern undefined4 g_course_update_6095c0;
/*006095c4*/ extern undefined1 g_course_update_6095c4;
/*00664414*/ extern undefined4 g_load_664414;
/*00664418*/ extern struct CourseCounts g_course_counts_data;
/*00665138*/ extern struct CourseFiles g_course_files_data;
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00675668*/ extern struct CourseLap g_course_lap_data;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8840*/ extern float g_course_8c8840;
/*008c8844*/ extern undefined4 g_course_8c8844;
/*008c8848*/ extern undefined4 g_course_8c8848;
/*008c884c*/ extern undefined4 g_course_fog_8c884c;
/*008c8850*/ extern undefined1 g_course_8c8850;
/*008c8854*/ extern undefined1 g_course_8c8854;
/*008c8858*/ extern undefined1 g_course_8c8858;
/*008c8860*/ extern undefined4 g_course_8c8860;
/*008c8864*/ extern undefined4 g_course_8c8864;
/*008c88a4*/ extern undefined4 g_game_8c88a4;
/*008c88b0*/ extern undefined4 g_course_8c88b0;
/*008c88f4*/ extern undefined4 g_game_8c88f4;
/*008c8900*/ extern undefined4 g_course_active_8c8900;
/*008c8944*/ extern undefined4 g_game_8c8944;
/*008c8950*/ extern undefined4 g_course_active_8c8950;
/*008c8994*/ extern undefined4 g_game_8c8994;
/*008c89a0*/ extern undefined4 g_course_active_8c89a0;
/*008c89e4*/ extern undefined4 g_game_8c89e4;
/*008c89f0*/ extern undefined4 g_course_active_8c89f0;
/*008c8a34*/ extern undefined4 g_game_8c8a34;
/*008c8a40*/ extern undefined4 g_course_active_8c8a40;
/*008c8a84*/ extern undefined4 g_game_8c8a84;
/*008c8a90*/ extern undefined4 g_course_active_8c8a90;
/*008c8ad4*/ extern undefined4 g_game_8c8ad4;
/* game_open_perm_piz */

/* "d:\ToastArt\common\perm.piz" */

extern void game_open_perm_piz(void);


/* frame_rotate_around_own_origin */

/* "translate to origin, RwFrameRotate about the up axis, translate back" */
/* Rotates the frame of obj around its own origin instead of the world origin:
 * the frame is moved so its position sits at the world origin, rotated by angle
 * about its up vector, then moved back by the same offset.
 * obj: object whose RwFrame pointer is at +4 (inferred). angle: passed to RwFrameRotate. */

/* TIER2: unvalidated 2026-10-19 authority=shadow */
void frame_rotate_around_own_origin(char *obj,RwReal angle)
{
  RwFrame *frame;
  RwV3d to_origin;

  /* to_origin = -frame position; translating by it moves the frame origin to the world origin */
  frame = *(RwFrame **)(obj + 4);
  to_origin.x = (RwFrame_modelling(frame)).pos.x * k_f_n1;
  to_origin.y = (RwFrame_modelling(frame)).pos.y * k_f_n1;
  to_origin.z = (RwFrame_modelling(frame)).pos.z * k_f_n1;
  RwFrameTranslate(frame,&to_origin,rwCOMBINEPOSTCONCAT);
  RwFrameRotate(*(RwFrame **)(obj + 4),&(RwFrame_modelling((*(RwFrame **)(obj + 4)))).up,angle,
                rwCOMBINEPOSTCONCAT);
  to_origin.x = to_origin.x * k_f_n1;
  to_origin.y = to_origin.y * k_f_n1;
  to_origin.z = to_origin.z * k_f_n1;
  RwFrameTranslate(*(RwFrame **)(obj + 4),&to_origin,rwCOMBINEPOSTCONCAT);
  return;
}


/* course_signs_render_visible */

/* "frustum tests each atomic and renders its three parts" */

extern void course_signs_render_visible(void);
#undef sphere
#undef RVar1
#undef piVar2


/* course_texture_dictionary_destroy */

/* "RwTexDictionaryDestroy(0x664414)" */

extern void course_texture_dictionary_destroy(void);


/* course_active_objects_count */

/* "counts the course object slots (0x14 dwords each) that have a clump and an atomic" */

extern int course_active_objects_count(void);


/* course_fog_apply */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "copies the course fog settings and the viewport colour into the render state at 0x8C8840" */

extern void course_fog_apply(void);


/* course_apply_clip_planes */

/* "near / far clip plane of the main camera from the course (0x8C8840 / 0x8C8844)" */

extern void course_apply_clip_planes(void);


/* course_get_world_object */

/* "returns &g_course_world" */

extern CourseWorld * course_get_world_object(void);


/* course_race_start */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "cb4 when present, sets 0x6095BC" */

extern void course_race_start(void);


/* course_get_rpworld */

/* "g_course_world.world" */

extern uint course_get_rpworld(void);


/* course_get_collision_bsp */

/* "g_course_world.collision_bsps[0]" */

extern uint course_get_collision_bsp(void);


/* course_get_ai_world */

/* "g_course_world.ai_worlds[0]" */

extern uint course_get_ai_world(void);


/* course_token_list_base */

/* "returns the base pointer of the course token list (used by the course_token_list_* helpers)" */

extern undefined4 course_token_list_base(void);


/* course_get_ambient_light */

/* "g_course_world.ambient_light" */

extern uint course_get_ambient_light(void);


/* course_get_directional_light */

/* "g_course_world.directional_light" */

extern uint course_get_directional_light(void);


/* course_is_outdoor */

/* "g_course_world.flag_outdoor" */

extern int course_is_outdoor(void);


/* course_get_light_position */

/* "address of the directional light frame position" */

extern int course_get_light_position(void);


/* copters_load */

/* "loads the copter models (SetCopter) and binds their general animations" */

extern void copters_load(void);


/* course_set_split_sector_6 */

/* "g_course_lap_data.split_sectors[6] = value" */

extern void course_set_split_sector_6(char *param_1);


/* course_hook10 */

/* "cb10 when the course is loaded (split_sectors[8] != 0)" */

extern void course_hook10(char *param_1);


/* course_set_split_sector_7 */

/* "g_course_lap_data.split_sectors[7] = value (camera in the collision world flag)" */

extern void __cdecl course_set_split_sector_7 (char *param_1,char *param_2,char *param_3,char *param_4,char *param_5);


/* course_uv_anim_update */

/* "scales the time by 1/3000 and updates the uv animations" */

extern void course_uv_anim_update(char *param_1,char *param_2,char *param_3);


/* course_camera_roll_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "computes the camera roll factor from the camera facing and calls game_set_option_flags" */

extern void course_camera_roll_update(char *param_1,RwCamera *camera);


/* course_unload */

/* "(course id): cb5, then course_destroy and the per-course resets" */

extern void __cdecl course_unload(char *param_1);


/* course_get_split_sector_8 */

/* "g_course_lap_data.split_sectors[8]" */

extern int course_get_split_sector_8(void);


/* course_get_ai_polygon_count */

/* "g_course_lap_data.ai_polygon_count" */

extern int course_get_ai_polygon_count(void);


/* course_get_safe_start_count */

/* "g_course_lap_data.safe_start_count" */

extern int course_get_safe_start_count(void);


/* course_hook7 */

/* "cb7(world) when present, then FUN_00492e00" */

extern void course_hook7(void);


/* course_hook6 */

/* "cb6 when present" */

extern void __cdecl course_hook6(char *param_1);


/* course_hook9 */

/* "cb9 when present" */

extern void __cdecl course_hook9(char *param_1);


/* course_hook8 */

/* "cb8 when present" */

extern void course_hook8(void);


/* course_hook11 */

/* "cb11 when present" */

extern void __cdecl course_hook11(char *param_1);


/* course_get_record_pos */

/* "(i): 0x681834 + i * 0x4C" */

extern int course_get_record_pos(char *param_1);


/* course_get_record_base */

/* "(i): 0x681828 + i * 0x4C" */

extern int course_get_record_base(char *param_1);


/* course_get_68b8b4_ptr */

/* "returns 0x68B8B4" */

extern undefined4 course_get_68b8b4_ptr(void);



