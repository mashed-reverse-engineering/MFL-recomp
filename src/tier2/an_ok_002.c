#include "mfl_tier2.h"
/*00401210*/ extern int cup_scene_setup_frames();
/*00405ab0*/ extern int vec3_multiply_components();
/*00409eb0*/ extern double player_rank_key();
/*0040fdc0*/ extern int race_get_phase();
/*0040fe00*/ extern double race_get_time_seconds();
/*004146a0*/ extern int trail_get_value();
/*00414760*/ extern undefined __cdecl trail_get_triple(undefined4 p0, undefined4 p1, undefined4 p2);
/*00414f00*/ extern int trail_update_00414f00();
/*0041a370*/ extern double ai_get_counter();
/*00421ca0*/ extern int vehicle_particles_render();
/*00421cf0*/ extern int game_set_particles_60761c();
/*00421d00*/ extern int game_set_particles_607620();
/*00421d10*/ extern int game_set_65a9b8_607624();
/*00421d30*/ extern int game_set_65a9c0_65a9c4();
/*00421fe0*/ extern int vehicle_set_field_198();
/*004221c0*/ extern double vehicle_get_float_184();
/*00422350*/ extern int vehicle_part_ltm();
/*00423c90*/ extern undefined __cdecl vehicle_render(int p0, struct RwCamera *p1);
/*004246f0*/ extern int car_set_damage();
/*00426b40*/ extern int car_kill();
/*0042a3f0*/ extern int course_get_rpworld();
/*0042a420*/ extern int course_token_list_base();
/*0042a440*/ extern int course_get_directional_light();
/*0042b1d0*/ extern int course_get_record_base();
/*0042c110*/ extern int text_draw_box();
/*00430b20*/ extern int frontend_stub_return_0_b();
/*00430b50*/ extern int ui_input_mode_menu();
/*00431a20*/ extern int ui_get_message_state();
/*00435040*/ extern int team_color_names();
/*00439040*/ extern int frontend_enter_race_menu();
/*00446310*/ extern int quick_race_defaults_set();
/*0044c540*/ extern int camera_get_car_8c747c();
/*00451c50*/ extern int camera_records_warmup();
/*00467d90*/ extern int weapons_unequip_all_a();
/*00467df0*/ extern int weapons_unequip_all_b();
/*0046fc20*/ extern int sound_bomb_countdown_voices();
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern void * get_viewport_camera_matrix();
/*00481fc0*/ extern int gfx_set_89ddfc();
/*00481fd0*/ extern double random_range();
/*004866b0*/ extern int fx_emitter_spawn_a_default();
/*0048a060*/ extern int spring_grid_reset_constants();
/*0048a0b0*/ extern int gfx_set_gfx_6f2ab8();
/*0048a0c0*/ extern int spring_grid_build();
/*0048a660*/ extern int gfx_set_vec3_6f2aa8(float p0, int p1, float p2);      /* defarity: the original body reads three stack slots (stores x, y, z to 0x6f2aa8); the call site passes (rr*at.x, 0, rr*at.z), the 2nd and 3rd are the leftover slots of the batched cleanup; the float arguments are dwords (with `()` they were pushed as doubles) */
/*0048a9e0*/ extern int spring_grid_update(float p0, void *p1);      /* defectlint: was `()`: g_tick_seconds was pushed as a double, the matrix pointer arrived in the wrong slot */
/*0048b030*/ extern int spring_grid_render();
/*0048b4d0*/ extern int gfx_set_6299d8();
/*0048e3b0*/ extern int rwp_object_set_user_data();
/*0048e3d0*/ extern int rwp_object_get_user_data();
/*00491ab0*/ extern int car_enter_rwp_mode();
/*00492b20*/ extern int car_apply_rwp_impulse();
/*00498f40*/ extern int fx_ring_add_point();
/*004a2990*/ extern int app_state_set_1();
/*004b2b48*/ extern int _ftol();
/*004b2f90*/ extern double floor();
/*004b4ba7*/ extern int entry();
/*004c7cc0*/ extern undefined __cdecl message(undefined4 p0, undefined4 p1);
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1d70*/ extern struct RwCamera * RwCameraShowRaster(struct RwCamera *p0, void *p1, RwUInt32 p2);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004f4440*/ extern struct RpWorld * RpWorldAddCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f4470*/ extern struct RpWorld * RpWorldRemoveCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*004f8140*/ extern struct RpMaterial * RpMaterialCreate();
/*004f81d0*/ extern RwBool RpMaterialDestroy(struct RpMaterial *p0);
/*004f8800*/ extern struct RpMorphTarget * RpMorphTargetCalcBoundingSphere(struct RpMorphTarget *p0, struct RwSphere *p1);
/*004f8b00*/ extern struct RpGeometry * RpGeometryTriangleSetMaterial(struct RpGeometry *p0, struct RpTriangle *p1, struct RpMaterial *p2);
/*004f8bd0*/ extern struct RpGeometry * RpGeometryUnlock(struct RpGeometry *p0);
/*004f8e30*/ extern struct RpGeometry * RpGeometryCreate(RwInt32 p0, RwInt32 p1, RwUInt32 p2);
/*0050b5e0*/ extern int nop_stub();
/*005dd330*/ extern float float_1_0;
/*005dd354*/ extern float k_f_637_5;
/*005dd358*/ extern float k_f_1800;
/*005dd35c*/ extern float k_f_n1_4;
/*005dd360*/ extern float k_f_1_9;
/*005dd364*/ extern float k_f_n1;
/*005dd378*/ extern float k_f_0_1;
/*005dd634*/ extern float k_f_2;
/*005dd7e4*/ extern float k_f_0_03;
/*005ddaf0*/ extern float k_f_1_0en5;
/*005ddb2c*/ extern float k_f_1_6999999;
/*005ddb30*/ extern float k_f_0_083333336;
/*005ddc28*/ extern float k_f_0;
/*005fb0a8*/ extern float g_bomb_5fb0a8;
/*005fb0ac*/ extern float g_course_5fb0ac;
/*005fb0b0*/ extern undefined4 g_bomb_5fb0b0;
/*0062ac74*/ extern float g_car_62ac74;
/*0064cbbc*/ extern undefined4 g_cup_64cbbc;
/*0064cbc0*/ extern undefined4 g_cup_64cbc0;
/*0064cbd4*/ extern undefined4 g_cup_64cbd4;
/*0064cbd8*/ extern undefined4 g_cup_64cbd8;
/*0064cbdc*/ extern undefined4 g_cup_scene_64cbdc;
/*0064cbe0*/ extern float g_cup_scene_64cbe0;
/*0064cbe8*/ extern undefined4 g_cup_64cbe8;
/*0064cbec*/ extern char *g_cup_scene_64cbec;
/*0064cbf0*/ extern char *g_cup_scene_64cbf0;
/*0064cbf4*/ extern char *g_cup_scene_64cbf4;
/*0064cc1c*/ extern char *g_cup_scene_64cc1c;
/*0064cc20*/ extern char *g_cup_64cc20;
/*0064cc28*/ extern char *g_cup_scene_64cc28;
/*0064cc38*/ extern char *g_cup_scene_64cc38;
/*0064d880*/ extern undefined4 g_cup_64d880;
/*0064d898*/ extern undefined4 g_log_64d898;
/*0064d8a8*/ extern float g_attract_timer_64d8a8;
/*0064d8ac*/ extern float g_attract_timer_64d8ac;
/*0064d8b0*/ extern float g_attract_timer;
/*0064d8b4*/ extern float g_scene_64d8b4;
/*0064d8b8*/ extern undefined4 g_attract_timer_64d8b8;
/*0064d8dc*/ extern undefined4 g_bomb_64d8dc;
/*0064d8e0*/ extern undefined4 g_bomb_64d8e0;
/*0064d8e4*/ extern undefined4 g_course_64d8e4;
/*0064d8f4*/ extern undefined4 g_course_64d8f4;
/*0064d908*/ extern undefined4 g_bomb_update_64d908;
/*0064d91c*/ extern undefined4 g_bomb_update_64d91c;
/*0064d930*/ extern undefined4 g_bomb_update_64d930;
/*0064d944*/ extern undefined4 g_bomb_update_64d944;
/*0064d950*/ extern undefined4 g_crown_64d950;
/*0064d958*/ extern undefined4 g_scene_tables_64d958;
/*006503a0*/ extern undefined4 g_scene_tables_6503a0;
/*00651198*/ extern undefined4 g_scene_tables_651198;
/*006511a0*/ extern undefined4 g_scene_tables_6511a0;
/*006511a4*/ extern undefined4 g_scene_tables_6511a4;
/*006511c4*/ extern undefined4 g_scene_tables_6511c4;
/*006511fc*/ extern undefined4 g_scene_tables_6511fc;
/*00651200*/ extern undefined4 g_scene_tables_651200;
/*00651210*/ extern undefined4 g_scene_tables_651210;
/*0065121c*/ extern undefined4 g_scene_tables_65121c;
/*00651220*/ extern undefined4 g_scene_tables_651220;
/*00651228*/ extern undefined4 g_scene_tables_651228;
/*0065122c*/ extern undefined4 g_scene_tables_65122c;
/*00651230*/ extern undefined4 g_scene_tables_651230;
/*00651234*/ extern undefined4 g_scene_tables_651234;
/*00651240*/ extern undefined4 g_scene_selected_object;
/*00651244*/ extern float g_scene_select_timer;
/*00651248*/ extern undefined4 g_scene_select_param;
/*00651260*/ extern undefined4 g_fx_651260;
/*00651264*/ extern undefined4 g_fx_651264;
/*00651298*/ extern undefined4 g_fx_651298;
/*006512ac*/ extern undefined4 g_fx_6512ac;
/*006512b0*/ extern undefined4 g_fx_6512b0;
/*006512b8*/ extern undefined4 g_fx_6512b8;
/*006512bc*/ extern undefined4 g_fx_6512bc;
/*006512c0*/ extern undefined4 g_fx_6512c0;
/*006512c4*/ extern undefined4 g_fx_6512c4;
/*006512c8*/ extern undefined4 g_fx_6512c8;
/*006512e4*/ extern undefined4 g_fx_6512e4;
/*006512e8*/ extern undefined4 g_fx_6512e8;
/*006512ec*/ extern undefined4 g_fx_6512ec;
/*006512f0*/ extern undefined4 g_fx_6512f0;
/*006512f4*/ extern undefined4 g_fx_6512f4;
/*006512f8*/ extern undefined4 g_fx_6512f8;
/*006512fc*/ extern char *g_ktc_new_6512fc;
/*00651300*/ extern undefined4 g_fx_651300;
/*00651304*/ extern undefined4 g_fx_651304;
/*0065131c*/ extern undefined4 g_fx_65131c;
/*00651320*/ extern undefined4 g_fx_651320;
/*00651324*/ extern undefined4 g_fx_651324;
/*00651328*/ extern undefined4 g_fx_651328;
/*0065132c*/ extern undefined4 g_fx_65132c;
/*00651330*/ extern undefined4 g_fx_651330;
/*00651334*/ extern undefined4 g_fx_651334;
/*00651338*/ extern undefined4 g_fx_651338;
/*00651aa8*/ extern undefined4 g_fx_651aa8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7b0*/ extern undefined4 g_car_7ef7b0;
/*007ef7c8*/ extern undefined4 g_bomb_7ef7c8;
/*007ef7cc*/ extern float g_course_7ef7cc;
/*007ef7ec*/ extern float g_frame_seconds;
/*007ef7f4*/ extern float g_tick_seconds;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* cup_scene_camera_attach */

/* "moves the current camera into the cup scene world (RpWorldRemoveCamera / AddCamera)" */

extern void cup_scene_camera_attach(void);

#define NO_SLOT (-1)                /* slot_index value for "no player in this slot" (inferred) */
#define COLOUR_CHANNEL_MAX (0xff)   /* full intensity of one RGBA byte */
#define RANDOM_MIN_BITS (0xbf800000) /* float -1.0 as bits: lower bound of random_range */
#define RANDOM_MAX_BITS (0x3f800000) /* float 1.0 as bits: upper bound of random_range */
#define MATRIX_FLOAT_COUNT (0x10)    /* floats in a 4x4 RwMatrix, copied one by one */
#define COLOUR_CHANNEL_RANGE (0x100) /* one past COLOUR_CHANNEL_MAX: clamp limit for an int colour value */


/* cup_scene_place_object */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/*
 * Places the cup scene marker model in front of the viewport camera, offset to one side, with a random tilt,
 * then draws it with the team texture of slot_index (inferred).
 * Parameters: alpha_float: fade value; alpha byte = clamp(alpha_float * 637.5, 0, 255), so it saturates at 0.4 (inferred);
 * flip != 0 moves the marker to the other side and turns it by 0 degrees instead of 180;
 * slot_index indexes g_player_slots, NO_SLOT = nothing to place. Returns nothing.
 */

/* TIER2: unvalidated 2026-10-22 authority=diffemu */
void cup_scene_place_object(undefined4 alpha_float,int flip,int slot_index)
{
  RwReal *camera_src;
  undefined4 team_texture;
  int alpha_byte;
  RwMatrix *matrix_cursor;
  double scratch;
  float random_z;
  float side_offset;
  RwReal rotate_degrees;
  RwV3d translation;
  float spawn_dx;
  float spawn_dy;
  float spawn_dz;
  RwMatrix object_matrix;

  translation.x = 0.0;
  translation.y = 0.0;
  translation.z = 0.0;
  side_offset = 2.0;
  rotate_degrees = 180.0;
  if (flip != 0) {
    side_offset = -2.0;
    rotate_degrees = 0.0;
  }
  if (slot_index != NO_SLOT) {
    /* copy the viewport camera matrix (MATRIX_FLOAT_COUNT floats) into object_matrix, then move it by the side offset */
    camera_src = (RwReal *)get_viewport_camera_matrix(0);
    matrix_cursor = &object_matrix;
    for (alpha_byte = MATRIX_FLOAT_COUNT; alpha_byte != 0; alpha_byte = alpha_byte + -1) {
      (matrix_cursor->right).x = *camera_src;
      camera_src = camera_src + 1;
      matrix_cursor = (RwMatrix *)&(matrix_cursor->right).y;
    }
    translation.x = object_matrix.pos.x * k_f_n1;
    translation.y = object_matrix.pos.y * k_f_n1;
    translation.z = object_matrix.pos.z * k_f_n1;
    RwMatrixTranslate(&object_matrix,&translation,rwCOMBINEPOSTCONCAT);
    spawn_dx = object_matrix.right.x * side_offset + object_matrix.up.x * k_f_n1_4 + object_matrix.at.x * k_f_1_9;
    spawn_dy = object_matrix.right.y * side_offset + object_matrix.up.y * k_f_n1_4 + object_matrix.at.y * k_f_1_9;
    spawn_dz = object_matrix.right.z * side_offset + object_matrix.up.z * k_f_n1_4 + object_matrix.at.z * k_f_1_9;
    /* random tilt: two random values in -1..1 scale the at axis (inferred); the third goes to the 3rd argument of gfx_set_vec3_6f2aa8 */
    scratch = (double)random_range(RANDOM_MIN_BITS,RANDOM_MAX_BITS);
    random_z = (float)(scratch * (double)object_matrix.at.z);      /* the original stores this in a stack slot that gfx_set_vec3_6f2aa8 reads as its 3rd argument */
    scratch = (double)random_range(RANDOM_MIN_BITS,RANDOM_MAX_BITS);
    gfx_set_vec3_6f2aa8((float)(scratch * (double)object_matrix.at.x),0,random_z);
    RwMatrixRotate(&object_matrix,&object_matrix.up,rotate_degrees,rwCOMBINEPOSTCONCAT);
    translation.x = translation.x * k_f_n1 + spawn_dx;
    translation.y = translation.y * k_f_n1 + spawn_dy;
    translation.z = translation.z * k_f_n1 + spawn_dz;
    RwMatrixTranslate(&object_matrix,&translation,rwCOMBINEPOSTCONCAT);
    /* the spring grid is rebuilt when more than k_f_1800 seconds have passed since g_cup_scene_64cbe0 (inferred) */
    if (g_cup_scene_64cbdc == 0) {
      scratch = (double)race_get_time_seconds();
      g_cup_scene_64cbe0 = (float)scratch;
      g_cup_scene_64cbdc = 1;
    }
    scratch = (double)race_get_time_seconds();
    if ((double)k_f_1800 < scratch - (double)g_cup_scene_64cbe0) {
      spring_grid_reset_constants();
      spring_grid_build();
      g_cup_scene_64cbdc = 0;
    }
    spring_grid_update(g_tick_seconds,&object_matrix);
    team_texture = team_color_names(g_player_slots[slot_index].team);
    gfx_set_gfx_6f2ab8(team_texture);
    /* draw with alpha blending, no depth test or depth write, no culling; restored at the end */
    RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
    alpha_byte = (int)(*(float *)&alpha_float * k_f_637_5);      /* diffemu: was `_ftol()` without its ST0 argument (alpha_float * 637.5) */
    /* clamp the alpha byte to 0..COLOUR_CHANNEL_MAX before handing it to gfx_set_6299d8 */
    if (alpha_byte < COLOUR_CHANNEL_RANGE) {
      if (alpha_byte < 0) {
        alpha_byte = 0;
      }
    }
    else {
      alpha_byte = COLOUR_CHANNEL_MAX;
    }
    gfx_set_6299d8(alpha_byte);
    spring_grid_render();
    /* restore the default render states: depth test and write on, back-face culling */
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLBACK);
  }
  return;
}


/* cup_scene_render */

/* "clears the camera and renders the cup scene clump(s)" */

extern void cup_scene_render(float param_1,char *param_2);

#define CUP_RECORD_SIZE (0x68)
#define CUP_RECORD_TABLE_SIZE (0xc98)
#define CUP_GROUP_A_CLUMPS (4)
#define CUP_GROUP_B_CLUMPS (6)


/* cup_scene_destroy_clumps */

/* "destroys the cup scene clumps (4 + 6 clumps per record at 0x64CC28 / 0x64CC38, not a 4 x 4 grid)"
   Walks the cup scene record table (CUP_RECORD_SIZE bytes per record from 0x64CBE8) and destroys the
   clumps of each record, then resets the record's fields. Takes no parameters, returns nothing.
   The record fields are described as far as known (inferred). */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void cup_scene_destroy_clumps(void)
{
  int remaining;
  uint record_offset;
  undefined4 *group_a_clump;
  undefined4 *group_b_clump;

  g_cup_64d880 = &g_cup_64cbe8;
  record_offset = 0;
  /* for each record: destroy its clumps, then reset its fields */
  do {
    group_b_clump = (undefined4 *)((char *)&g_cup_scene_64cc38 + record_offset);
    group_a_clump = (undefined4 *)((char *)&g_cup_scene_64cc28 + record_offset);
    /* group A: CUP_GROUP_A_CLUMPS clump pointers at record +0x40 */
    remaining = CUP_GROUP_A_CLUMPS;
    do {
      RpClumpDestroy((RpClump *)*group_a_clump);
      group_a_clump = group_a_clump + 1;
      remaining = remaining + -1;
    } while (remaining != 0);
    /* group B: CUP_GROUP_B_CLUMPS clump pointers at record +0x50 */
    remaining = CUP_GROUP_B_CLUMPS;
    do {
      RpClumpDestroy((RpClump *)*group_b_clump);
      group_b_clump = group_b_clump + 1;
      remaining = remaining + -1;
    } while (remaining != 0);
    /* the record's own clump at +0x00 (inferred): destroy it if set, then clear the pointer */
    if (*(RpClump **)((char *)&g_cup_64cbe8 + record_offset) != (RpClump *)0x0) {
      RpClumpDestroy(*(RpClump **)((char *)&g_cup_64cbe8 + record_offset));
      *(undefined4 *)((char *)&g_cup_64cbe8 + record_offset) = 0;
    }
    /* reset the record fields; +0x34 is set to 1.0f (0x3f800000, inferred) */
    *(undefined4 *)((char *)&g_cup_64cbe8 + record_offset) = 0;
    *(undefined4 *)((char *)&g_cup_scene_64cbec + record_offset) = 0;
    *(undefined4 *)((char *)&g_cup_scene_64cbf0 + record_offset) = 0;
    *(undefined4 *)((char *)&g_cup_scene_64cbf4 + record_offset) = 0;
    *(undefined4 *)((char *)&g_cup_scene_64cc1c + record_offset) = 0x3f800000;
    *(undefined4 *)((char *)&g_cup_64cc20 + record_offset) = 0;
    record_offset = record_offset + CUP_RECORD_SIZE;
  } while (record_offset < CUP_RECORD_TABLE_SIZE);
  return;
}


/* geometry_build_arrow */

/* "RpGeometry made of triangles with a material (vertices (-1,0,-1)...(1,0,4) style)" */

extern RpGeometry * geometry_build_arrow(void);


/* geometry_build_pointer */

/* "second small RpGeometry builder (triangles, material)" */

extern RpGeometry * geometry_build_pointer(void);


/* cup_scene_clear_screen */

/* "RwCameraClear black for each of the 4 viewports, then RwCameraShowRaster" */

extern void __cdecl cup_scene_clear_screen (char *param_1,char *param_2,char *param_3,char *param_4,char *param_5, char *param_6,char *param_7,char *param_8,char *param_9);


/* scene_get_log_64d898 */

/* "return g_log_64d898;" */

extern undefined4 scene_get_log_64d898(void);


/* attract_timer_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "adds g_frame_seconds while no message is shown; after the limit starts the attract / demo" */

extern void attract_timer_update(void);


/* pause_input_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(dt): player 0 pause / menu select press edge (g_player_inputs vs previous) after 2 s opens the
   pause or quit menu; unequips all weapons" */

/* pause_input_update: promoted to a byte-identical agent file (recomp/automatch_agent/ag_pause_input_update.c) */
extern void __cdecl pause_input_update(float param_1);


/* scene_get_bomb_5fb0a8 */

/* "return (double)g_bomb_5fb0a8;" */

extern double scene_get_bomb_5fb0a8(void);


/* bomb_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "Beat the Bomb: counts the bomb timer (g_race 0x7EF7CC) down, normalised in 0x5FB0A8; at 0 calls
   car_kill / car_enter_rwp_mode on the carrier and plays the explosion" */

extern void bomb_update(void);

#define TEXT_SHADOW_RGBA (0xff000000)  /* black, drawn first (inferred) */
#define TEXT_FILL_RGBA (0xc8ffffff)    /* white at alpha 0xc8, drawn on top (inferred) */
#define TEXT_FILL_ALT_RGBA (0xc800d8d8)  /* second fill colour, used by training_text_box_b only (inferred) */
#define TRAINING_STRING_234 (0x234)
#define TRAINING_STRING_F6 (0xf6)
#define TRAINING_STRING_235 (0x235)
#define TEXT_BOX_X (0x42f00000)              /* float 120.0: fill x of the box (inferred) */
#define TEXT_BOX_SHADOW_X (0x42f60000)       /* float 123.0: shadow x, fill x + 3 */
#define TEXT_BOX_Y (0x42a00000)              /* float 80.0: fill y of the first box (inferred) */
#define TEXT_BOX_SHADOW_Y (0x42a60000)       /* float 83.0: shadow y of the first box */
#define TEXT_BOX_Y_2 (0x42dc0000)            /* float 110.0: fill y of the second box */
#define TEXT_BOX_SHADOW_Y_2 (0x42e20000)     /* float 113.0: shadow y of the second box */
#define TEXT_BOX_WIDTH (0x43c80000)          /* float 400.0: box width (inferred) */
#define TEXT_BOX_HEIGHT (0x42a00000)         /* float 80.0: box height (inferred) */
#define TEXT_BOX_SCALE (0x3f4ccccd)          /* float 0.8: last argument, alpha / scale (inferred) */


extern void training_text_box_b(void);


/* training_text_box_c */

/* "strings 0x234" */

/* Draws the single training text box of string TRAINING_STRING_234 (inferred: tutorial subtitle).
   Drawn twice: a black copy offset by (+3, +3) as a drop shadow, then the white fill copy at the base position.
   Takes no parameters, returns nothing. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void training_text_box_c(void)
{
  text_draw_box(TRAINING_STRING_234,0x42f60000,0x42a60000,0x43c80000,0x42a00000,TEXT_SHADOW_RGBA,0x3f4ccccd);
  text_draw_box(TRAINING_STRING_234,0x42f00000,0x42a00000,0x43c80000,0x42a00000,TEXT_FILL_RGBA,0x3f4ccccd);
  return;
}


/* training_text_box_d */

/* "strings 0xF6 / 0x235" */

/* Draws the two tutorial text boxes of the training screen (TRAINING_STRING_F6, TRAINING_STRING_235).
   Each box is drawn twice: a black copy offset by (+3, +3) that reads as a drop shadow (inferred),
   then the white copy at the base position. The 7th argument (0.8f) is passed unchanged (inferred: alpha / scale).
   Takes no parameters, returns nothing. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void training_text_box_d(void)
{
  /* box 1: TRAINING_STRING_F6 */
  text_draw_box(TRAINING_STRING_F6,TEXT_BOX_SHADOW_X,TEXT_BOX_SHADOW_Y,TEXT_BOX_WIDTH,TEXT_BOX_HEIGHT,TEXT_SHADOW_RGBA,TEXT_BOX_SCALE);
  text_draw_box(TRAINING_STRING_F6,TEXT_BOX_X,TEXT_BOX_Y,TEXT_BOX_WIDTH,TEXT_BOX_HEIGHT,TEXT_FILL_RGBA,TEXT_BOX_SCALE);
  /* box 2: TRAINING_STRING_235 */
  text_draw_box(TRAINING_STRING_235,TEXT_BOX_SHADOW_X,TEXT_BOX_SHADOW_Y_2,TEXT_BOX_WIDTH,TEXT_BOX_HEIGHT,TEXT_SHADOW_RGBA,TEXT_BOX_SCALE);
  text_draw_box(TRAINING_STRING_235,TEXT_BOX_X,TEXT_BOX_Y_2,TEXT_BOX_WIDTH,TEXT_BOX_HEIGHT,TEXT_FILL_RGBA,TEXT_BOX_SCALE);
  return;
}


/* crown_destroy */

/* "RpClumpDestroy(0x64D950)" */

extern void crown_destroy(void);


/* scene_tables_clear */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "fills 0x28 dwords at 0x651198 with -1 and clears 0xE10 dwords at 0x64D958" */

extern void scene_tables_clear(void);


/* aabb_max_axis_distance_to_point */

/* "(box min/max, point): per axis distance to the nearer side, vec3_multiply_components squares,
   returns the max" */

extern double __cdecl aabb_max_axis_distance_to_point (float *param_1,float *param_2,char *param_3,char *param_4,char *param_5, char *param_6,char *param_7,char *param_8,char *param_9, char *param_10,char *param_11,char *param_12);


/* stub_return_zero */

/* "xor eax,eax; ret" - registered as Lua Course_Id and Course_Name (no-ops) and called from many
   unrelated places (identical code folded by the linker) */

extern undefined4 stub_return_zero(void);


/* scene_object_select_set */

/* "stores the selected object and its parameter or clears them when null" */

extern void __cdecl scene_object_select_set (char *param_1,char *param_2,char *param_3,char *param_4,char *param_5);


/* scene_select_timer_clear */

/* "clears g_scene_select_timer" */

extern void scene_select_timer_clear(void);


/* scene_object_select_is_active */

/* "tests the selection timer against the object's value at +0xC" */

extern undefined4 scene_object_select_is_active(void);

#define FX_RECORD_DWORDS (0x3b)          /* dwords per fx object record (stride of the g_fx_* arrays) */


/* fx_object_timer_subtract */

/* "(object, dt): object +0x5C -= dt" */

extern void __cdecl fx_object_timer_subtract (char *param_1,float param_2,char *param_3,char *param_4,char *param_5);


/* fx_distance_check */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "distance to the first camera path point against a threshold (inferred)" */

extern void fx_distance_check(float param_1);


/* scene_get_fx_651aa0 */

/* "return g_fx_651aa0;" */

extern undefined4 scene_get_fx_651aa0(void);


/* fx_object_get_sphere */

/* "(index, out xyzr): position of the object frame and radius 2.0; returns whether inactive" */

extern bool __cdecl fx_object_get_sphere(char *param_1,undefined4 *param_2);


/* fx_current_object_get_float */

/* "float at +0x1C of the current fx record" */

extern double fx_current_object_get_float(void);


/* course_token_list_find_entry */

/* "walks the course token list for the entry containing the id and returns its index" */

extern int course_token_list_find_entry(char *param_1);


/* player_route_progress_update */

/* "dot of the course record base vector with the car vector, stores progress at +0xAC, increments
   the wrong-way counter at +0xA8 when negative" */

extern void player_route_progress_update(char *param_1,char *param_2);


