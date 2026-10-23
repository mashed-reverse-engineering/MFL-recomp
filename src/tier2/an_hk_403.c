#include "mfl_tier2.h"
/*0042bd00*/ extern char *dat_get_string();
/* ui_format_number_999 */
#define MAX_DISPLAY_VALUE (999)
#define PLACE_HUNDRED (100)
#define PLACE_TEN (10)
#define ASCII_ZERO (0x30)
#define ASCII_SPACE (0x20)
/* header word of the wide string = characters written incl. the trailing space (inferred) */
#define LEN_3_DIGITS (4)
#define LEN_2_DIGITS (3)
#define LEN_1_DIGIT (2)

/* "(value, string id): clamps to 999 and formats three digits into a wide string" */

/*
 * Writes the decimal text of `value` (expected 0..999, negatives are not handled) into the
 * wide string returned by dat_get_string(string_id), leading zeros suppressed.
 * Layout: text[0] = count of the characters that follow incl. the trailing space (inferred),
 * then the digits, then ASCII_SPACE. Example: 5 -> {2,'5',' '}, 50 -> {3,'5','0',' '}.
 * Returns nothing; the string is written in place.
 */
void ui_format_number_999(int value,undefined4 string_id)

{
  short ones;
  undefined2 *text;
  int hundreds;
  int tens;

  text = (undefined2 *)dat_get_string(string_id);
  /* clamp to the three-digit display maximum */
  if (MAX_DISPLAY_VALUE < value) {
    value = MAX_DISPLAY_VALUE;
  }
  /* split into hundreds / tens / ones digits */
  hundreds = value / PLACE_HUNDRED;
  if (hundreds != 0) {
    *text = LEN_3_DIGITS;
    text[1] = (short)hundreds + ASCII_ZERO;
  }
  tens = (value % PLACE_HUNDRED) / PLACE_TEN;
  ones = (short)((value % PLACE_HUNDRED) % PLACE_TEN);
  /* tens digit is zero: 1-digit form, or hundreds form with a zero tens digit */
  if (tens == 0) {
    if (hundreds != 0) {
LAB_00431bad:
      text[2] = (short)tens + ASCII_ZERO;
      if (hundreds == 0) goto LAB_00431bb8;
      goto LAB_00431beb;
    }
LAB_00431bbc:
    *text = LEN_1_DIGIT;
    text[1] = ones + ASCII_ZERO;
  }
  /* tens digit is non-zero: 2-digit form, or 3-digit form */
  else {
    if (hundreds != 0) goto LAB_00431bad;
    *text = LEN_2_DIGITS;
    text[1] = (short)tens + ASCII_ZERO;
LAB_00431bb8:
    if (tens == 0) goto LAB_00431bbc;
    if (hundreds == 0) {
      text[2] = ones + ASCII_ZERO;
      goto LAB_00431bfb;
    }
LAB_00431beb:
    text[3] = ones + ASCII_ZERO;
  }
  /* trailing space goes right after the last digit written */
  if (hundreds != 0) {
    text[4] = ASCII_SPACE;
    return;
  }
LAB_00431bfb:
  if (tens != 0) {
    text[3] = ASCII_SPACE;
    return;
  }
  text[2] = ASCII_SPACE;
  return;
}


