#include "mfl_tier2.h"
/*0042bd00*/ extern char *dat_get_string();
/*004b2a60*/ extern int sprintf();
/*005dd330*/ extern float float_1_0;
/*005dda24*/ extern float k_f_0_75;
/*005de9a8*/ extern float k_f_0_009999;
/*005de9ac*/ extern undefined g_tutorial_text_5de9ac;

/* Length word stored at text_out[0] (inferred: count of characters that follow). */
#define RATIO_TEXT_LEN_FIXED (4)     /* "1.0 " / "0.0 " */
#define RATIO_TEXT_LEN_FRACTION (5)  /* "0.dd " from sprintf */
/* Characters written at fixed positions of the dat string. */
#define RATIO_CHAR_ZERO (0x30)
#define RATIO_CHAR_ONE (0x31)
#define RATIO_CHAR_SPACE (0x20)
/* tutorial_text_format_ratio */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/*
 * Writes the fraction count_b / count_a as text into the dat string buffer for string_id (inferred).
 * Output is "1.0 " or "0.0 " (4 chars) for the edge cases, or "0.dd " (5 chars) otherwise.
 * The character at index 2 (the decimal point / separator) is never written here; it comes from the dat template (inferred).
 * clamp_zero_counts != 0 raises zero counts to 1 (inferred).
 * Returns nothing.
 * (was: "writes a ratio string like 1:0 / 0:0 into the dat string buffer")
 */

void tutorial_text_format_ratio(int count_a,int count_b,undefined4 string_id,int clamp_zero_counts)

{
  double shown_ratio;
  double ratio_b_over_a;
  bool both_one;
  int tmp_a;
  int tmp_b;
  undefined2 *text_out;
  union {
    char gs_buf[8];
    struct {
      undefined1 int_part [2];
      char frac_tens;
      char frac_ones;
    } m;
  } frame;

  text_out = (undefined2 *)dat_get_string(string_id);
  both_one = false;
  tmp_a = count_a;
  tmp_b = count_b;
  /* clamp mode (inferred): zero counts become 1; both_one is set when both end up 1 */
  if (clamp_zero_counts != 0) {
    if (count_a == 0) {
      if (count_b == 0) goto LAB_00431d06;
      count_a = 1;
    }
    if (count_b == 0) {
      count_b = 1;
    }
    tmp_a = count_b;
    tmp_b = count_a;
    if ((count_b == 1) && (tmp_a = count_b, tmp_b = count_a, count_a == 1)) {
      both_one = true;
      tmp_a = count_b;
      tmp_b = count_a;
    }
  }
  /* copy back: in clamp mode tmp_a/tmp_b hold the counts swapped, so the slots end up swapped (inferred, check disassembly) */
  count_b = tmp_b;
  count_a = tmp_a;
  /* "1.0": count_a is zero (or both were zero in clamp mode) */
  if (count_a == 0) {
LAB_00431d06:
    *text_out = RATIO_TEXT_LEN_FIXED;
    text_out[1] = RATIO_CHAR_ONE;
    text_out[3] = RATIO_CHAR_ZERO;
    text_out[4] = RATIO_CHAR_SPACE;
    return;
  }
  /* "0.0": count_b is zero */
  if (count_b == 0) {
LAB_00431d41:
    *text_out = RATIO_TEXT_LEN_FIXED;
    text_out[1] = RATIO_CHAR_ZERO;
    text_out[3] = RATIO_CHAR_ZERO;
    text_out[4] = RATIO_CHAR_SPACE;
    return;
  }
  ratio_b_over_a = (float)count_b / (float)count_a;
  shown_ratio = k_f_0_75;
  /* ratio exactly 1.0 -> "1.0"; ratio <= 0.01 (k_f_0_009999) -> "0.0"; otherwise formatted. both_one shows the fixed k_f_0_75 */
  if (!both_one) {
    if (ratio_b_over_a == float_1_0) goto LAB_00431d06;
    shown_ratio = ratio_b_over_a;
    if (ratio_b_over_a <= k_f_0_009999) goto LAB_00431d41;
  }
  /* format the fraction, then copy its last two digits into the string */
  sprintf(frame.m.int_part,&g_tutorial_text_5de9ac,(double)shown_ratio);
  *text_out = RATIO_TEXT_LEN_FRACTION;
  text_out[1] = RATIO_CHAR_ZERO;
  text_out[3] = (short)frame.m.frac_tens;
  text_out[4] = (short)frame.m.frac_ones;
  text_out[5] = RATIO_CHAR_SPACE;
  return;
}


