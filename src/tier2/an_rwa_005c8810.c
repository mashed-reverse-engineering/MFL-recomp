/* rwa_stream_source_interface_unregister_005c8810 (0x005c8810): stream source (RwaStreamSourceInterfaceUnregister) */
#include "mfl_tier2.h"

/*005bd9e0*/ extern int RwaObjInterfaceDefDestroy(void *def);
/*007dbec4*/ extern undefined _rwaStreamSourceIntDef;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_source_interface_unregister_005c8810(void)
{
  return RwaObjInterfaceDefDestroy(&_rwaStreamSourceIntDef);
}
