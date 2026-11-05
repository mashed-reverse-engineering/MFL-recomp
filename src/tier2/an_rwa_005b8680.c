/* rwa_stream_container_attach_005b8680 (0x005b8680): streams: (re)create the stream's container for a media and file the stream in the active stream list */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
/*005c1820*/ extern int _rwaStreamContainerCreate(void *container, void *media, void *mediaData, RwUInt32 a3, RwUInt32 a4, RwUInt32 a5);

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

/*007dac24*/ extern RwaLink *g_data_7dac24;      /* active streams list head */
/*007dac00*/ extern void *g_data_7dac00;         /* stream list semaphore */
/*007daca8*/ extern int g_data_7daca8;           /* active stream count (the semaphore is used once there is one) */

/* stream+0x14: list link, +0xec: container, +0xd9c: cleared.  params: three words handed to the container.  Returns the
   stream, NULL when the container cannot be created (the stream stays out of the list). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_stream_container_attach_005b8680(char *stream, char *media, RwUInt32 *params)
{
  RwaLink *link = (RwaLink *)(stream + 0x14);

  if (link->next) {
    link->prev->next = link->next;
    link->next->prev = link->prev;
    link->prev = 0;
    link->next = 0;
  }
  *(RwUInt32 *)(stream + 0xd9c) = 0;
  if (!_rwaStreamContainerCreate(stream + 0xec, media, *(void **)(media + 0x2c), params[0], params[1], params[2]))
    return 0;

  if (g_data_7daca8)
    WaitForSingleObject(g_data_7dac00, (RwUInt32)-1);
  link->next = g_data_7dac24;
  link->prev = (RwaLink *)&g_data_7dac24;
  g_data_7dac24->prev = link;
  g_data_7dac24 = link;
  if (g_data_7daca8)
    ReleaseSemaphore(g_data_7dac00, 1, 0);
  g_data_7daca8++;
  return stream;
}
