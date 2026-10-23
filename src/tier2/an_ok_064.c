#include "mfl_tier2.h"
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern void * get_viewport_camera_matrix();
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
/*0076df38*/ extern char *g_effects_update_76df38;
/*0076df48*/ extern char *g_effects_update_76df48;
/*0076df4c*/ extern char *g_effects_update_76df4c;
/*0076df50*/ extern char *g_effects_update_76df50;
/*0076df58*/ extern char *g_effects_update_76df58;
/*0076df60*/ extern char *g_effects_update_76df60;
/*0076df64*/ extern char *g_effects_update_76df64;
/*0076df68*/ extern char *g_effects_update_76df68;
/*0076df6c*/ extern char *g_effects_update_76df6c;
/*0076df70*/ extern char *g_effects_update_76df70;
/*0076e3b8*/ extern undefined4 g_app_76e3b8;
/*0076e3bc*/ extern char *g_app_76e3bc;
/*0076e3c0*/ extern undefined4 g_app_76e3c0;
/*0076e3c4*/ extern undefined4 g_app_76e3c4;
/*0076e3c8*/ extern undefined4 g_app_76e3c8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*007ef7f4*/ extern float g_tick_seconds;
/*008869cc*/ extern undefined4 g_particles_8869cc;
/*008885a0*/ extern undefined4 g_particles_8885a0;


extern void __cdecl fx_ptank_b_lock_and_fill(char *param_1,char *param_2);


extern void fx_ptank_a_lock_and_fill(char *param_1);


extern void fx_ptank_render_b(void);


/* particles_alloc_system */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "finds a free system record (0x68 bytes), sets texture and dimensions" */

extern int * particles_alloc_system(RwTexture *texture,char *param_2,char *param_3,char *param_4);


/* fx_set_blend_modes */

/* "gfx_fn_486b00 (sets src / dest blend of the PTank)" */

extern void fx_set_blend_modes(void);


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

extern void particle_batch_set_size(int *param_1,char *param_2);


/* particle_batch_set_uv_b_c */

/* "stores 2+2 floats into the current entry" */

extern void particle_batch_set_uv_b_c(int *param_1,undefined4 *param_2,undefined4 *param_3);


/* particle_batch_clear */

/* "zeroes fields +8, +0xC, +0x54" */

extern void particle_batch_clear(char *param_1);


/* fx_system_flush */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "flushes the pending particles of the system and stores its count" */

extern void fx_system_flush(char *param_1);


/* fx_system_render */

/* "renders the particles of the system and clears the active flag" */

extern void fx_system_render(char *param_1);


/* fx_system_create */

/* "allocates an fx system (three buffers, slot via particles_fn_49b780)" */

extern undefined4 * fx_system_create(char *param_1,RwTexture *texture,char *param_3,char *param_4,undefined4 *param_5, char *param_6,char *param_7);


/* fx_system_reset_positions */

/* "sets every vertex position to the given point and clears the offsets" */

extern void fx_system_reset_positions(int *param_1,undefined4 *param_2);


/* particles_slot_set_vectors */

/* "stores three vectors into slot (system, index) (max 16 each)" */

extern undefined4 particles_slot_set_vectors (char *param_1,undefined4 *param_2,char *param_3,undefined4 *param_4, undefined4 *param_5);


/* fx_slot_table_clear */

/* "clears 0x200 entries of the per player table" */

extern void __cdecl fx_slot_table_clear(char *param_1);


/* fx_snow_ptank_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "spawns/updates the particles of system n near the camera, ages them (0x80 steps) and locks the
   ptank" */

extern void fx_snow_ptank_update (char *param_1,RwCamera *camera,undefined1 *param_3,float param_4,float param_5);


/* particle_slot_call_render_method */

/* "calls virtual method 0x48 of the slot object when both tables are set" */

extern void particle_slot_call_render_method(char *param_1);


/* fx_objects_clear */

/* "clears the object table at 0x75F828" */

extern void fx_objects_clear(void);


/* fx_objects_update_all */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "updates each active fx object and fades the block" */

extern void fx_objects_update_all(void);


/* fx_spawn_burst_default_scale */

/* "fx_spawn_burst(..., 1.0f)" */

extern void fx_spawn_burst_default_scale (float *param_1,char *param_2,char *param_3,undefined4 *param_4, undefined4 *param_5,float param_6);


/* fx_systems_destroy_all */

/* "particles_destroy of the systems at 0x76BE30 / 0x76BE70 / 0x76BFC0" */

extern void __fastcall fx_systems_destroy_all(char *param_1);


/* fx_systems_render_all */

/* "z write off, renders the systems, then the slots at 0x76DEE0" */

extern void fx_systems_render_all(void);


/* fx_texture_destroy */

/* "RwTextureDestroy(g_particles_76df24)" */

extern void fx_texture_destroy(void);
extern char g_fx_ptank_76df2c[];

#define FX_STYLE_COUNT (0x11)
#define FX_STYLE_RECORD_SIZE (0x44)
#define FX_STYLE_TABLE_BASE (((int)&g_fx_ptank_76df2c + 0x4))


/* fx_styles_init_all */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "initialises the 17 fx style records (0x44 bytes) with the common defaults and the per type style
   init" */

/* TIER2: validated 2026-10-22 authority=diffemu */
void fx_styles_init_all(void)
{
  int record_offset;
  int record_index;
  int style_no;

  /* walks the records from the last to the first: style_no is 1-based, record_index is the
     0-based record. Each record is FX_STYLE_RECORD_SIZE bytes at FX_STYLE_TABLE_BASE, the field
     at +0x00 is the style value handed to the per type init (inferred) */
  style_no = FX_STYLE_COUNT;
  do {
    record_index = style_no + -1;
    record_offset = record_index * FX_STYLE_RECORD_SIZE;
    *(undefined4 *)((char *)&g_effects_update_76df58 + record_offset) = 0x3eb33333;
    *(undefined4 *)(record_offset + ((char *)&g_effects_update_76df58 + 0x4)) = 0x3eb33333;
    *(undefined4 *)((char *)&g_effects_update_76df60 + record_offset) = 1;
    *(undefined4 *)((char *)&g_effects_update_76df68 + record_offset) = 0x19;
    *(undefined4 *)((char *)&g_effects_update_76df6c + record_offset) = 0x3f800000;
    *(undefined4 *)((char *)&g_effects_update_76df70 + record_offset) = 0x3ecccccd;
    *(undefined4 *)((char *)&g_effects_update_76df48 + record_offset) = 0;
    *(undefined4 *)((char *)&g_effects_update_76df4c + record_offset) = 0xbf800000;
    *(undefined4 *)((char *)&g_effects_update_76df50 + record_offset) = 0;
    *(undefined4 *)((char *)&g_effects_update_76df64 + record_offset) = 0x1102;
    /* per style type init (inferred): style numbers without a case get no init call */
    switch(style_no) {
    case 2:
      fx_style_init_j(record_offset + FX_STYLE_TABLE_BASE);
      break;
    case 7:
      fx_style_init_d(record_offset + FX_STYLE_TABLE_BASE);
      break;
    case 8:
      fx_style_init_a(record_offset + FX_STYLE_TABLE_BASE);
      break;
    case 9:
      fx_style_init_a(record_offset + FX_STYLE_TABLE_BASE);
      break;
    case 10:
      fx_style_init_b(record_offset + FX_STYLE_TABLE_BASE);
      break;
    case 0xb:
      fx_style_init_c(record_offset + FX_STYLE_TABLE_BASE);
      break;
    case 0xd:
      fx_style_init_i(record_offset + FX_STYLE_TABLE_BASE);
      break;
    case 0xe:
      fx_style_init_e(record_offset + FX_STYLE_TABLE_BASE);
      break;
    case 0xf:
      fx_style_init_f(record_offset + FX_STYLE_TABLE_BASE);
      break;
    case 0x10:
      fx_style_init_g(record_offset + FX_STYLE_TABLE_BASE);
      break;
    case 0x11:
      fx_style_init_h(record_offset + FX_STYLE_TABLE_BASE);
    }
    /* +0x08 = 1.0f / style value (inferred: a reciprocal of the value set by the init) */
    *(float *)((char *)&g_effects_update_76df38 + record_offset) = float_1_0 / *(float *)(record_offset + FX_STYLE_TABLE_BASE);
    style_no = record_index;
  } while (record_index != 0);
  return;
}


/* fx_smoke_particle_spawn_grey */

/* "spawns a grey (0x20) type 6 particle with scale 1.4" */

extern void __cdecl fx_smoke_particle_spawn_grey (char *param_1,char *param_2,char *param_3,char *param_4, char *param_5,char *param_6,char *param_7);


/* fx_smoke_particle_spawn_tinted */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "spawns a type 6 particle with a given colour and size+1" */

extern void fx_smoke_particle_spawn_tinted (char *param_1,char *param_2,undefined4 *param_3,char *param_4, float param_5);


/* line_rain_error */

/*  */

extern void __cdecl line_rain_error(char *param_1,char *param_2);


/* ui_colour_a_default */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "g_app_62c5d0..d3 = (0x78, 0x80, 0x88, 0x40)" */

extern void ui_colour_a_default(void);

/* rain drop tuning: drops live in blocks of DROPS_PER_BLOCK, one block per spawn-table slice */
#define DROPS_PER_BLOCK (224)
#define DROP_BLOCK_BYTES (0x1c00)
#define DROP_ARRAY_BYTES (0x7000)
#define SPAWN_POINT_BYTES (12)
#define MAX_DROP_AGE (7)


/* rain_drops_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "moves 4 x 224 rain drops by the wind vector and respawns them relative to the camera every 8
   ticks" */

/* Per-frame update of all rain drops. Takes no parameters and returns nothing.
   Each drop is moved by its wind step (normalized velocity scaled by g_app_62ac30) and has an
   age counter in ticks. When the age passes MAX_DROP_AGE the drop respawns at a spawn point from
   the g_app_76e3b8 table, placed through the viewport camera matrix so rain falls around the camera.
   Does nothing when g_app_76e3c0 is 0 (inferred: rain disabled). */

/* TIER2: validated 2026-10-18 authority=diffemu */
void rain_drops_update(void)
{
  char *spawn_ptr;
  float spawn_x;
  float view_scale_x;
  float view_scale_y;
  float scaled_spawn_z;
  float scaled_spawn_y;
  float *camera_matrix;
  int camera;
  int spawn_base_index;
  float *drop;
  int block_offset;
  RwV3d wind_step;

  spawn_base_index = 0;
  get_viewport_camera(0);
  camera_matrix = (float *)get_viewport_camera_matrix(0);
  camera = get_viewport_camera(0);
  view_scale_x = *(float *)(camera + 0x68);
  view_scale_y = *(float *)(camera + 0x6c);
  if (g_app_76e3c0 != 0) {
    block_offset = 0;
    /* outer loop: one pass per block of DROPS_PER_BLOCK drops; block_offset is the byte offset
       of the block in the drop array, spawn_base_index the first spawn point index of the block */
    do {
      drop = (float *)(block_offset + g_app_76e3bc);
      camera = 0;
      /* inner loop: every drop in the block. camera is reused here as the drop index within the
         block (it held the viewport camera pointer above) */
      do {
        /* move the drop by its wind step (drop + 4 holds its velocity, inferred) */
        RwV3dNormalize(&wind_step,(RwV3d *)(drop + 4));
        wind_step.x = g_app_62ac30 * wind_step.x;
        wind_step.y = g_app_62ac30 * wind_step.y;
        wind_step.z = g_app_62ac30 * wind_step.z;
        *drop = *drop - wind_step.x;
        drop[1] = drop[1] - wind_step.y;
        *(short *)(drop + 3) = *(short *)(drop + 3) + 1;
        drop[2] = drop[2] - wind_step.z;
        spawn_ptr = g_app_76e3b8;
        /* respawn: age passed MAX_DROP_AGE, reset the age and place the drop at a spawn point
           transformed by the camera matrix (position in drop + 0..2) */
        if (MAX_DROP_AGE < *(ushort *)(drop + 3)) {
          *(undefined2 *)(drop + 3) = 0;
          spawn_x = *(float *)(spawn_ptr + (camera + spawn_base_index) * SPAWN_POINT_BYTES);
          spawn_ptr = spawn_ptr + (camera + spawn_base_index) * SPAWN_POINT_BYTES;
          scaled_spawn_y = view_scale_y * *(float *)(spawn_ptr + 4);
          scaled_spawn_z = view_scale_x * *(float *)(spawn_ptr + 8);
          *drop = scaled_spawn_z * *camera_matrix + spawn_x * camera_matrix[8] + scaled_spawn_y * camera_matrix[4] + camera_matrix[0xc];
          drop[1] = spawn_x * camera_matrix[9] + scaled_spawn_y * camera_matrix[5] + scaled_spawn_z * camera_matrix[1] + camera_matrix[0xd];
          drop[2] = spawn_x * camera_matrix[10] + scaled_spawn_y * camera_matrix[6] + scaled_spawn_z * camera_matrix[2] + camera_matrix[0xe];
        }
        drop = drop + 8;
        camera = camera + 1;
      } while (camera < DROPS_PER_BLOCK);
      block_offset = block_offset + DROP_BLOCK_BYTES;
      spawn_base_index = spawn_base_index + DROPS_PER_BLOCK;
    } while (block_offset < DROP_ARRAY_BYTES);
  }
  return;
}


