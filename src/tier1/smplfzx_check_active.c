#include "mfl_tier2.h"
/*00496ea0*/ extern int __cdecl smplfzx_get_object(undefined4 p0);
/*004a8420*/ extern int debug_printf();
/*0071abf4*/ extern char *g_particles_71abf4;
#define SMPLFZX_ARRAY_OFFSET 0xc
#define SMPLFZX_ENTRY_SIZE 0x14


/* smplfzx_check_active */

/* "(index): prints 'SmplFzx : Not an active SmplFzx object %d' when absent" */
/* Check whether a SmplFzx object is active and return its entry state. */

undefined4 __cdecl smplfzx_check_active(int smplfzx_index,undefined4 unknown_param_2,undefined4 unknown_param_3)
{
  int smplfzx_object;
  
  /* Validate that the SmplFzx object is active */
  smplfzx_object = smplfzx_get_object(smplfzx_index);
  if (smplfzx_object == 0) {
    debug_printf("SmplFzx : Not an active SmplFzx object %d\n",smplfzx_index);
    return 0;
  }
  /* Return entry value from the particles table; specific field unknown */
  return *(undefined4 *)(*(char **)(g_particles_71abf4 + SMPLFZX_ARRAY_OFFSET) + smplfzx_index * SMPLFZX_ENTRY_SIZE);
}
