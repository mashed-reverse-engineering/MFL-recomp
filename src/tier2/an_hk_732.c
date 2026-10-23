#include "mfl_tier2.h"
/*0064d958*/ extern undefined4 g_scene_tables_64d958[0x1000];
/*006503a0*/ extern undefined4 g_scene_tables_6503a0[0x1000];
/*00651198*/ extern undefined4 g_scene_tables_651198[0x1000];
/*006511a0*/ extern undefined4 g_scene_tables_6511a0;
/*006511a4*/ extern undefined4 g_scene_tables_6511a4;
/*006511c4*/ extern undefined4 g_scene_tables_6511c4;
/*006511fc*/ extern undefined4 g_scene_tables_6511fc;
/*00651200*/ extern undefined4 g_scene_tables_651200;
/*00651210*/ extern undefined4 g_scene_tables_651210;
/*0065121c*/ extern undefined4 g_scene_tables_65121c;
/*00651220*/ extern undefined4 g_scene_tables_651220;
/*00651228*/ extern undefined4 g_scene_tables_651228;
/*0065122c*/ extern undefined4 g_scene_tables_65122c;
/*00651230*/ extern undefined4 g_scene_tables_651230;
/*00651234*/ extern undefined4 g_scene_tables_651234;
/*006503b8*/ extern char g_scene_tables_6503b8[];

#define SCENE_TABLE_651198_DWORDS (0x28)
#define SCENE_TABLE_64D958_DWORDS (0xe10)
#define SCENE_TABLE_FILL_VALUE (0xffffffff)

/* scene_tables_clear */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "fills 0x28 dwords at 0x651198 with -1 and clears 0xE10 dwords at 0x64D958" */

/* Resets the scene table state to its defaults (inferred from the body).
 * Takes no parameters and returns nothing.
 * Steps: fill two tables with SCENE_TABLE_FILL_VALUE, set a block of scalar
 * globals to fixed values, then write a fixed pattern into the table starting
 * at 0x6503a0 up to 0x6503b8 (contents not named, see inline comment).
 */
void scene_tables_clear(void)

{
  undefined4 *cursor;
  int remaining;

  /* Fill the first table with the fill value. */
  cursor = g_scene_tables_651198;
  for (remaining = SCENE_TABLE_651198_DWORDS; remaining != 0; remaining = remaining + -1) {
    *cursor = SCENE_TABLE_FILL_VALUE;
    cursor = cursor + 1;
  }
  /* Fill the second table with the fill value. */
  cursor = g_scene_tables_64d958;
  for (remaining = SCENE_TABLE_64D958_DWORDS; remaining != 0; remaining = remaining + -1) {
    *cursor = SCENE_TABLE_FILL_VALUE;
    cursor = cursor + 1;
  }
  /* Set the scalar scene globals to their defaults (meanings not named). */
  g_scene_tables_651210 = 0;
  g_scene_tables_651198[0] = 2;
  g_scene_tables_6511a0 = 3;
  g_scene_tables_6511fc = 2;
  g_scene_tables_6511c4 = 1;
  g_scene_tables_65121c = 1;
  g_scene_tables_651220 = 1;
  g_scene_tables_6511a4 = 3;
  g_scene_tables_651200 = 2;
  g_scene_tables_651228 = 3;
  g_scene_tables_65122c = 0;
  g_scene_tables_651230 = 2;
  g_scene_tables_651234 = 2;
  /* Write the fixed per-entry pattern into the table at 0x6503a0 up to 0x6503b8.
   * Each pass writes one dword column across the 6-dword records, so the offsets
   * are the record stride (inferred). Values are table data, not named.
   */
  cursor = g_scene_tables_6503a0;
  do {
    cursor[-6] = 8;
    *cursor = 0;
    cursor[6] = 4;
    cursor[0xc] = 0x10;
    cursor[0x12] = 0xc;
    cursor[0x18] = 0x14;
    cursor[0x1e] = 0x10;
    cursor[0x24] = 4;
    cursor[0x2a] = 8;
    cursor[0x30] = 4;
    cursor[0x36] = 0x10;
    cursor[0x3c] = 4;
    cursor[0x42] = 8;
    cursor[-0xa92] = 10;
    cursor[-0xa8c] = 2;
    cursor[-0xa86] = 6;
    cursor[-0xa80] = 0x12;
    cursor[-0xa7a] = 0xe;
    cursor[-0xa74] = 0x16;
    cursor[-0xa6e] = 0x12;
    cursor[-0xa68] = 6;
    cursor[-0xa62] = 10;
    cursor[-0xa5c] = 6;
    cursor[-0xa56] = 0x12;
    cursor[-0xa50] = 6;
    cursor[-0xa4a] = 10;
    cursor[-0x9de] = 8;
    cursor[-0x9d8] = 0;
    cursor[-0x9d2] = 4;
    cursor[-0x9cc] = 0x10;
    cursor[-0x9c6] = 0xc;
    cursor[-0x9c0] = 0x14;
    cursor[-0x9ba] = 0x10;
    cursor[-0x9b4] = 4;
    cursor[-0x9ae] = 8;
    cursor[-0x9a8] = 4;
    cursor[-0x9a2] = 0x10;
    cursor[-0x99c] = 4;
    cursor[-0x996] = 8;
    cursor[-0x1c8] = 10;
    cursor[-0x1c2] = 2;
    cursor[-0x1bc] = 6;
    cursor[-0x1b6] = 0x12;
    cursor[-0x1b0] = 0xe;
    cursor[-0x1aa] = 0x16;
    cursor[-0x1a4] = 0x12;
    cursor[-0x19e] = 6;
    cursor[-0x198] = 10;
    cursor[-0x192] = 6;
    cursor[-0x18c] = 0x12;
    cursor[-0x186] = 6;
    cursor[-0x180] = 10;
    cursor[-0x6b4] = 9;
    cursor[-0x6ae] = 1;
    cursor[-0x6a8] = 5;
    cursor[-0x6a2] = 0x11;
    cursor[-0x69c] = 0xd;
    cursor[-0x696] = 0x15;
    cursor[-0x690] = 0x11;
    cursor[-0x68a] = 5;
    cursor[-0x684] = 9;
    cursor[-0x67e] = 5;
    cursor[-0x678] = 0x11;
    cursor[-0x672] = 5;
    cursor[-0x66c] = 9;
    cursor[0x108] = 9;
    cursor[0x10e] = 1;
    cursor[0x114] = 5;
    cursor[0x11a] = 0x11;
    cursor[0x120] = 0xd;
    cursor[0x126] = 0x15;
    cursor[300] = 0x11;
    cursor[0x132] = 5;
    cursor[0x138] = 9;
    cursor[0x13e] = 5;
    cursor[0x144] = 0x11;
    cursor[0x14a] = 5;
    cursor[0x150] = 9;
    cursor[0x162] = 9;
    cursor[0x168] = 1;
    cursor[0x16e] = 5;
    cursor[0x174] = 0x11;
    cursor[0x17a] = 0xd;
    cursor[0x180] = 0x15;
    cursor[0x186] = 0x11;
    cursor[0x18c] = 5;
    cursor[0x192] = 9;
    cursor[0x198] = 5;
    cursor[0x19e] = 0x11;
    cursor[0x1a4] = 5;
    cursor[0x1aa] = 9;
    cursor[-0x984] = 0xb;
    cursor[-0x97e] = 3;
    cursor[-0x978] = 7;
    cursor[-0x972] = 0x13;
    cursor[-0x96c] = 0xf;
    cursor[-0x966] = 0x17;
    cursor[-0x960] = 0x13;
    cursor[-0x95a] = 7;
    cursor[-0x954] = 0xb;
    cursor[-0x94e] = 7;
    cursor[-0x948] = 0x13;
    cursor[-0x942] = 7;
    cursor[-0x93c] = 0xb;
    cursor[-0x16e] = 10;
    cursor[-0x168] = 2;
    cursor[-0x162] = 6;
    cursor[-0x15c] = 0x12;
    cursor[-0x156] = 0xe;
    cursor[-0x150] = 0x16;
    cursor[-0x14a] = 0x12;
    cursor[-0x144] = 6;
    cursor[-0x13e] = 10;
    cursor[-0x138] = 6;
    cursor[-0x132] = 0x12;
    cursor[-300] = 6;
    cursor[-0x126] = 10;
    cursor[0x216] = 10;
    cursor[0x21c] = 2;
    cursor[0x222] = 6;
    cursor[0x228] = 0x12;
    cursor[0x22e] = 0xe;
    cursor[0x234] = 0x16;
    cursor[0x23a] = 0x12;
    cursor[0x240] = 6;
    cursor[0x246] = 10;
    cursor[0x24c] = 6;
    cursor[0x252] = 0x12;
    cursor[600] = 6;
    cursor[0x25e] = 10;
    cursor[0x270] = 8;
    cursor[0x276] = 0;
    cursor[0x27c] = 4;
    cursor[0x282] = 0x10;
    cursor[0x288] = 0xc;
    cursor[0x28e] = 0x14;
    cursor[0x294] = 0x10;
    cursor[0x29a] = 4;
    cursor[0x2a0] = 8;
    cursor[0x2a6] = 4;
    cursor[0x2ac] = 0x10;
    cursor[0x2b2] = 4;
    cursor[0x2b8] = 8;
    cursor[0x2ca] = 10;
    cursor[0x2d0] = 2;
    cursor[0x2d6] = 6;
    cursor[0x2dc] = 0x12;
    cursor[0x2e2] = 0xe;
    cursor[0x2e8] = 0x16;
    cursor[0x2ee] = 0x12;
    cursor[0x2f4] = 6;
    cursor[0x2fa] = 10;
    cursor[0x300] = 6;
    cursor[0x306] = 0x12;
    cursor[0x30c] = 6;
    cursor[0x312] = 10;
    cursor[0x324] = 10;
    cursor[0x32a] = 2;
    cursor[0x330] = 6;
    cursor[0x336] = 0x12;
    cursor[0x33c] = 0xe;
    cursor[0x342] = 0x16;
    cursor[0x348] = 0x12;
    cursor[0x34e] = 6;
    cursor[0x354] = 10;
    cursor[0x35a] = 6;
    cursor[0x360] = 0x12;
    cursor[0x366] = 6;
    cursor[0x36c] = 10;
    cursor = cursor + 1;
  } while ((char *)cursor < ((char *)&g_scene_tables_6503b8));
  return;
}


