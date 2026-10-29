#include "mfl_tier2.h"

#define ANIM_CALLBACK_OFFSET 0xc4
/*0040fdb0*/ extern int get_player_count();
/*0045c7f0*/ extern int course_atomics_render_3();
/*0045c820*/ extern undefined __cdecl course_particle_init_random(undefined4 p0);
/*0045d560*/ extern int course_clump_render_with_physics();
/*0045e440*/ extern int weapon_pickups_clear();
/*0045f330*/ extern undefined __cdecl explosion_trigger_callback(struct RwV3d *p0, float *p1);
/*0045f880*/ extern undefined weapon_fx_explosion_slot_clear(undefined4 p0);
/*0045f8f0*/ extern undefined weapon_drum_drop(undefined4 p0);
/*0045fb00*/ extern int weapon_drum_update_one();
/*004600f0*/ extern undefined weapon_fx_explosion_pool_clear(undefined4 p0);
/*00460500*/ extern undefined weapon_fx_clump_remove_a(undefined4 p0);
/*00460530*/ extern int flashunit_state_clear(undefined4 p0);
/*00460540*/ extern undefined weapon_flashbang_start_effect(undefined4 p0);
/*004605a0*/ extern undefined weapon_flashbang_state_step(undefined4 p0);
/*00460610*/ extern undefined weapon_flashbang_flare_particles(undefined4 p0);
/*00465390*/ extern int weapon_effect_slot_clear();
/*00467a60*/ extern int weapons_update_frame();
/*00467e90*/ extern int course_get_rpworld_00467e90();
/*00472340*/ extern int sfx_play_player_set_at();
/*004726a0*/ extern int sfx_play_at();
/*0047a470*/ extern int car_set_wheel_contact();
/*0047b520*/ extern int car_current_matrix();
/*00481ea0*/ extern int player_marker_set_clump();
/*00481ee0*/ extern int player_marker_set_enabled();
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0);
/*00484680*/ extern int object_set_flag_4();
/*00486710*/ extern int particles_destroy();
/*00486c40*/ extern int particles_flush();
/*00496850*/ extern int trigger_sphere_register();
/*00499b40*/ extern int explosion_decal();
/*0049bd90*/ extern int fx_system_destroy();
/*0049bdd0*/ extern int fx_system_render();
/*0049c120*/ extern int fx_buffers_free();
/*004a5d70*/ extern int object_render();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004b4ba7*/ extern int entry();
/*004b5a47*/ extern int positive();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d1160*/ extern struct RwFrame * RwFrameAddChild(struct RwFrame *p0, struct RwFrame *p1);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d49c0*/ extern RwMatrix * RwMatrixTransform(RwMatrix *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004f44a0*/ extern struct RpWorld * RpWorldAddAtomic(struct RpWorld *p0, struct RpAtomic *p1);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*004f46d0*/ extern struct RpWorld * RpWorldRemoveClump(struct RpWorld *p0, struct RpClump *p1);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*005dd340*/ extern float k_f_0_5;
/*005dd378*/ extern float k_f_0_1;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005e64a4*/ extern float k_f_0;
/*0060d9c4*/ extern undefined g_car_60d9c4;
/*0060d9d8*/ extern undefined4 g_course_player_60d9d8;
/*006a2888*/ extern undefined g_course_6a2888;
/*006a291c*/ extern undefined4 g_course_get_6a291c;
/*006a2950*/ extern float g_course_get_6a2950;
/*006a29a4*/ extern undefined4 g_course_warzone_6a29a4;
/*006a29c0*/ extern undefined4 g_weapon_6a29c0;
/*006a29c4*/ extern undefined g_weapon_mortar_6a29c4;
/*006a29c8*/ extern undefined4 g_weapon_6a29c8;
/*006a29cc*/ extern undefined4 g_weapon_6a29cc;
/*006a29d0*/ extern undefined4 g_weapon_6a29d0;
/*006a2a70*/ extern undefined4 g_weapon_6a2a70;
/*006a2a80*/ extern undefined4 g_weapon_6a2a80;
/*006a4c28*/ extern struct ExplosionEntry g_explosion_queue[100];
/*006a5a38*/ extern undefined4 g_explosion_queue_6a5a38;
/*006a5a40*/ extern undefined g_weapon_6a5a40;
/*006a5a6c*/ extern undefined4 g_weapon_drum_6a5a6c;
/*006a5a78*/ extern undefined4 g_weapon_drum_6a5a78;
/*006a5c60*/ extern undefined g_weapon_6a5c60;
/*006a5c6c*/ extern undefined4 g_weapon_drum_6a5c6c;
/*006a5c70*/ extern undefined4 g_weapon_drum_6a5c70;
/*006a5c74*/ extern undefined g_weapon_drum_6a5c74;
/*006a5c78*/ extern undefined4 g_weapon_drum_6a5c78;
/*006a5c80*/ extern undefined4 g_weapon_drum_6a5c80;
/*006a5d10*/ extern undefined4 g_weapon_6a5d10;
/*006a5d14*/ extern undefined4 g_weapon_drum_6a5d14;
/*006a5d18*/ extern undefined g_weapon_6a5d18;
/*006a5d78*/ extern undefined g_weapon_6a5d78;
/*006a5d84*/ extern undefined4 g_weapon_flashbang_6a5d84;
/*006a5d88*/ extern undefined4 g_weapon_6a5d88;
/*006a5dc8*/ extern undefined4 g_weapon_6a5dc8;
/*006a5e38*/ extern undefined4 g_weapon_rocket_6a5e38;
/*006a5ff4*/ extern undefined g_weapon_rocket_6a5ff4;
/*006a5ff8*/ extern undefined g_weapon_6a5ff8;
/*006a6008*/ extern undefined4 g_weapon_6a6008;
/*006a6010*/ extern undefined g_weapon_rocket_6a6010;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008bf000*/ extern undefined g_weapon_8bf000;
/*008bf050*/ extern undefined4 g_car_8bf050;
/*008bf060*/ extern undefined4 g_course_warzone_8bf060;
/*008d8b30*/ extern undefined4 player_count;
/* anim_set_callback */

/* Sets the callback for an animation object (unknown structure). */
/* "stores the callback at +0xC4" */

void anim_set_callback(char *anim, undefined4 callback)
{
  /* Store callback pointer at offset 0xC4 in animation object */
  *(undefined4 *)(anim + ANIM_CALLBACK_OFFSET) = callback;
  return;
}
