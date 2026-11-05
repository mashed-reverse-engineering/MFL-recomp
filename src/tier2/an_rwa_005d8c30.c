/* rwa_soft_set_voice_handles_005d8c30 (0x005d8c30): software mixer: replace the list of voice handles it drives */
#include "mfl_tier2.h"

/*005b7300*/ extern int FUN_005b7300(void *handle, int param, int count, int value);      /* RwaObjHandleSetParam (16 bit) */

/*007dc348*/ extern void **g_rwa_soft_voice_handles;
/*007dc34c*/ extern RwUInt32 g_rwa_soft_voice_handle_count;

#define VOICE_PARAM_6 6

typedef struct RwaHandleList { RwUInt32 count; void **handles; } RwaHandleList;

/* list NULL = none.  Parameter 6 of every handle of the old list is set to 0 first. Returns obj unchanged. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void *rwa_soft_set_voice_handles_005d8c30(void *obj, int unused, const RwaHandleList *list)
{
  RwUInt32 i;

  (void)unused;
  if (g_rwa_soft_voice_handles)
    for (i = 0; i < g_rwa_soft_voice_handle_count; i++)
      FUN_005b7300(g_rwa_soft_voice_handles[i], VOICE_PARAM_6, 1, 0);
  if (list) {
    g_rwa_soft_voice_handle_count = list->count;
    g_rwa_soft_voice_handles = list->handles;
  } else {
    g_rwa_soft_voice_handles = 0;
    g_rwa_soft_voice_handle_count = 0;
  }
  return obj;
}
