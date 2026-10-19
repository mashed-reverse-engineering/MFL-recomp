#include "mfl_tier2.h"
/*00480230*/ extern int text_pages_clear();
/*006c2ee0*/ extern undefined4 g_rwp_trigger_group_count;
/*006c2f64*/ extern undefined4 g_car_6c2f64[0x1000];
/*006c3d98*/ extern undefined4 g_physics_scenario_count;
/*0089de60*/ extern undefined4 g_gfx_89de60[0x1000];
/*008b06e0*/ extern char player_car[];
/*006c33c4*/ extern char g_physics_scenarios_6c33c4[];

/* words per car record (0x8c bytes) */
#define CAR_RECORD_STRIDE_WORDS (0x23)
/* slots cleared per car record (inferred) */
#define CAR_SLOT_COUNT (0x10)
/* marker written into empty slots (inferred) */
#define EMPTY_SLOT_ID (0xffffffff)
/* words per gfx block (0x2510 bytes) */
#define GFX_BLOCK_STRIDE_WORDS (0x944)

/* physics_scenarios_reset */

/* "g_physics_scenario_count = 0 and text pages cleared" */

/*
 * Resets the physics scenario state at the start of a race: zeroes the
 * scenario count, clears the text pages, and zeroes the car records in
 * g_car_6c2f64 and the gfx blocks in g_gfx_89de60 up to player_car.
 * Takes no parameters and returns nothing.
 */
void physics_scenarios_reset(void)

{
  int start_index;
  undefined4 *block;
  undefined4 *write_ptr;
  int i;

  start_index = 0x0;
  g_physics_scenario_count = 0;
  text_pages_clear();
  block = g_car_6c2f64;
  i = start_index;
  /*
   * Car records, stride CAR_RECORD_STRIDE_WORDS, until the end of the car
   * table at g_physics_scenarios_6c33c4. Header words are zeroed per record.
   * The slot loop uses i without resetting it, so it only runs for the
   * first record (inferred, check disassembly).
   */
  do {
    block[1] = 0;
    *block = 0;
    block[2] = 0;
    for (; i < CAR_SLOT_COUNT; i = i + 1) {
      block[i + -0x20] = 0;
      block[i + -0x10] = EMPTY_SLOT_ID;
    }
    block = block + CAR_RECORD_STRIDE_WORDS;
  } while ((int)block < ((int)&g_physics_scenarios_6c33c4));
  g_rwp_trigger_group_count = 0;
  block = g_gfx_89de60;
  /*
   * Gfx blocks, stride GFX_BLOCK_STRIDE_WORDS, until player_car. Each block
   * has zeroed sections and an 8-entry table where each entry sets two words
   * to EMPTY_SLOT_ID and one word to 0.
   */
  do {
    /* zero 8 words starting 16 words before the block */
    write_ptr = block + -0x10;
    for (i = 8; i != 0; i = i + -1) {
      *write_ptr = 0;
      write_ptr = write_ptr + 1;
    }
    *block = 0;
    block[0x41] = 0;
    write_ptr = block + 0x42;
    for (i = 0x8b8; i != 0; i = i + -1) {
      *write_ptr = 0;
      write_ptr = write_ptr + 1;
    }
    write_ptr = block + 0x44;
    i = 8;
    do {
      write_ptr[-2] = EMPTY_SLOT_ID;
      *write_ptr = EMPTY_SLOT_ID;
      write_ptr[-1] = 0;
      write_ptr = write_ptr + 0x117;
      i = i + -1;
    } while (i != 0);
    block[0x8fa] = 0;
    write_ptr = block + 0x8fb;
    for (i = 0x38; i != 0; i = i + -1) {
      *write_ptr = 0;
      write_ptr = write_ptr + 1;
    }
    block[0x933] = 0;
    block = block + GFX_BLOCK_STRIDE_WORDS;
  } while ((int)block < ((int)&player_car));
  return;
}


