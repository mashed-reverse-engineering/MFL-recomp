#include "mfl_tier2.h"
/* FUN_005b6730 */

/*
 * Maps one flag bit (inferred: a single bit from 0x1 to 0x2000) to a small category
 * code (inferred). Returns 0 for any value not in the table.
 *   1, 2                 -> 1
 *   4, 8                 -> 2
 *   0x10, 0x20           -> 4
 *   0x40, 0x80           -> 8
 *   0x100, 0x200         -> 0x10
 *   0x400, 0x800         -> 4
 *   0x1000, 0x2000       -> 4
 */
undefined4 FUN_005b6730(int bit_flag)

{
  /* Values above 0x80: 0x100/0x200 -> 0x10, 0x400/0x800/0x1000/0x2000 -> 4, rest -> 0. */
  if (0x80 < bit_flag) {
    if (bit_flag < 0x801) {
      if (bit_flag != 0x800) {
        if ((bit_flag == 0x100) || (bit_flag == 0x200)) {
          return 0x10;
        }
        if (bit_flag != 0x400) {
          return 0;
        }
      }
    }
    else if ((bit_flag != 0x1000) && (bit_flag != 0x2000)) {
switchD_005b674a_caseD_3:
      return 0;
    }
switchD_005b674a_caseD_10:
    return 4;
  }
  /* Exactly 0x80 -> 8 (same as 0x40 in the switch below). */
  if (bit_flag == 0x80) {
switchD_005b674a_caseD_40:
    return 8;
  }
  /* Values up to 0x80: 1/2 -> 1, 4/8 -> 2, 0x10/0x20 -> 4, 0x40 -> 8, anything else -> 0 (default). */
  switch(bit_flag) {
  case 1:
  case 2:
    return 1;
  default:
    goto switchD_005b674a_caseD_3;
  case 4:
  case 8:
    return 2;
  case 0x10:
  case 0x20:
    goto switchD_005b674a_caseD_10;
  case 0x40:
    goto switchD_005b674a_caseD_40;
  }
}


