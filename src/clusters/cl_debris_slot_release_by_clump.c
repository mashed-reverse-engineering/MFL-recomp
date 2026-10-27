// cluster-entries: debris_slot_release_by_clump
/* Installed cluster debris_slot_release_by_clump (tools/cluster_install.py from output/cluster_pilot_zx/debris_slot_release_by_clump.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*00499b40*/ extern int explosion_decal();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*0065c0d0*/ extern undefined g_game_65c0d0;
extern char g_game_65c8f0[];

/* debris table layout: bank = 2 blocks, block = 10 slots of 0x18 bytes */
#define DEBRIS_BANK_STRIDE (0x208)
#define DEBRIS_BLOCKS_PER_BANK (2)
#define DEBRIS_BLOCK_STRIDE (0x100)
#define DEBRIS_BLOCK_VISIBLE_BITS_OFFSET (0xf0)
#define DEBRIS_BLOCK_LIVE_COUNT_OFFSET (0xf4)
#define DEBRIS_SLOTS_PER_BLOCK (10)
#define DEBRIS_SLOT_SIZE (0x18)
#define DEBRIS_SLOT_OWNER_OFFSET (0xc)
#define DEBRIS_SLOT_CLUMP_OFFSET (0x10)
/* random scale range 0.5f .. 0.8f, passed as float bits */
#define DEBRIS_SCALE_MIN_BITS (0x3f000000)
#define DEBRIS_SCALE_MAX_BITS (0x3f4ccccd)


/* debris_slot_release_by_clump */

/* "releases the first debris slot that references the clump (clears its visibility bit)" */

/*
 * Releases the first debris slot whose clump pointer (slot +0x10) equals `clump`:
 * clears its visibility bit, removes its body from the world, and spawns a small
 * explosion decal at its owner. Only the first match is handled; returns nothing.
 */
static int debris_body_remove_if_in_world_r(int p0);
extern void debris_body_remove_if_in_world(void);
void __cdecl debris_slot_release_by_clump(int clump)

{
  float scale_bits;
  undefined *block;
  int block_index;
  undefined *slot;
  undefined *bank;
  float10 random_scale;
  int slot_index;

  /* walk every debris bank from g_game_65c0d0 up to g_game_65c8f0 */
  bank = &g_game_65c0d0;
  do {
    /* each bank holds DEBRIS_BLOCKS_PER_BANK blocks */
    block_index = 0;
    block = bank;
    do {
      /* skip blocks with no live debris (live count at +0xf4, inferred) */
      if (*(int *)(block + DEBRIS_BLOCK_LIVE_COUNT_OFFSET) != 0) {
        /* scan the block's slots for one that references the clump */
        slot_index = 0;
        slot = block;
        do {
          if (*(int *)(slot + DEBRIS_SLOT_CLUMP_OFFSET) == clump) {
            /* clear this slot's visibility bit (bits packed 8 per byte, inferred) */
            block[(slot_index >> 3) + DEBRIS_BLOCK_VISIBLE_BITS_OFFSET] =
                 block[(slot_index >> 3) + DEBRIS_BLOCK_VISIBLE_BITS_OFFSET] & ~('\x01' << ((byte)slot_index & 7));
            debris_body_remove_if_in_world_r(slot);
            /* explosion scale: random float in 0.5..0.8 (inferred) */
            random_scale = (float10)rp_random_float_range(DEBRIS_SCALE_MIN_BITS,DEBRIS_SCALE_MAX_BITS);
            /* spawn the decal at (owner +4 pointer) + 0x40, passing the scale as float bits (inferred) */
            explosion_decal(*(int *)(*(char **)(slot + DEBRIS_SLOT_OWNER_OFFSET) + 4) + 0x40,(scale_bits = (float)random_scale, *(int *)&scale_bits));
            return;
          }
          slot_index = slot_index + 1;
          slot = slot + DEBRIS_SLOT_SIZE;
        } while (slot_index < DEBRIS_SLOTS_PER_BLOCK);
      }
      block_index = block_index + 1;
      block = block + DEBRIS_BLOCK_STRIDE;
    } while (block_index < DEBRIS_BLOCKS_PER_BANK);
    bank = bank + DEBRIS_BANK_STRIDE;
  } while ((LONG_PTR)bank < ((LONG_PTR)&g_game_65c8f0));
  return;
}

/* debris_body_remove_if_in_world_r: C -> original debris_body_remove_if_in_world (registers eax) */
__declspec(naked) int debris_body_remove_if_in_world_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call debris_body_remove_if_in_world
        ret
    }
}
