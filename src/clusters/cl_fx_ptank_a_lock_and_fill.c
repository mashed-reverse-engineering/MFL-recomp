// cluster-entries: fx_ptank_a_lock_and_fill
/* Installed cluster fx_ptank_a_lock_and_fill (tools/cluster_install.py from output/cluster_pilot_pg3/fx_ptank_a_lock_and_fill.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00475730*/ extern void *__cdecl get_viewport_camera(int index);
/*0049a960*/ extern int __cdecl RpPTankAtomicLock_00545d10(void *atomic, void *lock, int flags, int access);
/*0049a970*/ extern int __cdecl RpPTankAtomicUnlock_00545f20(void *atomic);
/*0049af20*/ extern void __cdecl fx_ptank_b_lock_and_fill(unsigned char *world);
/*004d5290*/ extern float __cdecl RwV3dNormalize(float *out, float *in);
/*004d5670*/ extern void __cdecl RwV3dTransformPoints(float *out, float *in, int count, RwMatrix *matrix);
/*00759698*/ extern unsigned char *g_fx_ptank_atomic_a;
/*007596a0*/ extern int g_particles_7596a0;           /* the particle systems are enabled */
/*007596a4*/ extern int g_particles_7596a4;           /* the ptank atomics are locked this frame */
/*007da6bc*/ extern int g_ptank_plugin_offset;
/*0062a940*/ extern unsigned int g_race_update_62a940[];        /* uv rectangles, 4 words each */
/*005dd32c*/ extern float k_f_3;

/* RpPTankAtomicLock flags */
#define PTANK_LOCK_POSITION 1
#define PTANK_LOCK_SIZE 4
#define PTANK_LOCK_COLOUR 2
#define PTANK_LOCK_UV 0x80
#define PTANK_LOCK_WRITE 0x40000000
#define PTANK_ACTIVE_COUNT_OFFSET 4
#define PTANK_FLAGS_OFFSET 0x40
#define PTANK_FLAG_UPDATED 0x800000
#define FX_PARTICLE_COUNT 128
#define SIZE_TABLE_PULSE_INDEX 4            /* entry 4 of the size table uses the "pulse" size curve */

/* the result of RpPTankAtomicLock: where the particle data of one component is written and the distance between particles */
typedef struct PTankLock {
  unsigned char *data;
  int stride;
} PTankLock;

/* one effect particle emitter (stride 0x38) */
typedef struct FxParticle {
  unsigned char style[4];        /* 0x00: handed to fx_particle_style_pulse */
  float position[3];             /* 0x04 */
  int count;                     /* 0x10: ptank particles it writes */
  int active;                    /* 0x14 */
  float alpha;                   /* 0x18 */
  int unknown_1c;
  float *sizes;                  /* 0x20: size per particle (entry 4: pulse size) */
  int *uv_index;                 /* 0x24: uv rectangle per particle */
  unsigned char *colours;        /* 0x28: RGBA per particle */
  RwMatrix *matrix;              /* 0x2c: transform of the emitter, 0 = world space */
  int unknown_30[2];
} FxParticle;
/*008869a0*/ extern FxParticle g_particles_8869a0[FX_PARTICLE_COUNT];

/* the effect world object: a direction (+0x20) and an origin (+0x30) */
typedef struct FxWorld {
  unsigned char unknown_00[0x20];
  float direction[3];            /* 0x20 */
  unsigned char unknown_2c[4];
  float origin[3];               /* 0x30 */
} FxWorld;

/* fx_ptank_a_lock_and_fill: once per frame locks the PTank atomic a and writes the particles of every active emitter (position along the
   direction to the viewpoint, size by style, colour, uv), then unlocks it, flags it as updated and stores the particle count; then does the
   same for atomic b */
static void fx_particle_style_pulse_r(void * p0, float * p1);
extern void fx_particle_style_pulse(void);
void fx_ptank_a_lock_and_fill(FxWorld *world)
{
  /* one frame object as in the original: the lock results, then the positions (callees write through pointers into it) */
  struct {
    float scale;
    float length;
    PTankLock lock[4];                  /* position, size, colour, uv */
    float position[3];
    float centre[3];
    float direction[3];
  } frame;
  FxParticle *particle;
  unsigned char *atomic;
  int total;
  int j;
  int uv;

  frame.position[0] = 0.0f;
  frame.position[1] = 0.0f;
  frame.position[2] = 0.0f;
  get_viewport_camera(0);
  if (g_particles_7596a0 != 0 && g_particles_7596a4 == 0) {
    g_particles_7596a4 = 1;
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_a, &frame.lock[0], PTANK_LOCK_POSITION, PTANK_LOCK_WRITE);
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_a, &frame.lock[1], PTANK_LOCK_SIZE, PTANK_LOCK_WRITE);
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_a, &frame.lock[2], PTANK_LOCK_COLOUR, PTANK_LOCK_WRITE);
    RpPTankAtomicLock_00545d10(g_fx_ptank_atomic_a, &frame.lock[3], PTANK_LOCK_UV, PTANK_LOCK_WRITE);
    total = 0;
    frame.centre[0] = (float)(world->direction[0] * k_f_3 + world->origin[0]);
    frame.centre[1] = (float)(world->direction[1] * k_f_3 + world->origin[1]);
    frame.centre[2] = (float)(world->direction[2] * k_f_3 + world->origin[2]);
    for (particle = g_particles_8869a0; particle < g_particles_8869a0 + FX_PARTICLE_COUNT; particle++) {
      if (particle->active == 0) {
        continue;
      }
      if (particle->matrix != 0) {
        RwV3dTransformPoints(frame.position, particle->position, 1, particle->matrix);
        frame.scale = particle->matrix->at.x * world->direction[0] + particle->matrix->at.y * world->direction[1] + particle->matrix->at.z * world->direction[2];
      } else {
        frame.position[0] = particle->position[0];
        frame.position[1] = particle->position[1];
        frame.position[2] = particle->position[2];
        frame.scale = 1.0f;
      }
      fx_particle_style_pulse_r(particle, &frame.scale);
      frame.scale = frame.scale * particle->alpha;
      if (frame.scale < 0.0f) {
        frame.scale = 0.0f;
      }
      if (frame.lock[0].data == 0) {
        continue;
      }
      frame.direction[0] = frame.position[0] - frame.centre[0];
      frame.direction[1] = frame.position[1] - frame.centre[1];
      frame.direction[2] = frame.position[2] - frame.centre[2];
      frame.length = RwV3dNormalize(frame.direction, frame.direction);
      for (j = 0; j < particle->count; j++) {
        ((float *)frame.lock[0].data)[0] = frame.length * frame.direction[0] + frame.centre[0];
        ((float *)frame.lock[0].data)[1] = frame.length * frame.direction[1] + frame.centre[1];
        ((float *)frame.lock[0].data)[2] = frame.length * frame.direction[2] + frame.centre[2];
        frame.lock[0].data += frame.lock[0].stride;
        if (j == SIZE_TABLE_PULSE_INDEX) {
          ((float *)frame.lock[1].data)[0] = (frame.scale * 10.0f + 2.0f) * particle->sizes[4] * frame.scale;
          ((float *)frame.lock[1].data)[1] = frame.scale * 0.5f * particle->sizes[4] * frame.scale;
        } else {
          ((float *)frame.lock[1].data)[0] = (frame.scale + 1.0f) * particle->sizes[j] * frame.scale;
          ((float *)frame.lock[1].data)[1] = (frame.scale + 1.0f) * particle->sizes[j] * frame.scale;
        }
        frame.lock[1].data += frame.lock[1].stride;
        frame.lock[2].data[0] = particle->colours[j * 4];
        frame.lock[2].data[1] = particle->colours[j * 4 + 1];
        frame.lock[2].data[2] = particle->colours[j * 4 + 2];
        frame.lock[2].data[3] = particle->colours[j * 4 + 3];
        frame.lock[2].data += frame.lock[2].stride;
        uv = particle->uv_index[j];
        ((unsigned int *)frame.lock[3].data)[0] = g_race_update_62a940[uv * 4];
        ((unsigned int *)frame.lock[3].data)[1] = g_race_update_62a940[uv * 4 + 1];
        ((unsigned int *)frame.lock[3].data)[2] = g_race_update_62a940[uv * 4 + 2];
        ((unsigned int *)frame.lock[3].data)[3] = g_race_update_62a940[uv * 4 + 3];
        frame.lock[3].data += frame.lock[3].stride;
        total++;
      }
    }
    atomic = g_fx_ptank_atomic_a;
    RpPTankAtomicUnlock_00545f20(atomic);
    *(unsigned int *)(*(unsigned char **)(g_ptank_plugin_offset + g_fx_ptank_atomic_a) + PTANK_FLAGS_OFFSET) |= PTANK_FLAG_UPDATED;
    *(int *)(*(unsigned char **)(g_ptank_plugin_offset + g_fx_ptank_atomic_a) + PTANK_ACTIVE_COUNT_OFFSET) = total;
    fx_ptank_b_lock_and_fill((unsigned char *)world);
  }
}

/* fx_particle_style_pulse_r: C -> original fx_particle_style_pulse (registers eax,edx) */
__declspec(naked) void fx_particle_style_pulse_r(void * p0, float * p1)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov edx, dword ptr [esp + 8]
        call fx_particle_style_pulse
        ret
    }
}
