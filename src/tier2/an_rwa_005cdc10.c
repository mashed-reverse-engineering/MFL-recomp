/* rwa_dxaudio_interface_unregister_005cdc10 (0x005cdc10): DirectX audio interface definition destroy (7dc15c) */
#include "mfl_tier2.h"

/*005bd9e0*/ extern int RwaObjInterfaceDefDestroy(void *def);
/*007dc15c*/ extern undefined g_data_7dc15c;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_dxaudio_interface_unregister_005cdc10(void)
{
  return RwaObjInterfaceDefDestroy(&g_data_7dc15c);
}
