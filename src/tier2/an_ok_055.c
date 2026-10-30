#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*004846b0*/ extern int alloc_zeroed();
/*00484830*/ extern undefined __cdecl material_uv_anim_check(struct RpMaterial *p0, undefined4 *p1);
/*004848b0*/ extern undefined __cdecl clump_for_all_materials(struct RpClump *p0, void *p1);
/*00485ae0*/ extern int particle_templates_init();
/*00485b80*/ extern int smoke_emitter_update();
/*00486470*/ extern undefined __cdecl fx_emitter_spawn_a(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*00486530*/ extern undefined __cdecl fx_emitter_spawn_b(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004865f0*/ extern undefined __cdecl fx_emitter_spawn_c(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*00486710*/ extern int particles_destroy();
/*00486840*/ extern int particles_set_color();
/*00486860*/ extern int gfx_set_fx_matrix();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*00486ef0*/ extern int particles_render();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*00487360*/ extern undefined effect_sprite_update(undefined4 p0, undefined4 p1);
/*004873d0*/ extern undefined effect_sprite_set_alpha_and_render(undefined4 p0);
/*00487bc0*/ extern int effect_pool_clear();
/*004960e0*/ extern int particle_system_set_current();
/*004a8420*/ extern int debug_printf();
/*004b2b48*/ extern int _ftol();
/*004b322b*/ extern int exit();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd340*/ extern float k_f_0_5;
/*005e64a4*/ extern float k_f_0;
/*00629460*/ extern undefined4 g_gfx_629460;
/*00629468*/ extern undefined4 g_gfx_629468;
/*0062946c*/ extern undefined4 g_gfx_62946c;
/*0062947c*/ extern undefined4 g_gfx_62947c;
/*006294a8*/ extern undefined4 g_gfx_6294a8;
/*006c40a4*/ extern int g_particles_6c40a4;
/*006c40a8*/ extern char *g_particles_6c40a8;
/*006c40ac*/ extern undefined4 g_particles_6c40ac;
/*006c40b0*/ extern undefined4 g_particles_6c40b0;
/*006c40b4*/ extern char *g_particles_6c40b4;
/*006c40b8*/ extern undefined4 g_gfx_6c40b8;
/*006c40c0*/ extern undefined g_particles_6c40c0;
/*006c4100*/ extern undefined g_particles_6c4100;
/*006c4210*/ extern undefined4 g_effects_update_6c4210;
/*006c5000*/ extern undefined g_particles_6c5000;
/*006c5040*/ extern undefined g_particles_6c5040;
/*006c5080*/ extern undefined g_particles_6c5080;
/*006c50c0*/ extern undefined4 g_particles_6c50c0;
/*006c50c8*/ extern float g_gfx_6c50c8;
/*006c5148*/ extern undefined4 g_gfx_6c5148;
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c51b0*/ extern undefined g_gfx_6c51b0;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/*006c57f8*/ extern undefined g_gfx_6c57f8;
/*006c5e10*/ extern undefined4 g_gfx_6c5e10;
/*006c5e18*/ extern undefined4 g_gfx_6c5e18;
/*006c5e20*/ extern undefined g_gfx_6c5e20;
/*006c5e3c*/ extern char *g_shockwave_load_6c5e3c;
/*006c60c0*/ extern char *g_gfx_6c60c0;
/*006c60c4*/ extern char *g_gfx_6c60c4;
/*006c60c8*/ extern char *g_gfx_6c60c8;
/*006c60cc*/ extern undefined4 g_gfx_6c60cc;
/*006c60d0*/ extern undefined4 g_gfx_6c60d0;
/*006c60d4*/ extern undefined4 g_race_render_6c60d4;
/*006c60d8*/ extern undefined4 g_race_update_6c60d8;
/*006c60dc*/ extern undefined4 g_race_update_6c60dc;
/*006f1e20*/ extern undefined1 g_gfx_6f1e20;
/*006f2ab8*/ extern undefined4 g_gfx_6f2ab8;
/*006f2abc*/ extern float g_spring_grid_6f2abc;
/*006f2ac0*/ extern undefined4 g_spring_grid_6f2ac0;
/*006f2ac4*/ extern undefined4 g_spring_grid_6f2ac4;
/*006f2ac8*/ extern undefined4 g_spring_grid_6f2ac8;
/*006f2acc*/ extern float g_spring_grid_6f2acc;
/*006f2ad0*/ extern float g_spring_grid_6f2ad0;
/*006f2ad4*/ extern undefined4 g_spring_grid_6f2ad4;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*007ef7f4*/ extern float g_tick_seconds;
/* clump_has_uv_anim */

/* "true when any material of the clump has a UV animation" */

extern undefined4 clump_has_uv_anim(RpClump *clump);


/* particle_pools_init */

/* "(count): allocates the pool tables g_particles_6c40ac / a8 / b4" */

extern void particle_pools_init(char *param_1);


/* particle_pool_add */

/* "(texture, count): particles_create(0x80c) + record allocation, g_particles_6c40b0++" */

extern void particle_pool_add(RwTexture *texture,char *param_2);

/* byte stride between the pools' handle bases (particle_pools_update_and_draw) */
#define PARTICLE_POOL_STRIDE (0x40)
/* byte offset from a pool's block to its first particle record's alive flag (inferred) */
#define POOL_PARTICLE_FLAG_OFFSET (0x48)
/* dword stride between particle records in a pool block (0x50 bytes) */
#define PARTICLE_STRIDE_DWORDS (0x14)


/* particle_pools_update_and_draw */

/* "ages every particle, fades alpha towards its end time, draws and frees expired ones" */

/* Takes no parameters, returns nothing. Walks every particle pool (g_particles_6c40b0 of them): per pool it
   ages each live particle by g_tick_seconds, fades it by its scaled age, queues it for drawing with particles_add
   and flushes the pool. A particle whose scaled age passes its lifetime is marked dead and its pool's alive count drops. */

/* TIER2: validated 2026-10-18 authority=diffemu */
void particle_pools_update_and_draw(void)
{
  int pool_handle;
  int *alive_count_slot;
  int scaled_age;
  int scratch;      /* reused: alive-count table base, then the particle's peak scaled age (inferred) */
  int *particle;
  int pool_index;
  int pool_offset;
  int particles_left;
  float age_scale;      /* diffemu: the original ages the particle with ftol(t * t * k * k), k = g_gfx_6c40b8[pool] / count; the decompile had `_ftol()` without argument */
  float particle_age;

  pool_index = 0;
  /* per pool: pool_handle = handle base of the pool, age_scale = alive count / particle count (inferred) */
  if (0 < (int)g_particles_6c40b0) {      /* diffemu: undefined4 global, the original compares signed */
    pool_offset = 0;
    do {
      particles_left = *(int *)(g_particles_6c40b4 + pool_index * 4);
      pool_handle = pool_offset + g_particles_6c40ac;
      age_scale = (float)*(int *)(g_gfx_6c40b8 + pool_index * 4) / (float)particles_left;
      if (0 < particles_left) {
        particle = (int *)(*(char **)(g_particles_6c40a8 + pool_index * 4) + POOL_PARTICLE_FLAG_OFFSET);
        scratch = g_gfx_6c40b8;
        /* per particle record: skip dead ones; live ones age, then either stay alive (drawn) or expire */
        do {
          if (*particle != 0) {
            particle_age = g_tick_seconds + *(float *)&particle[-1];
            *(float *)&particle[-1] = particle_age;
            scaled_age = (int)(particle_age * particle_age * age_scale * age_scale);
            /* still inside its lifetime: keep the peak age, set the alpha from it, draw the particle */
            if (scaled_age < (short)particle[1]) {
              scratch = (int)*(short *)((char *)particle + 6);
              if (scratch <= scaled_age) {
                *(short *)((char *)particle + 6) = (short)scaled_age;
                scratch = scaled_age;
              }
              *(char *)((char *)particle + -5) = (char)particle[1] - (char)scratch;
              particles_set_color(particle + -2);
              gfx_set_fx_matrix(particle + -0x12);
              particles_add(pool_handle);
              scratch = g_gfx_6c40b8;
            }
            /* lifetime over: mark the particle dead and decrement the pool's alive count */
            else {
              *particle = 0;
              alive_count_slot = (int *)(scratch + pool_index * 4);
              *alive_count_slot = *alive_count_slot + -1;
            }
          }
          particle = particle + PARTICLE_STRIDE_DWORDS;
          particles_left = particles_left + -1;
        } while (particles_left != 0);
      }
      particles_flush(pool_handle);
      pool_index = pool_index + 1;
      pool_offset = pool_offset + PARTICLE_POOL_STRIDE;
    } while (pool_index < (int)g_particles_6c40b0);
  }
  return;
}


/* particles_render_all_systems */

/* "sets g_particles_6c50c0 from the camera, renders the five systems without z write" */

extern void particles_render_all_systems(void);


/* particle_systems_destroy_all */

/* "particles_destroy for every system from 0x6C40C0 (0x40 byte stride)" */

extern void particle_systems_destroy_all(void);


/* smoke_particles_init */

/* "'smoke' texture and particle system" */

extern void smoke_particles_init(void);


/* particle_emitters_update_all */

/* "advances each active emitter (stride 0x3B dwords), spawns via effects_update_frame_sub_485b80,
   ends when time exceeds the duration" */

extern void particle_emitters_update_all(void);


/* fx_emitter_spawn_a_default */

/* "fx_emitter_spawn_a with the default flag" */

extern void fx_emitter_spawn_a_default(char *param_1,char *param_2,char *param_3);


/* fx_emitter_spawn_b_default */

/* "fx_emitter_spawn_b with the default flag" */

extern void fx_emitter_spawn_b_default (char *param_1,char *param_2,char *param_3,char *param_4);


/* fx_emitter_spawn_c_default */

/* "calls fx_emitter_spawn_c with a 0 argument" */

extern void fx_emitter_spawn_c_default (char *param_1,char *param_2,char *param_3,char *param_4);


/* gfx_set_gfx_6c50c8 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "g_gfx_6c50c8 = param_1; return;" */

extern void gfx_set_gfx_6c50c8(undefined4 param_1);

#define PTANK_SIZE_X_OFFSET (0xb8)
#define PTANK_SIZE_Y_OFFSET (0xbc)
#define PTANK_FLAGS_OFFSET (0x40)
#define PTANK_FLAG_SIZE (0x4000)
#define PTANK_COLOUR_OFFSET (0xd8) /* rgba (4 dwords) or colour table (8 dwords), mutually exclusive, flags 0x80000 / 0x100000 */
#define PTANK_PLUGIN_FLAG_COLOUR_TABLE (0x100000)
#define COLOUR_TABLE_DWORDS (8)


/* ptank_set_size */

/* "(ptank atomic, vec2*): size of the PTank particles, flag 0x4000" */

/* Sets the PTank particle size (x, y) in the atomic's ptank plugin data and marks it with PTANK_FLAG_SIZE.
   ptank_atomic: the atomic whose plugin data is written. size: the two floats to copy.
   Returns nothing. */

/* TIER2: validated 2026-10-22 authority=diffemu */
void ptank_set_size(char *ptank_atomic,undefined4 *size)
{
  uint *flags;
  char *ptank_plugin;

  /* NULL size: fall back to the default vector g_gfx_629460 (inferred) */
  if (size == (undefined4 *)0x0) {
    size = &g_gfx_629460;
  }
  ptank_plugin = *(char **)(*(char **)(ptank_atomic + 4) + g_ptank_plugin_offset);
  *(undefined4 *)(ptank_plugin + PTANK_SIZE_X_OFFSET) = *size;
  *(undefined4 *)(ptank_plugin + PTANK_SIZE_Y_OFFSET) = size[1];
  flags = (uint *)(*(char **)(*(char **)(ptank_atomic + 4) + g_ptank_plugin_offset) + PTANK_FLAGS_OFFSET);
  *flags = *flags | PTANK_FLAG_SIZE;
  return;
}


/* particle_system_set_tint */

/* "stores 1 colour (default 0x629468) into the ptank plugin and sets flag 0x2000" */

extern void __cdecl particle_system_set_tint(char *param_1,undefined4 *param_2);


/* number of dwords in a matrix table (4x4 floats, inferred) */
#define MATRIX_TABLE_DWORDS (0x10)
/* offset of the matrix table inside the PTank plugin data */
#define PTANK_MATRIX_TABLE_OFFSET (0xf8)
#define PTANK_PLUGIN_FLAG_MATRIX_TABLE (0x8000)


/* particle_system_set_matrix_table */

/* "copies 16 dwords (default 0x6c5148) to the ptank plugin and sets flag 0x8000" */

/* TIER2: unvalidated 2026-10-11 authority=diffemu */
/* Copies a matrix table (MATRIX_TABLE_DWORDS dwords) into the PTank plugin data of an atomic and sets
   PTANK_PLUGIN_FLAG_MATRIX_TABLE in its flags. ptank_atomic: the atomic whose plugin data is written. matrix_table: the
   source dwords; NULL uses the default table g_gfx_6c5148 (inferred). Returns nothing. */
/* TIER2: unvalidated 2026-10-22 authority=diffemu */
void __cdecl particle_system_set_matrix_table(char *ptank_atomic,undefined4 *matrix_table)
{
  char *dwords_left;
  undefined4 *matrix_dest;

  /* NULL matrix table: fall back to the default table (inferred) */
  if (matrix_table == (undefined4 *)0x0) {
    matrix_table = &g_gfx_6c5148;
  }
  matrix_dest = (undefined4 *)(*(char **)(*(char **)(ptank_atomic + 4) + g_ptank_plugin_offset) + PTANK_MATRIX_TABLE_OFFSET);
  /* copy the table one dword at a time into the plugin data; dwords_left counts down from MATRIX_TABLE_DWORDS */
  for (dwords_left = MATRIX_TABLE_DWORDS; dwords_left != 0; dwords_left = dwords_left + -1) {
    *matrix_dest = *matrix_table;
    matrix_table = matrix_table + 1;
    matrix_dest = matrix_dest + 1;
  }
  /* dwords_left is reused as the plugin data pointer again, to set the flag */
  dwords_left = *(char **)(*(char **)(ptank_atomic + 4) + g_ptank_plugin_offset);
  *(uint *)(dwords_left + PTANK_FLAGS_OFFSET) = *(uint *)(dwords_left + PTANK_FLAGS_OFFSET) | PTANK_PLUGIN_FLAG_MATRIX_TABLE;
  return;
}


/* particle_system_set_value_c0 */

/* "stores a value and sets flag 0x20000" */

extern void particle_system_set_value_c0(char *param_1,char *param_2);

/* PTank flag set by ptank_set_colour when the rgba colour is written (inferred) */
#define PTANK_FLAG_RGBA_COLOUR (0x80000)


/* ptank_set_colour */

/* "(ptank atomic, rgba*): colour of the PTank, default white" */

/* Sets the colour of the PTank particles of an atomic: copies the 4 rgba dwords into the atomic's ptank plugin data at
   PTANK_COLOUR_OFFSET and sets PTANK_FLAG_RGBA_COLOUR (inferred) in its flags. ptank_atomic: the atomic whose
   plugin data is written. rgba: the 4 colour dwords; NULL uses the default colour g_gfx_62946c (white). Returns nothing. */
/* TIER2: validated 2026-10-22 authority=diffemu */
void ptank_set_colour(char *ptank_atomic,undefined4 *rgba)
{
  uint *flags;
  char *ptank_plugin;

  /* NULL colour: fall back to the default white colour */
  if (rgba == (undefined4 *)0x0) {
    rgba = &g_gfx_62946c;
  }
  ptank_plugin = *(char **)(*(char **)(ptank_atomic + 4) + g_ptank_plugin_offset);
  *(undefined4 *)(ptank_plugin + PTANK_COLOUR_OFFSET) = *rgba;
  *(undefined4 *)(ptank_plugin + PTANK_COLOUR_OFFSET + 4) = rgba[1];
  *(undefined4 *)(ptank_plugin + PTANK_COLOUR_OFFSET + 8) = rgba[2];
  *(undefined4 *)(ptank_plugin + PTANK_COLOUR_OFFSET + 0xc) = rgba[3];
  flags = (uint *)(*(char **)(*(char **)(ptank_atomic + 4) + g_ptank_plugin_offset) + PTANK_FLAGS_OFFSET);
  *flags = *flags | PTANK_FLAG_RGBA_COLOUR;
  return;
}


/* particle_system_set_colour_table */

/* "copies 8 dwords (default 0x62947c) and sets flag 0x100000" */

/* TIER2: unvalidated 2026-10-11 authority=diffemu */
/* Copies the colour table (COLOUR_TABLE_DWORDS dwords) into the PTank plugin data of an atomic and sets
   PTANK_PLUGIN_FLAG_COLOUR_TABLE in its flags. ptank_atomic: the atomic whose plugin data is written. colour_table: the
   source dwords; NULL uses the default table g_gfx_62947c (inferred). Returns nothing. */
/* TIER2: unvalidated 2026-10-22 authority=diffemu */
void __cdecl particle_system_set_colour_table(char *ptank_atomic,undefined4 *colour_table)
{
  char *dwords_left;
  undefined4 *colour_dest;

  /* NULL colour table: fall back to the default table (inferred) */
  if (colour_table == (undefined4 *)0x0) {
    colour_table = &g_gfx_62947c;
  }
  colour_dest = (undefined4 *)(*(char **)(*(char **)(ptank_atomic + 4) + g_ptank_plugin_offset) + PTANK_COLOUR_OFFSET);
  /* copy the table one dword at a time into the plugin data; dwords_left counts down from COLOUR_TABLE_DWORDS */
  for (dwords_left = COLOUR_TABLE_DWORDS; dwords_left != 0; dwords_left = dwords_left + -1) {
    *colour_dest = *colour_table;
    colour_table = colour_table + 1;
    colour_dest = colour_dest + 1;
  }
  /* dwords_left is reused as the plugin data pointer again, to set the flag */
  dwords_left = *(char **)(*(char **)(ptank_atomic + 4) + g_ptank_plugin_offset);
  *(uint *)(dwords_left + PTANK_FLAGS_OFFSET) = *(uint *)(dwords_left + PTANK_FLAGS_OFFSET) | PTANK_PLUGIN_FLAG_COLOUR_TABLE;
  return;
}


/* effect_sprites_update_all */

/* "updates every active bit in both sprite sets" */

extern void effect_sprites_update_all(void);


/* effect_sprites_render_all */

/* "renders both sprite sets with z write off and additive blending restore" */

extern void effect_sprites_render_all(void);

/* number of effect slots (one bit each in the g_gfx_6f1e20 bitmap) */
#define EFFECT_SLOT_COUNT (0x100)
/* per-slot state is spaced this many dwords apart in the g_gfx_6c60d0 block (0x2c0 bytes per slot, as effect_trail_add_point uses) */
#define EFFECT_SLOT_STRIDE (0xb0)


/* effect_slot_alloc */

/* "finds a free slot of the 0x100 effect slots, initialises it, returns index + 1"
   Returns the 1-based slot number (1..EFFECT_SLOT_COUNT) of the first free slot, or 0 when all slots are in use.
   Marks the slot as used in the bitmap and resets its state (counter 0, flag 1, start value 30.0f (inferred)). */

/* TIER2: validated 2026-10-18 authority=diffemu */
int effect_slot_alloc(void)
{
  int slot_index;

  slot_index = 0;
  /* scan the slot bitmap (1 bit per slot) for the first clear bit; set it and reset the slot's state */
  do {
    if (((&g_gfx_6f1e20)[slot_index >> 3] & (byte)(1 << ((byte)slot_index & 7))) == 0) {
      (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] = 0;
      (&g_race_update_6c60d8)[slot_index * EFFECT_SLOT_STRIDE] = 0;
      (&g_race_render_6c60d4)[slot_index * EFFECT_SLOT_STRIDE] = 1;
      (&g_race_update_6c60dc)[slot_index * EFFECT_SLOT_STRIDE] = 0x41f00000;
      (&g_gfx_6f1e20)[slot_index >> 3] = (&g_gfx_6f1e20)[slot_index >> 3] | '\x01' << ((byte)slot_index & 7);
      return slot_index + 1;
    }
    slot_index = slot_index + 1;
  } while (slot_index < EFFECT_SLOT_COUNT);
  return 0;
}


/* per-slot trail block size in bytes (EFFECT_SLOT_STRIDE dwords) */
#define TRAIL_SLOT_BYTES (EFFECT_SLOT_STRIDE * 4)
/* size of one trail point record in the g_gfx_6c5e20 block (inferred) */
#define TRAIL_POINT_BYTES (0x48)
/* points a trail holds before it refuses new ones (count 8 = full) */
#define TRAIL_MAX_POINTS (8)


/* effect_trail_add_point */

/* "adds a trail point (distance limited) and builds the strip vertices at the new point" */

/* TIER2: unvalidated 2026-10-11 authority=diffemu */
/* Adds one point to trail slot slot_id (1-based). point: the new centre xyz. side_vec: offset direction for the
   strip vertices (NULL uses the default g_gfx_6294a8). width: strip width, half of it on each side of the point.
   max_dist: the point is refused when it is further than this from the previous one. Returns slot_id on success,
   0 when the slot is not in use, the trail is full or the point is too far (inferred). */
/* TIER2: unvalidated 2026-10-30 authority=static */
int effect_trail_add_point(int slot_id,float *point,float *side_vec,float width,float max_dist)
{
  int slot_index;
  float *point_dest;
  float half_width;
  float dir_z;
  float unused_f5;
  float unused_f6;
  float unused_f7;
  int slot_offset;
  RwReal segment_len;
  RwV3d work_vec;
  
  /* slot_id is 1-based */
  slot_index = slot_id + -1;
  if (((&g_gfx_6f1e20)[slot_index >> 3] & (byte)(1 << ((byte)slot_index & 7))) == 0) {
    return 0;
  }
  /* no side vector: use the default (inferred) */
  if (side_vec == (float *)0x0) {
    side_vec = (float *)&g_gfx_6294a8;
  }
  slot_offset = slot_index * TRAIL_SLOT_BYTES;
  /* trail not full: record the point */
  if ((&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] != TRAIL_MAX_POINTS) {
    /* first point: only remember it as the last point, no segment length yet */
    if ((&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] == 0) {
      *(float *)((char *)&g_gfx_6c60c0 + slot_offset) = *point;
      *(float *)((char *)&g_gfx_6c60c4 + slot_offset) = point[1];
      *(float *)((char *)&g_gfx_6c60c8 + slot_offset) = point[2];
      (&g_gfx_6c60cc)[slot_index * EFFECT_SLOT_STRIDE] = 0;
    }
    else {
      /* later points: segment length from the last point; refuse it when too far */
      work_vec.x = *(float *)((char *)&g_gfx_6c60c0 + slot_offset) - *point;
      work_vec.y = *(float *)((char *)&g_gfx_6c60c4 + slot_offset) - point[1];
      work_vec.z = *(float *)((char *)&g_gfx_6c60c8 + slot_offset) - point[2];
      segment_len = RwV3dLength(&work_vec);
      /* too far from the last point */
      if (max_dist < segment_len) {
        return 0;
      }
      *(float *)&(&g_gfx_6c60cc)[slot_index * EFFECT_SLOT_STRIDE] = segment_len;      /* diffemu: a float store into an undefined4 array became an int conversion */
    }
    /* the point becomes the trail centre; two strip vertices sit at +/- half width along side_vec (inferred) */
    half_width = width * k_f_0_5;
    point_dest = (float *)((char *)&g_gfx_6c5e20 + ((&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] + 0x30) * 0xc + slot_offset);
    *point_dest = *point;
    point_dest[1] = point[1];
    point_dest[2] = point[2];
    /* diffemu: the original keeps the offset point in the stack RwV3d (rounded to float) and reads it back; a float local stayed in an x87 register at full precision */
    work_vec.x = *point;
    work_vec.y = point[1];
    work_vec.z = point[2];
    work_vec.x = half_width * *side_vec + work_vec.x;
    work_vec.y = half_width * side_vec[1] + work_vec.y;
    work_vec.z = half_width * side_vec[2] + work_vec.z;
    *(float *)((char *)&g_gfx_6c5e20 + (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] * TRAIL_POINT_BYTES + slot_offset) = work_vec.x;
    *(float *)(slot_offset + (char *)&g_gfx_6c5e20 + 4 + (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] * TRAIL_POINT_BYTES) = work_vec.y;
    *(float *)(slot_offset + (char *)&g_gfx_6c5e20 + 8 + (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] * TRAIL_POINT_BYTES) = work_vec.z;
    *(undefined4 *)((char *)&g_shockwave_load_6c5e3c + (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] * TRAIL_POINT_BYTES + slot_offset) = 0;
    *(float *)(slot_offset + (char *)&g_shockwave_load_6c5e3c + 4 + (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] * TRAIL_POINT_BYTES) =
         (float)(int)(&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE];
    width = -width;
    /* half_width is reused here for side_vec.y */
    half_width = side_vec[1];
    dir_z = side_vec[2];
    *(float *)(slot_offset + (char *)&g_shockwave_load_6c5e3c + 8 + (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] * TRAIL_POINT_BYTES) = width * *side_vec + work_vec.x;
    *(float *)(slot_offset + (char *)&g_shockwave_load_6c5e3c + 12 + (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] * TRAIL_POINT_BYTES) = width * half_width + work_vec.y;
    *(float *)(slot_offset + (char *)&g_shockwave_load_6c5e3c + 16 + (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] * TRAIL_POINT_BYTES) = width * dir_z + work_vec.z;
    *(undefined4 *)(slot_offset + (char *)&g_shockwave_load_6c5e3c + 36 + (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] * TRAIL_POINT_BYTES) = 0x3f800000;
    *(float *)(slot_offset + (char *)&g_shockwave_load_6c5e3c + 40 + (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] * TRAIL_POINT_BYTES) =
         (float)(int)(&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE];
    (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] = (&g_gfx_6c60d0)[slot_index * EFFECT_SLOT_STRIDE] + 1;
    return slot_id;
  }
  return 0;
}


/* effect_pool_init */

/* "finds the effect texture and clears the pool" */

extern void effect_pool_init(void);


/* triangle_normal_cross */

/* "out = cross of two edges of triangle (a,b,c)" */

extern void triangle_normal_cross(RwV3d *out,float *param_2,float *param_3,float *param_4);


/* vector_normalize_copy */

/* "(out, ?, in): copies xyz and normalises" */

extern void __cdecl vector_normalize_copy(RwV3d *out,float *param_2,RwReal *param_3);


/* plane_classify_point */

/* "(plane, point, out dist): distance to the plane and 0 behind / 1 on / 2 in front" */

extern undefined4 plane_classify_point(float *param_1,float *param_2,RwReal *param_3);


/* bbox_plane_corner_test */

/* "tests the 8 corners of a bbox against a plane, 0 when any is inside" */

extern undefined4 __cdecl bbox_plane_corner_test(char *param_1,undefined4 *param_2);


/* sphere_plane_side */

/* "(plane, sphere): 2 when the sphere is in front, 0 / 1 otherwise using the radius" */

extern undefined4 __cdecl sphere_plane_side(char *param_1,char *param_2);


/* course_error_exit */

/* "Exiting due to course error %d" */

extern void course_error_exit(char *param_1);


/* particle_system_set_current_forward */

/* "calls particle_system_set_current" */

extern void __cdecl particle_system_set_current_forward(char *param_1,char *param_2,char *param_3);


/* spring_grid_reset_constants */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "sets the default constants of the spring grid" */

extern void spring_grid_reset_constants(void);


/* gfx_set_gfx_6f2ab8 */

/* "g_gfx_6f2ab8 = param_1; return;" */

extern void gfx_set_gfx_6f2ab8(undefined4 param_1);



