#include "mfl_tier2.h"
/*00405e40*/ extern int stub_return_zero();
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00481fd0*/ extern double random_range();
/*004846e0*/ extern int point_lerp_to_aux_camera();
/*00486710*/ extern int particles_destroy();
/*00486840*/ extern int particles_set_color();
/*00486860*/ extern int gfx_set_fx_matrix();
/*00486a30*/ extern int particle_system_set_value_c0();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*00486ef0*/ extern int particles_render();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*00496ea0*/ extern int __cdecl smplfzx_get_object(undefined4 p0);
/*00496ee0*/ extern int smplfzx_current_object_get_field(undefined4 p0);
/*00496fc0*/ extern undefined __cdecl atomic_bbox_calculate_from_geometry(struct RwBBox *p0, int p1);
/*00497750*/ extern int pool_stack_pop_index();
/*00497820*/ extern int __cdecl pool_create(undefined4 p0);
/*00498770*/ extern int vehicle_part_position_by_index();
/*00498fe0*/ extern undefined fx_decal_stamp_matrix(undefined4 p0, RwReal *p1, float p2, int p3);
/*00499c90*/ extern undefined fx_dust_create(undefined4 p0);
/*0049b780*/ extern int __cdecl particles_alloc_system(struct RwTexture *p0, int p1, int p2, uint p3);
/*0049b860*/ extern int fx_set_blend_modes();
/*0049b870*/ extern int particle_batch_mark_current_active();
/*0049b890*/ extern int particle_batch_add();
/*0049b910*/ extern int particle_batch_set_uv_a();
/*0049b940*/ extern int particle_batch_set_size();
/*0049b960*/ extern int particle_batch_set_uv_b_c();
/*0049b9b0*/ extern int particle_batch_clear();
/*0049bd50*/ extern int fx_system_flush();
/*004a8420*/ extern int debug_printf();
/*004b2b48*/ extern int _ftol();
/*004b4ba7*/ extern int entry();
/*004d1990*/ extern struct RwBBox * RwBBoxInitialize(struct RwBBox *p0, struct RwV3d *p1);
/*004d19d0*/ extern struct RwBBox * RwBBoxAddPoint(struct RwBBox *p0, struct RwV3d *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd330*/ extern float float_1_0;
/*005dd628*/ extern float k_f_0_25;
/*005dd7e4*/ extern float k_f_0_03;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005ddc28*/ extern float k_f_0;
/*005e07f0*/ extern float k_f_0_80999994;
/*005e0850*/ extern undefined g_effects_update_5e0850;
/*005e0858*/ extern undefined g_effects_update_5e0858;
/*0071abf4*/ extern undefined4 g_particles_71abf4;
/*0071c1e0*/ extern undefined g_smplfzx_get_71c1e0;
/*0072db20*/ extern undefined4 g_particles_get_72db20;
/*0072dfd8*/ extern undefined4 g_particles_72dfd8;
/*00745c48*/ extern undefined1 g_particles_745c48;
/*00745ce8*/ extern undefined g_particles_745ce8;
/*00745cec*/ extern undefined g_particles_745cec;
/*00745cf0*/ extern undefined g_particles_745cf0;
/*007519ac*/ extern float g_particles_7519ac;
/*007519b8*/ extern undefined4 g_particles_7519b8;
/*007519e4*/ extern undefined4 g_particles_7519e4;
/*00751a18*/ extern undefined4 g_effects_update_751a18;
/*00751b38*/ extern undefined4 g_effects_update_751b38;
/*00751b54*/ extern undefined g_effects_update_751b54;
/*00751b70*/ extern undefined g_effects_update_751b70;
/*00751b8c*/ extern undefined g_effects_update_751b8c;
/*00751ba8*/ extern undefined g_effects_update_751ba8;
/*00751bc4*/ extern undefined g_race_load_751bc4;
/*00752180*/ extern undefined g_load_752180;
/*00752184*/ extern undefined g_particles_752184;
/*007521c0*/ extern undefined g_sound_7521c0;
/*00752200*/ extern undefined1 g_particles_752200;
/*00752234*/ extern undefined4 g_sound_752234;
/*00752238*/ extern undefined4 g_sound_752238;
/*0075223c*/ extern undefined4 g_sound_75223c;
/*00752244*/ extern undefined4 g_particles_752244;
/*00752248*/ extern undefined4 g_particles_752248;
/*0075224c*/ extern undefined4 g_sound_75224c;
/*00752298*/ extern undefined4 g_particles_752298;
/*0075229c*/ extern undefined4 g_sound_75229c;
/*007522a0*/ extern undefined4 g_sound_7522a0;
/*00752364*/ extern undefined g_race_load_752364;
/*00759630*/ extern undefined4 g_particles_759630;
/*00759640*/ extern undefined g_fx_dust_759640;
/*00759648*/ extern undefined4 g_fx_dust_759648;
/*00759658*/ extern undefined g_fx_dust_759658;
/*00759660*/ extern undefined4 g_fx_dust_759660;
/*00759698*/ extern undefined4 g_fx_ptank_atomic_a;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*007ef7f4*/ extern float g_tick_seconds;
/* particles_get_particles_72db20 */

/* "return g_particles_72db20;" */

extern undefined4 particles_get_particles_72db20(void);


/* smplfzx_get_71c1e0 */

/* "returns &g_particles_71c1e0" */

extern undefined * smplfzx_get_71c1e0(void);


/* smplfzx_check_active */

/* "(index): prints 'SmplFzx : Not an active SmplFzx object %d' when absent" */

extern undefined4 __cdecl smplfzx_check_active(char *param_1,char *param_2,char *param_3);


/* smplfzx_object_get_position */

/* "copies the position vector of the smplfzx object" */

extern undefined4 smplfzx_object_get_position(char *param_1,undefined4 *param_2);


/* atomic_world_bbox_update */

/* "(atomic, bbox): geometry bbox transformed into world space" */

extern void atomic_world_bbox_update(char *param_1,RwBBox *boundBox);

/* object field offsets (from the obj pointer); bbox layout inferred from the copies below */
#define OBJ_BBOX_MIN_Y (0x43c)
#define OBJ_BBOX_MIN_Z (0x440)
#define OBJ_BBOX_MAX_Y (0x448)
#define OBJ_BBOX_MAX_Z (0x44c)


extern undefined4 pool_push(int *pool,undefined4 value);


extern void vec3_make_from_param(char *param_1,char *param_2,undefined4 *param_3);


extern undefined4 __cdecl pool_list_unlink(short *param_1,int param_2,char *param_3,char *param_4);


/* pool_clear */

/* "clears the pool header and its entries" */

extern void __cdecl pool_clear(undefined4 *param_1);


/* fx_pool_create */

/* "g_particles_7519b8 = pool_create(0x500) and fills the free list" */

extern void fx_pool_create(void);


/* fx_pool_alloc_particle */

/* "(out index): pops a free index (0..0x4FF) from g_particles_7519b8 and returns its 0x4C byte
   record" */

extern undefined4 * __cdecl fx_pool_alloc_particle(int *param_1);


/* fx_trail_particle_spawn */

/* "spawns a trail particle (white, type 3), rate limited by distance to the last emission point of
   the emitter slot" */

extern float * fx_trail_particle_spawn(float *param_1,float param_2,float param_3,char *param_4);


/* fx_trail_particle_spawn_interpolated */

/* "copies a template particle and lerps its position along the last emission point" */

extern void fx_trail_particle_spawn_interpolated(float *param_1,float param_2,float param_3,char *param_4);


#define PARTICLE_TABLE_DWORD_COUNT (0x5f00)
#define PARTICLE_POOL_SIZE (0x500)
#define FREE_BITMAP_BYTE_SHIFT (3)
#define FREE_BITMAP_BIT_MASK (7)


/* fx_tables_clear */

/* "clears 0x5F00 dwords at 0x72DFD8" */
/* Resets the particle state: zeroes the particle table at g_particles_72dfd8, empties the
   free slot pool g_particles_7519b8 and refills it with slots 0..0x4FF (inferred), clearing
   each slot's bit in the free bitmap g_particles_745c48 (inferred meaning). Takes no parameters
   and returns nothing. */

/* TIER2: validated 2026-10-18 authority=diffemu */
void fx_tables_clear(void)
{
  byte slot_low_byte;
  int slot;
  int bitmap_byte_index;
  undefined4 *cursor;

  /* zero the particle table, one dword per pass (slot is a countdown here) */
  cursor = &g_particles_72dfd8;
  for (slot = PARTICLE_TABLE_DWORD_COUNT; slot != 0; slot = slot + -1) {
    *cursor = 0;
    cursor = cursor + 1;
  }
  slot = 0;
  pool_clear(g_particles_7519b8);
  /* push every slot 0..PARTICLE_POOL_SIZE-1 into the free pool and clear its bit in the bitmap
     (inferred: a cleared bit means free) */
  do {
    pool_push(g_particles_7519b8,slot);
    bitmap_byte_index = slot >> FREE_BITMAP_BYTE_SHIFT;
    slot_low_byte = (byte)slot;
    slot = slot + 1;
    (&g_particles_745c48)[bitmap_byte_index] = (&g_particles_745c48)[bitmap_byte_index] & ~('\x01' << (slot_low_byte & FREE_BITMAP_BIT_MASK));
  } while (slot < PARTICLE_POOL_SIZE);
  return;
}


/* fx_explosion_systems_init */

/* "for 4 systems loads exp_cloud2, sets blend modes 5/2" */

extern void fx_explosion_systems_init(void);


/* fx_explosion_spawn */

/* "spawns an explosion in system param_1 slot param_2 (4 sub puffs)" */

extern void fx_explosion_spawn(int param_1,int param_2,char *param_3,char *param_4);


/* fx_trail_batches_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per slot ages the trails, shifts the position history and writes the 4-point particle batch with
   fading alpha" */

extern void fx_trail_batches_update(void);

#define PARTICLE_SYSTEM_COUNT (4)
#define PARTICLE_SYSTEM_RECORD_STRIDE (0x7a) /* in dwords */
#define ENTRIES_PER_SYSTEM (7)


/* effects_reset_four_systems */

/* "resets 4 particle system records and calls particle_batch_clear (0x49b9b0, was particles_fn_49b9b0)" */
/* Resets the particle system records from g_particles_752184 down to g_particles_7519e4: each
   record gets its system index (inferred), its batch is cleared, and its entry words are zeroed. */

/* TIER2: validated 2026-10-18 authority=diffemu */
void effects_reset_four_systems(void)
{
  undefined4 *cursor;
  int entries_left;
  int system_index;
  undefined4 *next_record;
  
  system_index = PARTICLE_SYSTEM_COUNT;
  cursor = (undefined4 *)&g_particles_752184;
  /* one pass per system record, walking down; stops when the next record is the end sentinel */
  do {
    next_record = cursor + -PARTICLE_SYSTEM_RECORD_STRIDE;
    system_index = system_index + -1;
    cursor[-2] = 0;
    cursor[-0x7b] = system_index;
    particle_batch_clear(cursor[-0x7a]);
    entries_left = ENTRIES_PER_SYSTEM;
    cursor = cursor + -0x10;
    /* zeroes the entry words of this record, ENTRIES_PER_SYSTEM passes (inferred) */
    do {
      entries_left = entries_left + -1;
      cursor[-8] = 0;
      cursor[-1] = 0;
      cursor[6] = 0;
      cursor[0xd] = 0;
      cursor[-0xf] = 0;
      cursor = cursor + -1;
    } while (entries_left != 0);
    cursor = next_record;
  } while (next_record != &g_particles_7519e4);
  return;
}


/* fx_ring_add_point ring: entries of RING_ENTRY_DWORDS dwords, live flags in a bitmap */
#define RING_LAST_INDEX (0xff)
#define RING_ENTRY_DWORDS (0x1d)
#define RING_BITMAP_BYTE_SHIFT (3)
#define RING_BITMAP_BIT_MASK (7)
#define RING_ENTRY_COUNT (0x100)
/* float bit patterns passed to random_range: 0.6f, 1.3f */


/* fx_ring_add_point */

/* "(pos*, value): adds a marker to the 256 entry ring at 0x752230 with a random 0.6..1.3 scale" */
/* Adds one marker to the ring of RING_LAST_INDEX + 1 entries at the write index g_particles_759630.
   pos is a 3 dword vector, copied into the entry; value and a random scale (0.6..1.3) are stored
   after it, and the entry's live bit is set. Returns nothing. Entry layout and the pos pointer
   stored in the entry are inferred. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
typedef struct { undefined4 a, b, c; } V3x;
extern void fx_ring_add_point(undefined4 *pos,undefined4 value);


/* fx_spawn_from_matrix_2 */

/* "particles_fn_498fe0(matrix, ?, 2)" */

extern void fx_spawn_from_matrix_2(RwReal *param_1,char *param_2);


/* fx_ring_clear */

/* "clears the 256 entry ring" */
/* Empties the fx_ring_add_point ring: clears the live bit of every entry and zeroes the
   per-entry dwords, then resets the write index g_particles_759630 to 0. Takes no parameters
   and returns nothing. Entry layout inferred from fx_ring_add_point. */

/* TIER2: validated 2026-10-18 authority=diffemu */
void fx_ring_clear(void)
{
  byte live_bits;
  undefined4 *entry_end;
  undefined4 *entry;
  int slot;

  /* walk the entries from the last one (slot 255) down to entry 0 at g_particles_752298;
     entry_end starts one past the last entry and trails each entry's base (inferred) */
  slot = RING_ENTRY_COUNT;
  entry_end = &g_fx_ptank_atomic_a;
  do {
    slot = slot + -1;
    entry = entry_end - RING_ENTRY_DWORDS;
    live_bits = (&g_particles_752200)[slot >> RING_BITMAP_BYTE_SHIFT];
    entry_end[-0x1c] = 0;
    *entry = 0;
    entry_end[-0x35] = 0;
    entry_end[-0x36] = 0;
    entry_end[-0x37] = 0;
    entry_end[-0x32] = 0;
    entry_end[-0x34] = 0;
    entry_end[-0x31] = 0;
    entry_end[-0x30] = 0;
    (&g_particles_752200)[slot >> RING_BITMAP_BYTE_SHIFT] = live_bits & ~('\x01' << ((byte)slot & RING_BITMAP_BIT_MASK));
    entry_end = entry;
  } while (entry != &g_particles_752298);
  g_particles_759630 = 0;
  return;
}


/* load_scorch_decal */

/* scorch */

extern void load_scorch_decal(void);


/* fx_dust_destroy */

/* "particles_destroy(&g_particles_759658)" */

extern void fx_dust_destroy(void);


/* fx_dust_reset_count */

/* "g_particles_759648 = 0" */

extern void fx_dust_reset_count(void);


/* fx_dust_add */

/* "adds one dust quad when the pool has room (alpha in the high byte)" */

extern void fx_dust_add(char *param_1,byte param_2);


/* fx_dust_flush */

/* "particles_flush(&g_particles_759658)" */

extern void fx_dust_flush(void);


/* fx_dust_render */

/* "culling mode 1 and z write off, renders the dust particles" */

extern void fx_dust_render(void);


/* fx_dust_render_0049a930 */

/* duplicate name: was fx_dust_render (first address 0049a8c0) */

extern void fx_dust_render_0049a930(void);


/* fx_dust_create_default */

/* "particles_fn_499c90(&g_particles_759640)" */

extern void __cdecl fx_dust_create_default(char *param_1,char *param_2,char *param_3);


/* RpPTankAtomicLock_00545d10 */

extern undefined4 RpPTankAtomicLock_00545d10(char *param_1,int *param_2,char *param_3,char *param_4);


/* RpPTankAtomicUnlock_00545f20 */

extern void RpPTankAtomicUnlock_00545f20(char *param_1);


