#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define FX_ACTIVE_FLAG_IDX (0x15) /* word index (inferred from the comment below) */
#define FX_PARTICLES_IDX (4) /* word index of the embedded particle buffer */
/* fx_system_destroy */

/* "clears the active flag and destroys the particle buffer"
 * Tears down one FX system: fx_system is the system (undefined4 words), returns nothing.
 * The system's first word is a buffer pointer that is freed with the engine's rwfree. */

void fx_system_destroy(undefined4 *fx_system)

{
  /* clear the active flag (inferred), then mark word 0x16 (inferred: state = 1) */
  fx_system[FX_ACTIVE_FLAG_IDX] = 0;
  fx_system[0x16] = 1;
  /* destroy the embedded particle buffer */
  particles_destroy(fx_system + FX_PARTICLES_IDX);
                    /* WARNING: Could not recover jumptable at 0x0049bdbc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  RwFree(*(void **)fx_system);
  return;
}


