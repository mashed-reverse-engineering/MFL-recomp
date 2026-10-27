#include "mfl_tier2.h"
/*004943c0*/ extern int rwp_world_clear_arena();
/*00494460*/ extern int physics_buffer_size();
/*004b2b48*/ extern int _ftol();
/*005dd634*/ extern float k_f_2;
/*005dd7e8*/ extern float k_f_60;
/*007d2128*/ extern char *RwEngineInstance;
/* physics_buffer_create */

/* "allocates and clears 0x19C bytes" */

/* (inferred names) buffer +0xc = seconds (float bits), +0x10 = step_divisor, +8 = user_data, +4 = size from physics_buffer_size, +0x14 = array of (seconds*60 / step_divisor + 2) entries of 0x24 bytes (count - 2 stored at +0x16c); zero_left_or_count first counts the 0x67 zeroing words, then holds the entry count */
undefined4 * physics_buffer_create(float seconds,int step_divisor,undefined4 user_data)

{
  int fbits;
  int ft1;
  int bytes;
  undefined4 *buffer;
  undefined4 buffer_size;
  void *entries;
  int zero_left_or_count;
  undefined4 *zero_cursor;

  buffer = RwMalloc(0x19c,0);
  zero_cursor = buffer;
  for (zero_left_or_count = 0x67; zero_left_or_count != 0; zero_left_or_count = zero_left_or_count + -1) {
    *zero_cursor = 0;
    zero_cursor = zero_cursor + 1;
  }
  fbits = *(int *)&seconds;
  buffer[3] = fbits;
  buffer[4] = step_divisor;
  buffer_size = physics_buffer_size(fbits,step_divisor);
  buffer[1] = buffer_size;
  buffer[2] = user_data;
  ft1 = (int)(seconds * 60.0f);
  zero_left_or_count = (int)((double)ft1 / step_divisor + k_f_2);
  bytes = zero_left_or_count * 0x24;
  buffer[0x5b] = zero_left_or_count + -2;
  entries = RwMalloc(bytes,0);
  buffer[5] = (undefined4)entries;
  *buffer = 0x10b;
  rwp_world_clear_arena(buffer);
  return buffer;
}
