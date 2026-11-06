/* rwa_listener_interface_unregister_005c44b0 (0x005c44b0): listener interface definition destroy */
#include "mfl_tier2.h"

/*005bd9e0*/ extern int RwaObjInterfaceDefDestroy(void *def);
/*007dbc9c*/ extern undefined _rwaListenerIntDef;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_listener_interface_unregister_005c44b0(void)
{
  return RwaObjInterfaceDefDestroy(&_rwaListenerIntDef);
}
