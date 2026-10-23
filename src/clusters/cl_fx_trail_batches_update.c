// cluster-entries: fx_trail_batches_update
/* Installed cluster fx_trail_batches_update (tools/cluster_install.py from output/cluster_pilot_pg3/fx_trail_batches_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004846e0*/ extern void __cdecl point_lerp_to_aux_camera(float *out, float *in, float amount);
/*0049b870*/ extern void __cdecl particle_batch_mark_current_active(void *batch);
/*0049b890*/ extern void __cdecl particle_batch_add(void *batch, float *position, unsigned char *colour);
/*0049b910*/ extern void __cdecl particle_batch_set_uv_a(void *batch, float *uv);
/*0049b940*/ extern void __cdecl particle_batch_set_size(void *batch, int size);
/*0049b960*/ extern void __cdecl particle_batch_set_uv_b_c(void *batch, float *uv_b, float *uv_c);
/*0049b9b0*/ extern void __cdecl particle_batch_clear(void *batch);
/*0049bd50*/ extern void __cdecl fx_system_flush(void *batch);
/*007ef7f4*/ extern float g_tick_seconds;
/*005e0850*/ extern float g_effects_update_5e0850[];     /* uv rectangles, 4 floats per entry: b at +0, c at +8 */
/*005dd628*/ extern float k_f_0_25;
/*005ddb0c*/ extern float k_f_15;
/*005dd7e4*/ extern float k_f_0_03;
/*005dd62c*/ extern float k_f_100;

#define FX_TRAIL_SYSTEM_COUNT 4
#define FX_TRAIL_SLOT_COUNT 7
#define FX_TRAIL_HISTORY 4                  /* positions kept per slot */
#define FX_TRAIL_UV_COUNT 16

/* one trail system (stride 0x1e8): a particle batch and 7 trails of 4 positions each */
typedef struct FxTrailSystem {
  void *batch;                                                     /* 0x000 */
  float history[FX_TRAIL_SLOT_COUNT][FX_TRAIL_HISTORY][3];         /* 0x004: newest position first */
  int active[FX_TRAIL_SLOT_COUNT];                                 /* 0x154 */
  float age[FX_TRAIL_SLOT_COUNT];                                  /* 0x170 */
  float lifetime[FX_TRAIL_SLOT_COUNT];                             /* 0x18c */
  float fade[FX_TRAIL_SLOT_COUNT];                                 /* 0x1a8 */
  int part[FX_TRAIL_SLOT_COUNT];                                   /* 0x1c4: vehicle part the trail follows */
  unsigned char unknown_1e0[8];
} FxTrailSystem;
/*007519e4*/ extern FxTrailSystem g_particles_7519e4[FX_TRAIL_SYSTEM_COUNT];

/* fx_trail_batches_update: per trail system and slot: ages the trail (and switches it off when it is older than its lifetime), shifts the
   position history and records the position of the vehicle part; then rebuilds the particle batch of the system from the active trails
   (4 particles per trail, fading alpha and uv) and flushes it */
static void vehicle_part_position_by_index_r(int p0, int p1, float * p2);
extern void vehicle_part_position_by_index(void);
void fx_trail_batches_update(void)
{
  /* one frame object as the original lays it out (the batch calls read the words behind the pointers they get) */
  struct {
    unsigned char colour[4];
    int k;
    int system_index;
    float k_float;
    float age_fraction;
    float uv[2];
    int unknown_40;
    float fade;
    float position[3];
    float out[3];
  } frame;
  FxTrailSystem *system;
  int slot;
  int k;
  int uv_index;

  frame.system_index = 0;
  for (system = g_particles_7519e4; system < g_particles_7519e4 + FX_TRAIL_SYSTEM_COUNT; system++) {
    for (slot = 0; slot < FX_TRAIL_SLOT_COUNT; slot++) {
      if (system->active[slot] != 0) {
        system->age[slot] = g_tick_seconds + system->age[slot];
        if (system->age[slot] >= system->lifetime[slot]) {
          system->active[slot] = 0;
        }
        vehicle_part_position_by_index_r(system->part[slot], frame.system_index, frame.out);
        for (k = FX_TRAIL_HISTORY - 1; k > 0; k--) {
          system->history[slot][k][0] = system->history[slot][k - 1][0];
          system->history[slot][k][1] = system->history[slot][k - 1][1];
          system->history[slot][k][2] = system->history[slot][k - 1][2];
        }
        system->history[slot][0][0] = frame.out[0];
        system->history[slot][0][1] = frame.out[1];
        system->history[slot][0][2] = frame.out[2];
      }
    }
    for (slot = 0; slot < FX_TRAIL_SLOT_COUNT; slot++) {
      if (system->active[slot] == 0) {
        particle_batch_clear(system->batch);
      } else {
        for (frame.k = 0; frame.k < FX_TRAIL_HISTORY; frame.k++) {
          frame.k_float = (float)frame.k;
          frame.age_fraction = system->age[slot] / system->lifetime[slot];
          uv_index = (int)(k_f_15 * (frame.k_float * k_f_0_25));
          frame.position[0] = system->history[slot][frame.k][0];
          frame.position[1] = system->history[slot][frame.k][1];
          frame.position[2] = system->history[slot][frame.k][2];
          frame.fade = system->fade[slot];
          frame.colour[0] = 0xff;
          frame.colour[1] = 0xff;
          frame.colour[2] = 0xff;
          if (slot != 5 && slot != 6) {
            frame.position[1] = frame.k_float * k_f_0_03 + frame.position[1];
          }
          frame.colour[3] = (unsigned char)((1.0f - frame.k_float * k_f_0_25) * k_f_100);
          point_lerp_to_aux_camera(frame.position, frame.position, 0.02f);
          particle_batch_add(system->batch, frame.position, frame.colour);
          frame.uv[0] = (1.0f - frame.age_fraction) * frame.fade;
          frame.uv[1] = frame.uv[0];
          particle_batch_set_uv_a(system->batch, frame.uv);
          particle_batch_set_size(system->batch, 0);
          particle_batch_set_uv_b_c(system->batch, &g_effects_update_5e0850[uv_index * 4], &g_effects_update_5e0850[uv_index * 4 + 2]);
        }
        particle_batch_mark_current_active(system->batch);
      }
    }
    fx_system_flush(system->batch);
    frame.system_index++;
  }
}

/* vehicle_part_position_by_index_r: C -> original vehicle_part_position_by_index (registers eax,ecx,esi) */
__declspec(naked) void vehicle_part_position_by_index_r(int p0, int p1, float * p2)
{
    __asm {
        push esi
        mov eax, dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 12]
        mov esi, dword ptr [esp + 16]
        call vehicle_part_position_by_index
        pop esi
        ret
    }
}
