/* rwa_soft_mixer_unregister_005d8140 (0x005d8140): software mixer: destroy its interface and object definitions */
#include "mfl_tier2.h"

/*005da9e0*/ extern int rwa_soft_dsp_interface_destroy_005da9e0(void);
/*005ba420*/ extern int __cdecl FUN_005ba420(void *a1);      /* RwaObjDefDestroy */

/*007dc344*/ extern int g_rwa_soft_mixer_registered;      /* set by the registration 0x5d8ba0 */
/*007dc2d0*/ extern undefined g_data_7dc2d0;      /* the class object definition */

/* Returns 0 when the interface definition could not be destroyed (the object definition then stays). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_soft_mixer_unregister_005d8140(void)
{
  if (g_rwa_soft_mixer_registered) {
    if (!rwa_soft_dsp_interface_destroy_005da9e0())
      return 0;
    g_rwa_soft_mixer_registered = 0;
  }
  return FUN_005ba420(&g_data_7dc2d0);
}
