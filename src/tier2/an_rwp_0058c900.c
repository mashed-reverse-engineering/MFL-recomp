/* rwp_atomic_plugin_attach (0x0058c900): register the RpAtomic plugin that holds the Rwp object of an atomic */
#include "mfl_tier2.h"

/*007daa18*/ extern int g_data_7daa18;   /* offset of the Rwp object slot in RpAtomic */
/*0063a5f0*/ extern void *PTR_LAB_0063a5f0;   /* the object sync callback of the Rwp layer */
/*004f7e80*/ extern int RpAtomicRegisterPlugin(int size, unsigned int pluginID, void *ctor, void *dtor, void *copy);
/*0053eac0*/ extern void *FUN_0053eac0(void *object, int offset, int size);   /* returns object (no-op plugin destructor / copy) */
/*0058c970*/ extern void *DMorphGeometryCtor(void *atomic, int offset, int size);   /* (misnamed) the plugin constructor: clears the slot */
/*0058c940*/ extern void *FUN_0058c940(void *obj);
#define RWP_ATOMIC_PLUGIN_ID 0x901

/* Registers the 4 byte atomic plugin and installs the object sync callback; returns TRUE. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_atomic_plugin_attach(void)
{
  g_data_7daa18 = RpAtomicRegisterPlugin(4, RWP_ATOMIC_PLUGIN_ID, DMorphGeometryCtor, FUN_0053eac0, FUN_0053eac0);
  PTR_LAB_0063a5f0 = FUN_0058c940;
  return 1;
}
