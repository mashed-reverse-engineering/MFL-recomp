#include "mfl_tier2.h"
/* gap_5ccb50 */

/* no context */

/* Copies the 12 words (48 bytes) at src_obj+0x44 into dst_words and returns src_obj (inferred). */
char *gap_5ccb50(char *src_obj,char *unused_2,undefined4 *dst_words)

{
  int words_left;
  undefined4 *src_words;
  
  src_words = (undefined4 *)(src_obj + 0x44);
  for (words_left = 0xc; words_left != 0; words_left = words_left + -1) {
    *dst_words = *src_words;
    src_words = src_words + 1;
    dst_words = dst_words + 1;
  }
  return src_obj;
}


