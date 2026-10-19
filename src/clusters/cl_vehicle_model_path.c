// cluster-entries: vehicle_model_path
/* Installed cluster vehicle_model_path (tools/cluster_install.py from output/cluster_pilot_zx/vehicle_model_path.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*00659d10*/ extern undefined g_vehicle_model_659d10;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

/* vehicle_model_path
 * Builds the vehicle model file path "toastart/vehicles/<name>.piz" in the
 * shared buffer g_vehicle_model_659d10 and returns that buffer, so the next
 * call overwrites the previous result.
 * shuriken_id: index passed to load_shuriken (inferred); returns the model name.
 */

/* "toastart/vehicles/ prefix" */

static char * load_shuriken_r(int p0);
extern void load_shuriken(void);
undefined * vehicle_model_path(undefined4 shuriken_id)

{
  RwChar *model_name;

  /* Look up the model name for this shuriken id. */
  model_name = (RwChar *)load_shuriken_r(shuriken_id);
  /* Start the buffer with the directory prefix, then append name and ".piz". */
  rwstrcpy(&g_vehicle_model_659d10,"toastart/vehicles/");
  rwstrcat(&g_vehicle_model_659d10,model_name);
  rwstrcat(&g_vehicle_model_659d10,".piz");
  return &g_vehicle_model_659d10;
}

/* load_shuriken_r: C -> original load_shuriken (registers esi) */
__declspec(naked) char * load_shuriken_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call load_shuriken
        pop esi
        ret
    }
}
