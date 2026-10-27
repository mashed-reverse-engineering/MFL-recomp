/* layout of *record in FUN_005bcfa0, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct FUN_005bcfa0_record_layout {
    char pad_0x0[4];
    int f_0x4;
    char pad_0x8[8];
    int f_0x10;
    int f_0x14;
} FUN_005bcfa0_record_layout;

#include "mfl_tier2.h"
/* FUN_005bcfa0
 * Returns a size for the record at `record` (inferred: a record-size helper).
 * Base size is 0x1c; a non-zero int at record+4 selects 0x2c instead.
 * If the int at record+0x10 is non-zero, the int at record+0x14 is added.
 * Field meanings are unknown (no dossier entries), so the offsets stay literal.
 */
int FUN_005bcfa0(FUN_005bcfa0_record_layout *record)
{
  int size;

  /* default size */
  size = 0x1c;
  /* flag at record+4 selects the larger size (inferred) */
  if (record->f_0x4 != 0) {
    size = 0x2c;
  }
  /* optional extra size at record+0x14, enabled by flag at record+0x10 (inferred) */
  if (record->f_0x10 != 0) {
    size = size + record->f_0x14;
  }
  return size;
}
