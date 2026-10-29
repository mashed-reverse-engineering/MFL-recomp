// cluster-entries: smoke_emitter_update
/* Installed cluster smoke_emitter_update (tools/cluster_install.py from output/cluster_pilot_x2/smoke_emitter_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd628*/ extern float k_f_0_25;
/*005e64a4*/ extern float k_f_0;
/*005dd9ac*/ extern float k_f_255;
/*005df458*/ extern float k_f_200;
/*00629428*/ extern float g_effects_update_629428;
/*00629434*/ extern float g_effects_update_629434;
/*00629440*/ extern float g_effects_update_629440;
/*00629444*/ extern float g_effects_update_629444;
/*00629448*/ extern float g_effects_update_629448;
/*00629454*/ extern float g_effects_update_629454;
/*00629458*/ extern float g_effects_update_629458;
/*0062945c*/ extern float g_effects_update_62945c;
/*006c40c0*/ extern char g_particles_6c40c0[];
/*006c5000*/ extern char g_particles_6c5000[];
/*006c5080*/ extern char g_particles_6c5080[];
/*006c50c0*/ extern float *g_particles_6c50c0;
/*007ef7f4*/ extern float g_tick_seconds;
/*004867f0*/ extern void particles_set_position(float *xyz);
/*00486820*/ extern void particles_set_size(float *size_xy);
/*00486840*/ extern void particles_set_color(unsigned char *rgba);
/*00486880*/ extern void gfx_set_gfx_6c50c8(float angle);
/*00486b50*/ extern void particles_add(void *particle_batch);
/*004980c0*/ extern void __cdecl fx_trail_particle_spawn_interpolated(void *particle, int a, int b, int c);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *out, struct RwV3d *in);
/*004d5410*/ extern float _rwSqrt(float x);

#define PUFFS 4
#define SMOKE_PARTICLE_FLAGS 0x1920
#define SMOKE_PARTICLE_KIND 5

/* particle record of fx_trail_particle_spawn_interpolated (the fields the smoke leaves unwritten stay unwritten) */
typedef struct TrailParticle {
  int pos[3];
  int colour;
  float size[2];
  float velocity[3];
  float gravity[3];
  float scale;
  int _gap_34;
  float life;
  int _gap_3c;
  int flags;
  char _gap_44;
  unsigned char kind;
  char _gap_46[2];
} TrailParticle;

/* A smoke emitter (0xec bytes): four puffs for each of three layers */
typedef struct SmokeEmitter {
  float puff_pos[PUFFS][3];       /* 0x00 main plume puff positions */
  float puff_vel[PUFFS][3];       /* 0x30 their velocities */
  float layer2_pos[PUFFS][3];     /* 0x60 */
  float layer2_dir[PUFFS][3];     /* 0x90 directions (normalised while the emitter is young) */
  float pos[3];                   /* 0xc0 emitter position */
  int _gap_cc;
  float age;                      /* 0xd0 */
  float size_scale;               /* 0xd4 */
  int _gap_d8;
  float intensity;                /* 0xdc */
  float distance_scale;           /* 0xe0 */
  int layer2_active;              /* 0xe4 */
  int burst_pending;              /* 0xe8 */
} SmokeEmitter;

/* smoke_emitter_update: per emitter and frame. Fades the intensity with the camera distance, spawns the initial trail burst once, then draws up to three layers of four puffs
   (main plume while young, the second layer, the final fade) by feeding size / colour / position / angle to the particle batch. */
static void particle_random_scale_r(float * p0, float p1, float p2);
extern void particle_random_scale(void);
static void effect_slot_init_with_ftol_r(unsigned char * p0, float p1);
extern void effect_slot_init_with_ftol(void);
void smoke_emitter_update(SmokeEmitter *e)
{
  float age = e->age;
  float root = _rwSqrt(e->size_scale);
  float size[2];
  unsigned char colour[4];
  float angle;
  float d0, d1, d2;
  float step;
  int i;

  if (!(age < k_f_0)) {      /* a NaN age takes the body, as the original's `test ah,5; jnp` does */
    if (g_particles_6c50c0 != 0) {
      d0 = g_particles_6c50c0[0] - e->pos[0];
      d1 = g_particles_6c50c0[1] - e->pos[1];
      d2 = g_particles_6c50c0[2] - e->pos[2];
      d2 = d0 * d0 + d1 * d1 + d2 * d2;
      if (d2 < k_f_4) {
        e->intensity = (float_1_0 - (k_f_4 - d2) * k_f_0_25) * e->distance_scale;
      }
    }
    if (e->burst_pending != 0) {
      TrailParticle p;

      p.pos[0] = ((int *)e->pos)[0];
      p.pos[1] = ((int *)e->pos)[1];
      p.pos[2] = ((int *)e->pos)[2];
      p.colour = -1;
      p.size[0] = e->intensity + e->intensity;
      p.size[1] = p.size[0];
      p.velocity[0] = 0.0f;
      p.velocity[1] = 0.0f;
      p.velocity[2] = 0.0f;
      p.gravity[0] = 0.0f;
      p.gravity[1] = 0.0f;
      p.gravity[2] = 0.0f;
      p.scale = 6.6666665f;
      p.life = 1.0f;
      p.flags = SMOKE_PARTICLE_FLAGS;
      p.kind = SMOKE_PARTICLE_KIND;
      fx_trail_particle_spawn_interpolated(&p, 0, 0, 0);
      e->burst_pending = 0;
    }
    if (age < g_effects_update_629434) {
      size[0] = g_effects_update_629448 * root * e->intensity + g_effects_update_629444;
      size[1] = size[0];
      colour[3] = colour[2] = colour[1] = colour[0] = (unsigned char)(int)((float_1_0 - age / g_effects_update_629434) * k_f_255);
      particles_set_size(size);
      particles_set_color(colour);
      for (i = 0; i < PUFFS; i++) {
        e->puff_pos[i][0] = g_tick_seconds * g_effects_update_629428 * e->puff_vel[i][0] + e->puff_pos[i][0];
        e->puff_pos[i][1] = g_tick_seconds * g_effects_update_629428 * e->puff_vel[i][1] + e->puff_pos[i][1];
        e->puff_pos[i][2] = g_tick_seconds * g_effects_update_629428 * e->puff_vel[i][2] + e->puff_pos[i][2];
        particles_set_position(e->puff_pos[i]);
        angle = ((i & 1) == 0) ? -age : age;
        gfx_set_gfx_6c50c8(angle);
        particles_add(g_particles_6c40c0 + (i & 1) * 0x40);
      }
    }
    if (e->layer2_active != 0) {
      particle_random_scale_r(size, e->size_scale, e->intensity);
      effect_slot_init_with_ftol_r(colour, e->size_scale);
      particles_set_size(size);
      particles_set_color(colour);
      for (i = 0; i < PUFFS; i++) {
        if (g_effects_update_629440 < age) {
          e->layer2_dir[i][1] = 1.0f;
          RwV3dNormalize((struct RwV3d *)e->layer2_dir[i], (struct RwV3d *)e->layer2_dir[i]);
        }
        e->layer2_pos[i][0] = g_tick_seconds * g_effects_update_629428 * e->layer2_dir[i][0] + e->layer2_pos[i][0];
        e->layer2_pos[i][1] = g_tick_seconds * g_effects_update_629428 * e->layer2_dir[i][1] + e->layer2_pos[i][1];
        e->layer2_pos[i][2] = g_tick_seconds * g_effects_update_629428 * e->layer2_dir[i][2] + e->layer2_pos[i][2];
        particles_set_position(e->layer2_pos[i]);
        angle = ((i & 1) == 0) ? -age : age;
        gfx_set_gfx_6c50c8(angle);
        particles_add(g_particles_6c5000 + (i & 1) * 0x40);
      }
    }
    if (age < g_effects_update_62945c) {
      float p[3];

      size[0] = g_effects_update_629454 * e->intensity;
      size[1] = size[0];
      colour[2] = colour[1] = colour[0] = 0;
      colour[3] = (unsigned char)(int)((float_1_0 - age / g_effects_update_62945c) * k_f_200);
      particles_set_size(size);
      particles_set_color(colour);
      for (i = 0; i < PUFFS; i++) {
        step = g_effects_update_629458 * age;
        p[0] = e->puff_vel[i][0] * step + e->puff_pos[i][0];
        p[1] = step * e->puff_vel[i][1] + e->puff_pos[i][1];
        p[2] = e->puff_vel[i][2] * step + e->puff_pos[i][2];
        particles_set_position(p);
        particles_add(g_particles_6c5080);
      }
    }
  }
}

/* particle_random_scale_r: C -> original particle_random_scale (registers eax,s0,s1) */
__declspec(naked) void particle_random_scale_r(float * p0, float p1, float p2)
{
    __asm {
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        mov eax, dword ptr [esp + 12]
        call particle_random_scale
        add esp, 8
        ret
    }
}

/* effect_slot_init_with_ftol_r: C -> original effect_slot_init_with_ftol (registers esi,s0) */
__declspec(naked) void effect_slot_init_with_ftol_r(unsigned char * p0, float p1)
{
    __asm {
        push esi
        push dword ptr [esp + 12]
        mov esi, dword ptr [esp + 12]
        call effect_slot_init_with_ftol
        add esp, 4
        pop esi
        ret
    }
}
