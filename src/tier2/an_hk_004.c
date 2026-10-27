#include "mfl_tier2.h"
/*0042b110*/ extern int current_course_id();
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*004846b0*/ extern int alloc_zeroed();
/*00486710*/ extern int particles_destroy();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486c40*/ extern int particles_flush();
/*00486ef0*/ extern int particles_render();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*00497fa0*/ extern int fx_trail_particle_spawn();
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*0049ae90*/ extern int fx_particle_style_pulse(undefined4 p0, undefined4 p1);
/*0049b9c0*/ extern int fx_ribbon_flush_spline();
/*0049c520*/ extern undefined particles_emit_random(undefined4 p0);
/*0049c950*/ extern undefined fx_object_apply_transform(undefined4 p0);
/*0049cc20*/ extern undefined __cdecl fx_object_update_frame(undefined4 p0, undefined4 p1);
/*0049cd60*/ extern undefined fx_spawn_burst(undefined4 p0, float *p1, int p2, float *p3, undefined4 *p4, undefined4 *p5, float p6);
/*0049f440*/ extern int fx_style_init_a();
/*0049f470*/ extern int fx_style_init_b();
/*0049f4a0*/ extern int fx_style_init_c();
/*0049f4d0*/ extern int fx_style_init_d();
/*0049f590*/ extern int fx_style_init_e();
/*0049f5d0*/ extern int fx_style_init_f();
/*0049f600*/ extern int fx_style_init_g();
/*0049f640*/ extern int fx_style_init_h();
/*0049f680*/ extern undefined __cdecl fx_style_init_i(undefined4 p0);
/*004a8420*/ extern int debug_printf();
/*004b322b*/ extern int exit();
/*004b4ba7*/ extern int entry();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004d5bf0*/ extern RwBool RwTextureDestroy(struct RwTexture *p0);
/*004d5e40*/ extern struct RwTexture * RwTextureRead(RwChar *p0, RwChar *p1);
/*004ddb80*/ extern RwChar * RwImageSetPath(RwChar *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8230*/ extern struct RpMaterial * RpMaterialSetTexture(struct RpMaterial *p0, struct RwTexture *p1);
/*00545170*/ extern RpAtomic *RpPTankAtomicCreate();
/*00545d00*/ extern RwBool RpAtomicDestroy_00545d00(struct RpAtomic *p0);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd620*/ extern float float_10_0;
/*005dd634*/ extern float k_f_2;
/*005ddb24*/ extern float k_f_150;
/*005ddc28*/ extern float k_f_0;
/*0062a940*/ extern float g_race_update_62a940;
/*0062a944*/ extern float g_race_update_62a944;
/*0062a948*/ extern float g_race_update_62a948;
/*0062a94c*/ extern float g_race_update_62a94c;
/*0062ac2c*/ extern float g_app_62ac2c;
/*0062ac30*/ extern float g_app_62ac30;
/*0062ac34*/ extern undefined4 g_app_62ac34;
/*0062ac38*/ extern undefined4 g_app_62ac38;
/*0062ac3c*/ extern undefined4 g_app_62ac3c;
/*0062ac40*/ extern undefined4 g_app_62ac40;
/*0062ac44*/ extern undefined4 g_app_62ac44;
/*0062ac48*/ extern undefined4 g_app_62ac48;
/*0062ac4c*/ extern undefined4 g_app_62ac4c;
/*0062ac50*/ extern undefined4 g_app_62ac50;
/*0062ac54*/ extern undefined4 g_app_62ac54;
/*0062ac58*/ extern undefined4 g_app_62ac58;
/*0062ac5c*/ extern undefined4 g_app_62ac5c;
/*0062ac60*/ extern undefined4 g_app_62ac60;
/*0062c5d0*/ extern undefined1 g_app_62c5d0;
/*0062c5d1*/ extern undefined1 g_app_62c5d1;
/*0062c5d2*/ extern undefined1 g_app_62c5d2;
/*0062c5d3*/ extern undefined1 g_app_62c5d3;
/*0062c5d4*/ extern undefined g_ui_colour_62c5d4;
/*0062c5d5*/ extern undefined g_ui_colour_62c5d5;
/*0062c5d6*/ extern undefined g_ui_colour_62c5d6;
/*0062c5d7*/ extern undefined g_ui_colour_62c5d7;
/*00759698*/ extern undefined4 g_fx_ptank_atomic_a;
/*0075969c*/ extern undefined4 g_fx_ptank_atomic_b;
/*007596a0*/ extern undefined4 g_particles_7596a0;
/*007596a4*/ extern undefined4 g_particles_7596a4;
/*00759700*/ extern undefined4 g_fx_system_759700;
/*00759708*/ extern undefined g_particles_759708;
/*00759760*/ extern undefined4 g_particles_759760;
/*0075bfa8*/ extern undefined g_particles_75bfa8;
/*0075bfac*/ extern undefined g_particles_75bfac;
/*0075bfb0*/ extern undefined g_particles_75bfb0;
/*0075bfb4*/ extern undefined g_particles_75bfb4;
/*0075bfb8*/ extern undefined g_particles_75bfb8;
/*0075bfbc*/ extern undefined g_particles_75bfbc;
/*0075bfc0*/ extern undefined g_particles_75bfc0;
/*0075bfc4*/ extern undefined g_particles_75bfc4;
/*0075bfc8*/ extern undefined g_particles_75bfc8;
/*0075bfcc*/ extern undefined g_particles_75bfcc;
/*0075bfd0*/ extern undefined g_particles_75bfd0;
/*0075bfd4*/ extern undefined g_particles_75bfd4;
/*0075bfd8*/ extern undefined g_particles_75bfd8;
/*0075bfdc*/ extern undefined g_particles_75bfdc;
/*0075c000*/ extern undefined g_particles_75c000;
/*0075cda8*/ extern undefined g_particles_75cda8;
/*0075cdb8*/ extern undefined4 g_particles_75cdb8;
/*0075cdbc*/ extern undefined g_particles_75cdbc;
/*0075cdcc*/ extern undefined g_particles_75cdcc;
/*0075cddc*/ extern undefined4 g_particles_75cddc;
/*0075cde0*/ extern float g_particles_75cde0;
/*0075cde4*/ extern float g_particles_75cde4;
/*0075cde8*/ extern float g_particles_75cde8;
/*0075cdec*/ extern undefined g_particles_75cdec;
/*0075f828*/ extern undefined4 g_particles_75f828;
/*0076be30*/ extern undefined g_particles_76be30;
/*0076be70*/ extern undefined g_particles_76be70;
/*0076bfc0*/ extern undefined4 g_particles_76bfc0;
/*0076bfec*/ extern undefined4 g_load_76bfec;
/*0076dee0*/ extern undefined4 g_particles_76dee0;
/*0076df18*/ extern undefined4 g_fx_ptank_76df18;
/*0076df24*/ extern struct RwTexture *g_particles_76df24;
/*0076df28*/ extern undefined4 g_fx_ptank_76df28;
/*0076df2c*/ extern undefined4 g_fx_ptank_76df2c;
/*0076df38*/ extern undefined g_effects_update_76df38;
/*0076df48*/ extern undefined g_effects_update_76df48;
/*0076df4c*/ extern undefined g_effects_update_76df4c;
/*0076df50*/ extern undefined g_effects_update_76df50;
/*0076df58*/ extern undefined g_effects_update_76df58;
/*0076df60*/ extern undefined g_effects_update_76df60;
/*0076df64*/ extern undefined g_effects_update_76df64;
/*0076df68*/ extern undefined g_effects_update_76df68;
/*0076df6c*/ extern undefined g_effects_update_76df6c;
/*0076df70*/ extern undefined g_effects_update_76df70;
/*0076e3b8*/ extern undefined4 g_app_76e3b8;
/*0076e3bc*/ extern undefined4 g_app_76e3bc;
/*0076e3c0*/ extern undefined4 g_app_76e3c0;
/*0076e3c4*/ extern undefined4 g_app_76e3c4;
/*0076e3c8*/ extern undefined4 g_app_76e3c8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*007ef7f4*/ extern float g_tick_seconds;
/*008869cc*/ extern undefined4 g_particles_8869cc;
/*008885a0*/ extern undefined4 g_particles_8885a0;

#define FLOAT_0_1_BITS (0x3dcccccd) /* IEEE-754 bit pattern of 0.1f */

/* fx_ptank_create */

/* "(texture, ?, count): PTank atomic with its frame and blend modes 2 / 2" */

/*
 * Creates a PTank (particle tank) atomic, attaches the shared fx frame g_fx_ptank_76df28
 * to it and sets its plugin data (blend modes, flag bits, bounding sphere, 0.1f values).
 * apply_texture (inferred): non-zero = also bind the particle texture to the atomic's material.
 * particle_count (inferred): stored in the PTank plugin data at +0x4 and passed to RpPTankAtomicCreate.
 * out_atomic: receives the new RpAtomic *. Returns nothing.
 */
void fx_ptank_create(int apply_texture,undefined4 *out_atomic,undefined4 particle_count)

{
  uint *ptank_flags;
  RpMorphTarget *morph_target;
  char *ptank_data;
  RwTexture *texture;
  RwFrame *frame;
  RpAtomic *atomic;
  
  g_fx_ptank_76df28 = RwFrameCreate();
  /* PTank plugin data: 2 / 2 blend modes and flag bits on the plugin flags word */
  atomic = (RpAtomic *)RpPTankAtomicCreate(particle_count,0x87,0);
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset) + 0xa4) = 2;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset) + 0xa8) = 2;
  ptank_flags = (uint *)(*(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset) + 0x40);
  *ptank_flags = *ptank_flags | 0x10000000;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset) + 0xac) = 1;
  ptank_flags = (uint *)(*(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset) + 0x40);
  *ptank_flags = *ptank_flags | 0x800000;
  frame = g_fx_ptank_76df28;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset) + 4) = particle_count;
  RpAtomicSetFrame(atomic,frame);
  /* Bounding sphere: centred on the origin, radius 100 */
  morph_target = RpGeometry_morphTarget(RpAtomic_geometry(atomic));
  (RpMorphTarget_boundingSphere(morph_target)).center.x = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).center.y = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).center.z = 0.0;
  (RpMorphTarget_boundingSphere(morph_target)).radius = 100.0;
  *(undefined4 *)(*(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset) + 0xac) = 1;
  ptank_data = *(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset);
  *(undefined4 *)(ptank_data + 0xb8) = FLOAT_0_1_BITS;
  *(undefined4 *)(ptank_data + 0xbc) = FLOAT_0_1_BITS;
  ptank_flags = (uint *)(*(char **)(&RpAtomic_object_object_type(atomic) + g_ptank_plugin_offset) + 0x40);
  *ptank_flags = *ptank_flags | 0x4000;
  texture = g_particles_76df24;
  /* Optional texture hookup: set the texture's filter/addressing byte and bind it to the material */
  if (apply_texture != 0) {
    *(undefined1 *)&RwTexture_filterAddressing(g_particles_76df24) = 2;
    RpMaterialSetTexture(*RpGeometry_matList_materials(RpAtomic_geometry(atomic)),texture);
    *out_atomic = atomic;
    return;
  }
  *out_atomic = atomic;
  return;
}
