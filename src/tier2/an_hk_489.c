#include "mfl_tier2.h"
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*005dd330*/ extern float float_1_0;
/*005ddc28*/ extern float k_f_0;
/*0062ac80*/ extern undefined g_weapon_62ac80;
/*007ef7f4*/ extern float g_tick_seconds;

/* byte offsets inside the rocket object */
#define ROCKET_STATE_OFF (0x1c) /* inferred: cleared together with the timer */
#define ROCKET_OWNER_PTR_OFF (0x20) /* points to a struct whose +4 is the RwFrame */
#define ROCKET_SPIN_ANGLE_OFF (0x24) /* float, grows by 1.0 each tick */
#define ROCKET_TIMER_OFF (0x28) /* float, counts down by g_tick_seconds */
#define ROCKET_SPIN_DEG (25.0) /* rotation applied to the frame per tick */

/* weapon_rocket_spin_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Per-tick update of a flying rocket's spin: advances the spin angle, rotates the rocket's frame
 * by ROCKET_SPIN_DEG, and counts the timer down by g_tick_seconds. When the timer runs out, the
 * timer and the state field are cleared.
 * rocket: pointer to the rocket object (fields at the ROCKET_*_OFF offsets). */

void __cdecl weapon_rocket_spin_update(char *rocket)

{
  RwFrame *frame;
  float timer;

  /* the frame hangs off the object at ROCKET_OWNER_PTR_OFF (+4) */
  frame = *(RwFrame **)(*(char **)(rocket + ROCKET_OWNER_PTR_OFF) + 4);
  *(float *)(rocket + ROCKET_SPIN_ANGLE_OFF) = *(float *)(rocket + ROCKET_SPIN_ANGLE_OFF) + 1.0f;
  RwFrameRotate(frame,(RwV3d *)&g_weapon_62ac80,ROCKET_SPIN_DEG,rwCOMBINEPRECONCAT);
  timer = *(float *)(rocket + ROCKET_TIMER_OFF) - g_tick_seconds;
  *(float *)(rocket + ROCKET_TIMER_OFF) = timer;
  /* timer expired: clear it and the state field */
  if (timer <= k_f_0) {
    *(undefined4 *)(rocket + ROCKET_TIMER_OFF) = 0;
    *(undefined4 *)(rocket + ROCKET_STATE_OFF) = 0;
  }
  return;
}


