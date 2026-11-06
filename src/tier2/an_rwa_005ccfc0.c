/* rwa_dxaudio_interface_unregister_005ccfc0 (0x005ccfc0): DirectX audio interface definition destroy (7dc134) */
#include "mfl_tier2.h"

/*005bd9e0*/ extern int RwaObjInterfaceDefDestroy(void *def);
/*007dc134*/ extern undefined g_data_7dc134;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_dxaudio_interface_unregister_005ccfc0(void)
{
  return RwaObjInterfaceDefDestroy(&g_data_7dc134);
}
