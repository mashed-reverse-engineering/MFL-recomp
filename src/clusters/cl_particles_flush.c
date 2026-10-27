// cluster-entries: particles_flush
/* Installed cluster particles_flush (tools/cluster_install.py from output/cluster_pilot_kb/particles_flush.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/* particles_flush */

/* "locks / unlocks the PTank and uploads the added particles" */

/* particle pool record (layout from the offsets this function uses) */
typedef struct ParticlePool {
    char unknown_00[4];
    int atomic;      /* 0x04: PTank atomic */
    char unknown_08[4];
    int count;       /* 0x0c: particles added since the last flush */
    char *positions; /* 0x10 */
    char *sizes;     /* 0x14 */
    char *colors;    /* 0x18 */
    char *matrices;  /* 0x1c */
    char *array_20;  /* 0x20 */
    char *array_24;  /* 0x24 */
    char *array_28;  /* 0x28 */
} ParticlePool;

/* pool fields: +4 PTank atomic, +0xc particle count, +0x10 positions (3 floats), +0x14 sizes (2 floats), +0x18 colours (1 word), +0x1c matrices (16 words), +0x20 / +0x24 / +0x28 further per-particle arrays (1 word / 4 words / 8 words); the second RpPTankAtomicLock argument is the PTank lock flag of that array (1 position, 2 colour, 4 size, 8 matrix, then 0x20, 0x80, 0x100) */
void particles_flush(ParticlePool *pool)

{
  uint *ptank_flags;
  int n;
  int w;
  int src_off;
  undefined4 *src;
  undefined4 *dst;
  struct {
    undefined4 *lock_ptr;
    int lock_stride;
  } frame;
  
  ptank_flags = (uint *)(*(char **)(g_ptank_plugin_offset + pool->atomic) + 0x40);
  *ptank_flags = *ptank_flags | 0x800000;
  *(undefined4 *)(*(char **)(g_ptank_plugin_offset + pool->atomic) + 4) =
       pool->count;
  if (pool->count != 0) {
    if (pool->positions != 0) {
      RpPTankAtomicLock_00545d10(pool->atomic,&frame.lock_ptr,1,0x40000000);
      n = 0;
      if (0 < pool->count) {
        src_off = 0;
        do {
          src = (undefined4 *)(pool->positions + src_off);
          *frame.lock_ptr = *src;
          frame.lock_ptr[1] = src[1];
          frame.lock_ptr[2] = src[2];
          frame.lock_ptr = (undefined4 *)((int)frame.lock_ptr + frame.lock_stride);
          n = n + 1;
          src_off = src_off + 0xc;
        } while (n < pool->count);
      }
    }
    if (pool->sizes != 0) {
      RpPTankAtomicLock_00545d10(pool->atomic,&frame.lock_ptr,4,0x40000000);
      n = 0;
      if (0 < pool->count) {
        do {
          src_off = (int)pool->sizes;
          *frame.lock_ptr = *(undefined4 *)(src_off + n * 8);
          frame.lock_ptr[1] = *(undefined4 *)(src_off + 4 + n * 8);
          frame.lock_ptr = (undefined4 *)((int)frame.lock_ptr + frame.lock_stride);
          n = n + 1;
        } while (n < pool->count);
      }
    }
    if (pool->colors != 0) {
      RpPTankAtomicLock_00545d10(pool->atomic,&frame.lock_ptr,2,0x40000000);
      n = 0;
      if (0 < pool->count) {
        do {
          *frame.lock_ptr = *(undefined4 *)(pool->colors + n * 4);
          frame.lock_ptr = (undefined4 *)((int)frame.lock_ptr + frame.lock_stride);
          n = n + 1;
        } while (n < pool->count);
      }
    }
    if (pool->matrices != 0) {
      RpPTankAtomicLock_00545d10(pool->atomic,&frame.lock_ptr,8,0x40000000);
      n = 0;
      if (0 < pool->count) {
        src_off = 0;
        do {
          src = (undefined4 *)(pool->matrices + src_off);
          dst = frame.lock_ptr;
          for (w = 0x10; w != 0; w = w + -1) {
            *dst = *src;
            src = src + 1;
            dst = dst + 1;
          }
          frame.lock_ptr = (undefined4 *)((int)frame.lock_ptr + frame.lock_stride);
          n = n + 1;
          src_off = src_off + 0x40;
        } while (n < pool->count);
      }
    }
    if (pool->array_20 != 0) {
      RpPTankAtomicLock_00545d10(pool->atomic,&frame.lock_ptr,0x20,0x40000000);
      n = 0;
      if (0 < pool->count) {
        do {
          *frame.lock_ptr = *(undefined4 *)(pool->array_20 + n * 4);
          frame.lock_ptr = (undefined4 *)((int)frame.lock_ptr + frame.lock_stride);
          n = n + 1;
        } while (n < pool->count);
      }
    }
    if (pool->array_24 != 0) {
      RpPTankAtomicLock_00545d10(pool->atomic,&frame.lock_ptr,0x80,0x40000000);
      n = 0;
      if (0 < pool->count) {
        src_off = 0;
        do {
          src = (undefined4 *)(pool->array_24 + src_off);
          *frame.lock_ptr = *src;
          frame.lock_ptr[1] = src[1];
          frame.lock_ptr[2] = src[2];
          frame.lock_ptr[3] = src[3];
          frame.lock_ptr = (undefined4 *)((int)frame.lock_ptr + frame.lock_stride);
          n = n + 1;
          src_off = src_off + 0x10;
        } while (n < pool->count);
      }
    }
    if (pool->array_28 != 0) {
      RpPTankAtomicLock_00545d10(pool->atomic,&frame.lock_ptr,0x100,0x40000000);
      n = 0;
      if (0 < pool->count) {
        src_off = 0;
        do {
          src = (undefined4 *)(pool->array_28 + src_off);
          dst = frame.lock_ptr;
          for (w = 8; w != 0; w = w + -1) {
            *dst = *src;
            src = src + 1;
            dst = dst + 1;
          }
          frame.lock_ptr = (undefined4 *)((int)frame.lock_ptr + frame.lock_stride);
          n = n + 1;
          src_off = src_off + 0x20;
        } while (n < pool->count);
      }
    }
    RpPTankAtomicUnlock_00545f20(pool->atomic);
    pool->count = 0;
  }
  return;
}

