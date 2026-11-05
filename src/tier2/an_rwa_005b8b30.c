/* rwa_stream_set_media_005b8b30 (0x005b8b30): streams (_rwaStreamSetMedia): give the stream its media description (copied into the stream, with its wave format), queue the set-media command and move the stream to the top-level object's list as pending start */
#include "mfl_tier2.h"

extern __declspec(dllimport) RwUInt32 __stdcall WaitForSingleObject(void *handle, RwUInt32 milliseconds);
extern __declspec(dllimport) RwBool __stdcall ReleaseSemaphore(void *semaphore, RwInt32 count, RwInt32 *previous);
/*005c1ad0*/ extern void _rwaStreamContainerCommandSetMedia(void *commands, void *media);
/*005bae80*/ extern char *RwaObjGetTopLevelObject(void);
/*007dac00*/ extern void *rwastream_objDefRegistered;      /* the stream lists' lock (semaphore) */

typedef struct RwaLink {
  struct RwaLink *next;
  struct RwaLink **prev;      /* the previous link's next field (or the list head) */
} RwaLink;

typedef struct RwaWaveFormat { RwUInt32 w[7]; } RwaWaveFormat;      /* 0x1c */
typedef struct RwaStreamMedia {      /* 0x24 */
  RwUInt32 w0, w4;
  RwaWaveFormat *format;      /* +0x08 */
  RwUInt32 w[6];
} RwaStreamMedia;

#define STREAM_LINK(s)      ((RwaLink *)((s) + 0x14))
#define STREAM_FORMAT(s)    ((RwaWaveFormat *)((s) + 0xc0))
#define STREAM_COMMANDS(s)  ((s) + 0x154)
#define STREAM_MEDIA(s)     ((RwaStreamMedia *)((s) + 0xd78))
#define STREAM_FLAGS(s)     (*(RwUInt32 *)((s) + 0xd9c))
#define STREAM_PENDING_START 0x1
#define TOP_STREAMS(top)    ((RwaLink **)((top) + 0xc))      /* the top-level object's list of streams to start */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_set_media_005b8b30(char *stream, RwaStreamMedia *media)
{
  RwaLink *link = STREAM_LINK(stream);

  WaitForSingleObject(rwastream_objDefRegistered, 0xffffffff);
  if (media) {
    if (media->format)
      *STREAM_FORMAT(stream) = *media->format;
    *STREAM_MEDIA(stream) = *media;
    media = STREAM_MEDIA(stream);
    media->format = STREAM_FORMAT(stream);
  }
  _rwaStreamContainerCommandSetMedia(STREAM_COMMANDS(stream), media);
  *link->prev = link->next;      /* unlink (the stream is on a list here) */
  link->next->prev = link->prev;
  STREAM_FLAGS(stream) |= STREAM_PENDING_START;
  ReleaseSemaphore(rwastream_objDefRegistered, 1, 0);

  if (link->next) {      /* unlink again, as the original's list macro */
    *link->prev = link->next;
    link->next->prev = link->prev;
  }
  link->next = *TOP_STREAMS(RwaObjGetTopLevelObject());
  link->prev = TOP_STREAMS(RwaObjGetTopLevelObject());
  (*TOP_STREAMS(RwaObjGetTopLevelObject()))->prev = (RwaLink **)link;
  *TOP_STREAMS(RwaObjGetTopLevelObject()) = link;
}
