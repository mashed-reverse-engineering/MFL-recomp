#include "mfl_tier2.h"
/*00472340*/ extern int sfx_play_player_set_at();
/*00499b40*/ extern int explosion_decal();
/*004b4ba7*/ extern int entry();
/*006a4c28*/ extern struct ExplosionEntry g_explosion_queue[100];
/*006a5a38*/ extern undefined4 g_explosion_queue_6a5a38;
#define EXPLOSION_DECAL_SIZE (0x3f99999a) /* float 1.2 bit pattern passed to explosion_decal (inferred: decal size) */
/* explosion_queue */

/* Queues an explosion in the ring of 100 g_explosion_queue entries (next index at
   g_explosion_queue_6a5a38). position is the xyz centre plus radius in position[0..3], damage
   is multiplied by the distance falloff later, push_horizontal and push_vertical are the
   knock-back strengths, frames is how long it stays active. Then plays the flash/sound
   (sfx_play_player_set_at) and a ground decal (explosion_decal). The damage itself comes later
   from the type 0x19 trigger sphere that weapons_update_frame registers for every frame the
   entry's countdown is positive. Returns nothing. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
typedef struct Vec4F { float v[4]; } Vec4F;
void explosion_queue(float *position,float damage,float push_horizontal,float push_vertical,int frames)
{
  struct ExplosionEntry *explosion;
  int ring_index;
  float *dst;
  float *src;

  /* advance the ring index, wrapping back to slot 0 after the last entry */
  ring_index = g_explosion_queue_6a5a38;
  if ((int)g_explosion_queue_6a5a38 >= 100) {      /* diffemu: undefined4 global, the original compares signed (jl) */
    ring_index = 0;
  }
  explosion = g_explosion_queue + ring_index;
  g_explosion_queue_6a5a38 = ring_index + 1;
  if (explosion != (ExplosionEntry *)0x0) {
    /* fill the slot: both countdowns start at frames, the position block is copied as is */
    explosion->damage = damage;
    explosion->push_horizontal = push_horizontal;
    explosion->push_vertical = push_vertical;
    explosion->frames = frames;
    explosion->frames_left = frames;
    *(Vec4F *)explosion->position_radius = *(Vec4F *)position;
    sfx_play_player_set_at(position);
    explosion_decal(position,EXPLOSION_DECAL_SIZE);
  }
  return;
}
