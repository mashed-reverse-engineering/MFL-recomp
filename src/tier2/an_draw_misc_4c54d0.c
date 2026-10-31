#include "mfl_tier2.h"
/* Small helpers between the drawing code and the memory routines: projecting a single point, clearing the first bits of
   a bit set and zeroing a buffer.  0x4c54d0, 0x4c6600, 0x4c66a0. */
/*004c4520*/ extern void camera_project_points(struct RwCamera *camera, struct RwV3d *points, float *out, int count);
/*004c6660*/ extern void *__cdecl FUN_004c6660(void *dest, int value, unsigned int count);   /* memset (C runtime) */
extern void *memset(void *, int, unsigned int);

/* Projects the one camera space point to the screen position out[0..1]. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void camera_project_point(struct RwCamera *camera, struct RwV3d *point, float *out)
{
  camera_project_points(camera, point, out, 1);
}

/* Clears the first `count` bits of the bit set (bit 0 of a byte is the first one of it). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void bitset_clear_first_n(unsigned char *bits, int count)
{
  int mask;
  int bit;

  if (count >= 8) {
    int bytes = count >> 3;

    memset(bits, 0, bytes);
    count -= bytes * 8;
    bits += bytes;
  }
  if (count != 0) {
    mask = 0;
    for (bit = count - 1; count != 0; count--) {
      mask |= 1 << bit;
      bit--;
    }
    *bits &= ~mask;
  }
}

/* Zeroes `size` bytes at `buffer`. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void *mem_zero(void *buffer, unsigned int size)
{
  return FUN_004c6660(buffer, 0, size);
}
