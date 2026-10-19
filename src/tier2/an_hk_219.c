#include "mfl_tier2.h"
/*00421d50*/ extern int vehicle_prepare_parts();
/*00486710*/ extern int particles_destroy();
/*004c4950*/ extern void FUN_004c4950(undefined4 p0);
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*0064d88c*/ extern undefined4 g_game_64d88c;
/*00659d60*/ extern undefined4 g_game_659d60;
/*00659d74*/ extern undefined4 g_game_659d74;
/*00659d78*/ extern undefined4 g_game_659d78;
/*0065a9b0*/ extern undefined4 g_load_65a9b0;
/*0065a9b4*/ extern undefined4 g_load_65a9b4;
/*0065a9cc*/ extern undefined4 g_particles_65a9cc;
/*0065a9d0*/ extern undefined4 g_particles_65a9d0;
/*00659d70*/ extern char g_particles_659d70[];

#define VEHICLE_COUNT (4)

/* vehicles_prepare_all
 *
 * Prepares every vehicle, then tears down the frame slots and particle
 * effects and, when g_game_64d88c is 0, the two load handles, clearing each
 * global after it is released. No parameters, no return value.
 */

/* "vehicle_prepare_parts for every vehicle" */

void vehicles_prepare_all(void)

{
  int vehicle_index;
  undefined4 *frame_slot;

  /* Prepare each vehicle in turn. */
  vehicle_index = 0;
  do {
    vehicle_prepare_parts(vehicle_index);
    vehicle_index = vehicle_index + 1;
  } while (vehicle_index < VEHICLE_COUNT);
  /* Destroy each RwFrame in the frame-slot globals (inferred: g_game_659d60 up to g_particles_659d70) and clear the slot. */
  frame_slot = &g_game_659d60;
  do {
    RwFrameDestroy((RwFrame *)*frame_slot);
    *frame_slot = 0;
    frame_slot = frame_slot + 1;
  } while ((LONG_PTR)frame_slot < ((LONG_PTR)&g_particles_659d70));
  /* Destroy the two particle effects. */
  particles_destroy(g_particles_65a9cc);
  g_particles_65a9cc = 0;
  particles_destroy(g_particles_65a9d0);
  g_particles_65a9d0 = 0;
  /* Release the load handles only when g_game_64d88c is 0 (inferred: meaning of the flag unknown). */
  if (g_game_64d88c == 0) {
    FUN_004c4950(g_load_65a9b0);
    g_load_65a9b0 = 0;
    g_game_659d74 = 0;
    FUN_004c4950(g_load_65a9b4);
    g_load_65a9b4 = 0;
    g_game_659d78 = 0;
  }
  return;
}


