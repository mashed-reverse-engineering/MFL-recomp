#include "mfl_tier2.h"
/*0042c440*/ extern int ui_rt2d_text_draw();
/*00609a58*/ extern undefined4 g_menu_609a58;

#define FADE_IN_MS (0xff)
#define FADE_OUT_START_MS (0x1389)
#define FADE_OUT_END_MS (0x1787)
#define ALPHA_OPAQUE (0xff)
#define TEXT_SCALE_BITS (0x3f19999a) /* float 0.6f (inferred) */
#define COLOUR_WHITE_RGB (0xffffff)
#define END_OF_TABLE (0xffffffff)
#define MENU_RESET_COUNT (0x14)
#define SHADOW_X_OFFSET (-0x1c)
#define SHADOW_Y_OFFSET (2)
#define TEXT_X_OFFSET (-0x1e)
/* credits_draw */

/* Draws the scrolling credits at current_time (ms since the credits started, inferred), returns nothing.
   credit_table is read in steps of 4 ints: x, y, start time (ms), string id of the next entry. Each
   entry whose start time has passed is drawn as white text with a black shadow, faded in and out.
   The first string id is set here and the list ends at END_OF_TABLE (inferred).
   The x of the first entry (credit_table[0]) is 0x1d6.
   At the end, g_menu_609a58[0..MENU_RESET_COUNT-1] is reset to its own index (inferred). */

void __cdecl credits_draw(int current_time)

{
  float shadow_x;
  float shadow_y;
  float text_x;
  float text_y;
  int y;
  byte alpha;
  int elapsed;
  int *entry;
  int string_id;
  struct {
    undefined4 text_colour;
    int shadow_colour;
  } colours;
  int credit_table [340];
  
  credit_table[0] = 0x1d6;   /* x of the first entry: the original writes [esp+0x24]=0x1d6 (was missing) */
  credit_table[2] = 0;
  credit_table[6] = 0;
  credit_table[1] = 300;
  credit_table[3] = 0x162;
  credit_table[4] = 0x1d6;
  credit_table[5] = 0x13c;
  credit_table[7] = 0x163;
  credit_table[8] = 0x1e0;
  credit_table[9] = 0x15a;
  credit_table[10] = 0x15e0;
  credit_table[0xb] = 0x164;
  credit_table[0xc] = 0x208;
  credit_table[0xd] = 0x182;
  credit_table[0xe] = 0x2198;
  credit_table[0xf] = 0x165;
  credit_table[0x10] = 0x1d6;
  credit_table[0x11] = 0x16e;
  credit_table[0x12] = 0x2d50;
  credit_table[0x13] = 0x166;
  credit_table[0x14] = 0x1fe;
  credit_table[0x15] = 0x13c;
  credit_table[0x16] = 0x3908;
  credit_table[0x17] = 0x167;
  credit_table[0x18] = 0x1cc;
  credit_table[0x19] = 0x15a;
  credit_table[0x1a] = 0x44c0;
  credit_table[0x1b] = 0x168;
  credit_table[0x1c] = 500;
  credit_table[0x1d] = 0x16e;
  credit_table[0x1e] = 0x5078;
  credit_table[0x1f] = 0x169;
  credit_table[0x20] = 0x1ea;
  credit_table[0x21] = 0x182;
  credit_table[0x22] = 0x5c30;
  credit_table[0x23] = 0x16a;
  credit_table[0x24] = 0x1cc;
  credit_table[0x25] = 0x140;
  credit_table[0x26] = 0x67e8;
  credit_table[0x27] = 0x16b;
  credit_table[0x28] = 0x1fe;
  credit_table[0x29] = 0x164;
  credit_table[0x2a] = 0x73a0;
  credit_table[0x2b] = 0x16c;
  credit_table[0x2c] = 500;
  credit_table[0x2d] = 0x178;
  credit_table[0x2e] = 0x7f58;
  credit_table[0x2f] = 0x16d;
  credit_table[0x30] = 0x1ea;
  credit_table[0x31] = 0x18c;
  credit_table[0x32] = 0x8b10;
  credit_table[0x33] = 0x16e;
  credit_table[0x34] = 0x1cc;
  credit_table[0x35] = 0x14a;
  credit_table[0x36] = 0x96c8;
  credit_table[0x37] = 0x16f;
  credit_table[0x38] = 0x1fe;
  credit_table[0x39] = 0x164;
  credit_table[0x3a] = 0xa280;
  credit_table[0x3b] = 0x170;
  credit_table[0x3c] = 0x1cc;
  credit_table[0x3d] = 300;
  credit_table[0x3e] = 48000;
  credit_table[0x3f] = 0x171;
  credit_table[0x40] = 0x1cc;
  credit_table[0x41] = 0x13c;
  credit_table[0x42] = 48000;
  credit_table[0x43] = 0x172;
  credit_table[0x44] = 0x1cc;
  credit_table[0x45] = 0x14c;
  credit_table[0x46] = 48000;
  credit_table[0x47] = 0x173;
  credit_table[0x48] = 0x208;
  credit_table[0x49] = 0x164;
  credit_table[0x4a] = 52000;
  credit_table[0x4b] = 0x174;
  credit_table[0x4c] = 0x208;
  credit_table[0x4d] = 0x174;
  credit_table[0x4e] = 52000;
  credit_table[0x4f] = 0x175;
  credit_table[0x50] = 0x1d6;
  credit_table[0x51] = 0x134;
  credit_table[0x52] = 56000;
  credit_table[0x53] = 0x176;
  credit_table[0x54] = 0x1d6;
  credit_table[0x55] = 0x144;
  credit_table[0x56] = 56000;
  credit_table[0x57] = 0x177;
  credit_table[0x58] = 0x1cc;
  credit_table[0x59] = 0x15c;
  credit_table[0x5a] = 59000;
  credit_table[0x5b] = 0x178;
  credit_table[0x5c] = 0x1cc;
  credit_table[0x5d] = 0x16c;
  credit_table[0x5e] = 59000;
  credit_table[0x5f] = 0x179;
  credit_table[0x60] = 0x1cc;
  credit_table[0x61] = 0x134;
  credit_table[0x62] = 62000;
  credit_table[99] = 0x17a;
  credit_table[100] = 0x1cc;
  credit_table[0x65] = 0x144;
  credit_table[0x66] = 62000;
  credit_table[0x67] = 0x17b;
  credit_table[0x68] = 0x1d6;
  credit_table[0x69] = 0x13c;
  credit_table[0x6a] = 68000;
  credit_table[0x6b] = 0x17c;
  credit_table[0x6c] = 0x1e0;
  credit_table[0x6d] = 0x15a;
  credit_table[0x6e] = 72000;
  credit_table[0x6f] = 0x17d;
  credit_table[0x70] = 0x208;
  credit_table[0x71] = 0x17c;
  credit_table[0x72] = 75000;
  credit_table[0x73] = 0x17e;
  credit_table[0x74] = 0x208;
  credit_table[0x75] = 0x18c;
  credit_table[0x76] = 75000;
  credit_table[0x77] = 0x17f;
  credit_table[0x78] = 0x1f0;
  credit_table[0x79] = 300;
  credit_table[0x7a] = 81000;
  credit_table[0x7b] = 0x180;
  credit_table[0x7c] = 0x1f0;
  credit_table[0x7d] = 0x13c;
  credit_table[0x7e] = 81000;
  credit_table[0x7f] = 0x181;
  credit_table[0x80] = 0x208;
  credit_table[0x81] = 0x150;
  credit_table[0x82] = 87000;
  credit_table[0x83] = 0x182;
  credit_table[0x84] = 0x208;
  credit_table[0x85] = 0x160;
  credit_table[0x86] = 87000;
  credit_table[0x87] = 0x185;
  credit_table[0x88] = 0x1d6;
  credit_table[0x89] = 0x174;
  credit_table[0x8a] = 90000;
  credit_table[0x8b] = 0x186;
  credit_table[0x8c] = 0x1d6;
  credit_table[0x8d] = 0x184;
  credit_table[0x8e] = 90000;
  credit_table[0x8f] = 0x187;
  credit_table[0x90] = 0x1d6;
  credit_table[0x91] = 0x194;
  credit_table[0x92] = 90000;
  credit_table[0x93] = 0x188;
  credit_table[0x94] = 500;
  credit_table[0x95] = 0x136;
  credit_table[0x96] = 93000;
  credit_table[0x97] = 0x189;
  credit_table[0x98] = 500;
  credit_table[0x99] = 0x146;
  credit_table[0x9a] = 93000;
  credit_table[0x9b] = 0x183;
  credit_table[0x9c] = 0x208;
  credit_table[0x9d] = 0x158;
  credit_table[0x9e] = 96000;
  credit_table[0x9f] = 0x184;
  credit_table[0xa0] = 0x208;
  credit_table[0xa1] = 0x168;
  credit_table[0xa2] = 96000;
  credit_table[0xa3] = 0x18a;
  credit_table[0xa4] = 0x1d6;
  credit_table[0xa5] = 0x17c;
  credit_table[0xa6] = 99000;
  credit_table[0xa7] = 0x18b;
  credit_table[0xa8] = 0x1d6;
  credit_table[0xa9] = 0x18c;
  credit_table[0xaa] = 99000;
  credit_table[0xab] = 0x18c;
  credit_table[0xac] = 500;
  credit_table[0xad] = 0x136;
  credit_table[0xae] = 0x18e70;
  credit_table[0xaf] = 0x18d;
  credit_table[0xb0] = 500;
  credit_table[0xb1] = 0x146;
  credit_table[0xb2] = 0x18e70;
  credit_table[0xb3] = 0x18e;
  credit_table[0xb4] = 0x1ea;
  credit_table[0xb5] = 0x158;
  credit_table[0xb6] = 0x19a28;
  credit_table[0xb7] = 399;
  credit_table[0xb8] = 0x1ea;
  credit_table[0xb9] = 0x168;
  credit_table[0xba] = 0x19a28;
  credit_table[0xbb] = 400;
  credit_table[0xbc] = 0x1d6;
  credit_table[0xbd] = 0x13c;
  credit_table[0xbe] = 0x1b580;
  credit_table[0xbf] = 0x191;
  credit_table[0xc0] = 0x1e0;
  credit_table[0xc1] = 0x15a;
  credit_table[0xc2] = 0x1cb60;
  credit_table[0xc3] = 0x192;
  credit_table[0xc4] = 0x208;
  credit_table[0xc5] = 0x182;
  credit_table[0xc6] = 0x1d718;
  credit_table[199] = 0x193;
  credit_table[200] = 0x1d6;
  credit_table[0xc9] = 0x16e;
  credit_table[0xca] = 0x1e2d0;
  credit_table[0xcb] = 0x194;
  credit_table[0xcc] = 0x1fe;
  credit_table[0xcd] = 0x13c;
  credit_table[0xce] = 0x1ee88;
  credit_table[0xcf] = 0x195;
  credit_table[0xd0] = 0x1cc;
  credit_table[0xd1] = 0x15a;
  credit_table[0xd2] = 0x1fa40;
  credit_table[0xd3] = 0x196;
  credit_table[0xd4] = 500;
  credit_table[0xd5] = 0x16e;
  credit_table[0xd6] = 0x205f8;
  credit_table[0xd7] = 0x197;
  credit_table[0xd8] = 0x1ea;
  credit_table[0xd9] = 0x182;
  credit_table[0xda] = 0x211b0;
  credit_table[0xdb] = 0x198;
  credit_table[0xdc] = 0x1cc;
  credit_table[0xdd] = 0x140;
  credit_table[0xde] = 0x21d68;
  credit_table[0xdf] = 0x199;
  credit_table[0xe0] = 0x1d6;
  credit_table[0xe1] = 0x13c;
  credit_table[0xe2] = 0x23668;
  credit_table[0xe3] = 0x19a;
  credit_table[0xe4] = 0x1e0;
  credit_table[0xe5] = 0x15a;
  credit_table[0xe6] = 150000;
  credit_table[0xe7] = 0x19b;
  credit_table[0xe8] = 0x208;
  credit_table[0xe9] = 0x182;
  credit_table[0xea] = 0x255a8;
  credit_table[0xeb] = 0x19c;
  credit_table[0xec] = 0x1d6;
  credit_table[0xed] = 0x16e;
  credit_table[0xee] = 0x26160;
  credit_table[0xef] = 0x19d;
  credit_table[0xf0] = 0x1fe;
  credit_table[0xf1] = 0x13c;
  credit_table[0xf2] = 0x26d18;
  credit_table[0xf3] = 0x19e;
  credit_table[0xf4] = 0x208;
  credit_table[0xf5] = 0x150;
  credit_table[0xf6] = 0x28c58;
  credit_table[0xf7] = 0x19f;
  credit_table[0xf8] = 0x208;
  credit_table[0xf9] = 0x160;
  credit_table[0xfa] = 0x28c58;
  credit_table[0xfb] = 0x1a0;
  credit_table[0xfc] = 0x1d6;
  credit_table[0xfd] = 0x174;
  credit_table[0xfe] = 170000;
  credit_table[0xff] = 0x1a1;
  credit_table[0x100] = 0x1d6;
  credit_table[0x101] = 0x184;
  credit_table[0x102] = 170000;
  credit_table[0x103] = 0x1a2;
  credit_table[0x104] = 0x1e0;
  credit_table[0x105] = 0x136;
  credit_table[0x106] = 0x2a3c8;
  credit_table[0x107] = 0x1a3;
  credit_table[0x108] = 0x1e0;
  credit_table[0x109] = 0x146;
  credit_table[0x10a] = 0x2a3c8;
  credit_table[0x10b] = 0x1a4;
  credit_table[0x10c] = 0x1e0;
  credit_table[0x10d] = 0x156;
  credit_table[0x10e] = 0x2a3c8;
  credit_table[0x10f] = 0x1a5;
  credit_table[0x110] = 0x1d6;
  credit_table[0x111] = 0x13c;
  credit_table[0x112] = 180000;
  credit_table[0x113] = 0x1a6;
  credit_table[0x114] = 0x1e0;
  credit_table[0x115] = 0x15a;
  credit_table[0x116] = 0x2d2a8;
  credit_table[0x117] = 0x1a7;
  credit_table[0x118] = 500;
  credit_table[0x119] = 0x182;
  credit_table[0x11a] = 0x2de60;
  credit_table[0x11b] = 0x1a8;
  credit_table[0x11c] = 0x1d6;
  credit_table[0x11d] = 0x16e;
  credit_table[0x11e] = 0x2ea18;
  credit_table[0x11f] = 0x1a9;
  credit_table[0x120] = 0x1fe;
  credit_table[0x121] = 0x13c;
  credit_table[0x122] = 0x2f5d0;
  credit_table[0x123] = 0x1aa;
  credit_table[0x124] = 0x1d6;
  credit_table[0x125] = 0x13c;
  credit_table[0x126] = 200000;
  credit_table[0x127] = 0x1ab;
  credit_table[0x128] = 0x1e0;
  credit_table[0x129] = 0x15a;
  credit_table[0x12a] = 0x320c8;
  credit_table[299] = 0x1ac;
  credit_table[300] = 0x208;
  credit_table[0x12d] = 0x182;
  credit_table[0x12e] = 0x32c80;
  credit_table[0x12f] = 0x1ad;
  credit_table[0x130] = 0x1d6;
  credit_table[0x131] = 0x16e;
  credit_table[0x132] = 0x33838;
  credit_table[0x133] = 0x1ae;
  credit_table[0x134] = 0x1fe;
  credit_table[0x135] = 0x13c;
  credit_table[0x136] = 0x343f0;
  credit_table[0x13c] = 0x1e0;
  credit_table[0x145] = 0x16e;
  credit_table[0x14c] = 0x1cc;
  credit_table[0x151] = 0x16e;
  credit_table[0x137] = 0x1af;
  credit_table[0x138] = 0x1d6;
  credit_table[0x139] = 0x13c;
  credit_table[0x13a] = 0x35f48;
  credit_table[0x13b] = 0x1b0;
  credit_table[0x13d] = 0x15a;
  credit_table[0x13e] = 0x372d0;
  credit_table[0x13f] = 0x1b1;
  credit_table[0x140] = 0x208;
  credit_table[0x141] = 0x182;
  credit_table[0x142] = 0x37e88;
  credit_table[0x143] = 0x1b2;
  credit_table[0x144] = 0x1d6;
  credit_table[0x146] = 0x38a40;
  credit_table[0x147] = 0x1b3;
  credit_table[0x148] = 0x1fe;
  credit_table[0x149] = 0x13c;
  credit_table[0x14a] = 0x395f8;
  credit_table[0x14b] = 0x1b4;
  credit_table[0x14d] = 0x15a;
  credit_table[0x14e] = 0x3a1b0;
  credit_table[0x14f] = 0x1b5;
  credit_table[0x150] = 500;
  credit_table[0x152] = 0x3ad68;
  credit_table[0x153] = END_OF_TABLE;
  colours.text_colour = COLOUR_WHITE_RGB;
  string_id = 0x161;
  entry = credit_table + 1;
  /* walk the table; draw each entry whose start time has passed, stop at END_OF_TABLE */
  do {
    if (entry[1] < current_time) {
      elapsed = current_time - entry[1];
      /* alpha: fade in over FADE_IN_MS, opaque until FADE_OUT_START_MS, then fade out by FADE_OUT_END_MS */
      if (elapsed < FADE_IN_MS) {
        alpha = (byte)elapsed;
      }
      else if (elapsed < FADE_OUT_START_MS) {
        alpha = ALPHA_OPAQUE;
      }
      else {
        elapsed = FADE_OUT_END_MS - elapsed;
        /* fade-out has ended: skip drawing this entry */
        if (elapsed < 0) goto LAB_00433d02;
        alpha = (byte)((int)(elapsed + (elapsed >> 0x1f & 3U)) >> 2);
      }
      /* elapsed is reused here for this entry's x */
      elapsed = entry[-1];
      /* black shadow at (+2, +2) first, then the white text on top */
      colours.shadow_colour = (uint)alpha << 0x18;
      colours.text_colour = CONCAT13(alpha,(undefined3)colours.text_colour);
      y = *entry;
      ui_rt2d_text_draw(string_id,(shadow_x = (float)(elapsed + SHADOW_X_OFFSET), *(int *)&shadow_x),(shadow_y = (float)(y + SHADOW_Y_OFFSET), *(int *)&shadow_y),colours.shadow_colour,TEXT_SCALE_BITS,2);
      ui_rt2d_text_draw(string_id,(text_x = (float)(elapsed + TEXT_X_OFFSET), *(int *)&text_x),(text_y = (float)y, *(int *)&text_y),colours.text_colour,TEXT_SCALE_BITS,2);
    }
LAB_00433d02:
    string_id = entry[2];
    entry = entry + 4;
    if (string_id == END_OF_TABLE) {
      /* end of table: reset the menu slots to their index (inferred); string_id is reused as the index */
      string_id = 0;
      do {
        (&g_menu_609a58)[string_id] = string_id;
        string_id = string_id + 1;
      } while (string_id < MENU_RESET_COUNT);
      return;
    }
  } while( true );
}


