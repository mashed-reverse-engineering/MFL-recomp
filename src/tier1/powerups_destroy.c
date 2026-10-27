#include "mfl_tier2.h"
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00486710*/ extern int particles_destroy();
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a8bc0*/ extern struct Powerup g_powerups[25];
/*006a9390*/ extern undefined g_powerup_glow;
/*006a93d4*/ extern undefined4 g_powerup_icons_txd;


/* powerups_destroy */

/* destroys the 25 clumps and the particle system */

void powerups_destroy(void)
{
  Powerup *pPVar1;
  
  pPVar1 = g_powerups;
  do {
    RpClumpDestroy((RpClump *)pPVar1->clump);
    pPVar1 = pPVar1 + 1;
  } while ((int)pPVar1 < ((int)&g_powerup_glow));
  particles_destroy(&g_powerup_glow);
  RwTexDictionaryDestroy_004d5ac0(g_powerup_icons_txd);
  return;
}
