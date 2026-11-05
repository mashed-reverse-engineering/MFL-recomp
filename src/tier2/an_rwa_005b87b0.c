/* rwa_stream_update_thread_005b87b0 (0x005b87b0): streams: the stream update thread: run every active stream's start / finish execute each 17 ms until told to stop */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
extern __declspec(dllimport) void __stdcall Sleep(int milliseconds);
/*005bac70*/ extern void *_rwaObjStartExecute(void *obj, int flag);
/*005bace0*/ extern void *_rwaObjFinishExecute(void *obj, int flag);

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/*007dac24*/ extern RwaLink *g_data_7dac24;      /* active streams list head (link at stream+0x14) */
/*007dac00*/ extern void *g_data_7dac00;         /* stream list semaphore */
/*007dac04*/ extern int g_data_7dac04;           /* stop request */
/*007daca4*/ extern void *g_data_7daca4;         /* released when the thread ends */

#define STREAM_PERIOD_MS 17

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_stream_update_thread_005b87b0(void)
{
  while (!g_data_7dac04) {
    RwaLink *l;

    WaitForSingleObject(g_data_7dac00, (RwUInt32)-1);
    for (l = g_data_7dac24; l != (RwaLink *)&g_data_7dac24; l = l->next) {
      char *stream = (char *)l - 0x14;

      _rwaObjStartExecute(stream, 0);
      _rwaObjFinishExecute(stream, 0);
    }
    ReleaseSemaphore(g_data_7dac00, 1, 0);
    Sleep(STREAM_PERIOD_MS);
  }
  ReleaseSemaphore(g_data_7daca4, 1, 0);
  return 0;
}
