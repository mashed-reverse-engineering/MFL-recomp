#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*00752200*/ extern undefined1 g_particles_752200;
/*00752230*/ extern undefined4 g_sound_752230;
/*00759630*/ extern undefined4 g_particles_759630;
/* fx_ring_add_point ring: entries of RING_ENTRY_DWORDS dwords, live flags in a bitmap */
#define RING_LAST_INDEX (0xff)
#define RING_BITMAP_BYTE_SHIFT (3)
#define RING_BITMAP_BIT_MASK (7)
/* float bit patterns passed to random_range: 0.6f, 1.3f */
#define FLOAT_0_6_BITS (0x3f19999a)
#define FLOAT_1_3_BITS (0x3fa66666)
/* fx_ring_add_point */

/* "(pos*, value): adds a marker to the 256 entry ring at 0x752230 with a random 0.6..1.3 scale" */
/* Adds one marker to the ring of RING_LAST_INDEX + 1 entries at the write index g_particles_759630.
   pos is a 3 dword vector, copied into the entry; value and a random scale (0.6..1.3) are stored
   after it, and the entry's live bit is set. Returns nothing. Entry layout and the pos pointer
   stored in the entry are inferred. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
typedef struct { undefined4 a, b, c; } V3x;
void fx_ring_add_point(undefined4 *pos,undefined4 value)
{
  char *rec;
  double scale;

  if ((int)g_particles_759630 >= RING_LAST_INDEX + 1) {
    g_particles_759630 = 0;
  }
  rec = (char *)&g_sound_752230 + g_particles_759630 * 0x74;
  *(undefined4 **)(rec + 0x70) = pos;
  *(undefined4 *)(rec + 0x6c) = 0;
  *(undefined4 *)(rec + 0x68) = 1;
  *(V3x *)rec = *(V3x *)pos;
  *(undefined4 *)(rec + 0x14) = value;
  *(undefined4 *)(rec + 0xc) = 0;
  scale = (double)random_range(FLOAT_0_6_BITS,FLOAT_1_3_BITS);
  *(float *)(rec + 0x18) = (float)scale;
  *(undefined4 *)(rec + 0x1c) = 0;
  (&g_particles_752200)[(int)g_particles_759630 >> RING_BITMAP_BYTE_SHIFT] =
       (&g_particles_752200)[(int)g_particles_759630 >> RING_BITMAP_BYTE_SHIFT] | '\x01' << ((int)g_particles_759630 & RING_BITMAP_BIT_MASK);
  g_particles_759630 = g_particles_759630 + 1;
  return;
}
