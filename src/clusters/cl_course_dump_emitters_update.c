// cluster-entries: course_dump_emitters_update
/* Installed cluster course_dump_emitters_update (tools/cluster_install.py from output/cluster_pilot_te5b/course_dump_emitters.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double __cdecl random_range(float low, float high);
/*004c45e0*/ extern double __cdecl FUN_004c45e0(float low, float high);
/*00482010*/ extern int __cdecl random_int_range(int low, int high);
/*004980c0*/ extern void __cdecl fx_trail_particle_spawn_interpolated(void *particle, int a, int b, int c);
/*005dd340*/ extern float k_f_0_5;
/*007ef804*/ extern unsigned int g_tick_number;

#define EMITTER_COUNT_OFFSET 0xc00c      /* number of emitters in the course block */
#define EMITTER_TABLE_OFFSET 0x2420      /* emitter table of the course block */
#define EMITTER_STRIDE 0x40
#define SPAWN_PHASE_MASK 3               /* every emitter spawns on every 4th tick, staggered by its index */
#define SHADE_MIN 100
#define SHADE_MAX 200
#define PARTICLE_ALPHA 0xa0
#define PARTICLE_FLAGS 0x1108
#define PARTICLE_KIND 6

/* one emitter (stride 0x40): position, and a velocity at +0x10 that is handed to the particle unchanged */
typedef struct DumpEmitter {
  float position[3];            /* 0x00 */
  float unknown_0c;
  unsigned int velocity[3];     /* 0x10 */
  unsigned char unknown_1c[0x24];
} DumpEmitter;

/* the particle record fx_trail_particle_spawn_interpolated takes (0x48 bytes; the unnamed bytes are not set by the caller) */
typedef struct TrailParticle {
  unsigned int velocity[3];     /* 0x00 */
  unsigned char colour[4];      /* 0x0c: r g b a */
  float size_a;                 /* 0x10 */
  float size_b;                 /* 0x14 */
  float position[3];            /* 0x18 */
  float zero[3];                /* 0x24 */
  float scale_a;                /* 0x30 */
  unsigned char unknown_34[4];
  float scale_b;                /* 0x38 */
  unsigned char unknown_3c[4];
  unsigned int flags;           /* 0x40 */
  unsigned char unknown_44;
  unsigned char kind;           /* 0x45 */
  unsigned char unknown_46[2];
} TrailParticle;

/* course_dump_emitters_update: lets each emitter of the course block puff a grey trail particle on its tick; position = twice the emitter position plus one random offset of up to +-0.5 */
void course_dump_emitters_update(char *course)
{
  int i;
  DumpEmitter *emitter;
  unsigned char shade;
  double jitter;
  TrailParticle particle;

  for (i = 0; i < *(int *)(course + EMITTER_COUNT_OFFSET); i++) {
    if (((g_tick_number ^ i) & SPAWN_PHASE_MASK) != 0) {
      continue;
    }
    emitter = (DumpEmitter *)(course + EMITTER_TABLE_OFFSET + i * EMITTER_STRIDE);
    shade = (unsigned char)random_int_range(SHADE_MIN, SHADE_MAX);
    jitter = FUN_004c45e0(-1.0f, 1.0f) * k_f_0_5;
    particle.velocity[0] = emitter->velocity[0];
    particle.velocity[1] = emitter->velocity[1];
    particle.velocity[2] = emitter->velocity[2];
    particle.position[0] = (float)(emitter->position[0] * 2.0 + jitter);
    particle.position[1] = (float)(emitter->position[1] * 2.0 + jitter);
    particle.position[2] = (float)(emitter->position[2] * 2.0 + jitter);
    particle.zero[0] = 0.0f;
    particle.zero[1] = 0.0f;
    particle.zero[2] = 0.0f;
    particle.scale_a = 0.8f;
    particle.scale_b = 1.2f;
    particle.flags = PARTICLE_FLAGS;
    particle.kind = PARTICLE_KIND;
    particle.colour[0] = shade;
    particle.colour[1] = shade;
    particle.colour[2] = shade;
    particle.colour[3] = PARTICLE_ALPHA;
    particle.size_a = (float)random_range(0.7f, 0.9f);
    particle.size_b = particle.size_a;
    fx_trail_particle_spawn_interpolated(&particle, 0, 0, 0);
  }
}

