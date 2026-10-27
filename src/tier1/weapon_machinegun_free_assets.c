#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a60d0*/ extern struct MachinegunState MachinegunState_ARRAY_006a60d0[4];
/*006a61f0*/ extern undefined g_weapon_6a61f0;
/*006a61f8*/ extern char g_weapon_machinegun_6a61f8[];
#define MACHINEGUN_STATE_STRIDE 0x12
#define MACHINEGUN_STATE_CLUMP_END ((int)&g_weapon_machinegun_6a61f8)


/* weapon_machinegun_free_assets */
/* Frees 3D clump models for all machinegun states and destroys particle assets. */
void weapon_machinegun_free_assets(void)
{
  uint *clump_ptr;
  
  clump_ptr = &MachinegunState_ARRAY_006a60d0[0].clump;
  do {
    /* Destroy 3D model clump for current machinegun state */
    RpClumpDestroy((RpClump *)*clump_ptr);
    clump_ptr = clump_ptr + MACHINEGUN_STATE_STRIDE;
  } while ((int)clump_ptr < MACHINEGUN_STATE_CLUMP_END);
  /* Destroy weapon particle effects */
  particles_destroy(&g_weapon_6a61f0);
  return;
}
