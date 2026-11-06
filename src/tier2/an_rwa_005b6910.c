/* rwa_virtual_voice_unregister_005b6910 (0x005b6910): virtual voices (RwaVirtualVoiceUnregister): interface definition (if registered), then the object definition */
#include "mfl_tier2.h"

/*005bf2a0*/ extern int RwaVirtualVoiceInterfaceUnregister(void);
/*005ba420*/ extern void RwaObjDefDestroy(void *def);
/*007dab24*/ extern RwUInt32 rwavirtualvoiceobj_objDefRegistered;
/*007daac0*/ extern undefined g_data_7daac0;      /* the virtual voice definition */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_virtual_voice_unregister_005b6910(void)
{
  if (rwavirtualvoiceobj_objDefRegistered) {
    if (!RwaVirtualVoiceInterfaceUnregister())
      return;
    rwavirtualvoiceobj_objDefRegistered = 0;
  }
  RwaObjDefDestroy(&g_data_7daac0);
}
