/* rwa_stream_container_renderer_playback_end_005c0760 (0x005c0760): _rwaStreamContainerRendererPlaybackEnd */
#include "mfl_tier2.h"

extern __declspec(dllimport) long __stdcall InterlockedDecrement(long *addend);

/* A renderer of the container reports the end of its playback: one active renderer less (+0x3c); the last one sets the
   container's "playback ended" flag (+0x38).  The first argument (the renderer) is not used. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_stream_container_renderer_playback_end_005c0760(void *renderer, char *container)
{
  long *active = (long *)(container + 0x3c);

  InterlockedDecrement(active);
  if (*active == 0)
    *(int *)(container + 0x38) = 1;
}
