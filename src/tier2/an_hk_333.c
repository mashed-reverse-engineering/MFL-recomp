#include "mfl_tier2.h"
/*004c6600*/ extern void FUN_004c6600(undefined4 p0, undefined4 p1);
/*0076beb0*/ extern undefined1 g_particles_76beb0;
/*0076bfc4*/ extern undefined4 g_particles_76bfc4;
/*0076c504*/ extern undefined4 g_fx_slots_76c504;
/*0076dee0*/ extern undefined4 g_particles_76dee0;
/*0076df04*/ extern char g_fx_slots_76df04[];
/*0076c4e4*/ extern char g_fx_slots_76c4e4[];
#define PARTICLE_STATE_DWORDS (8)
#define PARTICLE_STRIDE_DWORDS (0x29)
#define FX_SLOT_STRIDE_DWORDS (0xd)
/* fx_slots_clear_8
 * Zeroes the particle and fx slot state tables. Takes no parameters and returns nothing.
 * Also calls FUN_004c6600 on g_particles_76beb0 with 0x80 (meaning of the argument unknown).
 */

/* "clears 8 dwords at 0x76DEE0" */

void fx_slots_clear_8(void)

{
  int remaining;
  undefined4 *cursor;

  /* Clear the PARTICLE_STATE_DWORDS dwords at g_particles_76dee0. */
  cursor = &g_particles_76dee0;
  for (remaining = PARTICLE_STATE_DWORDS; remaining != 0; remaining = remaining + -1) {
    *cursor = 0;
    cursor = cursor + 1;
  }
  /* Walk the particle records from g_particles_76bfc4 up to g_fx_slots_76c4e4 (inferred),
   * zeroing six fields per record (offsets -1, 0, 1, 4, 5, 7). */
  cursor = &g_particles_76bfc4;
  do {
    cursor[1] = 0;
    *cursor = 0;
    cursor[-1] = 0;
    cursor[4] = 0;
    cursor[5] = 0;
    cursor[7] = 0;
    cursor = cursor + PARTICLE_STRIDE_DWORDS;
  } while ((LONG_PTR)cursor < ((LONG_PTR)&g_fx_slots_76c4e4));
  /* Walk the fx slot records from g_fx_slots_76c504 up to g_fx_slots_76df04 (inferred),
   * zeroing five fields per record (offsets -9, -8, -7, 0, 1). */
  cursor = &g_fx_slots_76c504;
  do {
    cursor[1] = 0;
    *cursor = 0;
    cursor[-7] = 0;
    cursor[-8] = 0;
    cursor[-9] = 0;
    cursor = cursor + FX_SLOT_STRIDE_DWORDS;
  } while ((LONG_PTR)cursor < ((LONG_PTR)&g_fx_slots_76df04));
  FUN_004c6600(&g_particles_76beb0,0x80);
  return;
}


