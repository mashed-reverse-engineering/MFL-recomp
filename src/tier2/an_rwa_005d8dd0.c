/* rwa_soft_output_unregister_005d8dd0 (0x005d8dd0): software output: destroy its interface and object definitions */
#include "mfl_tier2.h"

/*005db500*/ extern int rwa_soft_dsp2_interface_destroy_005db500(void);
/*005ba420*/ extern int __cdecl FUN_005ba420(void *a1);      /* RwaObjDefDestroy */

/*007dc3cc*/ extern int g_rwa_soft_output_registered;      /* set by the registration 0x5d9300 */
/*007dc370*/ extern undefined g_data_7dc370;      /* the class object definition */

/* Returns 0 when the interface definition could not be destroyed (the object definition then stays). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_soft_output_unregister_005d8dd0(void)
{
  if (g_rwa_soft_output_registered) {
    if (!rwa_soft_dsp2_interface_destroy_005db500())
      return 0;
    g_rwa_soft_output_registered = 0;
  }
  return FUN_005ba420(&g_data_7dc370);
}
