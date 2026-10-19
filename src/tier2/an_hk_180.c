#include "mfl_tier2.h"
/*005cdf40*/ extern int __cdecl FUN_005cdf40(int a1, int a2);
/*005e20ec*/ extern undefined4 g_data_5e20ec;
/*005e20f0*/ extern undefined4 g_data_5e20f0;
/*005e20f4*/ extern undefined4 g_data_5e20f4;
/*005e20f8*/ extern undefined4 g_data_5e20f8;
/*005e217c*/ extern undefined4 g_data_5e217c;
/*005e2180*/ extern undefined4 g_data_5e2180;
/*005e2184*/ extern undefined4 g_data_5e2184;
/*005e2188*/ extern undefined4 g_data_5e2188;
#define SRC_TAG_INLINE (0xfffe) /* first word of src_entry marking inline data (inferred) */
/* FUN_005cde70 */

/*
 * Fills the output record dst from a source entry (src_entry) and
 * the global defaults. (inferred: purpose from the code, not from a name.)
 *   src_entry: ushort record; word 0 is a tag or type id.
 *   dst:       undefined4 record of at least 0x12 words, filled in place.
 */
void __cdecl FUN_005cde70(ushort *src_entry,undefined4 *dst)

{
  ushort src_word_8;
  undefined4 cached_5e20f8;

  /* Copy four words of inline data from src_entry, or let FUN_005cdf40
     build them from the entry id (inferred). */
  if (*src_entry == SRC_TAG_INLINE) {
    dst[4] = *(undefined4 *)(src_entry + 0xc);
    dst[5] = *(undefined4 *)(src_entry + 0xe);
    dst[6] = *(undefined4 *)(src_entry + 0x10);
    dst[7] = *(undefined4 *)(src_entry + 0x12);
  }
  else {
    FUN_005cdf40((uint)*src_entry,dst + 4);
  }
  /* Copy the global defaults into the record. */
  *dst = g_data_5e217c;
  dst[1] = g_data_5e2180;
  dst[2] = g_data_5e2184;
  dst[3] = g_data_5e2188;
  dst[0xb] = g_data_5e20ec;
  dst[0xc] = g_data_5e20f0;
  dst[0xd] = g_data_5e20f4;
  cached_5e20f8 = g_data_5e20f8;
  dst[8] = 1;
  dst[0xe] = cached_5e20f8;
  dst[9] = 0;
  dst[10] = (uint)src_entry[6];
  dst[0xf] = 0;
  /* Type 1 entries point at the source record with index 0x12;
     others add src_word_8 to that index. (inferred) */
  if (*src_entry == 1) {
    dst[0x11] = src_entry;
    dst[0x10] = 0x12;
    return;
  }
  src_word_8 = src_entry[8];
  dst[0x11] = src_entry;
  dst[0x10] = src_word_8 + 0x12;
  return;
}
