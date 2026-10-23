// cluster-entries: fx_object_timer_subtract_thunk
/* Installed cluster fx_object_timer_subtract_thunk (tools/cluster_install.py from output/cluster_pilot_pg2/fx_object_timer_subtract_thunk.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00406d10*/ extern void __cdecl fx_object_timer_subtract(void *fx_object, float amount);

/* one effect object of the fx table g_fx_objects (record stride 0xec) */
typedef struct FxObject {
  unsigned char unknown_00[0x5c];
  float timer;                      /* 0x5c: fx_object_timer_subtract lowers it */
  unsigned char unknown_60[0x8c];
} FxObject;
/*00651250*/ extern FxObject g_fx_objects[9];

/* fx_object_timer_subtract_thunk: subtracts `amount` from the timer of fx object number `index` */
void fx_object_timer_subtract_thunk(int index, float amount)
{
  fx_object_timer_subtract(&g_fx_objects[index], amount);
}

