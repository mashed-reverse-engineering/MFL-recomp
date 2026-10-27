#include "mfl_tier2.h"
extern char LAB_0049fdb0[];
/*0049fdd0*/ extern undefined __cdecl geometry_set_flag_0x40_for_materials(int p0, void *p1);
/*004c3b70*/ extern int stream_read_file();
/*004c3e40*/ extern char *FUN_004c3e40(void *p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004c6730*/ extern int piz_find_entry_size_004c6a70();
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*00886020*/ extern undefined4 g_app_886020[0x1000];
/*00886980*/ extern char g_app_886980[];
#define MODEL_BUFFER_SIZE (0x400)
#define MODEL_SLOT_STRIDE (0x14)
/* tri_model_load */
/* compile: /GS */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "Tri.dff loaded from the piz" */
/* Loads Tri.dff from the piz archive and fills the model slots in g_app_886020.
 * The whole entry is read into a MODEL_BUFFER_SIZE scratch buffer; each pass of
 * the loop parses one RpClump from it (FUN_004c3e40, inferred: next clump in the
 * buffer), stores it in the next slot (slot[0] = 0, slot[1] = clump), walks the
 * clump's atomics with the LAB_0049fdb0 callback, and runs
 * geometry_set_flag_0x40_for_materials on slot[2] (name unknown).
 * Slots are MODEL_SLOT_STRIDE dwords apart, from g_app_886020 up to g_app_886980:
 * 0x960 bytes = 30 slots of 80 bytes (inferred from the loop bound).
 * Parameters: none. Returns nothing. */
void tri_model_load(void)
{
  undefined4 entry_size;
  RpClump *clump;
  undefined4 *model_slot;
  undefined1 file_buffer [MODEL_BUFFER_SIZE];

  /* Clear the scratch buffer, then read the Tri.dff entry into it. */
  mem_zero(file_buffer,MODEL_BUFFER_SIZE);
  thunk_FUN_004c6a60();
  entry_size = piz_find_entry_size_004c6a70("Tri.dff");
  stream_read_file("Tri.dff",file_buffer,entry_size);
  model_slot = g_app_886020;
  /* One pass per clump until the slot pointer reaches g_app_886980. */
  do {
    clump = (RpClump *)FUN_004c3e40(file_buffer,MODEL_BUFFER_SIZE,0,0);
    model_slot[1] = clump;
    *model_slot = 0;
    RpClumpForAllAtomics(clump,(RpAtomicCallBack)&LAB_0049fdb0,model_slot);
    geometry_set_flag_0x40_for_materials(model_slot[2],model_slot);
    model_slot = model_slot + MODEL_SLOT_STRIDE;
  } while ((int)model_slot < ((int)&g_app_886980));
  return;
}
