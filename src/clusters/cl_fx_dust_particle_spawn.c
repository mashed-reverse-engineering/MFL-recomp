// cluster-entries: fx_dust_particle_spawn
/* Installed cluster fx_dust_particle_spawn (tools/cluster_install.py from output/cluster_pilot_uc3/fx_dust_particle_spawn.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range(float low, float high);
/*004c45e0*/ extern double rp_random_float_range(float low, float high);

/* One particle record as fx_trail_particle_spawn_interpolated takes it (0x48 bytes). */
typedef struct FxParticle {
  RwV3d pos;                       /* +0x00 */
  unsigned char colour[4];         /* +0x0c r, g, b, a */
  float size_start, size_end;      /* +0x10 */
  RwV3d velocity;                  /* +0x18 */
  RwV3d accel;                     /* +0x24 */
  float life_scale;                /* +0x30 */
  float field_34;
  float field_38;
  float field_3c;
  unsigned int flags;              /* +0x40 */
  unsigned char flag_44, kind, pad[2];
} FxParticle;
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated(FxParticle *particle, float a, float b, int c);

/* One 0x44 byte effect colour record (colour bytes first, flags at +0x30). */
typedef struct EffectRecord { unsigned char r, g, b; char pad[0x2d]; unsigned char flags; char tail[0x13]; } EffectRecord;
extern EffectRecord g_effects_update_76df34[];

/* Spawns one dust puff of the effect colour `slot` (colour scaled by `scale`, clamped to 0.4 .. 1): at `position`, with the velocity `velocity` plus a random
   sideways jitter and an upward speed of 1.3 - random(0..1). A random roll above 0.6 makes a small fast puff, otherwise a doubled bigger one. */
void fx_dust_particle_spawn(float *velocity, float *position, int slot, float scale)
{
  FxParticle p;
  EffectRecord *rec;
  int big;

  if (scale > 1.0f) {
    scale = 1.0f;
  } else if (scale < 0.4f) {
    scale = 0.4f;
  }
  p.pos.x = position[0];
  p.pos.y = position[1];
  p.pos.z = position[2];
  p.life_scale = 1.0f;
  p.field_38 = 1.0f;
  big = random_range(0.0f, 1.0f) > 0.6f;
  rec = &g_effects_update_76df34[slot];
  p.colour[0] = (unsigned char)(rec->r * scale);
  p.colour[2] = (unsigned char)(rec->b * scale);
  p.colour[1] = (unsigned char)(rec->g * scale);
  if (big) {
    p.flags = 0x1100;
    p.size_start = (float)(rp_random_float_range(0.04f, 0.07f) * 2.0);
    p.size_end = p.size_start;
    p.kind = 2;
  } else {
    p.life_scale = p.life_scale + p.life_scale;
    p.flags = 0x1082;
    p.size_start = (float)(rp_random_float_range(0.03f, 0.09f) * 2.0);
    p.size_end = 0.025f;
  }
  p.velocity.x = velocity[0];
  p.velocity.y = velocity[1];
  p.velocity.z = velocity[2];
  p.colour[3] = 0xff;
  p.velocity.x = (float)(rp_random_float_range(-1.0f, 1.0f) * 0.4f + p.velocity.x);
  p.velocity.y = (float)(1.3f - rp_random_float_range(0.0f, 1.0f));
  p.velocity.z = (float)(rp_random_float_range(-1.0f, 1.0f) * 0.4f + p.velocity.z);
  p.accel.x = 0.0f;
  p.accel.y = -2.8f;
  p.accel.z = 0.0f;
  if ((rec->flags & 0x40) != 0) {
    p.flags |= 0x40;
  }
  fx_trail_particle_spawn_interpolated(&p, 0.0f, 0.0f, 0);
}

