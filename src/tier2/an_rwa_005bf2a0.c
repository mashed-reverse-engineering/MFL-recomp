/* rwa_virtual_voice_interface_unregister_005bf2a0 (0x005bf2a0): virtual voices (RwaVirtualVoiceInterfaceUnregister) */
#include "mfl_tier2.h"

/*005bd9e0*/ extern int RwaObjInterfaceDefDestroy(void *def);
/*007dbbf4*/ extern undefined _rwaVirtualVoiceIntDef;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_virtual_voice_interface_unregister_005bf2a0(void)
{
  return RwaObjInterfaceDefDestroy(&_rwaVirtualVoiceIntDef);
}
