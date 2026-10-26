// cluster-entries: fx_course_specific_update
/* Installed cluster fx_course_specific_update (tools/cluster_install.py from output/cluster_pilot_uc3/fx_course_specific_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042b110*/ extern int current_course_id(void);
/*0040fdc0*/ extern int race_get_phase(void);
/*004220f0*/ extern void vehicle_get_position_158(int vehicle, float *position);
/*00419fa0*/ extern double ai_get_float_8c9980(void);
extern float g_tick_seconds;
extern float k_f_0_011111111;     /* 1/90 and 100 as data (not folded into one constant: the original multiplies twice in extended precision) */
extern float k_f_100;

#define COURSE_ARCTIC 0
#define COURSE_CITY 0x1a

/* The course effect object (named fields only for what is read here). */
typedef struct CourseFx {
  char pad_0[0x4c];
  float progress;          /* +0x4c */
  char pad_50[0x10];
  float level;             /* +0x60 */
  char pad_64[4];
  float level_low;         /* +0x68 */
  char pad_6c[4];
  float field_70;
  char pad_74[0x20];
  float field_94;
  char pad_98[0xc];
  int active;              /* +0xa4 */
  float active_timer;      /* +0xa8 */
  float idle_time;         /* +0xac */
  char pad_b0[0x20];
  float race_time;         /* +0xd0 seconds */
} CourseFx;

/* Per frame update of the course effect `fx` (arctic / city have timed windows in which it stays off): switches the effect on after a random-ish idle
   time (shorter at higher AI skill) or when level - level_low > 0.2 (and runs it 4 seconds), off when its timer ran out or the race is not in phase 6;
   at the progress percent 50 / above 98 with level >= 0.95 it is forced on; above 3 percent it respawns its slot and a burst at the vehicle position. */
static void fx_reset_slot_r(struct CourseFx * p0, float * p1);
extern void fx_reset_slot(void);
static void fx_random_burst_r(struct CourseFx * p0, float * p1);
extern void fx_random_burst(void);
void fx_course_specific_update_n(CourseFx *fx)
{
  float position[4];
  float difference;
  float idle;
  double limit;
  int percent;
  int blocked = 0;

  percent = (int)(fx->progress * k_f_0_011111111 * k_f_100);
  vehicle_get_position_158(0, position);
  if (current_course_id() == COURSE_ARCTIC) {
    if ((fx->race_time > 12.0f && fx->race_time < 16.0f) || (fx->race_time > 24.0f && fx->race_time < 27.0f)) {
      blocked = 1;
    }
  } else if (current_course_id() == COURSE_CITY) {
    if (fx->race_time > 20.5f && fx->race_time < 25.0f) {
      blocked = 1;
    }
  }
  if (race_get_phase() != RACE_PHASE_RACING || blocked) {
    fx->active = 0;
    fx->idle_time = 0.0f;
  } else if (fx->active != 0) {
    fx->idle_time = 0.0f;
    fx->active_timer = fx->active_timer - g_tick_seconds;
    if (fx->active_timer <= 0.0f) {
      fx->active = 0;
    }
  } else {
    limit = (1.0f - ai_get_float_8c9980() * 0.1f) * 5.0f;
    difference = fx->level - fx->level_low;
    idle = g_tick_seconds + fx->idle_time;
    fx->idle_time = idle;
    if (limit < 1.0f) {
      limit = 1.0f;
    }
    if (idle > limit || difference > 0.2f) {
      fx->active = 1;
      fx->active_timer = 4.0f;
    }
  }
  if (percent == 0x32) {
    fx->field_94 = fx->field_94 + 0.03f;
  }
  if ((percent != 0x32 && percent >= 0x63) || fx->level >= 0.95f) {
    fx->field_70 = 1.0f;
    fx->field_94 = 1.0f;
    fx->active = 1;
  }
  if (percent > 3) {
    fx_reset_slot_r(fx, position);
    fx_random_burst_r(fx, position);
  }
}

/* fx_reset_slot_r: C -> original fx_reset_slot (registers ecx,eax) */
__declspec(naked) void fx_reset_slot_r(struct CourseFx * p0, float * p1)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        mov eax, dword ptr [esp + 8]
        call fx_reset_slot
        ret
    }
}

/* fx_random_burst_r: C -> original fx_random_burst (registers esi,ecx) */
__declspec(naked) void fx_random_burst_r(struct CourseFx * p0, float * p1)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 12]
        call fx_random_burst
        pop esi
        ret
    }
}

/* entry thunk at the original address of fx_course_specific_update: register inputs (eax) -> natural C fx_course_specific_update_n; ret 0 */
__declspec(naked) void fx_course_specific_update(void)
{
    __asm {
        push eax
        call fx_course_specific_update_n
        add esp, 4
        ret
    }
}
