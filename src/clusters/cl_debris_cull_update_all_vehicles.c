// cluster-entries: debris_cull_update_all_vehicles
/* Installed cluster debris_cull_update_all_vehicles (tools/cluster_install.py from output/cluster_pilot_zx/debris_cull_update_all_vehicles.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0065c0d0*/ extern undefined g_game_65c0d0;
/*0065c8f0*/ extern undefined4 g_game_65c8f0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define DEBRIS_RECORD_SIZE (0x208)

/* debris_cull_update_all_vehicles */

/* "stores curCamera, updates each 0x208 byte vehicle debris record" */

/* Per-frame debris cull update. Takes no parameters and returns nothing.
   Saves the current render camera (RwCameraGetCurrentCamera()) into g_game_65c8f0,
   then calls debris_cull_update_twice on every DEBRIS_RECORD_SIZE byte record
   from g_game_65c0d0 up to g_game_65c8f0 (inferred: that global is the end of the record array). */
static int debris_cull_update_twice_r(int p0);
extern void debris_cull_update_twice(void);
void debris_cull_update_all_vehicles(void)

{
  undefined *debris_record;

  g_game_65c8f0 = RwCameraGetCurrentCamera();
  debris_record = &g_game_65c0d0;
  /* walk the record array one record at a time until the end bound */
  do {
    debris_cull_update_twice_r(debris_record);
    debris_record = debris_record + DEBRIS_RECORD_SIZE;
  } while ((LONG_PTR)debris_record < ((LONG_PTR)&g_game_65c8f0));
  return;
}

/* debris_cull_update_twice_r: C -> original debris_cull_update_twice (registers eax) */
__declspec(naked) int debris_cull_update_twice_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call debris_cull_update_twice
        ret
    }
}
