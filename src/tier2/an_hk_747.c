#include "mfl_tier2.h"
/*00659d10*/ extern undefined g_vehicle_model_659d10;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
static char * load_shuriken_r(int reg_esi);
extern void load_shuriken(void);
__declspec(naked) char * load_shuriken_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call load_shuriken
        pop esi
        ret
    }
}

/* vehicle_model_path
 * Builds the vehicle model file path "toastart/vehicles/<name>.piz" in the
 * shared buffer g_vehicle_model_659d10 and returns that buffer, so the next
 * call overwrites the previous result.
 * shuriken_id: index passed to load_shuriken (inferred); returns the model name.
 */

/* "toastart/vehicles/ prefix" */

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


