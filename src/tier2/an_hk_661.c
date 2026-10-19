#include "mfl_tier2.h"
/*0042bd00*/ extern int dat_get_string();
/*004b2a60*/ extern int sprintf();
/*005de9b4*/ extern undefined g_frontend_5de9b4;
#define ASCII_ZERO (0x30)
#define TEXT_DIGITS (3)
#define TEXT_SLOT_0_OFFSET (2)
#define TEXT_SLOT_1_OFFSET (4)
#define TEXT_SLOT_2_OFFSET (6)
/* menu_text_format_and_draw */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* Formats number with the frontend format string and writes it as three zero-padded
   digit glyphs (16-bit values) into the text entry returned by dat_get_string(dat_string_id).
   Digits are right-aligned in slots 0..2 and the empty leading slots get ASCII_ZERO.
   A formatted string of 0 or more than 3 characters gives "000" (inferred from the branches).
   (was: "formats the dat string with param_2 and draws characters") */

void menu_text_format_and_draw(undefined4 dat_string_id,undefined4 number)

{
  char ch;
  int text_entry;
  char *cursor;
  int digit_count;
  uint glyph;
  int slot;
  char number_text[8];

  text_entry = dat_get_string(dat_string_id);
  sprintf(((undefined4 *)number_text),&g_frontend_5de9b4,number);
  cursor = (char *)((undefined4 *)number_text);
  /* find the NUL terminator; digit_count = strlen(number_text) */
  do {
    ch = *cursor;
    cursor = cursor + 1;
  } while (ch != '\0');
  digit_count = (int)cursor - ((int)((undefined4 *)number_text) + 1);
  slot = 0;
  glyph = (*(undefined4 *)number_text);
  /* one pass per output slot, slot 0 = leftmost digit (hundreds if the format is 3-wide, inferred) */
  do {
    /* slot 0: the first digit, or ASCII_ZERO padding for short strings */
    if (slot == 0) {
      if (digit_count == TEXT_DIGITS) {
        glyph = (uint)(ushort)(short)(char)(*(undefined4 *)number_text);
      }
      else {
        glyph = ASCII_ZERO;
      }
      *(short *)(text_entry + TEXT_SLOT_0_OFFSET) = (short)glyph;
    }
    /* slot 1: middle digit; for 2-character strings this is the first digit */
    else if (slot == 1) {
      if (digit_count == TEXT_DIGITS) {
        glyph = (uint)(ushort)(short)(*(byte *)((char *)((undefined4 *)number_text) + 1));
        *(short *)(text_entry + TEXT_SLOT_1_OFFSET) = (short)(*(byte *)((char *)((undefined4 *)number_text) + 1));
      }
      else if (digit_count == 2) {
        glyph = (uint)(ushort)(short)(char)(*(undefined4 *)number_text);
        *(short *)(text_entry + TEXT_SLOT_1_OFFSET) = (short)(char)(*(undefined4 *)number_text);
      }
      else {
        if (digit_count == 1) {
          glyph = ASCII_ZERO;
        }
        *(short *)(text_entry + TEXT_SLOT_1_OFFSET) = (short)glyph;
      }
    }
    /* slot 2: last digit (the least significant one) */
    else if (slot == 2) {
      if (digit_count == TEXT_DIGITS) {
        glyph = (uint)(ushort)(short)(*(byte *)((char *)((undefined4 *)number_text) + 2));
        *(short *)(text_entry + TEXT_SLOT_2_OFFSET) = (short)(*(byte *)((char *)((undefined4 *)number_text) + 2));
      }
      else if (digit_count == 2) {
        glyph = (uint)(ushort)(short)(*(byte *)((char *)((undefined4 *)number_text) + 1));
        *(short *)(text_entry + TEXT_SLOT_2_OFFSET) = (short)(*(byte *)((char *)((undefined4 *)number_text) + 1));
      }
      else {
        if (digit_count == 1) {
          glyph = (uint)(ushort)(short)(char)(*(undefined4 *)number_text);
        }
        *(short *)(text_entry + TEXT_SLOT_2_OFFSET) = (short)glyph;
      }
    }
    slot = slot + 1;
  } while (slot < TEXT_DIGITS);
  return;
}


