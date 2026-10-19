#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*0049d820*/ extern int fx_smoke_puffs_update();
/*0049da30*/ extern undefined __cdecl fx_floating_text_spawn(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*0049dba0*/ extern int fx_floating_text_update();
/*004b2b48*/ extern int _ftol();
/*004c40c0*/ extern int FUN_004c40c0(undefined4 p0);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*00555220*/ extern void * __cdecl RpMorphAtomicSetTime(void * p0, RwReal p1);
/*005dd330*/ extern float float_1_0;
/*005dd370*/ extern float k_f_0_3;
/*005de1f8*/ extern float k_f_3_1415927;
/*005de398*/ extern float k_f_0_35;
/*0076bfdc*/ extern undefined4 g_particles_76bfdc;
/*0076dee0*/ extern undefined4 g_particles_76dee0[0x1000];
/*007ef7f4*/ extern float g_tick_seconds;
extern double sin(double);
#define PARTICLE_RECORD_DWORDS (0x29)
#define PARTICLE_RECORDS_BYTES (0x520)
#define SPARKLE_MORPH_ATOMICS (10)
#define SPARKLE_MORPH_TIME_OFS (0x18)
#define SPARKLE_COLOUR_BASE (0xffffff)
#define SPARKLE_ALPHA_SCALE (255.0f)
#define SPARKLE_JITTER_MAX_BITS (0x3e4ccccd)
#define SPARKLE_TEXT_ARG3_BITS (0x3e4ccccd)
/* fx_pickup_sparkle_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "per active record (0x29 dwords) spawns timed sub effects, bobs the frame with sin and fades 10
   morph atomics" */

/* Per-frame update of the pickup sparkle effect (inferred from the body).
 * Walks the record pool g_particles_76bfdc (PARTICLE_RECORDS_BYTES bytes, PARTICLE_RECORD_DWORDS floats per record).
 * The matching g_particles_76dee0 entry is nonzero while its record is active. Record layout, float index from rec:
 *   [-7] [-6] [-5]  base position x, y, z
 *   [-4]            age copy (used for the fade)
 *   [-3]            age, in seconds
 *   [-2] [-1]       spawned count, spawn max (both int)
 *   [0]             spawn timer
 *   [1]             text height base (inferred)
 *   [3]             RwFrame * (bobbed)
 *   [4]..[13]       morph atomics, SPARKLE_MORPH_ATOMICS of them
 *   [14]..[23]      per-atomic morph speed
 *   [24]..[33]      per-atomic morph time
 * Takes no parameters and returns nothing. Calls fx_smoke_puffs_update and fx_floating_text_update at the end.
 */
void fx_pickup_sparkle_update(void)

{
  float text_height;
  float scratch_f;
  char fade_alpha;
  int atomic_material;
  int atomics_left;
  float *morph_time;
  float *rec;
  float10 scratch_d;
  undefined4 text_arg2;
  undefined4 text_arg3;
  struct {
    undefined4 colour;
    int *active_flag;
    undefined1 pad_10[4];
    RwV3d pos;
  } draw;

  draw.active_flag = g_particles_76dee0;
  rec = (float *)&g_particles_76bfdc;
  do {
    if (*draw.active_flag != 0) {
      /* Spawn timer: advances only while spawned count < spawn max. At 0.3 (k_f_0_3)
         it resets, bumps the count and spawns floating text at the record position with
         height text_height = random [0, 0.2) + rec[1] (inferred). */
      if ((((int *)rec)[-2] < ((int *)rec)[-1]) &&
         (scratch_f = g_tick_seconds + *rec, *rec = scratch_f, k_f_0_3 <= scratch_f)) {
        text_arg3 = SPARKLE_TEXT_ARG3_BITS;
        text_arg2 = 0;
        scratch_d = (float10)random_range(0x0, SPARKLE_JITTER_MAX_BITS);
        *rec = 0.0;
        ((int *)rec)[-2] = ((int *)rec)[-2] + 1;
        fx_floating_text_spawn((undefined4)(rec + -7),(text_height = (float)(scratch_d + (float10)rec[1]), *(int *)&text_height),text_arg2,text_arg3);
      }
      /* Age += tick (stored in rec[-3] and rec[-4]). Frame x and z come from the record,
         y = base y + sin(age * pi) * 0.35 (bob). */
      scratch_d = (float10)g_tick_seconds + (float10)rec[-3];
      draw.pos.x = rec[-7];
      draw.pos.z = rec[-5];
      rec[-3] = (float)scratch_d;
      rec[-4] = (float)scratch_d;
      scratch_d = (float10)sin(scratch_d * (float10)k_f_3_1415927);
      draw.pos.y = (RwReal)(scratch_d * (float10)k_f_0_35 + (float10)rec[-6]);
      RwFrameTranslate(*(RwFrame **)&rec[3],&draw.pos,rwCOMBINEREPLACE);
      /* Age reached 1.0: the record is finished, clear its active flag. */
      if (float_1_0 <= rec[-3]) {
        *draw.active_flag = 0;
      }
      /* Morph atomics: colour starts RGB white (alpha set per atomic below). */
      draw.colour = SPARKLE_COLOUR_BASE;
      morph_time = rec + SPARKLE_MORPH_TIME_OFS;
      atomics_left = SPARKLE_MORPH_ATOMICS;
      do {
        /* Per atomic: clamp morph time to 1.0, set it on the atomic, then advance it by tick * speed.
           Alpha byte = 255 - age * 255 (inferred), written into the colour. */
        scratch_f = g_tick_seconds * morph_time[-10];
        if (float_1_0 < *morph_time) {
          *morph_time = 1.0;
        }
        RpMorphAtomicSetTime(*(void **)&morph_time[-0x14],*morph_time);
        *morph_time = scratch_f + *morph_time;
        fade_alpha = ((int)(rec[-4] * SPARKLE_ALPHA_SCALE));
        draw.colour = CONCAT13(-1 - fade_alpha,(undefined3)draw.colour);
        atomic_material = FUN_004c40c0(*(int *)&morph_time[-0x14]);
        morph_time = morph_time + 1;
        atomics_left = atomics_left + -1;
        *(undefined4 *)(atomic_material + 4) = draw.colour;
      } while (atomics_left != 0);
    }
    rec = rec + PARTICLE_RECORD_DWORDS;
    draw.active_flag = draw.active_flag + 1;
  } while ((int)rec < (int)&g_particles_76bfdc + PARTICLE_RECORDS_BYTES);
  fx_smoke_puffs_update();
  fx_floating_text_update();
  return;
}


