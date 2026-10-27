#include "mfl_tier2.h"
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
/*0049f500*/ extern int fx_style_init_j();
/*0049f590*/ extern int fx_style_init_e();
/*0049f5d0*/ extern int fx_style_init_f();
/*0049f600*/ extern int fx_style_init_g();
/*0049f640*/ extern int fx_style_init_h();
/*0049f680*/ extern undefined __cdecl fx_style_init_i(undefined4 p0);
/*004a8420*/ extern int debug_printf();
/*004b322b*/ extern int exit();
/*004b4ba7*/ extern int entry();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1330*/ extern struct RwFrame * RwFrameRemoveChild(struct RwFrame *p0);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004d5bf0*/ extern RwBool RwTextureDestroy(struct RwTexture *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
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
/*0076df24*/ extern undefined4 g_particles_76df24;
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
/* fx_ptank_b_lock_and_fill */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "locks ptank atomic b and writes position/size/colour/uv for each active fx entry (two vertices
   per entry)" */

extern void __cdecl fx_ptank_b_lock_and_fill(int param_1,undefined4 param_2);


/* fx_ptank_a_lock_and_fill */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "locks ptank atomic a and writes the per-entry particles (position, size by style, colour, uv)"
    */

extern void fx_ptank_a_lock_and_fill(int param_1);


/* fx_ptank_render_b */

/* "renders g_particles_75969c" */

extern void fx_ptank_render_b(void);


/* fx_slots_clear */

/* "clears every 0x68 byte slot from 0x75C000" */

extern void fx_slots_clear(void);


/* particles_alloc_system */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "finds a free system record (0x68 bytes), sets texture and dimensions" */

extern int * particles_alloc_system(RwTexture *texture,int param_2,int param_3,uint param_4);


/* fx_set_blend_modes */

/* "gfx_fn_486b00 (sets src / dest blend of the PTank)" */

extern void fx_set_blend_modes(undefined4 p0, undefined4 p1, undefined4 p2);


/* particle_batch_mark_current_active */

/* "increments the count and sets 1 in the current 0x30 byte entry" */

extern void particle_batch_mark_current_active(int *param_1);


/* particle_batch_add */

/* "appends a particle (position, colour) to the batch if it is not full" */

extern void particle_batch_add(int *param_1,undefined4 *param_2,undefined1 *param_3);


/* particle_batch_set_uv_a */

/* "stores 2 floats into the current entry" */

extern void particle_batch_set_uv_a(int *param_1,undefined4 *param_2);


/* particle_batch_set_size */

/* "stores a value into the current entry" */

extern void particle_batch_set_size(int *param_1,undefined4 param_2);


/* particle_batch_set_uv_b_c */

/* "stores 2+2 floats into the current entry" */

extern void particle_batch_set_uv_b_c(int *param_1,undefined4 *param_2,undefined4 *param_3);


/* particle_batch_clear */

/* "zeroes fields +8, +0xC, +0x54" */

extern void particle_batch_clear(int param_1);


/* fx_system_flush */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "flushes the pending particles of the system and stores its count" */

extern void fx_system_flush(int param_1);


/* fx_system_render */

/* "renders the particles of the system and clears the active flag" */

extern void fx_system_render(int param_1);


/* fx_system_create */

/* "allocates an fx system (three buffers, slot via particles_fn_49b780)" */

undefined4 *
fx_system_create(int param_1,RwTexture *texture,uint param_3,int param_4,undefined4 *param_5,
                undefined4 param_6,undefined4 param_7)
{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  puVar1 = (undefined4 *)alloc_zeroed(1,0x2c);
  uVar2 = alloc_zeroed(param_1,0xc);
  *puVar1 = uVar2;
  uVar2 = alloc_zeroed(param_1,0xc);
  puVar1[1] = uVar2;
  uVar2 = alloc_zeroed(param_1,4);
  puVar1[2] = uVar2;
  puVar1[3] = param_1;
  puVar1[4] = param_1;
  uVar2 = particles_alloc_system(texture,param_1,param_4,param_3);
  puVar1[10] = uVar2;
  puVar1[5] = *param_5;
  puVar1[6] = param_6;
  puVar1[7] = param_7;
  puVar1[9] = 0;
  fx_set_blend_modes(uVar2,5,6);
  return puVar1;
}


/* fx_system_reset_positions */

/* "sets every vertex position to the given point and clears the offsets" */

extern void fx_system_reset_positions(int *param_1,undefined4 *param_2);


/* particles_slot_set_vectors */

/* "stores three vectors into slot (system, index) (max 16 each)" */

extern undefined4 particles_slot_set_vectors (int param_1,undefined4 *param_2,undefined4 param_3,undefined4 *param_4, undefined4 *param_5);


/* fx_slot_table_clear */

/* "clears 0x200 entries of the per player table" */

extern void __cdecl fx_slot_table_clear(int param_1);


/* fx_snow_ptank_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "spawns/updates the particles of system n near the camera, ages them (0x80 steps) and locks the
   ptank" */

extern void fx_snow_ptank_update (int param_1,RwCamera *camera,undefined1 *param_3,float param_4,float param_5);


/* particle_slot_call_render_method */

/* "calls virtual method 0x48 of the slot object when both tables are set" */

extern void particle_slot_call_render_method(int param_1);


/* fx_objects_clear */

/* "clears the object table at 0x75F828" */

extern void fx_objects_clear(void);


/* fx_objects_update_all */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "updates each active fx object and fades the block" */

extern void fx_objects_update_all(void);


/* fx_spawn_burst_default_scale */

/* "fx_spawn_burst(..., 1.0f)" */

extern void fx_spawn_burst_default_scale (float *param_1,int param_2,undefined4 param_3,undefined4 *param_4, undefined4 *param_5,float param_6);


/* fx_systems_destroy_all */

/* "particles_destroy of the systems at 0x76BE30 / 0x76BE70 / 0x76BFC0" */

extern void __fastcall fx_systems_destroy_all(undefined4 param_1);


/* fx_systems_render_all */

/* "z write off, renders the systems, then the slots at 0x76DEE0" */

extern void fx_systems_render_all(void);


/* fx_texture_destroy */

/* "RwTextureDestroy(g_particles_76df24)" */

extern void fx_texture_destroy(void);


/* fx_styles_init_all */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "initialises the 17 fx style records (0x44 bytes) with the common defaults and the per type style
   init" */

extern void fx_styles_init_all(void);


/* fx_smoke_particle_spawn_grey */

/* "spawns a grey (0x20) type 6 particle with scale 1.4" */

extern void __cdecl fx_smoke_particle_spawn_grey (undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4, undefined4 param_5,undefined4 param_6,undefined4 param_7);


/* fx_smoke_particle_spawn_tinted */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "spawns a type 6 particle with a given colour and size+1" */

extern void fx_smoke_particle_spawn_tinted (undefined4 param_1,undefined4 param_2,undefined4 *param_3,undefined4 param_4, float param_5);


/* line_rain_error */

/*  */

extern void __cdecl line_rain_error(undefined4 param_1,undefined4 param_2);


/* ui_colour_a_default */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "g_app_62c5d0..d3 = (0x78, 0x80, 0x88, 0x40)" */

extern void ui_colour_a_default(void);


/* rain_drops_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "moves 8 x 224 rain drops by the wind vector and respawns them relative to the camera every 8
   ticks" */

extern void rain_drops_update(void);



