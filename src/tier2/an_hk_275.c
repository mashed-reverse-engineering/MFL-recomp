#include "mfl_tier2.h"
/*00485b80*/ extern int smoke_emitter_update();
/*00486c40*/ extern int particles_flush();
/*006c40c0*/ extern undefined g_particles_6c40c0;
/*006c4210*/ extern undefined4 g_effects_update_6c4210;
/*006c5000*/ extern undefined g_particles_6c5000;
/*006c5080*/ extern undefined g_particles_6c5080;
/*007ef7f4*/ extern float g_tick_seconds;
/*006c4140*/ extern char g_gfx_6c4140[];
/*006c50d0*/ extern char g_particles_6c50d0[];

#define EMITTER_STRIDE_DWORDS (0x3b)
#define EMITTER_BASE_OFFSET_DWORDS (0x34)
#define PARTICLE_BUCKET_BYTES (0x40)

/* particle_emitters_update_all */

/* Advances every emitter slot in g_effects_update_6c4210 by one frame, then flushes the particle buckets.
   Each slot is 0x3B dwords; emitter_timing points at its timing block (the slot starts 0x34 dwords before it).
   Timing block fields (inferred): [-1] = duration, [0] = elapsed seconds, [1] = progress (elapsed / duration),
   [2] = active flag. Active slots past their duration are switched off; the rest tick elapsed by g_tick_seconds
   and call smoke_emitter_update() on the slot base. Takes no parameters, returns nothing. */

void particle_emitters_update_all(void)

{
  float new_elapsed;
  float *emitter_timing;
  undefined *particle_bucket;

  emitter_timing = (float *)&g_effects_update_6c4210;
  /* walk every emitter slot up to the particle buckets */
  do {
    if (*(int *)&emitter_timing[2] != 0) {      /* an int flag (original: cmp dword, 0) */
      /* still inside its duration: step elapsed time, publish progress, update the emitter */
      if (!(*emitter_timing > emitter_timing[-1])) {
        smoke_emitter_update(emitter_timing + -EMITTER_BASE_OFFSET_DWORDS);
        new_elapsed = g_tick_seconds + *emitter_timing;
        *emitter_timing = new_elapsed;
        emitter_timing[1] = new_elapsed / emitter_timing[-1];
      }
      else {
        /* duration over: deactivate the slot */
        emitter_timing[2] = 0.0;
      }
    }
    emitter_timing = emitter_timing + EMITTER_STRIDE_DWORDS;
  } while ((LONG_PTR)emitter_timing < ((LONG_PTR)&g_particles_6c50d0));
  /* flush the particle buckets in the first range (64 bytes each) */
  particle_bucket = &g_particles_6c40c0;
  do {
    particles_flush(particle_bucket);
    particle_bucket = particle_bucket + PARTICLE_BUCKET_BYTES;
  } while ((LONG_PTR)particle_bucket < ((LONG_PTR)&g_gfx_6c4140));
  /* flush the particle buckets in the second range (64 bytes each) */
  particle_bucket = &g_particles_6c5000;
  do {
    particles_flush(particle_bucket);
    particle_bucket = particle_bucket + PARTICLE_BUCKET_BYTES;
  } while ((LONG_PTR)particle_bucket < ((LONG_PTR)&g_particles_6c5080));
  /* last bucket, flushed on its own */
  particles_flush(&g_particles_6c5080);
  return;
}


