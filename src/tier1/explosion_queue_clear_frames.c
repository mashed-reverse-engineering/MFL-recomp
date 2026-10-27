#include "mfl_tier2.h"
/*006a4c28*/ extern struct ExplosionEntry g_explosion_queue[100];
/*006a5a44*/ extern char g_explosion_queue_6a5a44[];

#define EXPLOSION_QUEUE_FRAMES_END ((int)&g_explosion_queue_6a5a44)
#define EXPLOSION_ENTRY_STRIDE_WORDS 9

/* Sets frames = -1 for every ExplosionEntry in the explosion queue. */
void explosion_queue_clear_frames(void)
{
  int *entry_frames;
  
  entry_frames = &g_explosion_queue[0].frames;
  do {
    *entry_frames = -1;
    entry_frames = entry_frames + EXPLOSION_ENTRY_STRIDE_WORDS;
  } while ((int)entry_frames < EXPLOSION_QUEUE_FRAMES_END);
  return;
}
