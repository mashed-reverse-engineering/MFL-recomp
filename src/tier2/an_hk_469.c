#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*004980c0*/ extern int fx_trail_particle_spawn_interpolated();

#define PARTICLE_COLOUR_ARGB (0xffc0c0c0)
#define PARTICLE_FLAGS (0x1120)
#define RANDOM_VEL_MIN_BITS (0xbf800000) /* float -1.0 */
#define RANDOM_VEL_MAX_BITS (0x3f800000) /* float 1.0 */
#define PARTICLE_UP_VEL_BITS (0x3fc00000) /* float 1.5 */
#define PARTICLE_GRAVITY_BITS (0xc0100000) /* float -2.25 */

/* course_particle_init_random */

/* "initialises one particle (colour 0xffc0c0c0, flags 0x1120) with random velocity"
 * position: float[3] start position, copied into the particle (inferred).
 * Spawns the particle through fx_trail_particle_spawn_interpolated; returns nothing. */

void __cdecl course_particle_init_random(undefined4 *position)

{
  double rand_x;
  double rand_z;
  /* particle layout by byte offset (names for unk_ fields are unknown) */
  struct {
    undefined4 pos_x;
    undefined4 pos_y;
    undefined4 pos_z;
    undefined4 colour;
    undefined4 unk_10;
    undefined4 unk_14;
    float vel_x;
    undefined4 vel_y;
    float vel_z;
    undefined4 accel_x;
    undefined4 accel_y;
    undefined4 accel_z;
    undefined4 unk_30;
    undefined1 pad_14[4];
    undefined4 unk_38;
    undefined1 pad_c[4];
    undefined4 flags;
    undefined1 pad_4[1];
    undefined1 unk_45;
  } particle;

  /* copy the start position */
  particle.pos_x = *position;
  particle.pos_y = position[1];
  particle.pos_z = position[2];
  particle.unk_45 = 2;
  particle.unk_30 = 0x3f800000;
  particle.colour = PARTICLE_COLOUR_ARGB;
  particle.flags = PARTICLE_FLAGS;
  particle.unk_10 = 0x3f800000;
  particle.unk_14 = 0x3f800000;
  particle.unk_38 = 0x3f800000;
  /* random horizontal velocity, x and z in [-1, 1] */
  rand_x = (double)random_range(RANDOM_VEL_MIN_BITS,RANDOM_VEL_MAX_BITS);
  rand_z = (double)random_range(RANDOM_VEL_MIN_BITS,RANDOM_VEL_MAX_BITS);
  particle.vel_z = (float)rand_z;
  particle.vel_y = PARTICLE_UP_VEL_BITS;
  particle.accel_x = 0;
  particle.accel_y = PARTICLE_GRAVITY_BITS;
  particle.accel_z = 0;
  particle.vel_x = (float)rand_x;
  /* spawn; the trailing 0 arguments are not known */
  fx_trail_particle_spawn_interpolated(&particle.pos_x,0,0,0);
  return;
}


