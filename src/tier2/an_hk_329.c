#include "mfl_tier2.h"
/*004846b0*/ extern int alloc_zeroed();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*005dd330*/ extern float float_1_0;
/*00759708*/ extern undefined g_particles_759708;
/*00759760*/ extern undefined4 g_particles_759760;
/*0075bfe8*/ extern char g_particles_75bfe8[];
#define SYSTEM_RECORD_SIZE (0x68)
#define SYSTEM_RECORD_WORDS (0x1a) /* SYSTEM_RECORD_SIZE / 4 */
#define PARTICLE_SIZE (0x30)
#define PARTICLE_DEFAULT_FLAGS (0x817)
#define RW_BLEND_SRC_ALPHA (5) /* inferred: RwBlendFunction rwSRCALPHA */
#define RW_BLEND_INV_SRC_ALPHA (6) /* inferred: RwBlendFunction rwINVSRCALPHA */
/* particles_alloc_system */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "finds a free system record (0x68 bytes), sets texture and dimensions" */
/*
 * Claims a free particle system record and fills it in.
 * texture: texture for the particle table (passed to particle_system_setup_ptank).
 * particle_count: number of particles; each one is PARTICLE_SIZE bytes.
 * life_frames: lifetime in frames (inferred); the record stores 1.0 / life_frames as a float.
 * particle_flags: stored in the record and passed to setup; 0 means PARTICLE_DEFAULT_FLAGS.
 * Returns the record, or NULL when no record is free or either count is 0.
 */

int * particles_alloc_system(RwTexture *texture,int particle_count,int life_frames,uint particle_flags)

{
  int *record;
  undefined1 *particle;
  int record_index;
  int record_offset;

  /* Find a free record. A record is free while its marker word (at +0x58) is nonzero,
     so the search walks the table one record at a time and stops at the first free one. */
  record_index = 0;
  record = &g_particles_759760;
  while (*record == 0) {
    record = record + SYSTEM_RECORD_WORDS;
    record_index = record_index + 1;
    if (((LONG_PTR)&g_particles_75bfe8 + 0x17) < (LONG_PTR)record) {
      return (int *)0x0;
    }
  }
  record_offset = record_index * SYSTEM_RECORD_SIZE;
  record = (int *)((char *)&g_particles_759708 + record_offset);
  if (record == (int *)0x0) {
    return (int *)0x0;
  }
  /* Fill in the record and claim it. Reject zero counts. */
  if ((life_frames != 0) && (particle_count != 0)) {
    *(int *)(record_offset + ((LONG_PTR)&g_particles_759708 + 0x4)) = particle_count;
    *(float *)(record_offset + ((LONG_PTR)&g_particles_759760 + 0x4)) = float_1_0 / (float)life_frames;
    if (particle_flags == 0) {
      particle_flags = PARTICLE_DEFAULT_FLAGS;
    }
    *(uint *)(record_offset + ((LONG_PTR)&g_particles_759708 + 0x50)) = particle_flags;
    particle_system_setup_ptank((uint *)(record_offset + ((LONG_PTR)&g_particles_759708 + 0x10)),particle_flags,particle_count * life_frames,texture);
    ptank_set_blend_modes((uint *)(record_offset + ((LONG_PTR)&g_particles_759708 + 0x10)),RW_BLEND_SRC_ALPHA,RW_BLEND_INV_SRC_ALPHA);
    *(undefined4 *)(record_offset + ((LONG_PTR)&g_particles_759708 + 0x54)) = 1;
    /* Clear the marker word: the record is now in use. */
    (&g_particles_759760)[record_index * SYSTEM_RECORD_WORDS] = 0;
    /* Allocate the particle array. record_index is reused here: it holds the
       array pointer, then a byte offset in the loop below. */
    record_index = alloc_zeroed(particle_count,PARTICLE_SIZE);
    *record = record_index;
    /* Walk the particles from last to first, resetting each one:
       colour bytes = 0xff (white, opaque) and the words at +0x4, +0x8, +0xc = 0. */
    record_index = particle_count * PARTICLE_SIZE;
    do {
      record_index = record_index + -PARTICLE_SIZE;
      particle = (undefined1 *)(*record + record_index);
      particle_count = particle_count + -1;
      *(undefined4 *)(particle + 0xc) = 0;
      *(undefined4 *)(particle + 8) = 0;
      *(undefined4 *)(particle + 4) = 0;
      *particle = 0xff;
      particle[1] = 0xff;
      particle[2] = 0xff;
      particle[3] = 0xff;
    } while (particle_count != 0);
    return record;
  }
  return (int *)0x0;
}


