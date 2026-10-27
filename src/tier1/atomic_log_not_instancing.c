#include "mfl_tier2.h"
#define ATOMIC_PLUGIN_FLAG_NO_INSTANCE 4 /* unknown exact flag name */

/*004817c0*/ extern undefined __cdecl atomic_plugin_get_flags(void *p0);
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004f7f90*/ extern RwBool __cdecl RpAtomicInstance(struct RpAtomic *p0);


/* atomic_log_not_instancing */

/* "debug print 'Not instancing atomic %d' when the atomic flags have bit 4" */
/* Attempts to instance an atomic with logging, skipping if flagged to not instance. */

RpAtomic * atomic_log_not_instancing(RpAtomic *atomic,int *atomic_index)
{
  uint plugin_flags;
  RwBool instanced;
  
  plugin_flags = atomic_plugin_get_flags(atomic);
  /* Skip instancing if atomic plugin flag prohibits it */
  if ((plugin_flags & ATOMIC_PLUGIN_FLAG_NO_INSTANCE) != 0) {
    FID_conflict__wprintf("Not instancing atomic %d\n",*atomic_index);
    *atomic_index = *atomic_index + 1;
    return atomic;
  }
  instanced = RpAtomicInstance(atomic);
  /* Check if instancing succeeded */
  if (instanced != 0) {
    FID_conflict__wprintf("\tinstancing atomic %d\n",*atomic_index);
    *atomic_index = *atomic_index + 1;
    return atomic;
  }
  /* Instancing failed */
  FID_conflict__wprintf("INSTANCE ERROR: failed to instance atomic %d\n",*atomic_index);
  return (RpAtomic *)0x0;
}
