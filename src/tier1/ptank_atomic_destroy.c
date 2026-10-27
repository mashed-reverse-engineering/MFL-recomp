#include "mfl_tier2.h"
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*00545d00*/ extern RwBool RpAtomicDestroy_00545d00(struct RpAtomic *p0);
/*006f6930*/ extern undefined4 g_rw_6f6930;
/*006f693c*/ extern RpAtomic *g_ptank_atomic;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0088a844*/ extern undefined4 g_rw_88a844;
/* byte size of one entry in the g_rw_88a844 array (inferred: each entry is read as a void * at its start) */
#define PTR_ARRAY_ENTRY_SIZE (8)
/* ptank_atomic_destroy */
/* Teardown for the ptank atomic. Destroys g_ptank_atomic (0x6f693c) and its parent frame, then frees
   each pointer in the g_rw_88a844 array (g_rw_6f6930 entries) and the array itself, and resets the count.
   Takes no parameters, returns nothing. (Old comment said g_rw_6f693c; the global at 0x6f693c is g_ptank_atomic.) */
void ptank_atomic_destroy(void)
{
  RwFrame *frame;
  int entry_index;

  /* destroy the atomic and its frame; clear the global before the frame goes */
  if (g_ptank_atomic != (RpAtomic *)0x0) {
    frame = RpAtomic_object_object_parent(g_ptank_atomic);
    RpAtomicDestroy_00545d00(g_ptank_atomic);
    g_ptank_atomic = (RpAtomic *)0x0;
    RwFrameDestroy(frame);
  }
  /* free each pointer in the array (loop over g_rw_6f6930 entries), then the array itself */
  entry_index = 0;
  if (0 < (int)g_rw_6f6930) {
    do {
      RwFree(*(void **)((int)g_rw_88a844 + entry_index * PTR_ARRAY_ENTRY_SIZE));
      entry_index = entry_index + 1;
    } while (entry_index < (int)g_rw_6f6930);
  }
  RwFree(g_rw_88a844);
  g_rw_6f6930 = 0;
  return;
}
