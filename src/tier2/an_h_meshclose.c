#include "mfl_tier2.h"
/* RenderWare mesh module close (0x500bc0, engine plugin destructor): the last instance destroys the mesh free list.
   Written from the disassembly of MFL.exe.  fnsizes: the function is 0x2e bytes (the old row split it at 0x500bcd). */

typedef struct RwModuleInfo {
  RwInt32 globals_offset;
  RwInt32 num_instances;
} RwModuleInfo;

/*00947778*/ extern RwModuleInfo meshModule;
/*007d53c0*/ extern void *MeshStatic;                /* the mesh free list */
/*004e92b0*/ extern RwBool RwFreeListDestroy(void *free_list);

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void *rpMeshClose(void *instance, RwInt32 offset, RwInt32 size)
{
  meshModule.num_instances--;
  if (meshModule.num_instances == 0 && MeshStatic != NULL) {
    RwFreeListDestroy(MeshStatic);
    MeshStatic = NULL;
  }
  return instance;
}
