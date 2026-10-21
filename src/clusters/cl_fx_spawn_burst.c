// cluster-entries: fx_spawn_burst
/* Installed cluster fx_spawn_burst (tools/cluster_install.py from output/cluster_pilot_uc3/fx_spawn_burst.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range(float low, float high);
/*004d5290*/ extern float RwV3dNormalize(RwV3d *out, RwV3d *in);

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

/* Spawns `count` particles around `center`. velocity: the start velocity of all (NULL = random direction, speed 0.003); colour: RGBA of all (NULL = orange
   0xff8421, alpha from the upward speed when the velocity is random); accel: acceleration (NULL = none); life: lifetime scale (<= 0: 1/6).
   Each particle gets its colour brightened towards white by a random fraction (0..0.5) and is either a small one that starts at the centre moved by the
   velocity (random below 0.4) or a big one with a random size and triple life. All velocities are multiplied by 50. */
void fx_spawn_burst(float *center, int count, float *velocity, unsigned int *colour, float *accel, float life)
{
  FxParticle p;
  RwV3d v;
  double brighten;
  int remaining;

  if (life <= 0.0f) {
    p.life_scale = 0.16666667f;
  } else {
    p.life_scale = life;
  }
  p.pos.x = center[0];
  p.pos.y = center[1];
  p.pos.z = center[2];
  p.field_38 = 1.0f;
  if (count == 0) {
    return;
  }
  remaining = count;
  do {
    if (velocity == 0) {
      v.x = (float)random_range(-1.0f, 1.0f);
      v.y = (float)random_range(0.0f, 1.0f);
      v.z = (float)random_range(-1.0f, 1.0f);
      RwV3dNormalize(&v, &v);
      v.x = v.x * 0.003f;
      v.y = v.y * 0.003f;
      v.z = v.z * 0.003f;
      v.y = (float)random_range(0.007f, 0.013f);
    } else {
      v.x = velocity[0];
      v.y = velocity[1];
      v.z = velocity[2];
    }
    if (colour != 0) {
      *(unsigned int *)p.colour = *colour;
    } else {
      p.colour[0] = 0xff;
      p.colour[1] = 0x84;
      p.colour[2] = 0x21;
      if (velocity == 0) {
        p.colour[3] = (unsigned char)((v.y - 0.007f) * 76.92307f * 128.0f + 64.0f);
      } else {
        p.colour[3] = 0xff;
      }
    }
    brighten = random_range(0.0f, 0.5f);
    p.colour[0] = p.colour[0] + (unsigned char)((255 - p.colour[0]) * brighten);
    p.colour[1] = p.colour[1] + (unsigned char)((255 - p.colour[1]) * brighten);
    p.colour[2] = p.colour[2] + (unsigned char)((255 - p.colour[2]) * brighten);
    if (brighten < 0.4f) {
      p.flags = 0x80;
      p.pos.x = p.pos.x + v.x;
      p.size_start = 0.05f;
      p.size_end = 0.05f;
      p.pos.y = p.pos.y + v.y;
      p.pos.z = p.pos.z + v.z;
    } else {
      p.flags = 0x900;
      p.size_start = (float)((random_range(-0.075f, 0.025f) + 0.1f) * 2.4f);
      p.size_end = p.size_start;
      p.life_scale = p.life_scale * 3.0f;
    }
    p.velocity.x = v.x * 50.0f;
    p.flags |= 0x1002;
    p.kind = 5;
    p.velocity.y = v.y * 50.0f;
    p.velocity.z = v.z * 50.0f;
    if (accel != 0) {
      p.accel.x = accel[0];
      p.accel.y = accel[1];
      p.accel.z = accel[2];
    } else {
      p.accel.x = 0.0f;
      p.accel.y = 0.0f;
      p.accel.z = 0.0f;
    }
    fx_trail_particle_spawn_interpolated(&p, 0.0f, 0.0f, 0);
  } while (--remaining != 0);
}

